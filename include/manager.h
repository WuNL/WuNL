#ifndef MANAGER_H
#define MANAGER_H
#include "rtpReceiver.h"
#include "channel.h"
#include "fmDecoder.h"

#include "fmDecoder.h"
#include "glViewer.h"
#include "viewer.h"
#include "protobufServer.h"
#include <queue>

typedef std::vector<boost::shared_ptr<rtpReceiver> > rtpRecvPtrVec;
typedef std::vector<boost::shared_ptr<fmDecoder> > fmDecoderPtrVec;
typedef std::vector<boost::shared_ptr<int> > vs;
typedef boost::shared_ptr<std::vector<std::queue<AVFrame*> > > queuePtr;
typedef std::pair<std::queue<AVFrame*>,std::string> BUFFERPAIR;

/** \brief
 *  管理电视墙资源的类
 *
 */

class manager
{
public:
    manager();
    virtual ~manager();

protected:

private:

    rtpRecvPtrVec v;
    fmDecoderPtrVec fv;
    viewer vr;

    boost::shared_ptr<std::vector<channel> >  channelVecPtr;
    boost::shared_ptr<std::vector<int> > readIndex;
    boost::shared_ptr<std::vector<int> > writeIndex;
    std::vector<BUFFERPAIR> pFrameQueueVec;
    boost::shared_ptr<std::vector<BUFFERPAIR> > pFrameQueueVecPtr;

};

#endif // MANAGER_H
