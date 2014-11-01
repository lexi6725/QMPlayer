#include "mplayer.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Q_INIT_RESOURCE(images);
    MPlayer mplayer;
    mplayer.show();

    return a.exec();
}
