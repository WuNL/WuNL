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
    void setQueuePtr(boost::shared_ptr<std::vector<std::queue<AVFrame*> > > pFrameQueueVecPtr)
    {
        pFrameQueueVecPtr_=pFrameQueueVecPtr;
    }
protected:

private:
    GLFWwindow* window;
    boost::thread m_Thread;
    boost::shared_ptr<std::vector<std::queue<AVFrame*> > > pFrameQueueVecPtr_;
//    boost::mutex lock;

    GLuint VBO, VAO, EBO,pboIds,pboUV;
    GLuint vbo;
    GLuint texture,textureUV;
    Shader* ourShader;
};

#endif // VIEWER_H
