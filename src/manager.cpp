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
    myPcap.start();

//    for(std::vector<boost::shared_ptr<rtpReceiver> >::iterator pos =v.begin(); pos!=v.end(); ++pos)
//    {
//        int position = (pos)-v.begin();
//        (*pos) = boost::make_shared<rtpReceiver> ("127.0.0.1",8080+position,position);
//        (*pos)->setChannelVecPtr(channelVecPtr);
//        (*pos)->start();
//    }
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
    for(int i = 0; i < count; ++i)
    {
        vr[i] = new viewer(1440,900,i,true);
        vr[i]->setQueuePtr(pFrameQueueVecPtr,videoPositionVecPtr);
        vr[i]->run();
        printf("start viewer %d\n",i);
        usleep(3000000);
    }
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
    (*videoPositionVecPtr)[monitorIndex-1][index] = pos-1;
}
