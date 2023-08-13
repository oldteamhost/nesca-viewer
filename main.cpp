/*
 * NESCA-VIEWER
 * by oldteam & lomaster
 * license CC-BY-NC-SA 4.0
 *   Сделано от души 2023.
*/

#include "nesca_viewer.h"

#include <QApplication>
#include <QThread>
#include <QThreadPool>
#include <QRunnable>
#include <QtConcurrent/QtConcurrent>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    nesca_viewer w(nullptr);
    w.show();

    return a.exec();
}
