#ifndef GLVIEWER_H
#define GLVIEWER_H
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "params.h"
#include "Shader.h"
#include <queue>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/tss.hpp>
#include <boost/asio.hpp>
#include "channel.h"
class glViewer
{
public:
    glViewer();
    virtual ~glViewer();
    int init();
    int test();
    int render();
    void setQueuePtr(boost::shared_ptr<std::vector<std::queue<AVFrame*> > > pFrameQueueVecPtr)
    {
        pFrameQueueVecPtr_=pFrameQueueVecPtr;
    }
protected:

private:
    GLFWwindow* window;
    GLuint VBO[copyNum], VAO[copyNum], EBO,pboIds[copyNum];
    GLuint texture[copyNum];
    Shader* ourShader;
    boost::shared_ptr<std::vector<std::queue<AVFrame*> > > pFrameQueueVecPtr_;
};

#endif // GLVIEWER_H
