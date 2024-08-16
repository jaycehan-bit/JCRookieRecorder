//
//  JCAudioEncoder.cpp
//  JCRookiePlayer
//
//  Created by jaycehan on 2024/8/14.
//

extern "C"{
#include <libavutil/samplefmt.h>
}
#include "JCAudioEncoder.hpp"

JCAudioEncoderError JCAudioEncoder::init(int bitRate, int channels, int sampleRate, int bitsPerSample, const char *accFilePath, const char *codecName) {
    this->format_context = avformat_alloc_context();
    int status = avformat_alloc_output_context2(&this->format_context, NULL, codecName, accFilePath);
    if (status) {
        return JCAudioEncoderErrorOutputContext;
    }
    // 打开文件通道
    status = avio_open2(&(this->format_context->pb), accFilePath, AVIO_FLAG_WRITE, NULL, NULL);
    if (status < 0) {
        return JCAudioEncoderErrorIOOpen;
    }
    AVStream *stream = avformat_new_stream(this->format_context, NULL);
    // 创建编码器
    AVCodec *codec = avcodec_find_encoder_by_name(codecName);
    this->codec_context = avcodec_alloc_context3(codec);
    this->codec_context->codec_type = AVMEDIA_TYPE_AUDIO;
    this->codec_context->bit_rate = bitRate;
    this->codec_context->sample_rate = sampleRate;
    this->codec_context->channels = channels;
    // 立体声
    this->codec_context->channel_layout = AV_CH_LAYOUT_STEREO;
    // 两个字节表示一个采样点
    this->codec_context->sample_fmt = AV_SAMPLE_FMT_S16;
    this->codec_context->profile = FF_PROFILE_AAC_MAIN;
    // 60帧
    this->codec_context->framerate = AVRational{60, 1};
    this->codec_context->frame_size = 1024;
    status = avcodec_open2(this->codec_context, codec, NULL);
    if (status != 0) {
        return JCAudioEncoderErrorCodecOpen;
    }
    
    this->input_frame = av_frame_alloc();
    this->input_frame->format = AV_SAMPLE_FMT_S16;
    this->input_frame->channel_layout = av_get_default_channel_layout(channels);
    this->input_frame->sample_rate = sampleRate;
    // 为frame分配data和linesize
    av_frame_get_buffer(this->input_frame, 0);
    
    this->output_packet = av_packet_alloc();
    
    // 写视频头文件
    status = avformat_write_header(this->format_context, NULL);
    
    return JCAudioEncoderErrorSuccess;
}

void JCAudioEncoder::encode(std::byte *buffer, int size) {
    uint8_t *data = reinterpret_cast<uint8_t *>(buffer);
    int bytes_per_sample = av_get_bytes_per_sample(AV_SAMPLE_FMT_S16);
    for (uint8_t i = 0; i < this->codec_context->channels; i++) {
        memcpy(this->input_frame->data[i], data + i * this->input_frame->nb_samples * bytes_per_sample, this->input_frame->nb_samples * bytes_per_sample);
    }
    while (!avcodec_send_frame(this->codec_context, this->input_frame)) {}
    av_interleaved_write_frame(this->format_context, this->output_packet);
}

void JCAudioEncoder::destroy() {
    avformat_free_context(this->format_context);
    avcodec_free_context(&this->codec_context);
    av_frame_free(&this->input_frame);
    av_packet_free(&this->output_packet);
}
