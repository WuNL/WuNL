#ifndef AUDIOBARDRAWER_H
#define AUDIOBARDRAWER_H

#include "Shader.h"
#include<cstring>

class barDrawer
{
public:
    barDrawer(int window_width, int window_height);
    virtual ~barDrawer();
    virtual void draw(int audioScale,int xOffset, int yOffset, int height)=0;
protected:
    Shader shader;
    GLuint VAO, VBO, EBO;
    int window_width_;
    int window_height_;
private:


};

class audioBarDrawer:public barDrawer
{
public:
    audioBarDrawer(int window_width, int window_height);
    virtual ~audioBarDrawer();
    void draw(int audioScale,int xOffset, int yOffset,int height);
protected:

private:


};

#endif // AUDIOBARDRAWER_H
