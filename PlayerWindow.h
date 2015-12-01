#ifndef PLAYERWINDOW_H
#define PLAYERWINDOW_H
#include <QMainWindow>
#include <PaPlayer.h>
#include <QSlider>
/**
 * \brief Окно проигрывателя с кнопками управления.
 *
 * Окно создает объект "плеер", который уже собственно отвечает за загрузку и вывод звука на устройство.
 */
class PlayerWindow:public QMainWindow
{
    Q_OBJECT
    PaPlayer m_player;
    QSlider *m_trackSlider;

public:
    PlayerWindow(QWidget * parent = 0);

public slots:
    void openFile();
    void stop();
    void play();
    void pause();
    void showPlayerError();
    void sliderMoved(int value);

protected:
    void timerEvent(QTimerEvent*);
};

#endif
