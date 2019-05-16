#ifndef ENCODEDIMAGE_H
#define ENCODEDIMAGE_H
#include "params.h"
#include <memory>

class EncodedImage
{
public:
    /** Default constructor */
    EncodedImage();
    /** Default destructor */
    virtual ~EncodedImage();

    EncodedImage(AVFrame* curYUVFrame):
        nv12Frame(curYUVFrame)
    {

    }

protected:

public:
    AVFrame* nv12Frame = nullptr;
    uint8_t dupCount = 0;
};

#endif // ENCODEDIMAGE_H
