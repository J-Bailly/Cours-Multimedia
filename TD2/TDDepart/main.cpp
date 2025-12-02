#include <iostream>
#include <vector>
#include <array>
#include <fstream>

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

#define ENABLE_SHADERS

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Identifiant du programme envoyé à la carte graphique.
unsigned int progid;
// Identifiant de la matrice de transformation (3D->2D) envoyée à la carte graphique.
unsigned int mvpid;

// Matrices de transformation.
glm::mat4 view;
glm::mat4 view1;
glm::mat4 proj;
glm::mat4 mvp;

// Identifiant des tableaux passés à la carte graphique.
unsigned int vaoids[ 1 ];

float r = 0.0f;
/**
 * Fonction d'affichage.
 */
void display()
{
  std::cout << "display\n";

  // Nettoyage du buffer d'affichage par la couleur par défaut.
  glClear( GL_COLOR_BUFFER_BIT );

  // L'origine du repère est déplacée à -5 suivant l'axe z.
  view = glm::translate( glm::mat4( 1.0f ) , glm::vec3( 0.0f, -1.5f, -5.0f ) );
  view1 = view;
  view = glm::rotate( view, glm::radians( r ), glm::vec3( 0.0f, 1.0f, 0.0f ) );

  // Calcul de la matrice mvp.
  mvp = proj * view;

  // Passage de la matrice mvp au shader (envoi vers la carte graphique).
  glUniformMatrix4fv( mvpid , 1, GL_FALSE, &mvp[0][0]);

  // Dessin de 1 triangle à partir de 3 indices.
  glBindVertexArray( vaoids[ 0 ] );
  glDrawElements( GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0 );

  //----Cube2----//
  {
    glm::mat4 model1 = glm::mat4(1.0f);

    // Déplace le cube vers le haut
    model1 = glm::translate(model1, glm::vec3(2.0f, 0.0f, 0.0f));

    // Recalcule le MVP du cube 2
    glm::mat4 mvp2 = proj * view * model1;

    // Envoie la matrice au shader
    glUniformMatrix4fv(mvpid , 1, GL_FALSE, &mvp2[0][0]);

    glBindVertexArray(vaoids[0]);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0 );
  }

  //----Cube3----//
  {
    glm::mat4 model1 = glm::mat4(1.0f);

    // Déplace le cube vers le haut
    model1 = glm::translate(model1, glm::vec3(0.0f, 4.0f, 0.0f));

    // Recalcule le MVP du cube 2
    glm::mat4 mvp2 = proj * view1 * model1;

    // Envoie la matrice au shader
    glUniformMatrix4fv(mvpid , 1, GL_FALSE, &mvp2[0][0]);

    glBindVertexArray(vaoids[0]);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0 );
  }

  //----Cube4----//
  {
    glm::mat4 model1 = glm::mat4(1.0f);

    // Déplace le cube vers le haut
    model1 = glm::rotate(model1, glm::radians(r), glm::vec3(0.0f, 1.0f, 0.0f));
    model1 = glm::translate(model1, glm::vec3(2.0f, 4.0f, 0.0f));
    model1 = glm::scale(model1, glm::vec3(0.5f, 0.5f, 0.5f));
    model1 = glm::rotate(model1, glm::radians(r), glm::vec3(0.0f, 2.0f, 0.0f));

    // Recalcule le MVP du cube 2
    glm::mat4 mvp2 = proj * view1 * model1;

    // Envoie la matrice au shader
    glUniformMatrix4fv(mvpid , 1, GL_FALSE, &mvp2[0][0]);

    glBindVertexArray(vaoids[0]);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0 );
  }

  //----Cube5----//
  {
    glm::mat4 model1 = glm::mat4(1.0f);

    // Déplace le cube vers le haut
    model1 = glm::translate(model1, glm::vec3(0.0f, 4.0f, 0.0f));
    model1 = glm::rotate(model1, glm::radians(r), glm::vec3(1.0f, 0.0f, 0.0f));
    model1 = glm::scale(model1, glm::vec3(0.5f, 0.5f, 0.5f));
    model1 = glm::translate(model1, glm::vec3(0.0f, 3.0f, 0.0f));
    model1 = glm::rotate(model1, glm::radians(r), glm::vec3(2.0f, 0.0f, 0.0f));

    // Recalcule le MVP du cube 2
    glm::mat4 mvp2 = proj * view1 * model1;

    // Envoie la matrice au shader
    glUniformMatrix4fv(mvpid , 1, GL_FALSE, &mvp2[0][0]);

    glBindVertexArray(vaoids[0]);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0 );
  }
  glutSwapBuffers();

  
}

/**
 * Fonction appelée en boucle
 */
void idle()
{
  // Incrémentation de l'angle de rotation.
  r >= 360.0f ? r = 0.0f : r += 1.5f;
  // Ré-affichage de la scène.
  glutPostRedisplay();
}

/**
 * Fonction appelée à chaque redimensionnement de la fenêtre.
 */
void reshape( int w, int h )
{
  std::cout << "reshape\n";

  // Modification de la zone d'affichage OpenGL.
  glViewport(0, 0, w, h);
  // Modification de la matrice de projection à chaque redimensionnement de la fenêtre.
  proj = glm::perspective( glm::radians(90.0f), w/static_cast< float >( h ), 0.1f, 100.0f );
}


/**
 * Fonction définissant le maillage et effectuant son envoi sur la carte graphique.
 */
