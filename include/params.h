#ifndef PARAMS_H_INCLUDED
#define PARAMS_H_INCLUDED

#define CHANNELNUM 20
#define FRAME_BUFFER 3
#define VIDEO_SOURCE_WIDTH 1920
#define VIDEO_SOURCE_HEIGHT 1080
//output
#define VIDEO_WIDE 1920
#define VIDEO_HEIGHT 1080

#define WINDOW_WIDTH 1440
#define WINDOW_HEIGHT 900

#define useNpp 0

#define copyNum 4

#define PORT 12310
#define IPV4
#define showFPS 1
#define use_webrtc 1

//1: 1分屏
//4: 4分屏
//9: 9分屏

#define WINDOW_STYLE 16

#define __STDC_CONSTANT_MACROS




extern "C" {
#include <libavcodec/avcodec.h>
//#include <libavcodec/
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
#include <libavutil/opt.h>
#include <libavfilter/avfiltergraph.h>
#include <libavfilter/buffersink.h>
#include <libavfilter/buffersrc.h>
#include <libavutil/avutil.h>
#include <libavutil/imgutils.h>

#include <libavutil/avassert.h>
#include <libavutil/channel_layout.h>
#include <libavutil/opt.h>
#include <libavutil/mathematics.h>
#include <libavutil/timestamp.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
}
#include <queue>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/tss.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/optional.hpp>
#include <boost/asio/steady_timer.hpp>
//boost::mutex lock;



#endif // PARAMS_H_INCLUDED


