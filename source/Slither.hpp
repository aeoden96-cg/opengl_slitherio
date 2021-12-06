#include <deque>
#include <stdexcept>
#include <string>

struct Coord{
    Coord(float x,float y):x(x),y(y){}
    std::string toString() const{
        std::stringstream s;
        s << "(" << x << ", " << y << ")";
        return s.str();
    }
    float x;
    float y;
};

class Slither{
private:
    std::deque<Coord> snake;
    float radius = 5;

public:
    Slither(float x,float y){
        snake.push_back(Coord(x,y));
    }
    unsigned getLength(){
        return snake.size();
    }
    void add(float x, float y){
        snake.push_back(Coord(x,y));
        hop(x,y);
    }
    void hop(float x,float y){
        Coord temp = Coord(x,y);

        for (auto &chunk :snake)
            std::swap(chunk, temp);

    }
    bool cutTail(unsigned x){
        if(x >= snake.size()) return false;
        for (int i=0;i<x;i++)
            snake.pop_back();
        return true;
    }

    const GLfloat* getCoordinates(){
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
        s << "]";
        return s.str();
    }

};