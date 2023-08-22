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
#include <QPropertyAnimation>
#include <QKeyEvent>

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

struct tab_list
{
    QWidget *scrollAreaWidgetContents;
    QScrollArea *scrollArea;
    QGridLayout *gridLayout;
};

class nesca_viewer : public QMainWindow
{
    Q_OBJECT

public:
    nesca_viewer(QWidget *parent);
    void refresh_nesca_viewer();
    Ui::nesca_viewer *ui;

    ~nesca_viewer();
protected:
    void keyPressEvent(QKeyEvent *event) override
    {
        switch (event->key())
        {
        case Qt::Key_H:
            // Переключение на предыдущую вкладку
            tabWidget->setCurrentIndex((tabWidget->currentIndex() - 1 + tabWidget->count()) % tabWidget->count());
            break;
        case Qt::Key_L:
            // Переключение на следующую вкладку
            tabWidget->setCurrentIndex((tabWidget->currentIndex() + 1) % tabWidget->count());
            break;
        default:
            QMainWindow::keyPressEvent(event);
        }
    }
private slots:
    void open_image(QPixmap pixmap);
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_3_clicked();
    void edit_button_click(int index);

public slots:
    void add_blocks(QScrollArea* scrollArea, QGridLayout *gridLayout);

private:
    void pre_init();

    void init_count_files();
    void init_path_files();
    void parsed_next_files();
    QString get_program_on_view(const QString& protocol);

    void update_page_label();
    /*Функции для работы с блоками.*/
    void load_single_block(int index, QGridLayout *gridLayout);
    void update_loaded_blocks(int blocks_to_load);
    void reset_loaded_blocks();
    void save_removed_blocks();
    void reload_for_new_json();

    void remove_blocks(int start_index, int end_index);
    void close_image(QDialog *dialog);

    void log_plain(const QString& message);
    void decode_screeshot(int block_num, int port_num);
    void init_screenshots(int count_init);
    void remove_single_block(int index);

    void add_block_on_grid(const QString &http_title, const QString &dns, const QString &node, const QString &rtt,
                           QGridLayout *layout, int row, int col, const QString &group_name, int datablock_index);


    void create_tab_widget();
    void add_tab(QString title);
    void next_tab(int index);

    std::vector<tab_list> tabs_a;

    QTabWidget* tabWidget;
    QScrollArea* connectedScrollArea = nullptr;

    int cols = 4;
    int tab_numbers = 0;
    int block_height;
    int parsed_files = 0;
    int temp_tab = 0;
    int current_tab = 0;
    int previousScroll;
    int rows;
    int block_size;
    int loaded_blocks;
    int total_blocks;

};
#endif // NESCA_VIEWER_H
