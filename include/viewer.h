#ifndef VIEWER_H
#define VIEWER_H
#include <iostream>
#include <GL/glew.h>
#include <GL/glxew.h>
#include <GLFW/glfw3.h>
#include "GLError.h"
#include "params.h"
#include "Shader.h"
#include "textRender.h"
#include <mutex>
#include "channel.h"
#include <sched.h>
#include <thread>
#include <sys/time.h>
#include <time.h>
#include <math.h>

typedef std::pair<std::queue<AVFrame*>,std::string> BUFFERPAIR;

class baseViewer
{
public:
    virtual ~baseViewer() = default;
protected:
    int w;
    int h;
    int index;
    bool autoSwitch;
    bool active;
};



class viewer:public baseViewer
{
public:
    viewer(int width,int height,int ind,bool autoS);
    virtual ~viewer();
    void destoryWindow()
    {
//        if(window)
        {
            printf("destroying window\n");
            active = false;
            m_Thread.interrupt();
            m_Thread.join();
            glfwDestroyWindow(window);
            //glfwHideWindow(window);
        }
    }
    void updateImage(GLubyte* dst,int x,int y, int w,int h,void* data);
    int init();
    void run();
    void displayFun();
    void display();
    void devFun();
    void setStyle(int splitNum);
    void setWindow(GLFWwindow* wd)
    {
        window=wd;
    }
    GLFWwindow* getContext()
    {
        return window;
    }
    void setContext(GLFWwindow* context_)
    {
        context = context_;
    }

    void setQueuePtr(boost::shared_ptr<std::vector<BUFFERPAIR> > pFrameQueueVecPtr,boost::shared_ptr<std::vector<std::vector<int> > > videoPositionVecPtr)
    {
        pFrameQueueVecPtr_=pFrameQueueVecPtr;
        videoPositionVecPtr_ = videoPositionVecPtr;
    }
    void setMutexPtr(std::mutex* mutexPtr)
    {
        mutexPtr_ = mutexPtr;
    }
    void setTextStyle(int fontsize, int location, int color,bool showFps)
    {
        curSize = fontsize;
        curLocation = location;
        curColor = color;
        showfps = showFps;

        fontSize = fontsize*0.2f+0.2f;
        switch(location)
        {
            //left top
            case 0:
            {
                leftOffset = 35.0f;
                topOffset = -35.0f;
                break;
            }
            // left bottom
            case 1:
            {
                leftOffset = 35.0f;
                topOffset = -(w_height/sqrt(splitNum_))+35.0f;
                break;
            }

        }
    }
    void ifShowLastFrame(bool marker)
    {

    }
protected:

private:
    void setVertices(int splitNum, int style);
    void setStyleInter();
    /** \brief 在屏幕上绘制文字
     * 地点：左上角
     * 字体，大小，颜色：暂时写死
     * 要点：根据分屏数来确定绘制位置，自动调整
     * \return void
     *
     */
    void renderTexts(int splintNum,float fps = 0);

    GLFWwindow* window;
    GLFWwindow* context;
    boost::thread m_Thread;
    boost::shared_ptr<std::vector<BUFFERPAIR> > pFrameQueueVecPtr_;
    boost::shared_ptr<std::vector<std::vector<int> > > videoPositionVecPtr_;
    std::mutex* mutexPtr_;

//    boost::mutex lock;

    GLuint VBO[16], VAO[16], EBO,pboIds[16],pboUV[16];
    GLuint vbo;
    GLfloat* verticesVec[16];
    GLfloat vertics2D[16][20];
    GLuint texture,textureUV;
    Shader* ourShader;
    int frameWidth,frameHeight;
    int splitNum_,splitNum_old;

    int w_width,w_height;

    textRender* tr;
    int curSize,curLocation,curColor;
    float leftOffset,topOffset;
    float fontSize;
    bool showfps;
    std::vector<glm::vec3> colorVec;
};

#endif // VIEWER_H
