#include <iostream>
#include "protobufServer.h"
/*
#include "rtpReceiver.h"
#include "channel.h"
#include "fmDecoder.h"
#include <boost/bind.hpp>
#include <boost/smart_ptr.hpp>
#include "fmDecoder.h"
#include "glViewer.h"
#include "viewer.h"
#include "protobufServer.h"
#include <queue>
*/
/*
typedef std::vector<boost::shared_ptr<rtpReceiver> > rtpRecvPtrVec;
typedef std::vector<boost::shared_ptr<fmDecoder> > fmDecoderPtrVec;
typedef std::vector<boost::shared_ptr<int> > vs;
typedef boost::shared_ptr<std::vector<std::queue<AVFrame*> > > queuePtr;
*/

int main()
{
    pthread_t thread = pthread_self();
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    thread = pthread_self();
    CPU_SET(19, &cpuset);
    int rc = pthread_setaffinity_np(thread, sizeof(cpu_set_t), &cpuset);
    if (rc != 0)
        std::cout << "Error calling pthread_setaffinity_np !!! ";


    try
    {
        //定义io_service对象
        io_service ios;
        //定义服务端endpoint对象(协议和监听端口)
        ip::tcp::endpoint serverep(ip::tcp::v4(),PORT);

        //启动异步服务
        protobufServer server(ios, serverep);
//        server.setPtr(pFrameQueueVecPtr,vr,fv);
        //等待异步完成
        ios.run();
    }
    catch (std::exception& e)
    {
        std::cout<<e.what()<<std::endl;
    }



    /*
    boost::shared_ptr<std::vector<channel> >  channelVecPtr = boost::make_shared<std::vector<channel> >(CHANNELNUM);
    rtpRecvPtrVec v(CHANNELNUM);
    fmDecoderPtrVec fv(CHANNELNUM);
    boost::shared_ptr<std::vector<int> > readIndex = boost::make_shared<std::vector<int> >(CHANNELNUM,0);
    boost::shared_ptr<std::vector<int> > writeIndex = boost::make_shared<std::vector<int> >(CHANNELNUM,0);
    std::vector<std::vector<AVFrame*> > pFrameVec(CHANNELNUM,std::vector<AVFrame*>(FRAME_BUFFER, av_frame_alloc()));
    std::vector<std::queue<AVFrame*> > pFrameQueueVec(CHANNELNUM);
    boost::shared_ptr<std::vector<std::queue<AVFrame*> > > pFrameQueueVecPtr = boost::make_shared<std::vector<std::queue<AVFrame*> > >(pFrameQueueVec);
    boost::shared_ptr<std::vector<std::vector<AVFrame*> > > pFrameVecPtr = boost::make_shared<std::vector<std::vector<AVFrame*> > >(pFrameVec);
    printf("%d\n",pFrameVecPtr->size());

    std::pair<queuePtr,std::string> avFrameBufferPair(pFrameQueueVecPtr,"");



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
        (*pos)->setPtr(channelVecPtr,pFrameVecPtr,readIndex,writeIndex);
        (*pos)->setQueuePtr(pFrameQueueVecPtr);
        (*pos)->setThreadSeq(position);
        (*pos)->SetScreanNum(WINDOW_STYLE);
        (*pos)->startDecode();
    }


    viewer vr;
    vr.setQueuePtr(pFrameQueueVecPtr);
    vr.run();



    while(1)
    {
        int style = 0;
        std::cin>>style;
        if(style==2)
            style = 16;
        vr.setStyle(style);

        for(std::vector<boost::shared_ptr<fmDecoder> >::iterator pos =fv.begin(); pos!=fv.end()-4; ++pos)
        {
    //            std::vector<boost::shared_ptr<fmDecoder> >::iterator pos =fv.begin()+3;
            (*pos)->SetScreanNum(style);

        }

    }
      */
//    try
//    {
//        //定义io_service对象
//        io_service ios;
//        //定义服务端endpoint对象(协议和监听端口)
//
//        ip::tcp::endpoint serverep(ip::tcp::v4(),PORT);
//
//
//        //启动异步服务
//        protobufServer server(ios, serverep);
//        //等待异步完成
//        ios.run();
//    }
//    catch (std::exception& e)
//    {
//        std::cout<<e.what()<<std::endl;
//    }

    return 0;
}

