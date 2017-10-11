#include "manager.h"

manager::manager():channelVecPtr(boost::make_shared<std::vector<channel> >(CHANNELNUM)),
v(CHANNELNUM),
fv(CHANNELNUM),
readIndex(boost::make_shared<std::vector<int> >(CHANNELNUM,0)),
writeIndex(boost::make_shared<std::vector<int> >(CHANNELNUM,0)),
pFrameQueueVec(CHANNELNUM),
pFrameQueueVecPtr(boost::make_shared<std::vector<BUFFERPAIR> >(pFrameQueueVec))
{
    //ctor
}

manager::~manager()
{
    //dtor
}
