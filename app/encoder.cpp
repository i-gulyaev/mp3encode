#include <stdexcept>

#include "encoder.h"


Encoder::Encoder(int sampleRate,
                 int numChannels)
    : mNumChannels(numChannels)
{
    mLameContext = lame_init();
    if (mLameContext == nullptr)
        throw std::runtime_error("Failed to initilize lame encoder");
    
    lame_set_in_samplerate(mLameContext, sampleRate);
    lame_set_out_samplerate(mLameContext, sampleRate);
    lame_set_num_channels(mLameContext, numChannels);

    constexpr int goodQuality = 2;
    lame_set_quality(mLameContext, goodQuality);

    lame_set_VBR(mLameContext, vbr_default);
    
    auto result = lame_init_params(mLameContext);
    if (result < 0) {
        lame_close(mLameContext);
        throw std::runtime_error("failed to initialize lame encoder parameters");
    }
}

int Encoder::encodeSamples(PcmBuffer_t& pcm, int numSamples, Mp3Buffer_t& mp3)
{
    auto result = 0;
    if (mNumChannels == 1) {
        result = lame_encode_buffer(mLameContext,
                                    reinterpret_cast<int16_t*>(pcm.data()),
                                    nullptr, numSamples, mp3.data(), mp3.size());
    }
    else {
        // assume 2 channels
        result =  lame_encode_buffer_interleaved(mLameContext,
                                                 reinterpret_cast<int16_t*>(pcm.data()),
                                                 numSamples / 2, mp3.data(), mp3.size());
    }

    if (result < 0)
        throw std::runtime_error("encode buffer failed");
    return result;
}

int Encoder::flush(Mp3Buffer_t& mp3)
{
    return lame_encode_flush(mLameContext, mp3.data(), mp3.size());
}

Encoder::~Encoder()
{
    lame_close(mLameContext);
}


