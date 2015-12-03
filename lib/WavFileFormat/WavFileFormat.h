#ifndef WAVFILEFORMAT_H
#define WAVFILEFORMAT_H
#include <stdint.h>
#include <ByteOrderStream.h>
#include <vector>
/**
 * \brief Контейнер, который хранит представление WAV-файла.
 *
 * Собственно значения амплитуд хранятся в поле data.
 */
class WavFileFormat
{

    uint8_t m_chunkId[4];
    uint32_t m_chunkSize;
    uint8_t m_format[4];
    uint8_t m_subChunk1Id[4];
    uint32_t m_subChunk1Size;
    uint16_t m_audioFormat;
    uint16_t m_numChannels;
    uint32_t m_sampleRate;
    uint32_t m_byteRate;
    uint16_t m_blockAlign;
    uint16_t m_bitsPerSample;
    uint16_t m_cbSize;                  // nonPCM and extensible
    uint16_t m_wValidBitsPerSample;     // only extensible
    uint32_t m_dwChannelMask;           // only extensible
    uint8_t m_subFormat[16];            // only extensible
    uint8_t m_subChunk2Id[4];            // nonPCM and extensible
    uint32_t m_subChunk2Size;            // nonPCM and extensible
    uint32_t m_sampleLength;             // nonPCM and extensible

    uint8_t m_subChunk3Id[4];
    uint32_t m_subChunk3Size;

    struct audioData {
        std::vector<float> d;  // значения по каналам. Для 2 каналов - вектор на 2 элемента.
    };
    std::vector<audioData> data;// Все точки в хронологической последовательности.
    std::string lastErrorText;  // Текст ошибки чтения, если таковая была.


public:

    WavFileFormat();
    std::string lastError() const;
    uint16_t channelsCount() const;
    uint32_t audioSampleRate() const;
    uint32_t audioByteRate() const;
    bool readFromStream(ByteOrderStreamIn<> &stream);
    bool fillAudioData(float *out, size_t playProgress) const;

    size_t fullSize() const;

};

#endif
