#include "PaPlayer.h"
#include <stdexcept>
#include <portaudio.h>
#include <WavFileFormat.h>
#include <fstream>

static int wavPlayCallback( const void *, void *outputBuffer,
                            unsigned long framesPerBuffer,
                            const PaStreamCallbackTimeInfo* ,
                            PaStreamCallbackFlags ,
                            void *userData)
{
    PaPlayer *player = (PaPlayer *) userData;
    WavFileFormat *waveData = player->wavFile();
    float * out = (float*)outputBuffer;

    int result = paContinue;
    int channelCount = waveData->channelsCount();
    for(unsigned long  k=0; k<framesPerBuffer; k++ ) {
        float *outByChannel = out;
        if (!waveData->fillAudioData(outByChannel, player->m_playProgress )) {
            result = paComplete;
        }
        out += channelCount;
        player->m_playProgress++;
    }
    return result;
}

static void StreamFinished( void * userData)
{
   PaPlayer *player = (PaPlayer *) userData;
   player->hasFinished();
}


PaPlayer::PaPlayer()
    : m_wavFile(NULL)
    , m_stream(NULL)
    ,m_isPlaying(false)
    ,m_playProgress(0)
{
    PaError err;
    err = Pa_Initialize();
    if( err != paNoError ) {
         Pa_Terminate();
         throw std::runtime_error("Failed to initialize portAudio.");
    }
    m_wavFile = new WavFileFormat();
}

PaPlayer::~PaPlayer()
{
    close();
    Pa_Terminate();
    delete m_wavFile ;
}

std::string PaPlayer::lastError() const
{
    return m_lastError;
}

WavFileFormat *PaPlayer::wavFile() const
{
    return m_wavFile;
}

bool PaPlayer::openFile(const std::string &fileName)
{
    std::ifstream wav_fin;
    wav_fin.open(fileName.c_str(), std::ios_base::binary);
    ByteOrderStreamIn<> bytestream(wav_fin);


    WavFileFormat wavFile;
    if (!wavFile.readFromStream(bytestream) ){
        m_lastError = wavFile.lastError();
        return false;
    }
    wav_fin.close();
    *m_wavFile = wavFile;
    m_fullSize = m_wavFile->fullSize();

    return initStream();
}

#define CheckError(err) \
    m_lastError.clear(); \
    if( err != paNoError ) {\
        m_lastError = Pa_GetErrorText( err );\
        return false;\
    }


bool PaPlayer::initStream()
{
    close();
    PaStreamParameters outputParameters;
    PaError err;

    outputParameters.device = Pa_GetDefaultOutputDevice(); /* default output device */
    if (outputParameters.device == paNoDevice) {
       m_lastError = "No default output device.";
       return false;
    }
    outputParameters.channelCount = m_wavFile->channelsCount();       /* stereo/mono output */
    outputParameters.sampleFormat = paFloat32; /* 32 bit floating point output */
    outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultLowOutputLatency;
    outputParameters.hostApiSpecificStreamInfo = NULL;
    const double sampleRate = m_wavFile->audioSampleRate();
    const unsigned int framesPerBuffer = 64;

    err = Pa_OpenStream(
              &m_stream,
              NULL, // no input
              &outputParameters,
              sampleRate,
              framesPerBuffer,
              paClipOff,      // we won't output out of range samples so don't bother clipping them
              wavPlayCallback,
              this );
    CheckError(err);

    err = Pa_SetStreamFinishedCallback( m_stream, &StreamFinished );
    CheckError(err);

    err = Pa_StartStream( m_stream );
    CheckError(err);

    return true;
}

bool PaPlayer::play()
{
    m_isPlaying = true;
    PaError err = Pa_StartStream( m_stream );
    CheckError(err);
    return true;
}

bool PaPlayer::stop()
{
    PaError err = Pa_StopStream( m_stream );
    CheckError(err);
    m_isPlaying = false;
    m_playProgress = 0;
    return true;
}

bool PaPlayer::pause()
{
    PaError err = Pa_StopStream( m_stream );
    CheckError(err);
    return true;
}

void PaPlayer::close()
{
    if (m_stream) {
        Pa_AbortStream( m_stream );
        Pa_CloseStream( m_stream );
    }
    m_stream = NULL;
    m_isPlaying = false;
    m_playProgress = 0;
}

void PaPlayer::hasFinished()
{
    m_isPlaying = false;
}

size_t PaPlayer::fullSize() const
{
    return m_fullSize;
}

size_t PaPlayer::currentPosition() const
{
    return m_playProgress;
}

void PaPlayer::setPosition(size_t position)
{
    if (m_isPlaying) {
        stop();
    }
    m_playProgress = position;
}

bool PaPlayer::isPlaying() const
{
    return m_isPlaying;
}
