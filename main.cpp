#include "mplayer.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Q_INIT_RESOURCE(images);
    MPlayer w;
    w.show();

    return a.exec();
}
