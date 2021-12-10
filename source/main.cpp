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
Shader s_background;
Slither sl(0,0,0.1f);

struct Background{
    std::vector<GLfloat> points;

    float offsets[3] = {-1,0,1};

    std::vector<GLfloat> update(glm::vec3 position){
        points.clear();
        position.x = glm::round(position.x);
        position.y = glm::round(position.y);

        for(float i: offsets){
            for(float j: offsets){
                points.push_back(position.x+i);
                points.push_back(position.y+j);
                points.push_back(position.z);
            }
        }
        return points;
    }
};
Background b;

glm::mat4 addView(glm::mat4 model){
    glm::mat4 view = glm::lookAt(
            sl.head()+glm::vec3(0,0,1),
            sl.head(),
            glm::vec3(0,1,0)
    );
    return view * model;
}
glm::mat4 createMVPEye(glm::vec3 localEyePosition){
    glm::mat4 model(1.0f);

    model = glm::translate (model, sl.head());

    model = glm::rotate (model, sl.getDirection(),glm::vec3(0,0,1));
    model = glm::translate (model, localEyePosition);

    model = glm::rotate (model,
                         glm::atan(mousePos.y,mousePos.x)-sl.getDirection(),
                         glm::vec3(0,0,1));

    return addView(model);
}
glm::mat4 createMVPBody(){
    glm::mat4 model = glm::mat4(1.0f);
    return addView(model);
}

class Renderer{



    void enableVA(int numOfInputAttributes) const{
        glEnableVertexAttribArray(0);
        if(numOfInputAttributes > 1)
            glEnableVertexAttribArray(1);
    }
    void disableVA(int numOfInputAttributes) const{
        glDisableVertexAttribArray(0);
        if(numOfInputAttributes > 1)
            glDisableVertexAttribArray(1);

    }
public:
    Renderer(){
    }


    void setupData(const std::vector<GLfloat>& my_data,int positionWidth,int colorWidth){
        GLuint VAO;
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);
        GLuint VBO;
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*my_data.size(),  &my_data[0], GL_DYNAMIC_DRAW);
        glVertexAttribPointer(
                0,                                              // attribute 0.
                positionWidth,                                  // size
                GL_FLOAT,                                       // type
                GL_FALSE,                                       // normalized?
                sizeof(GLfloat)*(positionWidth+colorWidth),     // stride
                (void*)nullptr                                  // array buffer offset
        );

        //color
        if (colorWidth == 0) return;
        glVertexAttribPointer(
                1,                                              // attribute 1.
                colorWidth,
                GL_FLOAT,
                GL_FALSE,
                sizeof(GLfloat)*(positionWidth+colorWidth),
                (const GLvoid *)(sizeof(GLfloat) * positionWidth)
        );
    }


    void draw(glm::mat4& mvp,int count,Shader& current_shader,int numOfInputAttributes){
        enableVA(numOfInputAttributes);
        current_shader.setMat4("MVP" ,&mvp[0][0]);

        if(!current_shader.isUsingTess())
            glDrawArrays(GL_POINTS,0,count);
        else{
            glPatchParameteri(GL_PATCH_VERTICES,1);
            glDrawArrays(GL_PATCHES,0,count);
        }
        disableVA(numOfInputAttributes);
    }

    void render(Shader& current_shader,
                int positionWidth,
                int colorWidth,
                const std::vector<GLfloat>& data,
                glm::mat4& mvp){

        current_shader.use();
        this->setupData(data,positionWidth,colorWidth);
        this->draw(mvp,data.size()/(positionWidth+colorWidth),current_shader,1 + !!colorWidth);
    }

};

bool init_data();


void glutPassiveMotionFunc(int x, int y ) {

    float xPos = ((float)x)/((float)(WindowWidth-1));
    float yPos = ((float)y)/((float)(WindowHeight-1));
    xPos =2*xPos-1;
    yPos=-2*yPos+1;

    mousePos.x = xPos;
    mousePos.y = yPos;


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
            "SnakeVertexShader.vert",
            "SnakeFragmentShader.frag",
            "" ,
            "SnakeTessControl.tesc" ,
            "SnakeTessEval.tese"
    });

    s_background.load_shaders({
          "BackgroundVertexShader.vert",
          "BackgroundFragmentShader.frag",
          "BackgroundGeometryShader.geom" ,
          "" ,
          ""
    });

	return true;
}





Renderer r;
void myDisplay() {
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    sl.changeDirection(glm::degrees(glm::atan(mousePos.y,mousePos.x)));
    sl.hop();

    const std::vector<GLfloat> my_data = sl.getCoords();
    const std::vector<GLfloat> eye{
                    0.02,   0.0,    0,  1/70.0, 400,
                    0.0,    0.0,    0,  1/50.0, 300 };
    const std::vector<GLfloat> background_data = b.update(sl.head());


    //first eye
    s.setInt("eyes" , 1);
    glm::mat4 mvp = createMVPEye(glm::vec3(0.1, 0.1, 0));
    r.render(s,4,1,eye,mvp);

    //second eye
    mvp = createMVPEye(glm::vec3(0.1, -0.1, 0));
    s.setInt("eyes" , 1);
    // could be using this, setupData doesnt need to be used twice
    //r.draw(mvp,2,s,2);
    r.render(s,4,1,eye,mvp);

    //body
    mvp = createMVPBody();
    s.setInt("eyes" , 3);
    r.render(s,4,1,my_data,mvp);

    //background
    mvp = createMVPBody();
    r.render(s_background,3,0,background_data,mvp);


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



