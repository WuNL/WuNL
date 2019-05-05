#include "frameBuffer.h"
using namespace webrtc;

frameBuffer::frameBuffer()
{
    //ctor
}

frameBuffer::~frameBuffer()
{
    //dtor

}

frameBuffer::frameBuffer(const frameBuffer& other)
{
    //copy ctor
}

frameBuffer& frameBuffer::operator=(const frameBuffer& rhs)
{
    if (this == &rhs) return *this; // handle self assignment
    //assignment operator
    return *this;
}

void frameBuffer::OnAssembledFrame(std::unique_ptr<frameObject>frame)
{
//    std::cout<<"frameBuffer OnAssembledFrame !\n";
    std::lock_guard<std::mutex> lock(mutex_);

    FrameDecision decision = ManageFrameInternal(frame.get());

    switch (decision)
    {
    case kStash:
        if (stashed_frames_.size() > kMaxStashedFrames)
        {
            std::cout<<"stashed_frames_ full !\n";
            auto dropedFrame = std::move(stashed_frames_.back());
            dropedFrame.get()->ReturnFrame();
            stashed_frames_.pop_back();
        }

        stashed_frames_.push_front(std::move(frame));
        break;
    case kHandOff:

        if (available_frames_.size() > kMaxStashedFrames)
            available_frames_.pop_back();
        available_frames_.push_back(std::move(frame));

        RetryStashedFrames();
        break;
    case kDrop:
        break;
    }

    //std::cout<<"stashed_frames_ size "<<stashed_frames_.size()<<std::endl;
    //std::cout<<"available_frames_ size "<<available_frames_.size()<<std::endl;
}

FrameDecision frameBuffer::ManageFrameInternal(frameObject* frame)
{
    ManageFramePidOrSeqNum(frame);
}

void frameBuffer::RetryStashedFrames()
{
    bool complete_frame = false;
    do
    {
        complete_frame = false;
        for (auto frame_it = stashed_frames_.begin();
                frame_it != stashed_frames_.end();)
        {
            if(frame_it->get() == nullptr)
            {
                continue;
            }
            FrameDecision decision = ManageFrameInternal(frame_it->get());

            switch (decision)
            {
            case kStash:
                ++frame_it;
                break;
            case kHandOff:

                complete_frame = true;
                if (available_frames_.size() > kMaxStashedFrames)
                    available_frames_.pop_back();
                available_frames_.push_back(std::move(*frame_it));
                if((*frame_it).get()!=nullptr)
                    (*frame_it).get()->ReturnFrame();
                frame_it = stashed_frames_.erase(frame_it);

                break;
            case kDrop:
                std::cout<<"------------kDrop-------------- "<<std::endl;

                if((*frame_it).get()!=nullptr)
                    (*frame_it).get()->ReturnFrame();
                frame_it = stashed_frames_.erase(frame_it);

                break;
            }
        }
    }
    while (complete_frame);
}

