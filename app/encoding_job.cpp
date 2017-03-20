#include <iostream>

#include "encoder.h"
#include "wave_file_reader.h"

#include "encoding_job.h"

namespace EncodingJob {

void encode(const std::string& wavFile,
            const std::string& mp3File)
{
    try {
        WaveFileReader wav(wavFile);
    
        Encoder enc(wav.sampleRate(), wav.numChannels());

        std::ofstream mp3(mp3File, std::ios_base::binary);
    
        Mp3Buffer_t mp3Buffer;
        PcmBuffer_t pcmBuffer;
    
        auto samplesRead = 0;
        auto bytesEncoded = 0;
        while((samplesRead = wav.readSamples(pcmBuffer)) != 0) {
            bytesEncoded = enc.encodeSamples(pcmBuffer, samplesRead, mp3Buffer);
            mp3.write(reinterpret_cast<char*>(mp3Buffer.data()), bytesEncoded);
        }

        bytesEncoded = enc.flush(mp3Buffer);
        mp3.write(reinterpret_cast<char*>(mp3Buffer.data()), bytesEncoded);
    }
    catch(const std::exception& ex) {
        std::cerr << ex.what();
    }
}

}
