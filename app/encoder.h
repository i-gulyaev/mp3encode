#pragma once

#include <vector>
#include <cstdint>


#include <lame.h>

#include "common.h"

class Encoder
{
public:
    
    Encoder(int sampleRate, int numChannels);
    ~Encoder();
    
    int encodeSamples(PcmBuffer_t& pcm, int numSamples, Mp3Buffer_t& mp3);
    int flush(Mp3Buffer_t& mp3);
    
private:
    lame_t mLameContext;
    int mNumChannels;
};
