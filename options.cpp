/*
 * NESCA-VIEWER
 * by oldteam & lomaster
 * license CC-BY-NC-SA 4.0
 *   Сделано от души 2023.
*/

#include "config.h"
#include "options.h"
#include "ui_options.h"

config cfg1;

options::options(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::options)
{
    ui->setupUi(this);
    cfg1.load_from_file(CONFIG_PATH);

    ui->lineEdit->setText(cfg1.get_value(CONFIG_DATA));
    ui->lineEdit_2->setText(cfg1.get_value(CONFIG_TEMP));
    ui->lineEdit_3->setText(cfg1.get_value(CONFIG_LOAD));

    connect(ui->lineEdit, &QLineEdit::textChanged, this, &options::line_edit_refresh1);
    connect(ui->lineEdit_2, &QLineEdit::textChanged, this, &options::line_edit_refresh2);
    connect(ui->lineEdit_3, &QLineEdit::textChanged, this, &options::line_edit_refresh3);
}

void options::save_config()
{
    cfg1.save_to_file(CONFIG_PATH);
}

void options::line_edit_refresh1(const QString &text)
{
    cfg1.set_value(CONFIG_DATA, text);
    save_config();
}

void options::line_edit_refresh2(const QString &text)
{
    cfg1.set_value(CONFIG_TEMP, text);
    save_config();
}

void options::line_edit_refresh3(const QString &text)
{
    cfg1.set_value(CONFIG_LOAD, text);
    save_config();
}

options::~options()
{
    delete ui;
}
