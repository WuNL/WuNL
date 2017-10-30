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
    videoPositionVecPtr(boost::make_shared<std::vector<std::vector<int> > >(videoPositionVec))
{
    //ctor

    myPcap.setChannelVecPtr(channelVecPtr);

    for(std::vector<boost::shared_ptr<rtpReceiver> >::iterator pos =v.begin(); pos!=v.end(); ++pos)
    {
        int position = (pos)-v.begin();
        (*pos) = boost::make_shared<rtpReceiver> ("127.0.0.1",8080+position,position);
        (*pos)->setChannelVecPtr(channelVecPtr);
        (*pos)->start();
    }
    for(std::vector<boost::shared_ptr<fmDecoder> >::iterator pos =fv.begin(); pos!=fv.end()-4; ++pos)
    {
//        std::vector<boost::shared_ptr<fmDecoder> >::iterator pos =fv.begin()+3;
        int position = (pos)-fv.begin();
        (*pos) = boost::make_shared<fmDecoder> ();
        (*pos)->setPtr(channelVecPtr,readIndex,writeIndex);
        (*pos)->setQueuePtr(pFrameQueueVecPtr);
        (*pos)->setThreadSeq(position);
        (*pos)->SetScreanNum(WINDOW_STYLE);
        (*pos)->startDecode();
    }
//    vr->setQueuePtr(pFrameQueueVecPtr);
//    vr->run();

}

manager::~manager()
{
    //dtor
}

void manager::startViewer()
{
    glfwInit();
    int count;
    printf("count = %d\n",count);
    GLFWmonitor** monitors = glfwGetMonitors(&count);
    for(int i = 0; i < 2; ++i)
    {
        vr[i] = new viewer(1440,900,i,true);
        vr[i]->setQueuePtr(pFrameQueueVecPtr,videoPositionVecPtr);
        vr[i]->run();
        usleep(3000);
    }
}

void manager::setDecoderPara(int index,int splitNum)
{
    fv[index-1]->SetScreanNum(splitNum);
    printf("index: %d finished\n",index-1);
}

void manager::setViewerPara(int index,int splitNum)
{
    vr[index-1]->setStyle(splitNum);
}

void manager::runServer(int port)
{


}

void manager::setViewerPosition(int monitorIndex,int index, int pos)
{
    (*videoPositionVecPtr)[monitorIndex-1][index] = pos-1;
}
