#include <stdexcept>

#include <iostream>

#include "wave_file_reader.h"


namespace
{
    
inline int32_t byteArrayToInt32HL(uint8_t bytes[4])
{
    return (bytes[0] << 24) | (bytes[1] << 16) | (bytes[2] << 8) | bytes[3];
}

}


WaveFileReader::WaveFileReader(const std::string& filename)
    : mFile(filename, std::ios_base::binary)
{

    if (!mFile.is_open())
        throw std::runtime_error("failed to open file " + filename);

    constexpr auto RIFF_HEADER_CHUNK_SIZE = 12;
    constexpr auto FORMAT_CHUNK_SIZE = 24;
    constexpr auto DATA_HEADER_CHUNK_SIZE = 8;
    constexpr auto PCM_FORMAT_SIZE = 16;
    
    mFile.read(reinterpret_cast<char*>(&mRiff), RIFF_HEADER_CHUNK_SIZE);

    int const WAV_ID_RIFF = 0x52494646; /* "RIFF" */
    int const WAV_ID_WAVE = 0x57415645; /* "WAVE" */
    int const WAV_ID_FMT = 0x666d7420; /* "fmt " */
    int const WAV_ID_DATA = 0x64617461; /* "data" */

    
    
    if (byteArrayToInt32HL(mRiff.id) != WAV_ID_RIFF) {
        throw std::runtime_error("File " + filename + " is not RIFF");
    }

    if (byteArrayToInt32HL(mRiff.format) != WAV_ID_WAVE) {
        throw std::runtime_error("Format of the file " + filename + " is not WAVE");
    }

    mFile.read(reinterpret_cast<char*>(&mFormat), FORMAT_CHUNK_SIZE);

    if (byteArrayToInt32HL(mFormat.id) != WAV_ID_FMT) {
        throw std::runtime_error("File " + filename + " is corrupted no 'fmt' chunk found");
    }
    
    // Skip optional extra data
    auto extraFormatHeaderSize = mFormat.size - PCM_FORMAT_SIZE;
    if (extraFormatHeaderSize > 0) {
        uint16_t extraFormatDataSize = 0;
        mFile.read(reinterpret_cast<char*>(&extraFormatDataSize), extraFormatHeaderSize);
        int64_t pos = mFile.tellg();
        mFile.seekg(pos + static_cast<long int>(extraFormatDataSize));
    }
    
    mFile.read(reinterpret_cast<char*>(&mData), DATA_HEADER_CHUNK_SIZE);

    if (byteArrayToInt32HL(mData.id) != WAV_ID_DATA) {
        throw std::runtime_error("File " + filename + " is corrupted no 'data' chunk found");
    }
    
    
//    dumpHeader();
}

int WaveFileReader::sampleRate() const
{
    return mFormat.sampleRate;
}

int WaveFileReader::numChannels() const
{
    return mFormat.numChannels;
}

int WaveFileReader::readSamples(PcmBuffer_t& buffer)
{
    mFile.read(reinterpret_cast<char*>(buffer.data()), buffer.size());
    auto bytesRead = mFile.gcount();
    auto samplesRead = static_cast<int>(bytesRead) / sizeof(PcmBuffer_t::value_type);
    if (bytesRead % sizeof(PcmBuffer_t::value_type))
        samplesRead++;
    return samplesRead;
}


void WaveFileReader::dumpHeader()
{
    std::cout << "RIFF Header Chunk:\n"
              << "id: " << std::string(mRiff.id, mRiff.id + 4) << "\n"
              << "size: "<< mRiff.size << "\n"
              << "format: " << std::string(mRiff.format, mRiff.format + 4) << "\n";

    std::cout << "Format Chunk:\n"
              << "id: " << std::string(mFormat.id, mFormat.id + 4) << "\n"
              << "size: "<< mFormat.size << "\n"
              << "type: " << mFormat.type << "\n"
              << "numChannels: " << mFormat.numChannels << "\n"
              << "sampleRate: " << mFormat.sampleRate << "\n"
              << "byteRate: " << mFormat.byteRate << "\n"
              << "blockAlign: " << mFormat.blockAlign << "\n"
              << "bitsPerSample: " << mFormat.bitsPerSample << "\n";

    std::cout << "Data Chunk:\n"
              << "id: " << std::string(mData.id, mData.id + 4) << "\n"
              << "size: "<< mData.size << "\n";
}
