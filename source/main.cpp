// Kompajliranje:
// g++ -o SimpleAnim SimpleAnim.cpp util.cpp -lGLEW -lGL -lGLU -lglut -lpthread

#include <iostream>
#include <numeric>
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

//#ifdef LINUX_UBUNTU_SEGFAULT
//    #include <pthread.h>
//#endif


[[maybe_unused]] GLuint window;
GLuint sub_width = 500, sub_height = 500;
int WindowHeight;
int WindowWidth;
const double Xmin = 0.0, Xmax = 3.0;
const double Ymin = 0.0, Ymax = 3.0;

glm::mat4 projection;
glm::vec2 mousePos(0,0);
Shader shader_snake;
Shader shader_background;
Slither snake(0, 0, 0.1f);

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
Background background;


glm::mat4 addView(glm::mat4 model){
    glm::mat4 view = glm::lookAt(
            snake.head() + glm::vec3(0, 0, 1),
            snake.head(),
            glm::vec3(0,1,0)
    );
    return view * model;
}
glm::mat4 createMVPEye(glm::vec3 localEyePosition){
    glm::mat4 model(1.0f);

    model = glm::translate (model, snake.head());

    model = glm::rotate (model, snake.getDirection(), glm::vec3(0, 0, 1));
    model = glm::translate (model, localEyePosition);

    model = glm::rotate (model,
                         glm::atan(mousePos.y,mousePos.x) - snake.getDirection(),
                         glm::vec3(0,0,1));

    return addView(model);
}
glm::mat4 createMVPBody(){
    glm::mat4 model = glm::mat4(1.0f);
    return addView(model);
}

class Renderer{



    static void enableVA(unsigned numOfInputAttributes) {
        for (int i=0;i<numOfInputAttributes ; i++)
            glEnableVertexAttribArray(i);

    }
    static void disableVA(unsigned numOfInputAttributes) {
        for (int i=0;i<numOfInputAttributes ; i++)
            glDisableVertexAttribArray(i);

    }



    /**
     * Sends data to GPU.
     * @param points All data points organised AS SHOWN in 'positions'.
     * @param positions Shows how data points are organised by attribute.For example,
     *  (1,3) means first attribute has 1 dimension, second has 3.
     */
    static void setupData(const std::vector<GLfloat>& points, const std::vector<int>& positions){

        unsigned short W = std::accumulate(positions.begin(), positions.end(), 0);

        GLuint VAO;
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);
        GLuint VBO;
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, (unsigned)(sizeof(GLfloat) * points.size()), &points[0], GL_DYNAMIC_DRAW);
        int i=0;
        unsigned short formerAttribs=0;
        for (auto& currentAttrib : positions)
        {
            glVertexAttribPointer(
                    i,                                              //i.th attribute
                    currentAttrib,                                  //num of dimensions for ith attrib
                    GL_FLOAT,
                    GL_FALSE,
                    (int)(sizeof(GLfloat)*W),                            //width of one row
                    (const GLvoid *)(sizeof(GLfloat) * formerAttribs)   //offset
            );
            formerAttribs+=currentAttrib;
            i++;
        }
    }


    /**
     * Draws ONE instance of each of n='count' points.
     * It also enables/disables each attribute for that data set.
     * @param mvp
     * @param count
     * @param current_shader
     * @param numOfInputAttributes
     */
    static void draw(const glm::mat4& mvp,unsigned count,Shader& current_shader,unsigned numOfInputAttributes){
        enableVA(numOfInputAttributes);

        current_shader.setMat4("MVP" ,&mvp[0][0]);

        if(!current_shader.isUsingTess())
            glDrawArrays(GL_POINTS,0,(int)count);
        else{
            glPatchParameteri(GL_PATCH_VERTICES,1);
            glDrawArrays(GL_PATCHES,0,(int)count);
        }

        disableVA(numOfInputAttributes);
    }
public:
    Renderer()= default;
    /**
     * Render points in this manner:
     * for EVERY mvp in MVPs, draw ALL points from 'data'
     * @param current_shader
     * @param positions Show how attributes are organised. For example,
     *  (1,3) means first attribute has 1 dimension, second has 3.
     * @param data data organised AS SHOWN in 'positions'
     * @param MVPs multiple MVP matrices,each represents instance of set of points
     */
    static void render(Shader& current_shader,
                std::vector<int> positions,
                const std::vector<GLfloat>& data,
                std::vector<glm::mat4>& MVPs){



        current_shader.use();

        Renderer::setupData(data,positions);

        int sum = std::accumulate(positions.begin(), positions.end(), 0);

        for (const auto& mvp: MVPs){
            Renderer::draw(mvp,
                       data.size()/sum,   // = num of points to be rendered
                       current_shader,
                       positions.size());
        }


    }


};

