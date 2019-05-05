#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <string>
#include <iostream>
#include <queue>
#include <deque>
#include <assert.h>
#include <map>
#include "packetBuffer.h"

enum FrameDecision { kStash, kHandOff, kDrop };

class frameObject
{
public:
    frameObject(std::shared_ptr<packetBuffer> pb, uint16_t start_index, uint16_t end_index, size_t frame_size):
        buffer_len_(frame_size),
        first_seq_num(start_index),
        last_seq_num(end_index),
        pb_(pb.get())
    {
        VcmPacket* first_packet = pb->GetPacket(first_seq_num);
        assert(first_packet);
        frame_type_ = first_packet->frame_type_;
        timestamp_rtp_ = first_packet->timestamp;

        VcmPacket* last_packet = pb->GetPacket(last_seq_num);
        assert(last_packet);
        assert(last_packet->fu_a_end_ | last_packet->rtp_marker);

        prame_data_ptr_ = (uint8_t*)malloc(buffer_len_);


        if(pb->GetBitstream(*this, data()))
        {
            //std::cout<<"GetBitstream success!\n";
        }
    }
    virtual ~frameObject()
    {
//        std::cout<<"~frameObject()\n";
        if(prame_data_ptr_ != nullptr)
            free(prame_data_ptr_);
        //pb_->ReturnFrame(this);
    }
    void ReturnFrame()
    {
    if(pb_!=nullptr)
        pb_->ReturnFrame(this);
    }
    uint8_t* data()
    {
        if(prame_data_ptr_ != nullptr)
            return prame_data_ptr_;
    }

    uint16_t get_first_seq_num() const{
        return first_seq_num;
    }
    uint16_t get_last_seq_num() const{
        return last_seq_num;
    }

    uint8_t* prame_data_ptr_ = nullptr;
    uint16_t offset_ = 0;
    size_t buffer_len_ = 0;
    uint32_t timestamp_rtp_;
    size_t frame_size = 0;
    frametype frame_type_;

    uint16_t first_seq_num;
    uint16_t last_seq_num;
    uint8_t unknown_var;
private:
    packetBuffer* pb_ = nullptr;


};

class frameBuffer
{
public:
    /** Default constructor */
    frameBuffer();
    /** Default destructor */
    virtual ~frameBuffer();
    /** Copy constructor
     *  \param other Object to copy from
     */
    frameBuffer(const frameBuffer& other);
    /** Assignment operator
     *  \param other Object to assign from
     *  \return A reference to this
     */
    frameBuffer& operator=(const frameBuffer& other);

    void OnAssembledFrame(std::unique_ptr<frameObject> frame);

    std::unique_ptr<frameObject> get_available_frame()
    {
        if(available_frames_.empty())
            return nullptr;
        mutex_.lock();

//        auto frame_it = available_frames_.begin()
//        if(frame_it != available_frames_.end())
//        {
//
//            available_frames_.erase(frame_it);
//        }
        auto it = std::move(available_frames_.front());
        available_frames_.pop_front();

        //ClearTo(it.get_last_seq_num());

        mutex_.unlock();
        return std::move(it);


    }

    void ClearTo(uint16_t seq_num);

protected:

private:
    FrameDecision ManageFrameInternal(frameObject* frame);
    void RetryStashedFrames();
    FrameDecision ManageFramePidOrSeqNum(frameObject* frame);
    void UpdateLastPictureIdWithPadding(uint16_t seq_num);

private:
    static const int kMaxStashedFrames = 100;
    static const int kMaxNotYetReceivedFrames = 100;
    static const int kMaxGofSaved = 50;
    static const int kMaxPaddingAge = 100;
    std::deque<std::unique_ptr<frameObject>> stashed_frames_;

    std::deque<std::unique_ptr<frameObject>> available_frames_;
    //default shengxu
  std::map<uint16_t,
           std::pair<uint16_t, uint16_t>,
           webrtc::DescendingSeqNumComp<uint16_t>>
      last_seq_num_gop_;

      std::mutex mutex_;
      int cleared_to_seq_num_ ;
};

#endif // FRAMEBUFFER_H
