#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cmath>
#include <algorithm>

#if defined(__APPLE__)
#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl3.h>
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/freeglut.h>
#endif

#include "config.h"
#include "GLError.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <sys/timeb.h>

#define NBMESHES 4

struct shaderProg {
    unsigned int progid;
    unsigned int mid; 
    unsigned int vid; 
    unsigned int pid; 
    unsigned int LightID;
};

struct maillage {
    shaderProg shader;
    unsigned int vaoids; 
    unsigned int nbtriangles;
    float angle = 0.0f;
    float scale = 0.0f; 
    float inc = 0.1f;
    float x, y, z; 
};

const float YAW   = -1.5707963f;
const float PITCH =  0.0f;

struct Camera {
    glm::vec3 Position   = glm::vec3(0.0f, 0.0f, 5.0f);
    glm::vec3 Front      = glm::vec3(0.0f, 0.0f, -1.0f); // Direction du regard
    glm::vec3 Up         = glm::vec3(0.0f, 1.0f, 0.0f); // Le "haut" de la camera
    glm::vec3 WorldUp    = glm::vec3(0.0f, 1.0f, 0.0f);
    
    float Yaw   = YAW;   // Rotation gauche/droite
    float Pitch = PITCH; // Rotation haut/bas
    
    float MovementSpeed = 0.0f; // Vitesse actuelle (0 si pas de touche)
} globalcamera;

float yawRate   = 0.0f;
float pitchRate = 0.0f;
float speedStep = 4.0f; 
float rotStep   = 1.5f; 

bool isRotating = false;
float rotationAngle = 0.0f;

shaderProg shaders[NBMESHES];
maillage maillages[NBMESHES];
glm::mat4 view, proj;
float eye[3] = {0.0f, 0.0f, 5.0f};

shaderProg initShaders(std::string vertPath, std::string fragPath) {
    shaderProg sp;
    
    std::ifstream vs_ifs(std::string(MY_SHADER_PATH) + vertPath);
    std::ifstream fs_ifs(std::string(MY_SHADER_PATH) + fragPath);

    if(!vs_ifs || !fs_ifs) {
        std::cerr << "Erreur : Impossible d'ouvrir " << vertPath << " ou " << fragPath << std::endl;
        exit(1);
    }

    std::string vs_src((std::istreambuf_iterator<char>(vs_ifs)), std::istreambuf_iterator<char>());
    std::string fs_src((std::istreambuf_iterator<char>(fs_ifs)), std::istreambuf_iterator<char>());

    unsigned int vsid = glCreateShader(GL_VERTEX_SHADER);
    unsigned int fsid = glCreateShader(GL_FRAGMENT_SHADER);
    const char* vs_c = vs_src.c_str();
    const char* fs_c = fs_src.c_str();

    glShaderSource(vsid, 1, &vs_c, nullptr);
    glCompileShader(vsid);
    glShaderSource(fsid, 1, &fs_c, nullptr);
    glCompileShader(fsid);

    sp.progid = glCreateProgram();
    glAttachShader(sp.progid, vsid);
    glAttachShader(sp.progid, fsid);
    glLinkProgram(sp.progid);

    sp.mid = glGetUniformLocation(sp.progid, "m");
    sp.vid = glGetUniformLocation(sp.progid, "v");
    sp.pid = glGetUniformLocation(sp.progid, "p");

    return sp;
}