void glutPassiveMotionFunc(int x, int y ) {

    float xPos = ((float)x)/((float)(WindowWidth-1));
    float yPos = ((float)y)/((float)(WindowHeight-1));
    xPos =2*xPos-1;
    yPos=-2*yPos+1;

    mousePos.x = xPos;
    mousePos.y = yPos;


    glutPostRedisplay();

}

//void myKeyboardFunc( unsigned char key, int x, int y ){}

void mySpecialKeyFunc( int key, int x, int y )
{
	switch ( key ) {
        case GLUT_KEY_UP:
            snake.add();
            break;
        case GLUT_KEY_DOWN:
            snake.cutTail();
            break;

        case GLUT_KEY_LEFT:
            std::cout << snake.toString();
            throw std::exception();
        default:
            break;
    }
}

int main(int argc, char ** argv)
{

	// Sljedeci blok sluzi kao bugfix koji je opisan gore
	#ifdef LINUX_UBUNTU_SEGFAULT
        //int i=pthread_getconcurrency();
	#endif

	glutInitContextVersion(3, 3);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutInitContextFlags(GLUT_DEBUG);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
	glutInitWindowSize((int)sub_width,(int)sub_height);
	glutInitWindowPosition(100,100);
	glutInit(&argc, argv);

	window = glutCreateWindow("SimpleAnim" );
	glutReshapeFunc(resizeWindow);
	glutDisplayFunc(myDisplay);
//	glutKeyboardFunc( myKeyboardFunc );			// Handles "normal" ascii symbols
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

    for (int j=0 ;j < 3 ; j++) snake.add();

	std::cout << "Going to load programs... " << std::endl << std::flush;

	shader_snake.load_shaders({
            "SnakeVertexShader.vert",
            "SnakeFragmentShader.frag",
            "" ,
            "SnakeTessControl.tesc" ,
            "SnakeTessEval.tese"
    });

    shader_background.load_shaders({
          "BackgroundVertexShader.vert",
          "BackgroundFragmentShader.frag",
          "BackgroundGeometryShader.geom" ,
          "" ,
          ""
    });

	return true;
}






void myDisplay() {
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    snake.changeDirection(glm::degrees(glm::atan(mousePos.y, mousePos.x)));
    snake.hop();

    const std::vector<GLfloat> snake_points = snake.getCoords();
    const std::vector<GLfloat> eyes_points{
                    0.02,   0.0,    0,  1/70.0, 400,
                    0.0,    0.0,    0,  1/50.0, 300 };
    const std::vector<GLfloat> background_points = background.update(snake.head());



    /**
     * Eyes
     * We have two MVPs. One eye is 'copied' twice to the scene, on different coordinates.
     *
     * For eyes ,we use SnakeShader which requires data organised like this:
     *         I.attribute    |   II.attribute
     *     Position     Scale |     Color
     *     x y z        s     |     c
     *
     *     => positions = (4,1)
     */
    std::vector<glm::mat4> MVPs;
    shader_snake.setInt("eyes" , 1);
    MVPs.push_back(createMVPEye(glm::vec3(0.1, 0.1, 0)));
    MVPs.push_back(createMVPEye(glm::vec3(0.1, -0.1, 0)));
    Renderer::render(shader_snake, std::vector<int>({4, 1}) , eyes_points, MVPs);

    /**
     * Body
     * Data organisation same as above.
     */
    MVPs.clear();
    MVPs.push_back(createMVPBody());
    shader_snake.setInt("eyes" , 3);
    Renderer::render(shader_snake, std::vector<int>({4, 1}) , snake_points, MVPs);

    /**
     * Background
     * We use BackgroundShader which requires our data to be organised like this:
     *         I.attribute    |
     *          Position      |
     *           x y z        |
     *
     *           => positions = (3)
     */
    MVPs.clear();
    MVPs.push_back(createMVPBody());
    Renderer::render(shader_background, std::vector<int>({3}) , background_points, MVPs);


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



