#include "manager.h"

manager::manager():channelVecPtr(boost::make_shared<std::vector<channel> >(CHANNELNUM)),
    v(CHANNELNUM),
    fv(CHANNELNUM),
//    vr(new viewer(WINDOW_WIDTH,WINDOW_HEIGHT,true)),
    readIndex(boost::make_shared<std::vector<int> >(CHANNELNUM,0)),
    writeIndex(boost::make_shared<std::vector<int> >(CHANNELNUM,0)),
    pFrameQueueVec(CHANNELNUM),
    videoPositionVec(4, std::vector<int>(CHANNELNUM,-1)),
    pFrameQueueVecPtr(boost::make_shared<std::vector<BUFFERPAIR> >(pFrameQueueVec)),
    videoPositionVecPtr(boost::make_shared<std::vector<std::vector<int> > >(videoPositionVec)),
    mutexPtr(new std::mutex)
{
    //ctor
//    mutexPtr = boost::make_shared<std::mutex> (mutexInstance);

    for(int i = 0; i<4 ; ++i)
    {
        vr[i] = NULL;
        mytimer[i] = NULL;
    }

    myPcap.setChannelVecPtr(channelVecPtr);
    myPcap.start();

//    for(std::vector<boost::shared_ptr<rtpReceiver> >::iterator pos =v.begin(); pos!=v.end(); ++pos)
//    {
//        int position = (pos)-v.begin();
//        (*pos) = boost::make_shared<rtpReceiver> ("127.0.0.1",8080+position,position);
//        (*pos)->setChannelVecPtr(channelVecPtr);
//        (*pos)->start();
//    }
    for(std::vector<boost::shared_ptr<fmDecoder> >::iterator pos =fv.begin(); pos!=fv.end(); ++pos)
    {
//        std::vector<boost::shared_ptr<fmDecoder> >::iterator pos =fv.begin()+3;
        int position = (pos)-fv.begin();
        (*pos) = boost::make_shared<fmDecoder> ();
        (*pos)->setPtr(channelVecPtr,readIndex,writeIndex);
        (*pos)->setQueuePtr(pFrameQueueVecPtr);
        (*pos)->setMutexPtr(mutexPtr);
        (*pos)->setThreadSeq(position);
        (*pos)->SetScreanNum(WINDOW_STYLE);
        (*pos)->startDecode();
    }
}

manager::~manager()
{
    //dtor
}

void manager::startViewer()
{


    // Set all the required options for GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    int count;
    GLFWmonitor** monitors = glfwGetMonitors(&count);
//    GLFWwindow* offscreen_context[count];
//    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
//    offscreen_context[0] = glfwCreateWindow(640, 480, "offline 0", NULL, NULL);
//    glfwSetWindowMonitor(offscreen_context[0], monitors[0], 0, 0, 1440, 900, 60);
//    glfwMakeContextCurrent(offscreen_context[0]);
//    glfwSwapInterval(1);
//
//    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
//    offscreen_context[1] = glfwCreateWindow(640, 480, "offline 0", NULL, NULL);
//    glfwSetWindowMonitor(offscreen_context[1], monitors[1], 0, 0, 1440, 900, 60);
//    glfwMakeContextCurrent(offscreen_context[1]);
//    glfwSwapInterval(2);


    glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
//    // set vsync. 0:off max 1000+fps 1: on max 60fps 2: on max 30fps
    //glfwSwapInterval(1);
    for(int index= 0; index<count; ++index)
    {
        const GLFWvidmode* mode = glfwGetVideoMode(monitors[index]);
        //glfwSetWindowMonitor(window, NULL, 1440*index, 0, mode->width, mode->height, mode->refreshRate);

        vr[index] = new viewer(mode->width,mode->height,index,true);
        vr[index]->setQueuePtr(pFrameQueueVecPtr,videoPositionVecPtr);
        vr[index]->setMutexPtr(mutexPtr);
        //vr[index]->setContext(offscreen_context[index]);
        vr[index]->run();
        printf("start viewer %d\n",index);
        usleep(3000000);
    }






//    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
//    GLFWwindow* offscreen_context = glfwCreateWindow(640, 480, "", NULL, NULL);
//    glfwMakeContextCurrent(offscreen_context);
//    glfwSwapInterval(1);
//    //glfwInit();
//    int count = 2;
//    //printf("count = %d\n",count);
//    //GLFWmonitor** monitors = glfwGetMonitors(&count);
//    //glfwTerminate();
//    for(int i = 0; i < count; ++i)
//    //int i = 0;
//    {
//        vr[i] = new viewer(1440,900,i,true);
//        vr[i]->setQueuePtr(pFrameQueueVecPtr,videoPositionVecPtr);
//        vr[i]->setContext(offscreen_context);
//        vr[i]->run();
//        printf("start viewer %d\n",i);
//        usleep(3000000);
//    }
}

void manager::setChannelBuffer(int index, std::string ip_)
{
    if(index <= 0)
        return;
//    std::cout<<ip_.c_str()<<std::endl;
    unsigned int ip = inet_addr(ip_.c_str());
    int m1 = (ip & 0xff);
    int m2 = ((ip >> 8) & 0xff);
    int m3 = ((ip >> 16) & 0xff);
    int m4 = ((ip >> 24) & 0xff);
    (*channelVecPtr)[index - 1].ip[0] = m1;
    (*channelVecPtr)[index - 1].ip[1] = m2;
    (*channelVecPtr)[index - 1].ip[2] = m3;
    (*channelVecPtr)[index - 1].ip[3] = m4;
    (*channelVecPtr)[index - 1].port = 8080+index-1;
    //std::cout<<(*channelVecPtr)[index - 1].ip<<std::endl;
}

void manager::setDecoderPara(int index,int splitNum)
{
    if(index < 1)
        return;
    fv[index-1]->SetScreanNum(splitNum);
    printf("index: %d finished\n",index-1);
}

void manager::setViewerPara(int index,int splitNum)
{
    if(index < 1)
        return;
    vr[index-1]->setStyle(splitNum);
}

void manager::runServer(int port)
{


}

void manager::setViewerPosition(int monitorIndex,int index, int pos)
{
    if(monitorIndex<1)
        return;
    (*videoPositionVecPtr)[monitorIndex-1][index] = pos-1;
}

void manager::startViewer(int index)
{
    int trueIndex = index-1;
    if(vr[trueIndex])
        return;
    vr[trueIndex] = new viewer(1920,1080,trueIndex,true);
    //clearQueue(trueIndex);
    vr[trueIndex]->setQueuePtr(pFrameQueueVecPtr,videoPositionVecPtr);
    vr[trueIndex]->setMutexPtr(mutexPtr);
    vr[trueIndex]->run();
    printf("start viewer %d\n",trueIndex);
}

void manager::shutdownViewer(int index)
{
    if(index < 1)
        return;
    int trueIndex = index-1;
    if(vr[trueIndex])
    {
        vr[trueIndex]->destoryWindow();
        usleep(300000);
        delete vr[trueIndex];
        vr[trueIndex] = NULL;
    }
    else
    {
        return;
    }
}


void manager::clearQueue(int index)
{
//    (*pFrameQueueVecPtr)[index].first.clear();
//std::queue<AVFrame*> emptyQueue;
//std::swap((*pFrameQueueVecPtr)[index].first,emptyQueue);
}