maillage initVAOs(shaderProg sp, std::string meshPath) {
    maillage m;
    m.shader = sp;

    std::ifstream ifs(std::string(MY_SHADER_PATH) + meshPath);
    if (!ifs) {
        std::cerr << "Erreur : Maillage introuvable : " << meshPath << std::endl;
        exit(1);
    }

    std::string off;
    unsigned int nbpoints, tmp;
    ifs >> off >> nbpoints >> m.nbtriangles >> tmp;

    std::vector<float> vertices(nbpoints * 3);
    std::vector<unsigned int> indices(m.nbtriangles * 3);
    std::vector<float> normals(nbpoints * 3, 0.0f);

    for (unsigned int i = 0; i < vertices.size(); ++i) ifs >> vertices[i];
    for (unsigned int i = 0; i < m.nbtriangles * 3; i += 3) {
        ifs >> tmp >> indices[i] >> indices[i+1] >> indices[i+2];
    }

    float xmin = vertices[0], xmax = vertices[0];
    float ymin = vertices[1], ymax = vertices[1];
    float zmin = vertices[2], zmax = vertices[2];
    for (unsigned int i = 1; i < nbpoints; ++i) {
        xmin = std::min(xmin, vertices[i*3]);   xmax = std::max(xmax, vertices[i*3]);
        ymin = std::min(ymin, vertices[i*3+1]); ymax = std::max(ymax, vertices[i*3+1]);
        zmin = std::min(zmin, vertices[i*3+2]); zmax = std::max(zmax, vertices[i*3+2]);
    }
    m.x = (xmin + xmax) / 2.0f; m.y = (ymin + ymax) / 2.0f; m.z = (zmin + zmax) / 2.0f;
    m.scale = 1.0f / std::max({xmax - xmin, ymax - ymin, zmax - zmin});

    for (unsigned int i = 0; i < m.nbtriangles * 3; i += 3) {
        glm::vec3 p1(vertices[3*indices[i]], vertices[3*indices[i]+1], vertices[3*indices[i]+2]);
        glm::vec3 p2(vertices[3*indices[i+1]], vertices[3*indices[i+1]+1], vertices[3*indices[i+1]+2]);
        glm::vec3 p3(vertices[3*indices[i+2]], vertices[3*indices[i+2]+1], vertices[3*indices[i+2]+2]);
        glm::vec3 n = glm::normalize(glm::cross(p2 - p1, p3 - p1));
        for (int j = 0; j < 3; ++j) {
            normals[3*indices[i+j]] += n.x; normals[3*indices[i+j]+1] += n.y; normals[3*indices[i+j]+2] += n.z;
        }
    }
    for (unsigned int i = 0; i < normals.size(); i+=3) {
        glm::vec3 n = glm::normalize(glm::vec3(normals[i], normals[i+1], normals[i+2]));
        normals[i] = n.x; normals[i+1] = n.y; normals[i+2] = n.z;
    }

    glGenVertexArrays(1, &m.vaoids);
    glBindVertexArray(m.vaoids);
    
    unsigned int vbo[3];
    glGenBuffers(3, vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float), normals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[2]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);
    return m;
}

