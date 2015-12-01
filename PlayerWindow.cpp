#include "PlayerWindow.h"
#include <QAction>
#include <QFileDialog>
#include <QToolBar>
#include <QMessageBox>
PlayerWindow::PlayerWindow(QWidget *parent)
    :QMainWindow(parent)
{
    QToolBar* main = addToolBar(tr("Main"));

    QAction* act_openFile = new QAction(tr("Open WAV"), this);
    main->addAction(act_openFile);

    QAction* act_play = new QAction(tr("Play"), this);
    main->addAction(act_play);

    QAction* act_stop = new QAction(tr("Stop"), this);
    main->addAction(act_stop);

    QAction* act_pause = new QAction(tr("Pause"), this);
    main->addAction(act_pause);

    m_trackSlider = new QSlider(Qt::Horizontal, this);
    setCentralWidget(m_trackSlider);
    startTimer(200);

    connect(act_openFile,   SIGNAL(triggered(bool)), this, SLOT(openFile()));
    connect(act_play,       SIGNAL(triggered(bool)), this, SLOT(play()));
    connect(act_stop,       SIGNAL(triggered(bool)), this, SLOT(stop()));
    connect(act_pause,      SIGNAL(triggered(bool)), this, SLOT(pause()));
    connect(m_trackSlider, SIGNAL(sliderMoved(int)), this, SLOT(sliderMoved(int)));
}

void PlayerWindow::openFile()
{
    std::string fileName = QFileDialog::getOpenFileName(this, tr("Open WAV"), "", "Wav files (*.wav)").toStdString();
    if (fileName.empty()) return;

    if (!m_player.openFile(fileName)){
        showPlayerError();
        return;
    }
    m_player.initStream();
    m_player.play();

}

void PlayerWindow::stop()
{
    m_player.stop();
}

void PlayerWindow::play()
{
    m_player.play();
}

void PlayerWindow::pause()
{
    m_player.pause();
}

void PlayerWindow::showPlayerError()
{
    QString errText = QString::fromStdString(m_player.lastError());
    if (!errText.isEmpty())
        QMessageBox::warning(this, tr("Error"), errText);
}

void PlayerWindow::sliderMoved(int value)
{
    m_player.setPosition(value);
}

void PlayerWindow::timerEvent(QTimerEvent *)
{
    m_trackSlider->setMaximum(m_player.fullSize());
    m_trackSlider->setValue(m_player.currentPosition());
}
