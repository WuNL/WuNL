#ifndef AUDIOBARDRAWER_H
#define AUDIOBARDRAWER_H

#include "Shader.h"
#include<cstring>

class barDrawer
{
public:
    barDrawer(int window_width, int window_height);
    virtual ~barDrawer();
    virtual void draw()=0;
protected:
    Shader shader;
    GLuint VAO, VBO, EBO;

private:


};

class audioBarDrawer:public barDrawer
{
    public:
        audioBarDrawer(int window_width, int window_height);
        virtual ~audioBarDrawer();
        void draw();
    protected:

    private:

};

#endif // AUDIOBARDRAWER_H