void displayMesh(maillage &m, glm::mat4 modelMatrix) {
    glUseProgram(m.shader.progid);
    
    glm::mat4 mLocal = glm::scale(modelMatrix, glm::vec3(m.scale));
    mLocal = glm::translate(mLocal, glm::vec3(-m.x, -m.y, -m.z));

    glUniformMatrix4fv(m.shader.mid, 1, GL_FALSE, &mLocal[0][0]);
    glUniformMatrix4fv(m.shader.vid, 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(m.shader.pid, 1, GL_FALSE, &proj[0][0]);

    glBindVertexArray(m.vaoids);
    glDrawElements(GL_TRIANGLES, m.nbtriangles * 3, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void key(unsigned char k, int x, int y) {
    if (k == ' ' || k == 'r') {
        isRotating = true;
    }
    if (k == 'z' || k == 'w') globalcamera.MovementSpeed = speedStep;
    if (k == 's') globalcamera.MovementSpeed = -speedStep;

    if (k == 27) exit(0); // Echap pour quitter
}

void keyUp(unsigned char k, int x, int y) {
    if (k == ' ' || k == 'r') {
        isRotating = false;
    }
    if (k == 'z' || k == 'w' || k == 's') globalcamera.MovementSpeed = 0.0f;
}

void special(int k, int x, int y) {
    if (k == GLUT_KEY_LEFT)  yawRate = -rotStep;
    if (k == GLUT_KEY_RIGHT) yawRate = rotStep;
    if (k == GLUT_KEY_UP)    pitchRate = rotStep;
    if (k == GLUT_KEY_DOWN)  pitchRate = -rotStep;
}

void specialUp(int k, int x, int y) {
    if (k == GLUT_KEY_LEFT || k == GLUT_KEY_RIGHT) yawRate = 0.0f;
    if (k == GLUT_KEY_UP   || k == GLUT_KEY_DOWN)  pitchRate = 0.0f;
}

void updateCameraVectors(Camera *c) {
    glm::vec3 f;
    // Formule mathématique pour convertir des angles en vecteur directionnel
    f.x = cos(c->Yaw) * cos(c->Pitch);
    f.y = sin(c->Pitch);
    f.z = sin(c->Yaw) * cos(c->Pitch);
    
    c->Front = glm::normalize(f);
    // On recalcule aussi le vecteur Right pour d'éventuels pas latéraux
    glm::vec3 Right = glm::normalize(glm::cross(c->Front, c->WorldUp));  
    c->Up = glm::normalize(glm::cross(Right, c->Front));
}

int lastTime = 0;
double elapsed = 0.0;

int getMilliCount() {
    timeb tb;
    ftime(&tb);
    return tb.millitm + (tb.time & 0xfffff) * 1000;
}

void calcTime() {
    int now = getMilliCount();
    if (lastTime != 0) {
        // Temps écoulé en secondes (ex: 0.016 pour 60 FPS)
        elapsed = double(now - lastTime) / 1000.0;
        
        // 1. Appliquer la rotation
        globalcamera.Yaw   += yawRate * elapsed;
        globalcamera.Pitch += pitchRate * elapsed;

        // Limite pour ne pas se briser le cou (bloqué à la verticale)
        if (globalcamera.Pitch > 1.5f)  globalcamera.Pitch = 1.5f;
        if (globalcamera.Pitch < -1.5f) globalcamera.Pitch = -1.5f;

        // 2. Mettre à jour les vecteurs de direction
        updateCameraVectors(&globalcamera);

        // 3. Avancer ou reculer
        if (globalcamera.MovementSpeed != 0) {
            globalcamera.Position += globalcamera.Front * globalcamera.MovementSpeed * (float)elapsed;
        }
    }
    lastTime = now;
}

/// void display()
///      {
///    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
///    view = glm::lookAt( glm::vec3( eye[ 0 ], eye[ 1 ], eye[ 2 ] ),
///                        glm::vec3( eye[ 0 ], eye[ 1 ], eye[ 2 ]-1.0f ),
///                        glm::vec3( 0.0f, 1.0f, 0.0f ) );
///
///    float decal=1.25f;
///    glm::mat4 model;
///
///    model = glm::mat4( 1.0f );
///    model = glm::translate( model, glm::vec3( -decal, -decal/1.5, 0.0f ) );
///    model = glm::rotate( model, glm::radians(maillages[0].angle += maillages[0].inc)/2, glm::vec3( 0.0f, 1.0f, 0.0f ) );
///    model = glm::rotate( model, glm::radians(maillages[0].angle += maillages[0].inc)/2, glm::vec3( 1.0f, 0.0f, 0.0f ) );
///    model = glm::rotate( model, glm::radians(maillages[0].angle += maillages[0].inc)/2, glm::vec3( 0.0f, 0.0f, 1.0f ) );
///    model = glm::scale( model, glm::vec3(1.5f) );
///    displayMesh(maillages[0], model);
///
///
///    model = glm::mat4( 1.0f );
///    model = glm::translate( model, glm::vec3( decal, decal/1.5, 0.0f ) );
///    model = glm::rotate( model, glm::radians(maillages[0].angle += maillages[0].inc)/2, glm::vec3( 0.0f, 1.0f, 0.0f ) );
///    model = glm::rotate( model, glm::radians(maillages[0].angle += maillages[0].inc)/2, glm::vec3( 1.0f, 0.0f, 0.0f ) );
///    model = glm::rotate( model, glm::radians(maillages[0].angle += maillages[0].inc)/2, glm::vec3( 0.0f, 0.0f, 1.0f ) );
///    model = glm::scale( model, glm::vec3(1.5f) );
///    displayMesh(maillages[1], model);
///
///    model = glm::mat4( 1.0f );
///    model = glm::translate( model, glm::vec3( -decal, decal/1.5, 0.0f ) );
///    model = glm::rotate( model, glm::radians(maillages[0].angle += maillages[0].inc)/2, glm::vec3( 0.0f, 1.0f, 0.0f ) );
///    model = glm::rotate( model, glm::radians(maillages[0].angle += maillages[0].inc)/2, glm::vec3( 1.0f, 0.0f, 0.0f ) );
///    model = glm::rotate( model, glm::radians(maillages[0].angle += maillages[0].inc)/2, glm::vec3( 0.0f, 0.0f, 1.0f ) );
///    model = glm::scale( model, glm::vec3(1.5f) );
///    displayMesh(maillages[2], model);
///
///    model = glm::mat4( 1.0f );
///    model = glm::translate( model, glm::vec3( decal, -decal/1.5, 0.0f ) );
///    model = glm::rotate( model, glm::radians(maillages[0].angle += maillages[0].inc)/2, glm::vec3( 0.0f, 1.0f, 0.0f ) );
///    model = glm::rotate( model, glm::radians(maillages[0].angle += maillages[0].inc)/2, glm::vec3( 1.0f, 0.0f, 0.0f ) );
///    model = glm::rotate( model, glm::radians(maillages[0].angle += maillages[0].inc)/2, glm::vec3( 0.0f, 0.0f, 1.0f ) );
///    model = glm::scale( model, glm::vec3(1.5f) );
///    displayMesh(maillages[3], model);
///
///    glutSwapBuffers();
///}

void display() {

    calcTime(); // On calcule tout le mouvement ici !
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // On regarde de POSITION vers POSITION + FRONT
    view = glm::lookAt(globalcamera.Position, 
                       globalcamera.Position + globalcamera.Front, 
                       globalcamera.Up);

    // Si on appuie, l'angle de rotation augmente
    if (isRotating) {
        rotationAngle += 0.35f; 
    } else {
        rotationAngle = 0.0f; // Retour instantané à l'origine
    }

    float decal = 1.25f;
    for (int i = 0; i < NBMESHES; i++) {
        glm::mat4 model = glm::mat4(1.0f);
        
        // Placement aux coins
        float posX = (i % 2 == 0) ? -decal : decal;
        float posY = (i < 2) ? decal/1.5f : -decal/1.5f;
        
        model = glm::translate(model, glm::vec3(posX, posY, 0.0f));
        
        // On applique la rotation (sera 0 si on ne touche pas à Espace)
        model = glm::rotate(model, rotationAngle, glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, rotationAngle, glm::vec3(1.0f, 0.0f, 0.0f));
        
        model = glm::scale(model, glm::vec3(1.5f));
        displayMesh(maillages[i], model);
    }

    glutSwapBuffers();
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    proj = glm::perspective(glm::radians(45.0f), (float)w/h, 0.1f, 100.0f);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(800, 600);
    glutCreateWindow("TD6 - Refactorisation");

    glewExperimental = GL_TRUE;
    glewInit();
    glEnable(GL_DEPTH_TEST);

    shaders[0] = initShaders("/shaders/phong.vert.glsl", "/shaders/toon.frag.glsl");
    maillages[0] = initVAOs(shaders[0], "/meshes/space_shuttle2.off");

    shaders[1] = initShaders("/shaders/phong.vert.glsl", "/shaders/phong.frag.glsl");
    maillages[1] = initVAOs(shaders[1], "/meshes/space_station2.off");

    shaders[2] = initShaders("/shaders/phong.vert.glsl", "/shaders/phongVert.frag.glsl");
    maillages[2] = initVAOs(shaders[2], "/meshes/milleniumfalcon.off/milleniumfalcon.off");

    shaders[3] = initShaders("/shaders/phong.vert.glsl", "/shaders/phongRouge.frag.glsl");
    maillages[3] = initVAOs(shaders[3], "/meshes/rabbit.off");

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutIdleFunc([](){ glutPostRedisplay(); });
    
    glutKeyboardFunc(key);
    glutKeyboardUpFunc(keyUp); // Très important pour le retour à l'état initial
    glutSpecialFunc(special);
    glutSpecialUpFunc(specialUp);
    updateCameraVectors(&globalcamera); // Initialise la direction au depart
    
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glutMainLoop();
    return 0;
}