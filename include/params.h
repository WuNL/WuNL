#ifndef PARAMS_H_INCLUDED
#define PARAMS_H_INCLUDED

#define CHANNELNUM 20
#define FRAME_BUFFER 40
#define VIDEO_SOURCE_WIDTH 1920
#define VIDEO_SOURCE_HEIGHT 1080
//output
#define VIDEO_WIDE 1920
#define VIDEO_HEIGHT 1080

#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080

#endif // PARAMS_H_INCLUDED

#define __STDC_CONSTANT_MACROS
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
}