FrameDecision frameBuffer::ManageFramePidOrSeqNum(frameObject* frame)
{
    bool is_keyframe = (frame->frame_type_ == frametype::IDR)
                       | (frame->frame_type_ == frametype::SPS)
                       | (frame->frame_type_ == frametype::PPS);
    if (is_keyframe)
    {
        last_seq_num_gop_.insert(std::make_pair(
                                     frame->get_last_seq_num(),
                                     std::make_pair(frame->get_last_seq_num(), frame->get_last_seq_num())));
//        std::cout<<"last_seq_num_gop_ insert  "<<" "<<frame->get_first_seq_num()<<" "<<frame->get_last_seq_num()<<" "<<frame->get_last_seq_num()<<std::endl;
    }

    // We have received a frame but not yet a keyframe, stash this frame.
    if (last_seq_num_gop_.empty())
        return kStash;

    // Clean up info for old keyframes but make sure to keep info
    // for the last keyframe.
    auto clean_to = last_seq_num_gop_.lower_bound(frame->get_last_seq_num() + 100);
    for (auto it = clean_to;
            it != last_seq_num_gop_.end() && last_seq_num_gop_.size() > 1 ;)
    {
//        std::cout<<frame->get_last_seq_num()<<"first clean gop (first (first second))  "<< it->first <<" "<<it->second.first<<" "<<it->second.second<<" gop size "<<last_seq_num_gop_.size()<<"\n";
        it = last_seq_num_gop_.erase(it);
    }
    clean_to = last_seq_num_gop_.lower_bound(frame->get_last_seq_num() - 100);
    for (auto it = last_seq_num_gop_.begin();
            it != clean_to && last_seq_num_gop_.size() > 1 ;)
    {
//        std::cout<<frame->get_last_seq_num()<<"second clean gop (first (first second))  "<< it->first <<" "<<it->second.first<<" "<<it->second.second<<" gop size "<<last_seq_num_gop_.size()<<"\n";
        it = last_seq_num_gop_.erase(it);
    }


    // Find the last sequence number of the last frame for the keyframe
    // that this frame indirectly references.
    auto seq_num_it = last_seq_num_gop_.upper_bound(frame->get_last_seq_num());
    if (seq_num_it == last_seq_num_gop_.begin())
    {
        std::cout << "Generic frame with packet range ["
                  << frame->get_first_seq_num() << ", "
                  << frame->get_last_seq_num()
                  << "] has no GoP, dropping frame.\n";
        std::cout<<"last gop is "<< seq_num_it->first<<std::endl;
        return kDrop;
    }
    seq_num_it--;

    // Make sure the packet sequence numbers are continuous, otherwise stash
    // this frame.
    uint16_t last_picture_id_gop = seq_num_it->second.first;
    uint16_t last_picture_id_with_padding_gop = seq_num_it->second.second;

//    std::cout<<"last_picture_id_gop "<<last_picture_id_gop<<std::endl;
//    std::cout<<"last_picture_id_with_padding_gop "<<last_picture_id_with_padding_gop<<std::endl;

    if (frame->frame_type_ == frametype::OTHER)
    {
        uint16_t prev_seq_num = frame->get_first_seq_num() - 1;

        if (prev_seq_num != last_picture_id_with_padding_gop)
        {

//            std::cout<<"------------kStash-------------- "<<" "<<frame->get_first_seq_num()<<" "<<frame->get_last_seq_num()<<" "<<last_picture_id_gop<<std::endl;
            return kStash;
        }

    }

    assert(webrtc::AheadOrAt(frame->get_last_seq_num(), seq_num_it->first));

    // Since keyframes can cause reordering we can't simply assign the
    // picture id according to some incrementing counter.
    uint16_t picture_id = frame->get_last_seq_num();

    if (AheadOf<uint16_t>(picture_id, last_picture_id_gop))
    {
        seq_num_it->second.first = picture_id;
        seq_num_it->second.second = picture_id;
    }

    UpdateLastPictureIdWithPadding(frame->get_last_seq_num());

    return kHandOff;
}

void frameBuffer::UpdateLastPictureIdWithPadding(uint16_t seq_num)
{
//    std::cout<<"UpdateLastPictureIdWithPadding erase gop! \n";
    auto gop_seq_num_it = last_seq_num_gop_.upper_bound(seq_num);

    // If this padding packet "belongs" to a group of pictures that we don't track
    // anymore, do nothing.
    if (gop_seq_num_it == last_seq_num_gop_.begin())
        return;
    --gop_seq_num_it;


    // In the case where the stream has been continuous without any new keyframes
    // for a while there is a risk that new frames will appear to be older than
    // the keyframe they belong to due to wrapping sequence number. In order
    // to prevent this we advance the picture id of the keyframe every so often.
    if (ForwardDiff(gop_seq_num_it->first, seq_num) > 3000)
    {
//        std::cout<<"UpdateLastPictureIdWithPadding erase gop! "<< gop_seq_num_it->first<<" "<<seq_num<<"\n";
        assert(1ul == last_seq_num_gop_.size());
        last_seq_num_gop_[seq_num] = gop_seq_num_it->second;
        last_seq_num_gop_.erase(gop_seq_num_it);
    }
}


void frameBuffer::ClearTo(uint16_t seq_num)
{
    std::lock_guard<std::mutex> lock(mutex_);
    cleared_to_seq_num_ = seq_num;

    auto it = stashed_frames_.begin();
    while (it != stashed_frames_.end())
    {
        if (AheadOf<uint16_t>(cleared_to_seq_num_, (*it)->get_first_seq_num()))
        {
            it = stashed_frames_.erase(it);
        }
        else
        {
            ++it;
        }
    }
}









