#ifndef VIEWER_H
#define VIEWER_H
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "GLError.h"
#include "params.h"
#include "Shader.h"

#include "channel.h"
#include <sched.h>
#include <thread>
#include <sys/time.h>
#include <time.h>

class viewer
{
public:
    viewer();
    virtual ~viewer();
    void updateImage(GLubyte* dst,int x,int y, int w,int h,void* data);
    int init();
    void run();
    void displayFun();
    void display();
    void devFun();
    void setStyle(int splitNum);

    void setQueuePtr(boost::shared_ptr<std::vector<std::queue<AVFrame*> > > pFrameQueueVecPtr)
    {
        pFrameQueueVecPtr_=pFrameQueueVecPtr;
    }
protected:

private:
    void setVertices(int splitNum, int style);
    void setStyleInter();
    GLFWwindow* window;
    boost::thread m_Thread;
    boost::shared_ptr<std::vector<std::queue<AVFrame*> > > pFrameQueueVecPtr_;
//    boost::mutex lock;

    GLuint VBO[16], VAO[16], EBO,pboIds[16],pboUV[16];
    GLuint vbo;
    GLfloat* verticesVec[16];
    GLfloat vertics2D[16][20];
    GLuint texture,textureUV;
    Shader* ourShader;
    int frameWidth,frameHeight;
    int splitNum_,splitNum_old;
};

#endif // VIEWER_H
