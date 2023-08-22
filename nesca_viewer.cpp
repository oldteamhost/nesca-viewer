/*
 * NESCA-VIEWER
 * by oldteam & lomaster
 * license CC-BY-NC-SA 4.0
 *   Сделано от души 2023.
*/

#include "nesca_viewer.h"
#include "options.h"
#include "./ui_nesca_viewer.h"
#include <QVBoxLayout>
#include "utils.h"
#include <iostream>
#include <QtWidgets>
#include <QGridLayout>
#include "readfiles.h"
#include <QScroller>
#include <QScrollArea>
#include "config.h"
#include <QPropertyAnimation>

void pre_init_files(){
    QString currentPath = QDir::currentPath();

    QString dataFolderPath = currentPath + "/data";
    QString tempFolderPath = currentPath + "/temp";

    QDir dataDir(dataFolderPath);
    QDir tempDir(tempFolderPath);

    if (!dataDir.exists()) {
        if (!dataDir.mkpath(dataFolderPath)) {
            qDebug() << "Не удалось создать папку 'data'";
        } else {
            qDebug() << "Папка 'data' успешно создана";
        }
    } else {
        qDebug() << "Папка 'data' уже существует";
    }

    if (!tempDir.exists()) {
        if (!tempDir.mkpath(tempFolderPath)) {
            qDebug() << "Не удалось создать папку 'temp'";
        } else {
            qDebug() << "Папка 'temp' успешно создана";
        }
    } else {
        qDebug() << "Папка 'temp' уже существует";
    }
}

struct displayed_block_info
{
    QGroupBox *group_box;
    int data_index;
};

/*Класс для хранение данных о файлах.*/
class data_files
{
public:
    int count_files = 0;

    QString path_json = "data";
    QString path_temp = "temp";

    QStringList file_paths_json;
    QStringList file_paths_xml;

    int i_jsons = 0;
    int i_xmls = 0;
    int i_screenshots;
};
data_files df;

/*Класс для хранения дата блоков,
 * Это те блоки где лежат скриншоты, ip, и т.д.*/
class blocks_data
{
public:
    /*Сам массив дата блоков.*/
    QList<datablock> datablocks;

    QMap<int, datablock*> displayed_blocks;
    QList<displayed_block_info> displayed_block_info_list;

    int search_port_80_and_screenshot(int datablock);
};
blocks_data bkd;

int blocks_data::search_port_80_and_screenshot(int datablock)
{
    if (datablock >= 0 && datablock < datablocks.size())
    {
        const QList<port>& portsList = datablocks[datablock]._details.ports;

        for (int i = 0; i < portsList.size(); ++i)
        {
            const port& p = portsList.at(i);
            if (p.port == 80)
            {
                if (p.screenshot.size() > 1)
                {
                    return i;
                }
            }
        }
    }
    return -1;
}

bool
file_found(const QString& folder_path, const QString& file_name)
{
    QFileInfo fileInfo(folder_path + QDir::separator() + file_name);
    return fileInfo.exists() && fileInfo.isFile();
}

void nesca_viewer::pre_init()
{
    pre_init_files();
    config cfg;
    cfg.load_from_file(CONFIG_PATH);
    df.path_json = cfg.get_value(CONFIG_DATA);
    df.path_temp = cfg.get_value(CONFIG_TEMP);
    block_size = cfg.get_value(CONFIG_LOAD).toInt();

    init_count_files();
    ui->label_2->setText(QString::number(df.count_files) + " files loaded");
    init_path_files();

    /*Парсим первые файлы, для прогрузки первый блоков.*/
    log_plain("Run parse files");
    parsed_next_files();

    total_blocks = 32;
    loaded_blocks = 0; /*Начальное количество загруженных блоков.*/

    add_blocks(tabs_a[0].scrollArea, tabs_a[0].gridLayout);
}

void nesca_viewer::refresh_nesca_viewer()
{
    close();
    QApplication::quit();

    QProcess::startDetached(QCoreApplication::applicationFilePath(), QCoreApplication::arguments());
    qApp->exit();
}

