#include <iostream>
#include <vector>
#include <array>
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
#include "repere.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define NBMESHES 4

struct shaderProg {
    unsigned int progid;
    unsigned int mid;
    unsigned int vid;
    unsigned int pid;
};

struct maillage {
    shaderProg shader;
    unsigned int vaoids; 
    unsigned int nbtriangles;
    float scale = 1.0f;
    float x = 0.0f, y = 0.0f, z = 0.0f;
};

repere rep(1.0);
shaderProg shaders[NBMESHES];
maillage maillages[NBMESHES];
glm::mat4 view, proj;
std::array<float, 3> eye = { 0.0f, 0.0f, 5.0f };

// --- FONCTION DE CHARGEMENT SHADER ---
shaderProg initShaders(std::string vertPath, std::string fragPath) {
    shaderProg sp;
    
    std::string vpath = std::string(MY_SHADER_PATH) + vertPath;
    std::string fpath = std::string(MY_SHADER_PATH) + fragPath;

    std::ifstream vs_ifs(vpath);
    std::ifstream fs_ifs(fpath);

    if(!vs_ifs.is_open()) {
    std::cerr << "CRITICAL: Impossible d'ouvrir le Vertex Shader: " << vpath << std::endl;
    exit(1);
    }
    if(!fs_ifs.is_open()) {
        std::cerr << "CRITICAL: Impossible d'ouvrir le Fragment Shader: " << fpath << std::endl;
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

// --- FONCTION DE CHARGEMENT MAILLAGE ---
maillage initVAOs(shaderProg sp, std::string meshPath) {
    maillage m;
    m.shader = sp;

    std::ifstream ifs(std::string(MY_SHADER_PATH) + meshPath);
    if (!ifs) {
        std::cerr << "Erreur maillage introuvable: " << meshPath << std::endl;
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

    // Calcul Boite Englobante
    float xmin = vertices[0], xmax = vertices[0], ymin = vertices[1], ymax = vertices[1], zmin = vertices[2], zmax = vertices[2];
    for (unsigned int i = 1; i < nbpoints; ++i) {
        xmin = std::min(xmin, vertices[i*3]);   xmax = std::max(xmax, vertices[i*3]);
        ymin = std::min(ymin, vertices[i*3+1]); ymax = std::max(ymax, vertices[i*3+1]);
        zmin = std::min(zmin, vertices[i*3+2]); zmax = std::max(zmax, vertices[i*3+2]);
    }
    m.x = (xmin + xmax) / 2.0f; m.y = (ymin + ymax) / 2.0f; m.z = (zmin + zmax) / 2.0f;
    m.scale = 1.0f / std::max({xmax - xmin, ymax - ymin, zmax - zmin});

    // Calcul Normales
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

    // Positions (Location 0 dans le shader)
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    // Normales (Location 1 dans le shader)
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float), normals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    // Indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[2]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);
    return m;
}

void displayMesh(maillage &m, glm::mat4 modelMatrix) {
    glUseProgram(m.shader.progid);
    
    // Matrice finale : Translation Monde * Scale Objet * Translation de recentrage
    glm::mat4 finalModel = glm::scale(modelMatrix, glm::vec3(m.scale));
    finalModel = glm::translate(finalModel, glm::vec3(-m.x, -m.y, -m.z));

    glUniformMatrix4fv(m.shader.mid, 1, GL_FALSE, &finalModel[0][0]);
    glUniformMatrix4fv(m.shader.vid, 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(m.shader.pid, 1, GL_FALSE, &proj[0][0]);

    glBindVertexArray(m.vaoids);
    glDrawElements(GL_TRIANGLES, m.nbtriangles * 3, GL_UNSIGNED_INT, 0);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    view = glm::lookAt(glm::vec3(eye[0], eye[1], eye[2]), glm::vec3(eye[0], eye[1], eye[2]-1.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    float d = 1.25f;
    displayMesh(maillages[0], glm::translate(glm::mat4(1.0f), glm::vec3(-d, -d, 0.0f)));
    displayMesh(maillages[1], glm::translate(glm::mat4(1.0f), glm::vec3(d, d, 0.0f)));
    displayMesh(maillages[2], glm::translate(glm::mat4(1.0f), glm::vec3(-d, d, 0.0f)));
    // Réduction du scale du lapin comme dans l'énoncé (0.7f)
    glm::mat4 modelLapin = glm::translate(glm::mat4(1.0f), glm::vec3(d, -d, 0.0f));
    modelLapin = glm::scale(modelLapin, glm::vec3(0.7f));
    displayMesh(maillages[3], modelLapin);

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
    glutCreateWindow("TD6 - Final");

    // Sécurité GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Erreur d'initialisation de GLEW" << std::endl;
        return 1;
    }

    glEnable(GL_DEPTH_TEST);

    // --- CHARGEMENT SANS SLASH INITIAL ---
    // Note: On utilise "shaders/..." et "meshes/..." (sans le / devant)
    shaders[0] = initShaders("shaders/phong.vert.glsl", "shaders/phong.frag.glsl");
    maillages[0] = initVAOs(shaders[0], "meshes/space_shuttle2.off");

    shaders[1] = initShaders("shaders/phong.vert.glsl", "shaders/toon.frag.glsl");
    maillages[1] = initVAOs(shaders[1], "meshes/space_station2.off");

    shaders[2] = initShaders("shaders/phong.vert.glsl", "shaders/phongVert.frag.glsl");
    maillages[2] = initVAOs(shaders[2], "meshes/milleniumfalcon.off");

    shaders[3] = initShaders("shaders/phong.vert.glsl", "shaders/phongRouge.frag.glsl");
    maillages[3] = initVAOs(shaders[3], "meshes/rabbit.off");

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutIdleFunc([](){ glutPostRedisplay(); });
    
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glutMainLoop();
    return 0;
}