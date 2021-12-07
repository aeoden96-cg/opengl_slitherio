#include <deque>
#include <stdexcept>
#include <string>
#include <glm/glm.hpp>
struct Coord{
    Coord(float x,float y):pos(glm::vec2(x,y)),z(0),dir(glm::vec2(1,0)){}
    std::string toString() const{
        std::stringstream s;
        s << "(" << pos.x << ", " << pos.y << ")";
        return s.str();
    }
    void normalize(){
        pos = glm::normalize(pos);
    }

    void changeDirectionForce(glm::vec2 newdir){
        newdir = glm::normalize(newdir);
        dir = newdir;
    }
    void changeDirection(glm::vec2 newdir){
        newdir = glm::normalize(newdir);
        dir = glm::normalize(newdir + dir);
    }


    glm::vec2 pos;
    float z;
    glm::vec2 dir;
};

class Slither{
private:

    float radius = 5;
    GLfloat dir = 0;

public:
    std::deque<Coord> snake;
    Slither(float x,float y){
        snake.push_back(Coord(x,y));
    }
    unsigned getLength(){
        return snake.size();
    }
    void changeDirection(GLfloat fi){
        if(fi > 360) fi -=360;
        if(fi < 0) fi+=360;
        dir=fi;
    }
    void add(){
        GLfloat rads = glm::radians(dir);
        Coord newCoords = Coord(-glm::cos(rads)*snake.size()*0.06,-glm::sin(rads)*snake.size()*0.06);
        Coord lastChunk = snake.back();

        newCoords.changeDirectionForce(lastChunk.pos-newCoords.pos);

        snake.push_back(newCoords);
        //hop(x,y);
    }

    void hop(){
        Coord head = snake.front();
        GLfloat rads = glm::radians(dir);
        Coord newChunk = Coord(glm::cos(rads)*0.003+head.pos.x,glm::sin(rads)*0.003+head.pos.y);

        float ampl = glm::length(newChunk.pos-head.pos);




        for (auto & i : snake){
              Coord newt(i.dir.x,i.dir.y);
              newt.changeDirection(newChunk.pos - i.pos);
              newt.normalize();

              newt.pos = newt.pos*ampl+i.pos;
//            Coord temp2 = Coord(i.x,i.y);
//            i.x= temp.x;
//            i.y= temp.y;
//            temp.x = temp2.x;
//            temp.y= temp2.y;
              std::swap(i,newt);
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
            T.push_back(c.pos.x);
            T.push_back(c.pos.y);
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
            q[i]=chunk.pos.x;
            q[i+1] =chunk.pos.y;
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