void nesca_viewer::create_tab_widget()
{
    tabWidget = new QTabWidget(this);
    tabWidget->setGeometry(10, 60, 1121, 521); // Установите координаты (x, y) и размер (width, height)
    tabWidget->setStyleSheet("color: rgb(255, 255, 255);");
}

void nesca_viewer::add_tab(QString title)
{
    QScrollArea* scrollArea = new QScrollArea;
    scrollArea->setWidgetResizable(true);
    scrollArea->setStyleSheet("color: rgb(255, 255, 255);");

    QWidget* scrollAreaWidgetContents = new QWidget(scrollArea);
    QGridLayout* gridLayout = new QGridLayout(scrollAreaWidgetContents);
    gridLayout->setSpacing(10);
    scrollAreaWidgetContents->setLayout(gridLayout);

    QScrollBar* scrollBar = scrollArea->verticalScrollBar();
    QScroller::grabGesture(scrollBar, QScroller::LeftMouseButtonGesture);
    QScrollerProperties properties = QScroller::scroller(scrollBar)->scrollerProperties();
    properties.setScrollMetric(QScrollerProperties::DragVelocitySmoothingFactor, 0.5);
    properties.setScrollMetric(QScrollerProperties::DecelerationFactor, 0.5);
    QScroller::scroller(scrollBar)->setScrollerProperties(properties);
    scrollBar->setSingleStep(25);

    gridLayout->setSpacing(10);
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(scrollAreaWidgetContents);

    tabWidget->addTab(scrollArea, title);

    tab_list tab;
    tab.scrollAreaWidgetContents = scrollAreaWidgetContents;
    tab.scrollArea = scrollArea;
    tab.gridLayout = gridLayout;

    tabs_a.push_back(tab);
}

void nesca_viewer::next_tab(int index)
{
    /*Следующая страница.*/
    if (index > current_tab)
    {
        loaded_blocks = total_blocks;
        total_blocks += total_blocks;

        add_blocks(tabs_a[index].scrollArea, tabs_a[index].gridLayout);
    }
    else /*Преведущая.*/
    {
    }

    log_plain("page " + QString::number(index));
}

nesca_viewer::nesca_viewer(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::nesca_viewer)
{
    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
    ui->setupUi(this);
    log_plain("Welcome to Nesca-Viewer!");
    create_tab_widget();
    add_tab(QString::number(tab_numbers) + " tab");
    connect(tabWidget, &QTabWidget::currentChanged, this, &::nesca_viewer::next_tab);
    pre_init();
};

void nesca_viewer::init_screenshots(int count_init)
{
    /*Проверка размера указаного для иницилизации.*/
    int all_size = bkd.datablocks.size();
    if (count_init > all_size)
    {
        count_init = all_size;
    }

    /*Для избежание дублированния обработки одного и того же скриншота.*/
    count_init += df.i_screenshots;

    for (; df.i_screenshots < count_init; ++df.i_screenshots)
    {
        int port_index = bkd.search_port_80_and_screenshot(df.i_screenshots);

        if (port_index != -1)
        {
            if (!file_found(df.path_temp, bkd.datablocks[df.i_screenshots].ip_address + ".png"))
            {
                decode_screeshot(df.i_screenshots, port_index);
                log_plain("Screenshot \"" + bkd.datablocks[df.i_screenshots].ip_address + ".png\" decoded");
            }
            else
            {
                log_plain("Screenshot \"" + bkd.datablocks[df.i_screenshots].ip_address + ".png\" already exists");
            }
        }
    }
}

#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
void nesca_viewer::log_plain(const QString& message)
{
    /* Инициализируем текущее время. */
    QDateTime currentTime = QDateTime::currentDateTime();

    /* Форматируем в такой формат [20:21:43]: */
    QString formattedTime = currentTime.toString("[hh:mm:ss]:");

    /* Собираем наш лог. */
    QString logMessage = formattedTime + message;

    /* Создаём анимацию прозрачности. */
    QGraphicsOpacityEffect* opacityEffect = new QGraphicsOpacityEffect(ui->plainTextEdit);
    ui->plainTextEdit->setGraphicsEffect(opacityEffect);

    QPropertyAnimation* animation = new QPropertyAnimation(opacityEffect, "opacity");
    animation->setDuration(400); // Длительность анимации в миллисекундах
    animation->setStartValue(0.0); // Прозрачность 0 (полностью прозрачно)
    animation->setEndValue(1.0);   // Прозрачность 1 (полностью видимо)

    /* Выводим текст в plainTextEdit и запускаем анимацию. */
    ui->plainTextEdit->appendPlainText(logMessage);
    animation->start();
}

