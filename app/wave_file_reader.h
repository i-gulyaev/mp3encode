#pragma once

#include <string>
#include <fstream>
#include <cstdint>
#include <vector>


#include "common.h"

class WaveFileReader
{
public:
    explicit WaveFileReader(const std::string& filename);
    ~WaveFileReader() = default;
    int sampleRate() const ;
    int numChannels() const;
    int readSamples(PcmBuffer_t& buffer);
    
private:
    struct RIFFHeaderChunk {
        uint8_t id[4];
        uint32_t size;
        uint8_t format[4];
    };

    struct FormatChunk {
        uint8_t id[4];
        uint32_t size;
        uint16_t type;
        uint16_t numChannels;
        uint32_t sampleRate;
        uint32_t byteRate;
        uint16_t blockAlign;
        uint16_t bitsPerSample;
    };

    struct DataChunk {
        uint8_t id[4];
        uint32_t size;
    };


private:
    RIFFHeaderChunk mRiff;
    FormatChunk mFormat;
    DataChunk mData;

    std::ifstream mFile;

    void dumpHeader();
};