void initVAOs()
{
  // Identifiant du maillage.
  unsigned int vboids[ 3 ];

  // Points du maillage.
  std::vector< float > vertices = {
    -0.5f, -0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f,  0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
  };

  // Couleurs du maillage.
  std::vector< float > colors = {
    1.0f, 0.0f, 0.0f,
    1.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 1.0f,
    1.0f, 0.0f, 0.0f,
    1.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 1.0f,
    1.0f, 0.0f, 0.0f,
    1.0f, 1.0f, 0.0f,
  };

  // Indices du maillage.
  std::vector< unsigned short > indices = {
    0, 1, 2,
    1, 2, 3,
    4, 5, 6,
    5, 6, 7,
    0, 5, 1,
    0, 5, 4,
    6, 3, 7,
    6, 3, 2,
    4, 2, 0, 
    4, 2, 6,
    1, 5, 7,
    1, 3, 7
  };

  // Génération d'un Vertex Array Object (VAO) contenant 3 Vertex Buffer Objects.
  glGenVertexArrays( 1, &vaoids[ 0 ] );
  // Activation du VAO.
  glBindVertexArray( vaoids[ 0 ] );

  // Génération de 3 VBO.
  glGenBuffers( 3, vboids );

  // VBO contenant les sommets : activation puis envoi.
  glBindBuffer( GL_ARRAY_BUFFER, vboids[ 0 ] );
  glBufferData( GL_ARRAY_BUFFER, vertices.size() * sizeof( float ), vertices.data(), GL_STATIC_DRAW );

  // L'attribut in_pos du vertex shader est associé aux données de ce VBO.
  auto pos = glGetAttribLocation( progid, "in_pos" );
  glVertexAttribPointer( pos, 3, GL_FLOAT, GL_FALSE, 0, 0 );
  glEnableVertexAttribArray( pos );

  // VBO contenant les couleurs.
  glBindBuffer( GL_ARRAY_BUFFER, vboids[ 1 ] );
  glBufferData( GL_ARRAY_BUFFER, colors.size() * sizeof( float ), colors.data(), GL_STATIC_DRAW );

  // L'attribut in_color du vertex shader est associé aux données de ce VBO.
  auto color = glGetAttribLocation( progid, "in_color" );
  glVertexAttribPointer( color, 3, GL_FLOAT, GL_FALSE, 0, 0 );
  glEnableVertexAttribArray( color );

  // VBO contenant les indices.
  glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vboids[ 2 ] );
  glBufferData( GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof( unsigned short ), indices.data(), GL_STATIC_DRAW );

  // Désactivation du VAO.
  glBindVertexArray( 0 );
}


/**
 * Initialisation des vertex et fragment shaders.
 */
void initShaders()
{
  // Identifiants des vertex et fragment shaders.
  unsigned int vsid, fsid;

  // Programme du vertex shader : transformation du point 3D par la matrice MVP.
  std::string vs(
         "#version 150 core\n"                                    \
         "in vec3 in_pos;\n"                            \
         "in vec3 in_color;\n"                          \
         "uniform mat4 mvp;\n"                          \
         "out vec3 color;\n"                            \
         "void main(void)\n"                            \
         "{\n"                                          \
         "  gl_Position = mvp * vec4( in_pos, 1.0 );\n" \
         "  color = in_color;\n"                        \
         "}\n"
  );
  // Programme du fragment shader : chaque point prend la couleur qui lui est associée.
  std::string fs(
         "#version 150 core\n"                       \
         "in vec3 color;\n"                     \
         "out vec4 frag_color;\n"               \
         "void main(void)\n"                    \
         "{\n"                                  \
         "  frag_color = vec4( color, 1.0 );\n" \
         "}\n"
  );

  // Création et compilation du vertex shader.
  vsid = glCreateShader( GL_VERTEX_SHADER );
  char const * vs_char = vs.c_str();
  glShaderSource( vsid, 1, &vs_char, nullptr );
  glCompileShader( vsid );

  // Idem pour le fragment shader.
  fsid = glCreateShader( GL_FRAGMENT_SHADER );
  char const * fs_char = fs.c_str();
  glShaderSource( fsid, 1, &fs_char, nullptr );
  glCompileShader( fsid );

  // Création du programme (vertex + fragment shader)
  progid = glCreateProgram();

  glAttachShader( progid, vsid );
  glAttachShader( progid, fsid );

  glLinkProgram( progid );

  glUseProgram( progid );

  // Récupération de l'identifiant de la matrice MVP dans le shader.
  mvpid = glGetUniformLocation( progid, "mvp" );
}


int main( int argc, char * argv[] )
{ //glewInit();
  // Initialisation de l'affichage.
  glutInit( &argc, argv );

  #if defined(__APPLE__) && defined(ENABLE_SHADERS)
      glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA|GLUT_3_2_CORE_PROFILE);
  #else
      glutInitContextVersion( 3, 2 );
      //glutInitContextVersion( 4, 5 );
      glutInitDisplayMode( GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE );
      glewInit();
  #endif
  //

  glutInitWindowSize( 640, 480 );
  // Buffer d'affichage contenant des couleurs au format RGBA (Alpha pour la transparence)
  // Activation du buffer de profondeur pour déterminer pour chaque pixel si un nouveau pixel est devant ou derrière un pixel déjà ajouté.
  // Activation d'un buffer d'affichage double pour accélérer l'affichage : on dessine dans un buffer pendant que le buffer précédent est affiché.

  // Affichage de la fenêtre.
  glutCreateWindow( argv[ 0 ]  );

  // Passage des fonctions par défaut pour l'affichage et le redimensionnement.
  glutDisplayFunc( display );
  glutReshapeFunc( reshape );

  // Initialisation de la bibliothèque GLEW.
 #if not defined(__APPLE__)
         glewInit();
#endif

    initShaders();
  initVAOs();

  // Couleur par défaut pour nettoyer le buffer d'affichage.
  glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );

  glutIdleFunc( idle );

  // Lancement de la boucle de rendu.
  glutMainLoop();

  return 0;
}