void nesca_viewer::init_count_files()
{
    /*Получение количества файлов.*/
    df.count_files = count_files(df.path_json, {"*.xml"});
    df.count_files += count_files(df.path_json, {"*.json"});
}

void nesca_viewer::init_path_files()
{
    df.file_paths_xml = get_all_file_paths_in_folder(df.path_json, {"*.xml"});
    df.file_paths_json = get_all_file_paths_in_folder(df.path_json, {"*.json"});
}

void nesca_viewer::remove_single_block(int index)
{
    auto it = std::find_if(bkd.displayed_block_info_list.begin(), bkd.displayed_block_info_list.end(),
                           [index](const displayed_block_info& blockInfo) {
                               return blockInfo.data_index == index;
                           });

    if (it != bkd.displayed_block_info_list.end())
    {
        const displayed_block_info& blockInfo = *it;

        if (blockInfo.group_box)
        {
            QLayout* layout = blockInfo.group_box->layout();
            if (layout)
            {
                while (QLayoutItem* item = layout->takeAt(0))
                {
                    if (QWidget* widget = item->widget())
                    {
                        widget->deleteLater();
                    }
                    delete item;
                }
            }

            delete blockInfo.group_box;
        }

        if (bkd.displayed_blocks.contains(index))
        {
            datablock* blockToRemove = bkd.displayed_blocks.take(index);
            delete blockToRemove;
        }

        bkd.displayed_block_info_list.erase(it);
    }
}

void nesca_viewer::load_single_block(int index, QGridLayout* gridLayout)
{
    if (index >= 0 && index < bkd.datablocks.size())
    {
        datablock& db = bkd.datablocks[index];
        QString group_name = QString::number(index + 1) + " block";

        if (!bkd.displayed_blocks.contains(index))
        {
            datablock* displayedBlock = new datablock(db);
            bkd.displayed_blocks.insert(index, displayedBlock);

            add_block_on_grid(db._details.ports[0].http_title, db._details.dns_name, db.ip_address,
                              QString::number(db._details.rtt), gridLayout, index / cols, index % cols, group_name, index);
        }
    }
}

void nesca_viewer::update_loaded_blocks(int blocks_to_load)
{
    loaded_blocks += blocks_to_load;

    if (loaded_blocks >= total_blocks && df.i_xmls < df.file_paths_xml.size())
    {
        reset_loaded_blocks();
        reload_for_new_json();
    }

    if (loaded_blocks >= total_blocks && df.i_jsons < df.file_paths_json.size())
    {
        reset_loaded_blocks();
        reload_for_new_json();
    }
}

void nesca_viewer::reset_loaded_blocks()
{
    df.i_screenshots = 0;

    parsed_next_files();
    loaded_blocks = 0;
}

void nesca_viewer::reload_for_new_json()
{
    df.i_screenshots = 0;
    init_screenshots(loaded_blocks);

    parsed_next_files();
    loaded_blocks = 0;
}

void nesca_viewer::add_blocks(QScrollArea* scrollArea, QGridLayout* gridLayout)
{
    if (connectedScrollArea)
    {
        disconnect(connectedScrollArea->verticalScrollBar(), &QScrollBar::valueChanged, this, nullptr);
    }

    connect(scrollArea->verticalScrollBar(), &QScrollBar::valueChanged, this, [this, scrollArea, gridLayout]()
    {
        add_blocks(scrollArea, gridLayout);
    });

    connectedScrollArea = scrollArea;
    int maxScroll = scrollArea->verticalScrollBar()->maximum();
    int currentScroll = scrollArea->verticalScrollBar()->value();
    int middleScroll = maxScroll / 2;

    if (currentScroll >= middleScroll && loaded_blocks < total_blocks)
    {
        int remainingBlocks = total_blocks - loaded_blocks;
        int blocksToLoad = qMin(block_size, remainingBlocks);

        init_screenshots(loaded_blocks);

        for (int i = 0; i < blocksToLoad; ++i)
        {
            int index = loaded_blocks + i;
            load_single_block(index, gridLayout);
        }

        update_loaded_blocks(blocksToLoad);
    }
    else if (currentScroll == maxScroll && total_blocks <= bkd.datablocks.size())
    {
        tab_numbers++;
        add_tab(QString::number(tab_numbers) + " tab");
    }
}

