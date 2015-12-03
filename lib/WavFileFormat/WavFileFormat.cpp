#include "WavFileFormat.h"


WavFileFormat::WavFileFormat()
    : m_chunkId{0,0,0,0}
{
    m_numChannels =0;
}

std::string WavFileFormat::lastError() const
{
    return lastErrorText;
}

uint16_t WavFileFormat::channelsCount() const
{
    return  m_numChannels ;
}

uint32_t WavFileFormat::audioSampleRate() const
{
    return  m_sampleRate ;
}

uint32_t WavFileFormat::audioByteRate() const
{
    return  m_byteRate;
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
    stream.Read(m_chunkId, sizeof(m_chunkId)); // здесь и далее не делим на sizeof(uint8).

    if (std::string((const char*)m_chunkId, sizeof(m_chunkId))!= "RIFF") {
        lastErrorText = "RIFF file expected!";
        return false;
    }
    m_chunkSize = 0;
    stream >> m_chunkSize;
    stream.Read(m_format, sizeof(m_format));   // == "WAVE"
    stream.Read(m_subChunk1Id,  sizeof(m_subChunk1Id));// == "fmt "
    if (std::string((const char*)m_format, sizeof(m_format))!= "WAVE"
        || std::string((const char*)m_subChunk1Id, sizeof(m_subChunk1Id))!= "fmt ") {
        lastErrorText = "WAVE format file expected!";
        return false;
    }
    stream >> m_subChunk1Size
           >> m_audioFormat
           >> m_numChannels
           >> m_sampleRate
           >> m_byteRate
           >> m_blockAlign
           >> m_bitsPerSample
            ;

    switch(m_audioFormat)
    {
        case WAVE_FORMAT_PCM:
            break;
        case WAVE_FORMAT_IEEE_FLOAT:
            stream  >> m_cbSize;
            stream.Read(m_subChunk2Id,  sizeof(m_subChunk2Id));
            stream >> m_subChunk2Size
                   >> m_sampleLength
                    ;

            break;
        case WAVE_FORMAT_EXTENSIBLE:

            /*stream  >> m_cbSize
                    >> m_wValidBitsPerSample
                    >> m_dwChannelMask;
            stream.Read(m_subFormat,    sizeof(m_subFormat));
            stream.Read(m_subChunk2Id,  sizeof(m_subChunk2Id));
            stream >> m_subChunk2Size
                   >> m_sampleLength
                    ;
            break;*/
        case WAVE_FORMAT_ALAW:
        case WAVE_FORMAT_MULAW:
        default:
            lastErrorText = "Unsupported WAV format";

            return false;
            break;
    }

    stream.Read(m_subChunk3Id,  sizeof(m_subChunk3Id));
    stream >> m_subChunk3Size;
    size_t subChunk3Count = m_subChunk3Size/ (m_bitsPerSample / 8) / m_numChannels;
    data.resize(subChunk3Count);
    for (size_t i=0; i< subChunk3Count;i++) {
        data[i].d.resize(m_numChannels);
        for (size_t channel = 0; channel < m_numChannels; channel++) {
            float &channelValue = data[i].d[channel];
            switch(m_audioFormat)
            {
                case WAVE_FORMAT_PCM:{
                    int16_t value;
                    stream >> value;
                    channelValue =  value / 32768.;
                    if( channelValue > 1 ) channelValue = 1;
                    if( channelValue < -1 ) channelValue = -1;
                    }
                    break;
                case WAVE_FORMAT_IEEE_FLOAT:
                    stream >> channelValue;
                    break;

            };

        }
    }

    return true;
}

bool WavFileFormat::fillAudioData(float *out, size_t playProgress) const
{
    if (playProgress >= data.size()) {
        for (size_t i=0; i< m_numChannels; i++){
            *out++ = 0.0;
        }
        return false;
    }
    const audioData &audioChunk = data[playProgress];
    playProgress ++;
    for (uint16_t channel = 0; channel< m_numChannels; channel++) {
        *out++ = audioChunk.d[channel];
    }
    return true;
}

size_t WavFileFormat::fullSize() const
{
    return data.size();
}

