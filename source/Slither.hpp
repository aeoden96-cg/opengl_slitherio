#include <deque>
#include <stdexcept>
#include <string>

struct Coord{
    Coord(float x,float y):x(x),y(y),z(0){}
    std::string toString() const{
        std::stringstream s;
        s << "(" << x << ", " << y << ")";
        return s.str();
    }
    float x;
    float y;
    float z;
};

class Slither{
private:

    float radius = 5;
    std::pair<GLfloat,GLfloat> direction = std::pair(1,0);

public:
    std::deque<Coord> snake;
    Slither(float x,float y){
        snake.push_back(Coord(x,y));
    }
    unsigned getLength(){
        return snake.size();
    }
    void changeDirection(GLfloat x,GLfloat y){
        direction=std::pair(x,y);
    }
    void add(float x, float y){
        snake.push_back(Coord(x,y));
        //hop(x,y);
    }
    void hop2(int dir, float offset){
        Coord temp = Coord(snake.front().x+offset,snake.front().y);
        for (auto &chunk :snake)
            std::swap(chunk, temp);
    }
    void hop(float x,float y){
        Coord temp = Coord(x,y);

        for (int i=0; i< snake.size();i++){
            Coord temp2 = Coord(snake[i].x,snake[i].y);
            snake[i].x= temp.x;
            snake[i].y= temp.y;
            temp.x = temp2.x;
            temp.y= temp2.y;
        }




    }
    bool cutTail(unsigned x){
        if(x >= snake.size()) return false;
        for (int i=0;i<x;i++)
            snake.pop_back();
        return true;
    }
    std::vector<GLfloat> getCoords(){
        std::vector<GLfloat> T;
        for(const auto& c: snake){
            T.push_back(c.x);
            T.push_back(c.y);
            T.push_back(c.z);
        }
        return T;
    }
    const GLfloat* getCoordinates(){
        throw std::runtime_error("deprecated");

        float *q = new float[snake.size()*3];
        int i=0;
        for (auto &chunk :snake)
        {
            q[i]=chunk.x;
            q[i+1] =chunk.y;
            q[i+2]= 0;
            i+=3;
        }
        return q;
    };

    std::string toString(){
        std::stringstream s;
        s << "**Snake**[";
        for (auto &chunk :snake)
            s << chunk.toString()<<"-";
        s << "]\n";
        s<< "Front: " << snake.front().toString() << "\n";
        s<< "Back: " << snake.back().toString() << "\n";
        return s.str();
    }

};