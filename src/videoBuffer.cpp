#include "videoBuffer.h"

videoBuffer::videoBuffer():
    readIdxVec(CHANNELNUM),
    writeIdxVec(CHANNELNUM),
    pFrameQueueVec(CHANNELNUM),
    videoPositionVec(4, std::vector<int>(CHANNELNUM,-1)),
    pFrameQueueVecPtr(boost::make_shared<std::vector<BUFFERPAIR> >(pFrameQueueVec)),
    videoPositionVecPtr(boost::make_shared<std::vector<std::vector<int> > >(videoPositionVec)),
    audioNumber(CHANNELNUM),
    t_lastAudio(CHANNELNUM),
    t_lastVideo(CHANNELNUM)
{
    //ctor
    for(int i =0; i < CHANNELNUM; ++i)
    {
        mutexPtr[i] = new std::mutex;
    }
}

videoBuffer::~videoBuffer()
{
    //dtor
    delete mutexPtr;
}

int videoBuffer::needDelay(int frameQueueIndex)
{
    if(frameQueueIndex<0 || frameQueueIndex>=CHANNELNUM)
        return -1;
    if(pFrameQueueVec[frameQueueIndex].first.size()<5)
        return 1;
    return 0;
}

void videoBuffer::push(int frameQueueIndex,AVFrame* pFrame)
{
    if(frameQueueIndex<0 || frameQueueIndex>=CHANNELNUM)
    {
        av_frame_free(&pFrame);
        return;
    }
    if(pFrameQueueVec[frameQueueIndex].first.size()<=FRAME_BUFFER)
    {
        pFrameQueueVec[frameQueueIndex].first.push(pFrame);
    }
    else
    {
        mutexPtr[frameQueueIndex]->lock();
        AVFrame* tmp = pFrameQueueVec[frameQueueIndex].first.front();
        pFrameQueueVec[frameQueueIndex].first.pop();
        av_frame_free(&tmp);

        pFrameQueueVec[frameQueueIndex].first.push(pFrame);
        mutexPtr[frameQueueIndex]->unlock();
    }
}

int videoBuffer::get(int frameQueueIndex,AVFrame* &pFrame)
{
    if(frameQueueIndex<0 || frameQueueIndex>=CHANNELNUM)
        return -1;
    if(needDelay(frameQueueIndex))
        return 0;
    mutexPtr[frameQueueIndex]->lock();
    pFrame=pFrameQueueVec[frameQueueIndex].first.front();
    pFrameQueueVec[frameQueueIndex].first.pop();
    mutexPtr[frameQueueIndex]->unlock();
    return 1;
}
