#ifndef TIMER_H
#define TIMER_H
#include <params.h>
#include "viewer.h"
#include "videoBuffer.h"
#include "timer.h"
#include <iostream>


using namespace boost::asio;

class MyTimer
{
    using Clock = std::chrono::steady_clock;

public:
    MyTimer(int index,Clock::duration duration,boost::shared_ptr<videoBuffer> videoBufferPtr,viewer* vr,std::vector<std::vector<std::string> >& pollingVec)
        : _work(_ios)
        , _timer(_ios)
        , _thread([this]
    {
        _ios.run();
    })
    , marker(false)
    , _duration(duration)
    , _videoBufferPtr(videoBufferPtr)
    , _vr(vr)
    , _pollingVec(pollingVec)
    , counter(1)
    , _index(index)
    {
        _ios.post([this, duration] { start(duration); });
    }

    ~MyTimer()
    {
        printf("~mytimer\n");
        _ios.post([this] { stop(); });
        _thread.join();
    }

    void setMarker(bool bl)
    {
        marker = bl;
    }

private:

private:
    template <class T>
    void convertFromString(T &value, const std::string &s)
    {
        std::stringstream ss(s);
        ss >> value;
    }
    void timerCallBackv1(const boost::system::error_code& e,boost::asio::steady_timer* t)
    {
//        for(int i = 0; i<_pollingVec.size();++i)
//        {
//            printf("_pollingVec[%d]: ",i);
//            for(int j = 0; j<_pollingVec[i].size();++j)
//            {
//                std::cout<<_pollingVec[i][j]<<"\t";
//            }
//            printf("\n");
//
//        }
        for(int i = 0; i<_pollingVec.size(); ++i)
        {
//            std::cout<<_pollingVec[i][counter%_pollingVec[i].size()]<<"\t";
            int tid = 0;
            convertFromString(tid,_pollingVec[i][counter%_pollingVec[i].size()]);
            if(_index<1)
                return;
            _videoBufferPtr->setPosition(_index-1,i,tid-1);
        }
//        printf("\n");
        counter++;

        // NOTE: Be careful here as this is run from inside
        //       the thread.
        if (!_work)
        {
            // Already stopped.
            std::cout << "Stopped" << std::endl;
            return;
        }
//        std::cout << "Timer fired" << std::endl;
        t->expires_from_now(_duration);

        t->async_wait(boost::bind(&MyTimer::timerCallBackv1,this,boost::asio::placeholders::error,t));
    }

    void start(Clock::duration duration)
    {
        _timer.expires_from_now(duration);

        _timer.async_wait(boost::bind(&MyTimer::timerCallBackv1,this,boost::asio::placeholders::error,&_timer));
    }

    void stop()
    {
        _work.reset();
        _timer.cancel();
    }

private:
    io_service _ios;
    boost::optional<io_service::work> _work;
    boost::asio::steady_timer _timer;
    std::thread _thread;
    bool marker;
    Clock::duration _duration;

    boost::shared_ptr<videoBuffer> _videoBufferPtr;

    viewer* _vr;
    std::vector<std::vector<std::string> > _pollingVec;
    int counter;
    int _index;
};


#endif // TIMER_H
