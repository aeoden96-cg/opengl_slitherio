#include <deque>
#include <stdexcept>
#include <string>
#include <glm/glm.hpp>

bool approximatelyEqual(float a, float b, float epsilon)
{
    return fabs(a - b) <= ( (fabs(a) < fabs(b) ? fabs(b) : fabs(a)) * epsilon);
}

bool essentiallyEqual(float a, float b, float epsilon)
{
    return fabs(a - b) <= ( (fabs(a) > fabs(b) ? fabs(b) : fabs(a)) * epsilon);
}

bool definitelyGreaterThan(float a, float b, float epsilon)
{
    return (a - b) > ( (fabs(a) < fabs(b) ? fabs(b) : fabs(a)) * epsilon);
}

bool definitelyLessThan(float a, float b, float epsilon)
{
    return (b - a) > ( (fabs(a) < fabs(b) ? fabs(b) : fabs(a)) * epsilon);
}

struct Coord{
    glm::vec2 pos;
    float z;
    glm::vec2 dir;

    Coord( glm::vec2 position , glm::vec2 direction):pos(position),dir(direction),z(0){}

    std::string toString() const{
        std::stringstream s;
        s << "(" << pos.x << ", " << pos.y << ")";
        return s.str();
    }
    void setDirection(glm::vec2 new_dir){
        dir = new_dir;
    }

    float getMaxDistance() const{
        return glm::length(dir-pos);
    }

    glm::vec2 unitStep(){
        return glm::vec2(dir-pos)/getMaxDistance();
    }



};

class Slither{
private:

    float radius = 0.2;
    GLfloat dir = 0;
    const float diff;

public:
    std::deque<Coord> snake;
    Slither(float x,float y,float diff):diff(diff){
        //push in snake HEAD
        //don't use head DIRECTION
        snake.push_back(Coord(glm::vec2(x,y), glm::vec2(0,0)));
    }
    unsigned getLength(){
        return snake.size();
    }
    glm::vec3 head(){
        return {snake.front().pos.x,snake.front().pos.y,snake.front().z};
    }
    void changeDirection(GLfloat fi){
        if(fi > 360) fi -=360;
        if(fi < 0) fi+=360;
        dir=fi;
    }
    void add(){
        GLfloat rads = glm::radians(dir);

        Coord& last = snake.back();
        Coord chunk(last.pos + glm::vec2(diff,0),glm::vec2(last.pos));

        snake.push_back(chunk);
    }

    void hop(float step_n=0.003f){
        Coord &head = snake.front(); //never use head.dir
        GLfloat rads = glm::radians(dir);  //direction of new step

        head.pos = head.pos + glm::vec2(glm::cos(rads),glm::sin(rads))*step_n;

//        Coord newChunk(glm::cos(rads),glm::sin(rads));
//        newChunk.pos =newChunk.pos*0.003f + head.pos;
        //float ampl = glm::length(newChunk.pos-head.pos);
        Coord lastChunck = head;

        bool first = true;
        for (auto & i : snake)
        {
            if (first) { first = false; continue; }

            float m = i.getMaxDistance();
            if (definitelyGreaterThan(m,step_n,1e-5)){
                i.pos += i.unitStep()*step_n;
            }
            else if(definitelyGreaterThan(step_n,m,1e-5)){
                i.pos = i.dir;
                i.dir = lastChunck.pos;

                if(definitelyGreaterThan( diff,i.getMaxDistance(),1e-5) ){
                    i.pos += -i.unitStep()*(diff-i.getMaxDistance());

                }



                i.pos += i.unitStep()*(step_n-m);
            }

            lastChunck=i;
        }

//        for (auto & i : snake){
//                std::swap(i,newChunk);
//        }


    }
    bool cutTail(unsigned x){
        if(x >= snake.size()) return false;
        for (int i=0;i<x;i++)
            snake.pop_back();
        return true;
    }
    std::vector<GLfloat> getCoords(){
        GLfloat i = 1;
        std::vector<GLfloat> T;
        for(const auto& c: snake){
            T.push_back(c.pos.x);
            T.push_back(c.pos.y);
            T.push_back(c.z);
            T.push_back(radius);
            T.push_back(i);
            i++;
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