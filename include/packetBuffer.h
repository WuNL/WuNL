#ifndef PACKETBUFFER_H
#define PACKETBUFFER_H

#include <iostream>
#include <vector>
#include <memory>
#include <set>
#include <mutex>
#include <cstring>
#include <cstddef>
#include "helper.h"
//#include "frameBuffer.h"

const uint16_t PACKETBUFFERLEN = 2048;

namespace
{
uint32_t reversebytes_uint32t(uint32_t value)
{
    return (value & 0x000000FFU) << 24 | (value & 0x0000FF00U) << 8 |
           (value & 0x00FF0000U) >> 8 | (value & 0xFF000000U) >> 24;
}

} // end namespace

class frameObject;
class frameBuffer;

enum rtptype
{
    FU_A = 0,
    STAP_A = 1,
    SINGLE_NALU = 2,
};
enum frametype
{
    IDR = 0,
    SPS = 1,
    PPS = 2,
    OTHER = 3,
};

struct continuityInfo
{
    uint16_t seq_num;
    bool frame_begin = false;
    bool frame_end = false;
    bool used = false;
    bool continuous = false;
    bool frame_created = false;
};

class VcmPacket
{
public:

    VcmPacket(const u_char* pkt_data, const size_t len):len_(0), ori_pkt_(pkt_data)
    {
        ParsePacketH264(pkt_data,len);
    }
    VcmPacket():pkt_(nullptr), len_(0) {};
    ~VcmPacket()
    {
//        if(pkt_ != nullptr)
//            free(pkt_);
//        pkt_ = nullptr;
    }

    uint8_t* pkt_ = nullptr;
    size_t len_;
    uint16_t seqNum;
    uint8_t payloadType;
    uint32_t timestamp;
    bool rtp_marker = false;
    bool frame_begin = false;

    rtptype rtp_type_;
    frametype frame_type_;
    bool fu_a_start_ = false;
    bool fu_a_end_ = false;

private:
    const u_char* ori_pkt_;
    void ParsePacketH264(const u_char* pkt_data, const size_t len)
    {
        payloadType = ori_pkt_[43] & 0x7f;
        rtp_marker = (ori_pkt_[43] & 0x80) == 0x80;
//        std::cout<< "ori_pkt_[43] "<<std::hex<<(unsigned int) (unsigned char)ori_pkt_[43]<<std::endl;
//        std::cout<< "payloadType "<<payloadType<<" rtp_marker "<<rtp_marker<<std::endl;
        unsigned char seq[2];
        seq[0] = ori_pkt_[45];
        seq[1] = ori_pkt_[44];
        uint16_t *seqNum_t = (uint16_t *)&seq;
        seqNum = *seqNum_t;

        uint32_t* ts_p = (uint32_t*)&ori_pkt_[46];
        timestamp = reversebytes_uint32t(*ts_p);

        uint8_t type = pkt_data[54] & 0x1f;
        switch(type)
        {
        case 28:
            rtp_type_ = FU_A;
            break;
        case 24:
            std::cout<<"unsupport now!\n";
            rtp_type_ = STAP_A;
            break;
        case 8:
            rtp_type_ = SINGLE_NALU;
            frame_type_ = PPS;
            rtp_marker = true;
            frame_begin = true;
            break;
        case 7:
            rtp_type_ = SINGLE_NALU;
            frame_type_ = SPS;
            frame_begin = true;
            rtp_marker = true;
            break;
        case 5:
            rtp_type_ = SINGLE_NALU;
            frame_type_ = IDR;
            frame_begin = true;
            break;

        default:
            rtp_type_ = SINGLE_NALU;
            frame_type_ = OTHER;
            frame_begin = true;
            break;
        }

        char naluHeader[4] = {0, 0, 0, 1};
        if(rtp_type_ == SINGLE_NALU)
        {
            len_ = 4+len-54;
            pkt_ = (uint8_t*)malloc(len_);

            memcpy(pkt_, &naluHeader, 4);
            memcpy(pkt_+4, &ori_pkt_[54], len-54);
        }
        else if(rtp_type_ == FU_A)
        {
            fu_a_start_ = (ori_pkt_[55] & 0x80)==0x80 ? true:false;
            fu_a_end_ = (ori_pkt_[55] & 0x40)==0x40 ? true:false;

            uint8_t ft = ori_pkt_[55] & 0x1f ;
            if(ft == 5)
                frame_type_ = IDR;
            else
                frame_type_ = OTHER;

            if(fu_a_start_)
            {
                unsigned char new_fu_header = (ori_pkt_[54] & 0xe0) | (ori_pkt_[55] & 0x1f);

                len_ = 4 + 1 + len - 54 -2;
                pkt_ = (uint8_t*)malloc(len_);

                memcpy(pkt_, &naluHeader, 4);
                memcpy(pkt_+4, &new_fu_header, 1);
                memcpy(pkt_+4+1, &ori_pkt_[56], len-54-2);
            }
            else
            {
                len_ = len - 54 -2;
                pkt_ = (uint8_t*)malloc(len_);
                memcpy(pkt_, &ori_pkt_[56], len-54-2);
            }
        }
    }

};

