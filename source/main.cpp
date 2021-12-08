// Kompajliranje:
// g++ -o SimpleAnim SimpleAnim.cpp util.cpp -lGLEW -lGL -lGLU -lglut -lpthread

#ifdef _WIN32
#include <windows.h>             //bit ce ukljuceno ako se koriste windows
#endif

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>

#include <GL/glew.h>
#include <GL/freeglut.h>

// Ukljuci potporu za osnovne tipove glm-a: vektore i matrice
#include <glm/glm.hpp>

// Ukljuci funkcije za projekcije, transformacije pogleda i slicno
#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>

// Ako ste na Ubuntu-u i program se odmah po pokretanju srusi sa segmentation fault,
// radi se o poznatom bugu:
// https://bugs.launchpad.net/ubuntu/+source/nvidia-graphics-drivers-319/+bug/1248642
// potreban je sljedeci define i program treba linkati s -lpthread:
#ifndef _WIN32
#define LINUX_UBUNTU_SEGFAULT
#endif

#ifdef LINUX_UBUNTU_SEGFAULT
//ss1
#include <pthread.h>
#endif

// Nasa pomocna biblioteka za ucitavanje, prevodenje i linkanje programa shadera
#include "util/Shader.h"
#include "main.hpp"
#include "Slither.hpp"


GLuint window; 
GLuint sub_width = 500, sub_height = 500; 

GLuint VAO;
GLuint programID;
GLuint MVPMatrixID;

glm::mat4 projection; 
Shader s;
int RunMode = 1;		// Used as a boolean (1 or 0) for "on" and "off"

int WindowHeight;
int WindowWidth;
// The next global variable controls the animation's state and speed.
float CurrentAngle = 0.0f;			// Angle in degrees
float AnimateStep = 1.0f;			// Rotation step per update

// These variables set the dimensions of the rectanglar region we wish to view.
const double Xmin = 0.0, Xmax = 3.0;
const double Ymin = 0.0, Ymax = 3.0;
float ss= -0.05;
Slither sl(0,0,0.1f);

bool init_data(); // nasa funkcija za inicijalizaciju podataka



void glutPassiveMotionFunc(int x, int y ) {

    float xPos = ((float)x)/((float)(WindowWidth-1));
    float yPos = ((float)y)/((float)(WindowHeight-1));
    xPos =2*xPos-1;
    yPos=-2*yPos+1;
    sl.changeDirection(glm::degrees(glm::atan(yPos,xPos)));


    glutPostRedisplay();

}

void myKeyboardFunc( unsigned char key, int x, int y )
{
	switch ( key ) {
	case 'r':
//		RunMode = 1-RunMode;		// Toggle to opposite value
//		if ( RunMode==1 ) {
//			glutPostRedisplay();
//		}
		break;
	case 's':
//		RunMode = 1;
//		myDisplay();
//		RunMode = 0;
		break;
    case 'w':
        sl.hop();
        ss -= 0.05;
        break;
    case 'd':
        sl.add();
        break;

	case 27:	// Escape key
		exit(1);
	}
    myDisplay();
}

void mySpecialKeyFunc( int key, int x, int y )
{
	switch ( key ) {
	case GLUT_KEY_UP:		
//		if ( AnimateStep < 1.0e3) {			// Avoid overflow problems
//			AnimateStep *= sqrt(2.0);		// Increase the angle increment
//		}
        sl.hop();
		break;
	case GLUT_KEY_DOWN:
//		if (AnimateStep>1.0e-6) {		// Avoid underflow problems.
//			AnimateStep /= sqrt(2.0);	// Decrease the angle increment
//		}
    sl.add();
		break;
	}
}

int main(int argc, char ** argv)
{

	// Sljedeci blok sluzi kao bugfix koji je opisan gore
	#ifdef LINUX_UBUNTU_SEGFAULT
        //ss2
        int i=pthread_getconcurrency();        
	#endif

	glutInitContextVersion(3, 3);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutInitContextFlags(GLUT_DEBUG);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
	glutInitWindowSize(sub_width,sub_height);
	glutInitWindowPosition(100,100);
	glutInit(&argc, argv);

	window = glutCreateWindow("SimpleAnim" );
	glutReshapeFunc(resizeWindow);
	glutDisplayFunc(myDisplay);
	glutKeyboardFunc( myKeyboardFunc );			// Handles "normal" ascii symbols
    //glutMouseFunc(myMouseFunc);
    glutPassiveMotionFunc(glutPassiveMotionFunc);
	glutSpecialFunc( mySpecialKeyFunc );		// Handles "special" keyboard keys

	glewExperimental = GL_TRUE;
	glewInit();

	if(!init_data()) return 1;

	// Omogući uporabu Z-spremnika
	glEnable(GL_DEPTH_TEST);


    sl.add();
    sl.add();
    sl.add();
    sl.add();
    sl.add();
    sl.add();
    sl.add();
    sl.add();
    sl.add();
    sl.add();
    sl.add();
    sl.add();
    sl.add();
    sl.add();
//      std::cout << sl.toString();
//      sl.hop(-0.1,-0.1);
//      std::cout << sl.toString();
//    std::vector<GLfloat> t  = sl.getCoords();
//    for(auto i: t)
//        std::cout << i << "\n";
//
//    std::cout << "Size " << sizeof(GLfloat)*sl.getLength()*3 << "\n";


	glutMainLoop();
    return 0;
}

