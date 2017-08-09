#ifndef FMDECODER_H
#define FMDECODER_H
//Linux...
#include "params.h"

#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/tss.hpp>
#include <boost/asio.hpp>
#include "channel.h"
#include "params.h"
#include <queue>
#include <sched.h>
#include <thread>

using namespace boost::asio;
class fmDecoder
{
public:
    fmDecoder();
    virtual ~fmDecoder();
    int Init();
    int Parse(int cur_size, uint8_t *cur_ptr);
    int Decode(AVFrame *pFrame);
    int GetPacketSize();
    void SetScreanNum(int);
    void startDecode();
    void setThreadSeq(int seq);
    void run();
    void setPtr(boost::shared_ptr<std::vector<channel> > cvPtr,
                boost::shared_ptr<std::vector<std::vector<AVFrame*> > > pFrameVecPtr,
                boost::shared_ptr<std::vector<int> >readIndex,
                boost::shared_ptr<std::vector<int> > writeIndex);
    void setQueuePtr(boost::shared_ptr<std::vector<std::queue<AVFrame*> > > pFrameQueueVecPtr)
    {
        pFrameQueueVecPtr_=pFrameQueueVecPtr;
    }
protected:

private:
    AVCodec *pCodec;
    AVCodecContext *pCodecCtx;
    AVCodecParserContext *pCodecParserCtx;
    AVPacket packet;
    AVCodecID codecId;
    struct SwsContext *img_convert_ctx16;
    struct SwsContext *img_convert_ctx9;
    struct SwsContext *img_convert_ctx4;
    struct SwsContext *img_convert_ctx1;

    struct SwsContext *convertCtx;
    AVFrame	*pFrameYUV;
    int screanNum;
    unsigned char *out_buffer;
    int threadSeq_;
    boost::thread m_Thread;
    boost::shared_ptr<std::vector<channel> > cvPtr_;
    boost::shared_ptr<std::vector<int> > readIndex_;
    boost::shared_ptr<std::vector<int> > writeIndex_;
    boost::shared_ptr<std::vector<std::vector<AVFrame*> > > pFrameVecPtr_;
    boost::shared_ptr<std::vector<std::queue<AVFrame*> > > pFrameQueueVecPtr_;

};

#endif // FMDECODER_H