void nesca_viewer::decode_screeshot(int block_num, int port_num)
{
    /*Будущая длинна бинарника.*/
    size_t decoded_length = 0;

    /*Конвертирование поля screenshot с base64 в std::sting.*/
    std::string base64_input = bkd.datablocks[block_num]._details.ports[port_num].screenshot.toStdString();

    /*Декодим base64 строку, в бинарный текст.*/
    const unsigned char* decoded_data = base64_decode(base64_input, decoded_length);

    /*И уже бинарный текст сохраняем в файл .png.*/
    save_binary_file(df.path_temp + "/" + bkd.datablocks[block_num].ip_address + ".png", decoded_data, decoded_length);

    /*Чистим память за декодированием base64.*/
    delete[] decoded_data;
}

void nesca_viewer::close_image(QDialog *dialog)
{
    if (dialog){dialog->close();}
}

void nesca_viewer::open_image(QPixmap pixmap)
{
    QDialog *image_dialog = new QDialog(this); // Создание нового диалога
    image_dialog->setAttribute(Qt::WA_DeleteOnClose); // Автоматическое удаление при закрытии
    image_dialog->setWindowTitle("/img/");
    image_dialog->setModal(true);

    QVBoxLayout *layout = new QVBoxLayout(image_dialog);

    ClickableLabel *imageLabel = new ClickableLabel(image_dialog);
    imageLabel->setPixmap(pixmap);

    layout->addWidget(imageLabel);
    image_dialog->setLayout(layout);

    connect(imageLabel, &ClickableLabel::clicked, image_dialog, [this, image_dialog]() {
        close_image(image_dialog);
    });
    image_dialog->show();
}

