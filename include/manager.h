#ifndef MANAGER_H
#define MANAGER_H
#include "rtpReceiver.h"
#include "pcapGrapper.h"
#include "channel.h"
#include "fmDecoder.h"

#include "glViewer.h"
#include "viewer.h"
#include "timer.h"
#include <queue>
#include <mutex>
#include <sys/time.h>
#include <time.h>
typedef std::vector<boost::shared_ptr<rtpReceiver> > rtpRecvPtrVec;
typedef std::vector<boost::shared_ptr<fmDecoder> > fmDecoderPtrVec;


typedef std::vector<boost::shared_ptr<int> > vs;
typedef boost::shared_ptr<std::vector<std::queue<AVFrame*> > > queuePtr;
typedef std::pair<std::queue<AVFrame*>,std::string> BUFFERPAIR;

/** \brief
 *  管理电视墙资源的类
 *  执行者
 *
 */

class manager
{
public:
    manager();
    virtual ~manager();

    void runServer(int port = PORT);
    void startViewer();
    void startViewer(int index);
    void shutdownViewer(int index);
    void setChannelBuffer(int index, std::string ip);
    void setDecoderPara(int index,int splitNum);
    void setViewerPara(int index,int spitNum);
    void setViewerPosition(int monitorIndex,int index,int pos);
    void setFrameQueue(int index,std::string name)
    {
        if( index <1 )
            return;
        (*pFrameQueueVecPtr)[index-1].second = name;
    }
    void startTimer(int index,int seconds,std::vector<std::vector<std::string> >& pollingVec)
    {
        if(seconds<1)
        {
            return;
        }
        if(mytimer[index]==NULL)
        {
            mytimer[index] = new MyTimer(index,std::chrono::seconds(seconds),videoPositionVecPtr,vr[index],pollingVec
            );
        }
        else
        {
            delete mytimer[index];
            mytimer[index] = NULL;
            mytimer[index] = new MyTimer(index,std::chrono::seconds(seconds),videoPositionVecPtr,vr[index],pollingVec
            );
        }
    }

protected:

private:

    rtpRecvPtrVec v;
    pcapGrepper myPcap;
    fmDecoderPtrVec fv;
    viewer* vr[4];
    MyTimer* mytimer[4];


    boost::shared_ptr<std::vector<channel> >  channelVecPtr;
    boost::shared_ptr<std::vector<int> > readIndex;
    boost::shared_ptr<std::vector<int> > writeIndex;
    std::vector<BUFFERPAIR> pFrameQueueVec;
    std::vector<std::vector<int> >videoPositionVec;
    boost::shared_ptr<std::vector<BUFFERPAIR> > pFrameQueueVecPtr;
    boost::shared_ptr<std::vector<std::vector<int> > > videoPositionVecPtr;
    std::mutex* mutexPtr;

    void clearQueue(int index);

};

#endif // MANAGER_H
