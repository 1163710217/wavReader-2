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

    uint8_t chunkId[4];
    uint32_t chunkSize;
    uint8_t format[4];
    uint8_t subChunk1Id[4];
    uint32_t subChunk1Size;
    uint16_t audioFormat;
    uint16_t numChannels;
    uint32_t sampleRate;
    uint32_t byteRate;
    uint16_t blockAlign;
    uint16_t bitsPerSample;
    uint16_t cbSize;                  // nonPCM and extensible
    uint16_t wValidBitsPerSample;     // only extensible
    uint32_t dwChannelMask;           // only extensible
    uint8_t SubFormat[16];            // only extensible
    uint8_t subChunk2Id[4];            // nonPCM and extensible
    uint32_t subChunk2Size;            // nonPCM and extensible
    uint32_t sampleLength;             // nonPCM and extensible
    uint8_t subChunk3Id[4];
    uint32_t subChunk3Size;

    struct audioData {
        std::vector<float> d;  // значения по каналам. Для 2 каналов - вектор на 2 элемента.
    };
    std::vector<audioData> data;// Все точки в хронологической последовательности.
    std::string lastErrorText;  // Текст ошибки чтения, если таковая была.
    bool initialized;           // Признак того, что структура проиницилизирована (чтобы не инициализировать все поля).


public:

    WavFileFormat();
    std::string lastError() const;
    uint16_t channelsCount() const;
    uint32_t audioSampleRate() const;
    uint32_t audioByteRate() const;
    bool readFromStream(ByteOrderStreamIn<> &stream);
    bool fillAudioData(float *out, size_t playProgress);

    size_t fullSize() const;

};

#endif