QString nesca_viewer::get_program_on_view(const QString& protocol)
{
    if (protocol == "FTP")
    {
        return "Filezilla";
    }
    else if (protocol == "HIKVISION")
    {
        return "IVMS-4200";
    }

    return "";
}
void nesca_viewer::edit_button_click(int index)
{
    QDialog *editDialog = new QDialog(this);
    editDialog->setAttribute(Qt::WA_DeleteOnClose);
    editDialog->setWindowTitle("Edit Block");
    editDialog->setModal(true);

    QVBoxLayout *layout = new QVBoxLayout(editDialog);

    QScrollArea *scrollArea = new QScrollArea(editDialog);
    scrollArea->setWidgetResizable(true);

    QWidget *scrollWidget = new QWidget();
    QVBoxLayout *scrollLayout = new QVBoxLayout(scrollWidget);

    for (int i = 0; i < bkd.datablocks[index]._details.ports.size(); i++)
    {
        const port& currentPort = bkd.datablocks[index]._details.ports[i];
        QString portAsString = "";
        if (i != 0){portAsString += "\n";}
        portAsString += currentPort.protocol + " & " + QString::number(currentPort.port) + " port";

        QLabel *label1 = new QLabel(portAsString);
        label1->setFont(QFont("Monospace", 12, QFont::Bold));
        label1->setStyleSheet("color: rgb(255, 255, 255);");
        scrollLayout->addWidget(label1);

        if (currentPort.protocol == "HTTP")
        {
            QLabel *label10 = new QLabel("Title: " + currentPort.http_title);
            label10->setFont(QFont("Monospace", 10));
            label10->setStyleSheet("color: rgb(232, 172, 50);");
            scrollLayout->addWidget(label10);
        }

        if (currentPort.protocol == "FTP")
        {
            QLabel *label11 = new QLabel("Title: " + currentPort.description);
            label11->setFont(QFont("Monospace", 10));
            label11->setStyleSheet("color: rgb(232, 172, 50);");
            scrollLayout->addWidget(label11);
        }

        if (currentPort.passwd.length() > 1)
        {
            QStringList parts = currentPort.passwd.split(":"); QString pass; QString login;
            if (parts.size() == 2)
            {
                login = parts[0]; // "admin"
                pass = parts[1].remove("@"); // "ftp"
            }
            QString use = get_program_on_view(currentPort.protocol);

            QLabel *label0 = new QLabel("Cracked:");
            label0->setFont(QFont("Monospace", 10));
            if (use != "") {label0->setText("Cracked, on view use (" + use + "):");}
            label0->setStyleSheet("color: rgb(34, 255, 0);");
            scrollLayout->addWidget(label0);

            QLabel *label2 = new QLabel("Login:");
            label2->setStyleSheet("color: rgb(255, 255, 255);");
            label2->setFont(QFont("Monospace", 9));
            scrollLayout->addWidget(label2);

            QLineEdit *lineEdit = new QLineEdit;
            lineEdit->setText(login);
            lineEdit->setStyleSheet("color: rgb(34, 255, 0); border: 0.5px solid white;");
            lineEdit->setFont(QFont("Monospace", 9));
            lineEdit->setReadOnly(true);
            scrollLayout->addWidget(lineEdit);

            QObject::connect(lineEdit, &QLineEdit::selectionChanged, [=]() {
                QClipboard *clipboard = QGuiApplication::clipboard();
                QString selectedText = lineEdit->selectedText();
                clipboard->setText(lineEdit->selectedText());
            });

            QLabel *label3 = new QLabel("Password:");
            label3->setStyleSheet("color: rgb(255, 255, 255);");
            label3->setFont(QFont("Monospace", 9));
            scrollLayout->addWidget(label3);

            QLineEdit *lineEdit1 = new QLineEdit;
            lineEdit1->setText(pass);
            lineEdit1->setStyleSheet("color: rgb(34, 255, 0); border: 0.5px solid white;");
            lineEdit1->setFont(QFont("Monospace", 9));
            lineEdit1->setReadOnly(true);
            scrollLayout->addWidget(lineEdit1);

            QObject::connect(lineEdit1, &QLineEdit::selectionChanged, [=]() {
                QClipboard *clipboard = QGuiApplication::clipboard();
                clipboard->setText(lineEdit1->selectedText());
            });

        }
    }

    scrollWidget->setLayout(scrollLayout);
    scrollArea->setWidget(scrollWidget);

    layout->addWidget(scrollArea);
    editDialog->setLayout(layout);
    editDialog->exec();
}

