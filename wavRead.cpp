#include <QDir>
#include <QApplication>

#include "PaPlayer.h"
#include "PlayerWindow.h"

int main( int argc, char *argv[] )
{
    QApplication app(argc, argv);
    QDir::setCurrent(qApp->applicationDirPath());
    PlayerWindow pwindow;
    pwindow.show();
    return app.exec();
}

