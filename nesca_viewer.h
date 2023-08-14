/*
 * NESCA-VIEWER
 * by oldteam & lomaster
 * license CC-BY-NC-SA 4.0
 *   Сделано от души 2023.
*/

#ifndef NESCA_VIEWER_H
#define NESCA_VIEWER_H

#include "qapplication.h"
#include "qlabel.h"
#include <QMainWindow>
#include <QGridLayout>
#include <QTimer>
#include <QScroller>
#include <QScrollArea>

QT_BEGIN_NAMESPACE
namespace Ui { class nesca_viewer; }
QT_END_NAMESPACE

class ClickableLabel : public QLabel
{
    Q_OBJECT

public:
    ClickableLabel(QWidget *parent = nullptr) : QLabel(parent) {}

signals:
    void clicked();

protected:
    void mousePressEvent(QMouseEvent *event) override
    {
        emit clicked();
        QLabel::mousePressEvent(event);
    }
};

class nesca_viewer : public QMainWindow
{
    Q_OBJECT

public:
    nesca_viewer(QWidget *parent);
    void refresh_nesca_viewer();
    Ui::nesca_viewer *ui;

    ~nesca_viewer();

private slots:
    void open_image(QPixmap pixmap);
    void on_pushButton_clicked();
    void on_scroll(int value);
    void on_pushButton_2_clicked();
    void on_pushButton_3_clicked();

public slots:
    void add_blocks();

private:
    void pre_init();

    void init_count_files();
    void init_path_files();
    void parsed_next_files();

    void update_page_label();
    /*Функции для работы с блоками.*/
    void load_single_block(int index);
    void update_loaded_blocks(int blocks_to_load);
    void reset_loaded_blocks();
    void save_removed_blocks();
    void remove_single_block(int index);
    void restore_blocks();
    void reload_for_new_json();

    void remove_blocks(int start_index, int end_index);
    void close_image(QDialog *dialog);

    void log_plain(const QString& message);
    void decode_screeshot(int block_num, int port_num);
    void init_screenshots(int count_init);

    void add_block_on_grid(const QString &http_title, const QString &dns, const QString &node, const QString &rtt,
                           QGridLayout *layout, int row, int col, const QString &group_name, int datablock_index);
    QGridLayout *gridLayout;
    QTimer *blockTimer;
    QScrollBar *scrollBar;
    int cols = 4;
    int block_height;
    int previousScroll;
    int rows;
    int block_size;
    int loaded_blocks;
    int total_blocks;

};
#endif // NESCA_VIEWER_H
