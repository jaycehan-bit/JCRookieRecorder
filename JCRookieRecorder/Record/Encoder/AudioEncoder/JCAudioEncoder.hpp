//
//  JCAudioEncoder.hpp
//  JCRookiePlayer
//
//  Created by jaycehan on 2024/8/14.
//

#ifndef JCAudioEncoder_hpp
#define JCAudioEncoder_hpp

#include <cstddef>
#include <stdio.h>
extern "C"{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}

typedef enum : int32_t {
    JCAudioEncoderErrorSuccess = 0,
    JCAudioEncoderErrorOutputContext = 1,
    JCAudioEncoderErrorIOOpen = 2,
    JCAudioEncoderErrorCodecOpen = 3,
} JCAudioEncoderError;

class JCAudioEncoder {
    
private:
    AVCodecContext *codec_context;
    AVFormatContext *format_context;
    AVFrame *input_frame;
    AVPacket *output_packet;
public:
    /**
     * @brief 初始化编码器
     * @param bitRate 比特率，即文件码率
     * @param channels 声道数
     * @param sampleRate 采样率
     * @param bitsPerSample 位深
     * @param accFilePath 编码文件路径
     * @param codecName 编码器，M4A需要填AAC编码器名（libfdk_aac），MP3需要传入MP3编码器名（lame）。需要和accFilePath对应
     */
    JCAudioEncoderError init(int bitRate, int channels, int sampleRate, int bitsPerSample, const char *accFilePath, const char *codecName);
    
    /**
     * @brief 将传递进来的PCM数据编码并写入文件
     * @param buffer 待编码数据
     * @param size 数组长度
     */
    void encode(std::byte *buffer, int size);
    
    /**
     * @brief 销毁上下文
     */
    void destroy();
};

#endif /* JCAudioEncoder_hpp */
