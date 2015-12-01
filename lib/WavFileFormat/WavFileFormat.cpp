#include "WavFileFormat.h"


WavFileFormat::WavFileFormat()
{
    initialized = false;
}

std::string WavFileFormat::lastError() const
{
    return lastErrorText;
}

uint16_t WavFileFormat::channelsCount() const
{
    return initialized?  numChannels : 0;
}

uint32_t WavFileFormat::audioSampleRate() const
{
    return initialized ? sampleRate : 0;
}

uint32_t WavFileFormat::audioByteRate() const
{
    return initialized ? byteRate : 0;
}

enum WaveFormat {
    WAVE_FORMAT_PCM         = 0x0001,
    WAVE_FORMAT_IEEE_FLOAT  = 0x0003,
    WAVE_FORMAT_ALAW        = 0x0006,
    WAVE_FORMAT_MULAW       = 0x0007,
    WAVE_FORMAT_EXTENSIBLE  = 0xFFFE
};

bool WavFileFormat::readFromStream(ByteOrderStreamIn<> &stream)
{
    stream.Read(chunkId, sizeof(chunkId)); // здесь и далее не делим на sizeof(uint8).

    if (std::string((const char*)chunkId, sizeof(chunkId))!= "RIFF") {
        lastErrorText = "RIFF file expected!";
        return false;
    }
    chunkSize = 0;
    stream >> chunkSize;
    stream.Read(format, sizeof(format));   // == "WAVE"
    stream.Read(subChunk1Id,  sizeof(subChunk1Id));// == "fmt "
    if (std::string((const char*)format, sizeof(format))!= "WAVE"
        || std::string((const char*)subChunk1Id, sizeof(subChunk1Id))!= "fmt ") {
        lastErrorText = "WAVE format file expected!";
        return false;
    }
    stream >> subChunk1Size
           >> audioFormat
           >> numChannels
           >> sampleRate
           >> byteRate
           >> blockAlign
           >> bitsPerSample
            ;

    switch(audioFormat)
    {
        case WAVE_FORMAT_PCM:
            break;
        case WAVE_FORMAT_IEEE_FLOAT:
            stream  >> cbSize;
            stream.Read(subChunk2Id,  sizeof(subChunk2Id));
            stream >> subChunk2Size
                   >> sampleLength
                    ;

            break;
        case WAVE_FORMAT_EXTENSIBLE:

            stream  >> cbSize
                    >> wValidBitsPerSample
                    >> dwChannelMask;
            stream.Read(SubFormat,    sizeof(SubFormat));
            stream.Read(subChunk2Id,  sizeof(subChunk2Id));
            stream >> subChunk2Size
                   >> sampleLength
                    ;
            break;
        case WAVE_FORMAT_ALAW:
        case WAVE_FORMAT_MULAW:
        default:
            lastErrorText = "Unsupported WAV format";

            return false;
            break;
    }

    stream.Read(subChunk3Id,  sizeof(subChunk3Id));
    stream >> subChunk3Size;
    size_t subChunk3Count = subChunk3Size/ 8;
    data.resize(subChunk3Count);
    for (size_t i=0; i< subChunk3Count;i++) {
        data[i].d.resize(2);
        stream >> data[i].d[0];
        stream >> data[i].d[1];
    }

    initialized = true;
    return true;
}

bool WavFileFormat::fillAudioData(float *out, size_t playProgress)
{
    if (!initialized || playProgress >= data.size()) {
        for (size_t i=0; i< numChannels; i++){
            *out++ = 0.0;
        }
        return false;
    }
    const audioData &audioChunk = data[playProgress];
    playProgress ++;
    for (uint16_t channel = 0; channel< numChannels; channel++) {
        *out++ = audioChunk.d[channel];
    }
    return true;
}

size_t WavFileFormat::fullSize() const
{
    return data.size();
}

