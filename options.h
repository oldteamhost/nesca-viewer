/*
 * NESCA-VIEWER
 * by oldteam & lomaster
 * license CC-BY-NC-SA 4.0
 *   Сделано от души 2023.
*/

#ifndef OPTIONS_H
#define OPTIONS_H

#include <QDialog>

namespace Ui {
class options;
}

class options : public QDialog
{
    Q_OBJECT

public:
    explicit options(QWidget *parent = nullptr);
    ~options();
public slots:
    void line_edit_refresh1(const QString &text);
    void line_edit_refresh2(const QString &text);
    void line_edit_refresh3(const QString &text);
    void save_config();
private:
    Ui::options *ui;
};

#endif // OPTIONS_H
