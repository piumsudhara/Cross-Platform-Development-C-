#include "MyNotePad.h"
#include <QApplication>
#include <QSplashScreen>
#include <QTimer>

int main(int argc, char *argv[])
{
    QApplication Editor(argc, argv);
    MyNotePad w;
    QTimer::singleShot(3000, &w, SLOT(show()));

    return Editor.exec();
}
