#include "packetBuffer.h"
#include "frameBuffer.h"
#include <math.h>
#include <algorithm>
#include <cstdint>
using namespace webrtc;

rtpDepacketer::rtpDepacketer()
{

}

rtpDepacketer::~rtpDepacketer()
{

}

bool rtpDepacketer::ProcessStapAOrSingleNalu()
{

}

bool rtpDepacketer::ParseFuaNalu()
{

}


packetBuffer::packetBuffer(frameBuffer* assembled_frame_callback):
    data_buffer_(PACKETBUFFERLEN),
    sequence_buffer_(PACKETBUFFERLEN),
    assembled_frame_callback_(assembled_frame_callback)
{
    //ctor
    allowed_pt.insert(96);
    allowed_pt.insert(106);

    //po_ = new frameObject(shared_from_this(),1,2,3);
}

packetBuffer::~packetBuffer()
{
    //dtor
}

void packetBuffer::insertPacket(const u_char* pkt_data, const size_t len)
{
    VcmPacket temp(pkt_data,len);
    std::vector<std::unique_ptr<frameObject>> found_frames;

    uint16_t seq_num = temp.seqNum;
    size_t index = seq_num % PACKETBUFFERLEN;

    uint8_t payload_type = temp.payloadType;
    if(allowed_pt.find(payload_type)==allowed_pt.end())
    {
        std::cout<<"unknown payload type! "<<payload_type<<std::endl;
        return;
    }

    std::lock_guard<std::mutex> lock(mutex_);

    if (!first_packet_received_)
    {
        first_seq_num_ = seq_num;
        first_packet_received_ = true;
    }
    else if (AheadOf(first_seq_num_, seq_num))
    {
        first_seq_num_ = seq_num;
    }

    if (sequence_buffer_[index].used)
    {
        // Duplicate packet
        if (data_buffer_[index].seqNum == seq_num)
        {
            return;
        }

        // Packet buffer is full.
        if (sequence_buffer_[index].used)
        {
            std::cout<<"packet buffer full! \n";
            clear();
            return;
        }
    }

    sequence_buffer_[index].frame_begin = temp.frame_begin | temp.fu_a_start_;
    sequence_buffer_[index].frame_end = temp.rtp_marker | temp.fu_a_end_;
    sequence_buffer_[index].seq_num = seq_num;
    sequence_buffer_[index].continuous = false;
    sequence_buffer_[index].frame_created = false;
    sequence_buffer_[index].used = true;

    data_buffer_[index] = std::move(temp);

//    std::cout<<std::endl;
//    std::cout<<"frame_type_ "<<data_buffer_[index].frame_type_<<std::endl;
//    std::cout<<"seqNum "<<data_buffer_[index].seqNum<<std::endl;
//    std::cout<<"timestamp "<<data_buffer_[index].timestamp<<std::endl;
//    std::cout<<"rtp_type_ "<<data_buffer_[index].rtp_type_<<std::endl;
//
//    std::cout<<"frame_begin "<<sequence_buffer_[index].frame_begin<<std::endl;
//    std::cout<<"frame_end "<<sequence_buffer_[index].frame_end<<std::endl;

    found_frames = FindFrames(seq_num);
//    std::cout<<"found_frames size: "<<found_frames.size()<<std::endl;
//    std::cout<<std::endl;

    for (auto& frame : found_frames)
        assembled_frame_callback_->OnAssembledFrame(std::move(frame));
}

bool packetBuffer::GetBitstream(const frameObject& frame, uint8_t* destination)
{
    //std::cout<<"in GetBitstream \n";
    //std::lock_guard<std::mutex> lock(mutex_);
    size_t size_ = PACKETBUFFERLEN;

    size_t index = frame.first_seq_num % size_;
    size_t end = (frame.last_seq_num + 1) % size_;
    uint16_t seq_num = frame.first_seq_num;
    uint32_t timestamp = frame.timestamp_rtp_;
    uint8_t* destination_end = destination + frame.buffer_len_;

    do
    {
        // Check both seq_num and timestamp to handle the case when seq_num wraps
        // around too quickly for high packet rates.
        if (!sequence_buffer_[index].used ||
                sequence_buffer_[index].seq_num != seq_num ||
                data_buffer_[index].timestamp != timestamp)
        {
            return false;
        }

        assert(data_buffer_[index].seqNum == sequence_buffer_[index].seq_num);
        size_t length = data_buffer_[index].len_;
        if (destination + length > destination_end)
        {
            std::cout << " bitstream buffer is not large enough. \n";
            return false;
        }

        const uint8_t* source = data_buffer_[index].pkt_;
        memcpy(destination, source, length);
        destination += length;
        index = (index + 1) % size_;
        ++seq_num;
    }
    while (index != end);

    return true;
}

void packetBuffer::clear()
{
    for (size_t i = 0; i < PACKETBUFFERLEN; ++i)
    {
        sequence_buffer_[i].used = false;
    }

    first_packet_received_ = false;
    is_cleared_to_first_seq_num_ = false;
    std::cout<<"packet buffer cleared already! \n";
}


std::vector<std::unique_ptr<frameObject>> packetBuffer::FindFrames(uint16_t seq_num)
{
    std::vector<std::unique_ptr<frameObject>> found_frames;

    size_t size_ = PACKETBUFFERLEN;

    for (size_t i = 0; i < size_ && PotentialNewFrame(seq_num); ++i)
    {
        size_t index = seq_num % size_;
        sequence_buffer_[index].continuous = true;

        // If all packets of the frame is continuous, find the first packet of the
        // frame and create an RtpFrameObject.
        if (sequence_buffer_[index].frame_end)
        {
            size_t frame_size = 0;
            uint16_t start_seq_num = seq_num;

            // Find the start index by searching backward until the packet with
            // the |frame_begin| flag is set.
            int start_index = index;
            size_t tested_packets = 0;

            int64_t frame_timestamp = data_buffer_[start_index].timestamp;

            while (true)
            {
                ++tested_packets;
                frame_size += data_buffer_[start_index].len_;

                if (tested_packets == size_)
                    break;

                start_index = start_index > 0 ? start_index - 1 : size_ - 1;

                // In the case of H264 we don't have a frame_begin bit (yes,
                // |frame_begin| might be set to true but that is a lie). So instead
                // we traverese backwards as long as we have a previous packet and
                // the timestamp of that packet is the same as this one. This may cause
                // the PacketBuffer to hand out incomplete frames.
                // See: https://bugs.chromium.org/p/webrtc/issues/detail?id=7106
                if (true &&
                        (!sequence_buffer_[start_index].used ||
                         data_buffer_[start_index].timestamp != frame_timestamp))
                {
                    break;
                }

                --start_seq_num;
            }

            found_frames.emplace_back(
                new frameObject(shared_from_this(), start_seq_num, seq_num, frame_size));
        }
        ++seq_num;
    }
    //std::cout<<"packetBuffer::FindFrames with size: "<<found_frames.size()<<std::endl;;
    return found_frames;
}











































