#ifndef VIDEOBUFFER_H
#define VIDEOBUFFER_H
#include <mutex>
#include "params.h"

typedef std::pair<std::queue<AVFrame*>,std::string> BUFFERPAIR;

class videoBuffer
{
public:
    videoBuffer();
    virtual ~videoBuffer();
    boost::shared_ptr<std::vector<BUFFERPAIR> > pFrameQueueVecPtr;
    boost::shared_ptr<std::vector<std::vector<int> > > videoPositionVecPtr;
    std::mutex* mutexPtr;
    void setPosition(int monitorIndex,int index, int pos)
    {
        videoPositionVec[monitorIndex][index] = pos;
    }
    int getPosition(int monitorIndex,int index)
    {
        return videoPositionVec[monitorIndex][index];
    }
    int needDelay(int frameQueueIndex);
    void push(int frameQueueIndex,AVFrame* pFrame);
//      return value:
//       1:success get frame
//       0:no frame
//       -1:something error
    int get(int frameQueueIndex,AVFrame* &pFrame);

    void setString(int frameQueueIndex,std::string s)
    {
        if(frameQueueIndex<0 || frameQueueIndex>=CHANNELNUM)
            return;
        pFrameQueueVec[frameQueueIndex].second = s;
    }

    std::string getString(int frameQueueIndex)
    {
        if(frameQueueIndex<0 || frameQueueIndex>=CHANNELNUM)
            return "";
        return pFrameQueueVec[frameQueueIndex].second;
    }
protected:

private:
    std::vector<int> readIdxVec;
    std::vector<int> writeIdxVec;
    std::vector<BUFFERPAIR> pFrameQueueVec;
    std::vector<std::vector<int> >videoPositionVec;
};


class videoBufferWithUpdateTime:public videoBuffer
{
public:

private:


};

#endif // VIDEOBUFFER_H
