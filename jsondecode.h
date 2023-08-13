/*
 * NESCA-VIEWER
 * by oldteam & lomaster
 * license CC-BY-NC-SA 4.0
 *   Сделано от души 2023.
*/

#ifndef JSONDECODE_H
#define JSONDECODE_H
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

struct port
{
    int port;
    QString protocol;
    QString http_title;
    QString screenshot;
    QString content;
    QString passwd;
};

struct __details
{
    QString dns_name;
    double rtt;
    QList<port> ports;
};

struct datablock
{
    QString ip_address;
    __details _details;
};

/*Функция для парсинга json в datablock.*/
datablock
parse_data_block(const QJsonObject& json_object);

/*Функция для парсинга сразу кучи файлов json,
в datablock-и.*/
QList<datablock>
parse_json_files(const QStringList& file_paths);


#endif
