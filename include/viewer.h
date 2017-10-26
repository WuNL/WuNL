#ifndef VIEWER_H
#define VIEWER_H
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "GLError.h"
#include "params.h"
#include "Shader.h"
#include "textRender.h"

#include "channel.h"
#include <sched.h>
#include <thread>
#include <sys/time.h>
#include <time.h>

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
};



class viewer:public baseViewer
{
public:
    viewer(int width,int height,int ind,bool autoS);
    virtual ~viewer();
    void updateImage(GLubyte* dst,int x,int y, int w,int h,void* data);
    int init();
    void run();
    void displayFun();
    void display();
    void devFun();
    void setStyle(int splitNum);

    void setQueuePtr(boost::shared_ptr<std::vector<BUFFERPAIR> > pFrameQueueVecPtr,boost::shared_ptr<std::vector<std::vector<int> > > videoPositionVecPtr)
    {
        pFrameQueueVecPtr_=pFrameQueueVecPtr;
        videoPositionVecPtr_ = videoPositionVecPtr;
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
    boost::thread m_Thread;
    boost::shared_ptr<std::vector<BUFFERPAIR> > pFrameQueueVecPtr_;
    boost::shared_ptr<std::vector<std::vector<int> > > videoPositionVecPtr_;

//    boost::mutex lock;

    GLuint VBO[16], VAO[16], EBO,pboIds[16],pboUV[16];
    GLuint vbo;
    GLfloat* verticesVec[16];
    GLfloat vertics2D[16][20];
    GLuint texture,textureUV;
    Shader* ourShader;
    int frameWidth,frameHeight;
    int splitNum_,splitNum_old;

    textRender* tr;
};

#endif // VIEWER_H
