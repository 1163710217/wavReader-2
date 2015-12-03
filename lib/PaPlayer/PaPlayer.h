#ifndef PAPLAYER_H
#define PAPLAYER_H
#include <string>
#include <memory>
class WavFileFormat;
typedef void PaStream;
/**
 * \brief Аудио-плеер. Обертка над PortAudio.
 *
 * Позволяет загружать WAV-файл через функцию openFile.
 *
 * Для проигрывания и остановки - play() и stop().
 *
 * Для получения длины аудиофайла в фреймах и текущей позиции - fullSize(), currentPosition().
 *
 * hasFinished() - показывает, завершилось ли уже проигрывание.
 */
class PaPlayer {
    std::unique_ptr<WavFileFormat> m_wavFile;
    PaStream *m_stream;
    std::string m_lastError;
    bool m_isPlaying;
    size_t m_fullSize;            // полный размер файла.
public:
    size_t m_playProgress;        // Смещение относительно начала данных.

    PaPlayer();
    ~PaPlayer();
    std::string lastError() const;
    const WavFileFormat * wavFile() const;
    bool openFile(const std::string& fileName);
    bool initStream();
    bool play();
    bool stop();
    bool pause();
    void close();
    void hasFinished();
    size_t fullSize() const;
    size_t currentPosition() const;
    void setPosition(size_t position);
    bool isPlaying() const;
};

#endif