void nesca_viewer::add_block_on_grid(const QString &http_title, const QString &dns, const QString &node, const QString &rtt,
                                     QGridLayout *layout, int row, int col, const QString &group_name, int datablock_index)
{
    QString formatted_dns;
    for (int i = 0; i < dns.length(); i += 30)
    {
        formatted_dns += dns.mid(i, 30) + "\n";
    }
    formatted_dns.chop(1);

    QString formatted_title;
    for (int i = 0; i < http_title.length(); i += 30)
    {
        formatted_title += http_title.mid(i, 30) + "\n";
    }
    formatted_title.chop(1);

    QGroupBox *groupBox = new QGroupBox(group_name);
    QVBoxLayout *groupBoxLayout = new QVBoxLayout(groupBox);
    groupBox->setStyleSheet("background-color: rgb(0, 0, 0);");

    displayed_block_info displayedBlockInfo;
    displayedBlockInfo.group_box = groupBox;
    displayedBlockInfo.data_index = datablock_index;
    bkd.displayed_block_info_list.append(displayedBlockInfo);

    QHBoxLayout *horizontalLayout = new QHBoxLayout();

    QPixmap pixmap;
    pixmap.load("resources/no_image_255_255.png");
    int port_80_index = bkd.search_port_80_and_screenshot(datablock_index);
    if (port_80_index != -1)
    {
        if (!bkd.datablocks[datablock_index]._details.ports[port_80_index].screenshot.isNull())
        {
            pixmap.load(df.path_temp + "/" + bkd.datablocks[datablock_index].ip_address + ".png");
        }
    }

    QPixmap scaledPixmap = pixmap.scaled(QSize(225, 225), Qt::KeepAspectRatio);
    ClickableLabel *imageLabel = new ClickableLabel;
    imageLabel->setPixmap(scaledPixmap);

    connect(imageLabel, &ClickableLabel::clicked, [=]() {open_image(pixmap);});

    QLabel *label = new QLabel(formatted_title);
    label->setStyleSheet("color: rgb(232, 172, 50);");

    QLabel *label_dns = new QLabel(formatted_dns);
    label_dns->setStyleSheet("color: rgb(220, 220, 0);");

    QLabel *rttLabel = new QLabel(rtt+"ms");
    rttLabel->setStyleSheet("color: rgb(34, 255, 0);");

    for (int i = 0; i < bkd.datablocks[datablock_index]._details.ports.size(); i++)
    {
        if (bkd.datablocks[datablock_index]._details.ports[i].passwd.length() > 1)
        {
            QLabel *bruteLabel = new QLabel("It`s cracked...");
            bruteLabel->setStyleSheet("color: rgb(255, 38, 38);");
            groupBoxLayout->addWidget(bruteLabel);
            break;
        }
    }

    groupBoxLayout->addWidget(imageLabel);
    horizontalLayout->addWidget(label);
    horizontalLayout->addStretch();
    horizontalLayout->addWidget(rttLabel);
    groupBoxLayout->addLayout(horizontalLayout);

    bool http_port_found = false;
    for (int i = 0; i < bkd.datablocks[datablock_index]._details.ports.size(); i++){
        if (bkd.datablocks[datablock_index]._details.ports[i].port == 80)
        {
            QLabel *label1 = new QLabel(node);
            QString linkText = "<a style=\"color: rgb(46, 139, 87); text-decoration: underline;\" href=\"http://" + node + "\">" + node + "</a>";
            label1->setText(linkText);
            label1->setOpenExternalLinks(true);
            groupBoxLayout->addWidget(label1);
            http_port_found = true;
            break;
        }
    }

    if (!http_port_found)
    {
        QLabel *label1 = new QLabel(node);
        label1->setStyleSheet("color: rgb(46, 139, 87);");
        groupBoxLayout->addWidget(label1);

        label->setStyleSheet("color: rgb(232, 172, 50);");
        label->setText(bkd.datablocks[datablock_index]._details.ports[0].protocol);
    }

    groupBoxLayout->addWidget(label_dns);

    if (bkd.datablocks[datablock_index]._details.ports.size() > 1 || bkd.datablocks[datablock_index]._details.ports[0].protocol != "HTTP")
    {
        QPushButton *edit_button = new QPushButton("more");
        connect(edit_button, &QPushButton::clicked, [=]() {edit_button_click(datablock_index);});
        edit_button->setStyleSheet("QPushButton{color: rgb(255, 255, 255);}QPushButton:pressed{background-color: rgb(42, 42, 42);}");
        groupBoxLayout->addWidget(edit_button);
    }

    groupBox->setLayout(groupBoxLayout);
    layout->addWidget(groupBox, row, col);
}

void nesca_viewer::parsed_next_files()
{
    if (df.i_jsons < df.file_paths_json.size())
    {
        QString _file_path_json = df.file_paths_json[df.i_jsons];
        df.i_jsons++;
        QList<datablock> _json_datablocks = parse_json_files(_file_path_json);
        log_plain("Parsed " + _file_path_json + " file");
        bkd.datablocks += _json_datablocks;
        parsed_files++;
    }

    if (df.i_xmls < df.file_paths_xml.size())
    {
        QString _file_path_xml = df.file_paths_xml[df.i_xmls];
        df.i_xmls++;
        QList<datablock> _xml_datablocks = parse_xml_files(_file_path_xml);
        log_plain("Parsed " + _file_path_xml + " file");
        bkd.datablocks += _xml_datablocks;
        parsed_files++;
    }
}

nesca_viewer::~nesca_viewer()
{
    delete ui;
}

void nesca_viewer::on_pushButton_clicked()
{
}

void nesca_viewer::on_pushButton_3_clicked()
{

}

void nesca_viewer::on_pushButton_2_clicked()
{
    options o;
    o.setModal(true);
    o.exec();
}

