/*
 * NESCA-VIEWER
 * by oldteam & lomaster
 * license CC-BY-NC-SA 4.0
 *   Сделано от души 2023.
*/

#include "nesca_viewer.h"
#include "ui_nesca_viewer.h"

#include <QApplication>
#include <QThread>
#include <QThreadPool>
#include <QRunnable>
#include <QtConcurrent/QtConcurrent>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    nesca_viewer w(nullptr);
    QObject::connect(w.ui->pushButton, &QPushButton::clicked, &w, &nesca_viewer::refresh_nesca_viewer);
    w.show();

    return a.exec();
}
