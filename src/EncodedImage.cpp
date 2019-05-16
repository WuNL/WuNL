#include "EncodedImage.h"

EncodedImage::EncodedImage()
{
    //ctor
}

EncodedImage::~EncodedImage()
{
    //dtor
    if(nv12Frame != nullptr)
    {
        av_frame_free(&nv12Frame);
    }
}
