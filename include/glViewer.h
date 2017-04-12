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
#include <sched.h>
#include <thread>
class glViewer
{
public:
    glViewer();
    virtual ~glViewer();
    int init();
    int test();
    int render();
    bool bufferEmpty();
    void setWindow(GLfloat** verticesVec__,int channelNum);
    void setStyle(int style);
    void run();

    void setQueuePtr(boost::shared_ptr<std::vector<std::queue<AVFrame*> > > pFrameQueueVecPtr)
    {
        pFrameQueueVecPtr_=pFrameQueueVecPtr;
    }
protected:

private:
    GLFWwindow* window;
    GLuint VBO[16], VAO[16], EBO,pboIds[16];
    GLuint texture[16];
    Shader* ourShader;
    int windowStyle;
    int windowChange;
    GLfloat* verticesVec[16];
    boost::thread m_Thread;
    boost::shared_ptr<std::vector<std::queue<AVFrame*> > > pFrameQueueVecPtr_;
};

#endif // GLVIEWER_H