// depacket the rtp header;
// get raw video data to data_buffer_
// get rtp packet info to sequence_buffer_
class rtpDepacketer
{
public:
    rtpDepacketer();
    virtual ~rtpDepacketer();

    bool ProcessStapAOrSingleNalu();

    bool ParseFuaNalu();
};

class packetBuffer :public std::enable_shared_from_this<packetBuffer>
{
public:
    packetBuffer(frameBuffer* assembled_frame_callback);
    virtual ~packetBuffer();

    void insertPacket(const u_char* pkt_data, const size_t len);

    void ClearTo(uint16_t seq_num);

    bool GetBitstream(const frameObject& frame, uint8_t* destination);

    void ReturnFrame(frameObject* frame);

    VcmPacket* GetPacket(uint16_t seq_num)
    {
        size_t index = seq_num % PACKETBUFFERLEN;
        if (!sequence_buffer_[index].used ||
          seq_num != sequence_buffer_[index].seq_num)
        {
            return nullptr;
        }
        return &data_buffer_[index];
    }

private:
    void clear();
protected:

private:
    std::vector<std::unique_ptr<frameObject>> FindFrames(uint16_t seq_num);

    bool PotentialNewFrame(uint16_t seq_num) const
    {
        size_t index = seq_num % PACKETBUFFERLEN;
        int prev_index = index > 0 ? index - 1 : PACKETBUFFERLEN - 1;

        if (!sequence_buffer_[index].used)
            return false;
        if (sequence_buffer_[index].seq_num != seq_num)
            return false;
        if (sequence_buffer_[index].frame_created)
            return false;
        if (sequence_buffer_[index].frame_begin)
            return true;
        if (!sequence_buffer_[prev_index].used)
            return false;
        if (sequence_buffer_[prev_index].frame_created)
            return false;
        if (sequence_buffer_[prev_index].seq_num !=
                static_cast<uint16_t>(sequence_buffer_[index].seq_num - 1))
        {
//            std::cout<<"seq_num problem  "<<sequence_buffer_[prev_index].seq_num<<"   "<<static_cast<uint16_t>(sequence_buffer_[index].seq_num - 1)<<"\n";
            return false;
        }
        if (data_buffer_[prev_index].timestamp != data_buffer_[index].timestamp)
            return false;
        if (sequence_buffer_[prev_index].continuous)
            return true;

        return false;
    }

private:

    rtpDepacketer rtp_depacketer;

    std::vector<VcmPacket> data_buffer_;
    std::vector<continuityInfo> sequence_buffer_;

    std::set<uint8_t> allowed_pt;

    std::mutex mutex_;

    frameBuffer* assembled_frame_callback_;

private:
    bool first_packet_received_ = false;
    bool is_cleared_to_first_seq_num_ = false;
    uint16_t first_seq_num_ = 0;
    webrtc::SeqNumUnwrapper<uint16_t> rtp_seq_num_unwrapper_;

};

#endif // PACKETBUFFER_H