//*********************************************************************************
//	Funkcija u kojoj se radi inicijalizacija potrebnih VAO-a i VBO-ova. 
//*********************************************************************************

bool init_data()
{
	glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );

	// Antialijasiranje poligona, ovisno o implementaciji
//	glEnable(GL_POLYGON_SMOOTH);
//	glEnable(GL_BLEND);
//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//    glHint(GL_POLYGON_SMOOTH, GL_DONT_CARE);
//



	std::cout << "Going to load programs... " << std::endl << std::flush;

	programID = s.load_shaders({"SimpleVertexShader.vert", "SimpleFragmentShader.frag","" , "" , ""});
	if(programID==0) {
		std::cout << "Zbog grešaka napuštam izvođenje programa." << std::endl;
		return false;
	}

	// Get a handle for our "MVP" uniform for later when drawing...
	//MVPMatrixID = glGetUniformLocation(programID, "MVP");

    glPointSize(5.0);


	return true;
}


void myDisplay() {
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (RunMode == 1) {
        // Calculate animation parameters
        CurrentAngle += AnimateStep;
        if (CurrentAngle > 360.0) {
            CurrentAngle -= 360.0 * floor(CurrentAngle / 360.0);    // Don't allow overflow
        }
    }

    sl.hop();

    // Model matrix :
    //glm::mat4 model = glm::mat4(1.0f);

    //model = glm::translate (model,glm::vec3(1.5f, 1.5f, 0.0f));

// 	model = glm::rotate (model,
//                          (float) (CurrentAngle*M_PI/180.0),
//                          glm::vec3(0.0f, 0.0f, 1.0f));

    //model = glm::translate (model,glm::vec3(-1.5f, -1.5f, 0.0f));

    // Our ModelViewProjection : multiplication of our 2 matrices
    //glm::mat4 mvp = projection * model; // Kasnije se mnozi matrica puta tocka - model matrica mora biti najbliza tocki


    //DATA POINTS
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    const std::vector<GLfloat> my_data = sl.getCoords();
    GLuint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * sl.getLength() * 3, &my_data[0], GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *) 0);


    glEnableVertexAttribArray(0);
    s.use();

    //glUniformMatrix4fv(MVPMatrixID, 1, GL_FALSE, &mvp[0][0]);

    //glDrawArrays(GL_TRIANGLES, 0, 9); // Starting from vertex 0; 9 vertices total -> 3 triangles
    glDrawArrays(GL_POINTS, 0, sl.getLength());

    glDisableVertexAttribArray(0);
//	glDisableVertexAttribArray(1);

//	if ( RunMode==1 ) {
//		glutPostRedisplay();	// Trigger an automatic redraw for animation
//	}
    glutSwapBuffers();
    glutPostRedisplay();
}

void resizeWindow(int w, int h)
{
	double scale, center;
	double windowXmin, windowXmax, windowYmin, windowYmax;

	// Define the portion of the window used for OpenGL rendering.
	glViewport( 0, 0, w, h );	// View port uses whole window

	// Set up the projection view matrix: orthographic projection
	// Determine the min and max values for x and y that should appear in the window.
	// The complication is that the aspect ratio of the window may not match the
	//		aspect ratio of the scene we want to view.
	w = (w==0) ? 1 : w;
	h = (h==0) ? 1 : h;
    WindowHeight = (h>1) ? h : 2;
    WindowWidth = (w>1) ? w : 2;
	if ( (Xmax-Xmin)/w < (Ymax-Ymin)/h ) {
		scale = ((Ymax-Ymin)/h)/((Xmax-Xmin)/w);
		center = (Xmax+Xmin)/2;
		windowXmin = center - (center-Xmin)*scale;
		windowXmax = center + (Xmax-center)*scale;
		windowYmin = Ymin;
		windowYmax = Ymax;
	}
	else {
		scale = ((Xmax-Xmin)/w)/((Ymax-Ymin)/h);
		center = (Ymax+Ymin)/2;
		windowYmin = center - (center-Ymin)*scale;
		windowYmax = center + (Ymax-center)*scale;
		windowXmin = Xmin;
		windowXmax = Xmax;
	}
	
	// Now that we know the max & min values for x & y 
	//		that should be visible in the window,
	//		we set up the orthographic projection.
    projection = glm::ortho(
            (float) windowXmin,
            (float) windowXmax,
            (float) windowYmin,
            (float) windowYmax,
            -1.0f,
            1.0f);

}



