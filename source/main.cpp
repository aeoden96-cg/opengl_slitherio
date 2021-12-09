// Kompajliranje:
// g++ -o SimpleAnim SimpleAnim.cpp util.cpp -lGLEW -lGL -lGLU -lglut -lpthread

#include <cstdlib>
#include <iostream>

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "util/Shader.h"
#include "main.hpp"
#include "Slither.hpp"

#ifdef _WIN32
    #include <windows.h>
#endif

#ifndef _WIN32
    #define LINUX_UBUNTU_SEGFAULT
#endif

#ifdef LINUX_UBUNTU_SEGFAULT
    #include <pthread.h>
#endif


GLuint window;
GLuint sub_width = 500, sub_height = 500;
int WindowHeight;
int WindowWidth;
const double Xmin = 0.0, Xmax = 3.0;
const double Ymin = 0.0, Ymax = 3.0;
glm::vec2 mousePos(0,0);
glm::mat4 projection;

Shader s;
Slither sl(0,0,0.1f);

bool init_data();


void glutPassiveMotionFunc(int x, int y ) {

    float xPos = ((float)x)/((float)(WindowWidth-1));
    float yPos = ((float)y)/((float)(WindowHeight-1));
    xPos =2*xPos-1;
    yPos=-2*yPos+1;

    mousePos.x = xPos;
    mousePos.y = yPos;



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
            sl.add();
            break;
        case GLUT_KEY_DOWN:
            sl.cutTail();
            break;

        case GLUT_KEY_LEFT:
            std::cout << sl.toString();
            throw std::exception();
            break;
        default:
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

	init_data();

	// Omogući uporabu Z-spremnika
	glEnable(GL_DEPTH_TEST);




	glutMainLoop();
    return 0;
}

bool init_data()
{
	glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );

	// Antialijasiranje poligona, ovisno o implementaciji
    //	  glEnable(GL_POLYGON_SMOOTH);
    //	  glEnable(GL_BLEND);
    //    glHint(GL_POLYGON_SMOOTH, GL_DONT_CARE);
    //    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    for (int j=0 ;j < 3 ; j++) sl.add();

	std::cout << "Going to load programs... " << std::endl << std::flush;

	s.load_shaders({
        "SimpleVertexShader.vert",
        "SimpleFragmentShader.frag",
        "" ,
        "TessControl.tesc" ,
        "TessEval.tese"});

	return true;
}

void setupData(const std::vector<GLfloat>& my_data){
    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    GLuint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*my_data.size(),  &my_data[0], GL_STATIC_DRAW);
    glVertexAttribPointer(
            0,                  // attribute 0.
            4,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            sizeof(GLfloat)*5,  // stride
            (void*)nullptr            // array buffer offset
    );
    glVertexAttribPointer(
            1,                  // attribute 0.
            1,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            sizeof(GLfloat)*5,  // stride
            (const GLvoid *)(sizeof(GLfloat) * 4)  // array buffer offset
    );
}
void draw(glm::mat4& mvp,int eyes){
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    float scale;
    int patchLen;
    int patchBegin;
    switch ( eyes){
        case 1: //white
            scale = 1/50.0;
            patchLen = 2;
            patchBegin=0;
            break;

//        case 2: //black
//            scale = 1/70.0;
//            patchLen = 2;
//            patchBegin=2;
//            break;
        case 3: //red other
            scale = 1/16.0;
            patchLen = sl.getLength();
            patchBegin=0;
            break;
    }

    s.setMat4("MVP" ,&mvp[0][0]);
    s.setFloat("uScale",scale);
    s.setInt("eyes" , eyes);

    //glPolygonMode(GL_FRONT_AND_BACK,GL_FILL); //GL_LINE
    glPatchParameteri(GL_PATCH_VERTICES,1);
    glDrawArrays(GL_PATCHES,patchBegin,patchLen);
    //glDrawArrays(GL_POINTS, 0, sl.getLength());
}

glm::mat4 setupMatrix(){
    glm::mat4 model = glm::mat4(1.0f);
    //model = glm::translate (model,glm::vec3(1.5f, 1.5f, 0.0f));
    glm::mat4 view(1.0f);
    view = glm::lookAt(
            sl.head()+glm::vec3(0,0,1),
            sl.head(),
            glm::vec3(0,1,0)
            );
    glm::mat4 mvp = view * model;

    return mvp;
}
glm::mat4 setupMatrixEyes(bool black){
    glm::mat4 model = glm::mat4(1.0f);

    if (black){
        model = glm::translate (model, sl.head());
        model = glm::rotate (model, sl.getDirection(),glm::vec3(0,0,1));


    }
    else{
        model = glm::translate (model, sl.head());
        model = glm::rotate (model, sl.getDirection(),glm::vec3(0,0,1));
    }

    glm::mat4 view(1.0f);
    view = glm::lookAt(
            sl.head()+glm::vec3(0,0,1),
            sl.head(),
            glm::vec3(0,1,0)
    );
    glm::mat4 mvp = view * model;

    return mvp;
}

void myDisplay() {
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    s.use();


    sl.hop();
    const std::vector<GLfloat> my_data = sl.getCoords();
//    const std::vector<GLfloat> eyes{0.1,0.1,0,0,0,
//                                    0.1,-0.1,0,0,0,
//                                    0.12,0.1,0,0,0,
//                                    0.12,-0.1,0,0,0};
    const std::vector<GLfloat> eye{
                    0.02,0.0,0,1/70.0,400,
                    0.0,0.0,0,1/50.0,300
                                   };
    setupData(eye);
//    glm::mat4 mvp2 = setupMatrixEyes(true);

    //first eye
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate (model, sl.head());

    model = glm::rotate (model, sl.getDirection(),glm::vec3(0,0,1));
    model = glm::translate (model, glm::vec3(0.1,0.1,0));

    model = glm::rotate (model,
                         glm::atan(mousePos.y,mousePos.x)-sl.getDirection(),
                         glm::vec3(0,0,1));


    glm::mat4 view(1.0f);
    view = glm::lookAt(
            sl.head()+glm::vec3(0,0,1),
            sl.head(),
            glm::vec3(0,1,0)
    );
    glm::mat4 mvp = view * model;

    draw(mvp,1);

//    mvp2 = setupMatrixEyes(false);
    //second exe
    model = glm::mat4(1.0f);
    model = glm::translate (model, sl.head());

    model = glm::rotate (model, sl.getDirection(),glm::vec3(0,0,1));
    model = glm::translate (model, glm::vec3(0.1,-0.1,0));

    model = glm::rotate (model,
                         glm::atan(mousePos.y,mousePos.x)-sl.getDirection(),
                         glm::vec3(0,0,1)
    );


    view = glm::lookAt(
            sl.head()+glm::vec3(0,0,1),
            sl.head(),
            glm::vec3(0,1,0)
    );
    mvp = view * model;
    draw(mvp,1);


    //BODY
    mvp = setupMatrix();
    setupData(my_data);
    draw(mvp,3);


    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

    glutSwapBuffers();
    glutPostRedisplay();
}

void resizeWindow(int w, int h)
{
	double scale, center;
	double windowXmin, windowXmax, windowYmin, windowYmax;

	glViewport( 0, 0, w, h );	// View port uses whole window

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

    projection = glm::ortho(
            (float) windowXmin,
            (float) windowXmax,
            (float) windowYmin,
            (float) windowYmax,
            -1.0f,
            1.0f);

}



