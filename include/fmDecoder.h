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
#include <mutex>
#include <sched.h>
#include <thread>

using namespace boost::asio;

typedef std::pair<std::queue<AVFrame*>,std::string> BUFFERPAIR;

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
    void runSaveFileTest();
    int testFun();
    void setPtr(boost::shared_ptr<std::vector<channel> > cvPtr,
                boost::shared_ptr<std::vector<int> >readIndex,
                boost::shared_ptr<std::vector<int> > writeIndex);
    void setQueuePtr(boost::shared_ptr<std::vector<BUFFERPAIR> > pFrameQueueVecPtr)
    {
        pFrameQueueVecPtr_=pFrameQueueVecPtr;
    }
    void setMutexPtr(std::mutex* mutexPtr)
    {
        mutexPtr_ = mutexPtr;
    }
protected:

private:

    int initFilter();
    AVCodec *pCodec;
    AVCodecContext *pCodecCtx;
    AVCodecParserContext *pCodecParserCtx;
    AVPacket packet;
    AVCodecID codecId;
    struct SwsContext *img_convert_ctx16;
    struct SwsContext *img_convert_ctx9;
    struct SwsContext *img_convert_ctx4;
    struct SwsContext *img_convert_ctx1;

    AVFilterContext *buffersink_ctx[3];
    AVFilterContext *buffersrc_ctx[3];
    AVFilterGraph *filter_graph[3];
    AVFilter *buffersrc[3];
    AVFilter *buffersink[3];
    AVFilterInOut *outputs[3];
    AVFilterInOut *inputs[3];
    AVBufferSinkParams *buffersink_params[3];


    struct SwsContext *convertCtx[3];
    struct SwsContext *convertCtx720P[4];
    struct SwsContext *convertCtx576P[4];
    AVFrame	*pFrameYUV[3];
    AVFrame *pFrameYUV1080P;
    int screanNum,screanNum_old;
    int sws_seq;
    int sws_width_,sws_height_;
    unsigned char *out_buffer;
    int threadSeq_;
    boost::thread m_Thread;
    boost::shared_ptr<std::vector<channel> > cvPtr_;
    boost::shared_ptr<std::vector<int> > readIndex_;
    boost::shared_ptr<std::vector<int> > writeIndex_;
    boost::shared_ptr<std::vector<BUFFERPAIR> > pFrameQueueVecPtr_;

    std::mutex* mutexPtr_;

};

#endif // FMDECODER_H
