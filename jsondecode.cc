/*
 * NESCA-VIEWER
 * by oldteam & lomaster
 * license CC-BY-NC-SA 4.0
 *   Сделано от души 2023.
*/

#include "jsondecode.h"

datablock parse_data_block(const QJsonObject& json_object)
{
    datablock dataBlock;
    dataBlock.ip_address = json_object["ip_address"].toString();
    if (json_object.contains("details") && json_object["details"].isObject()) {
        QJsonObject detailsJson = json_object["details"].toObject();
        dataBlock._details.dns_name = detailsJson["dns_name"].toString();
        dataBlock._details.rtt = detailsJson["rtt"].toDouble();

        if (detailsJson.contains("ports") && detailsJson["ports"].isArray()) {
            QJsonArray portsJsonArray = detailsJson["ports"].toArray();
            for (const QJsonValue& portValue : portsJsonArray) {
                if (portValue.isObject()) {
                    QJsonObject portJsonObject = portValue.toObject();
                    port port;
                    port.port = portJsonObject["port"].toInt();
                    port.protocol = portJsonObject["protocol"].toString();
                    port.http_title = portJsonObject["http_title"].toString();
                    port.screenshot = portJsonObject["screenshot"].toString();
                    port.content = portJsonObject["content"].toString();
                    port.passwd = portJsonObject["passwd"].toString();
                    dataBlock._details.ports.append(port);
                }
            }
        }
    }

    return dataBlock;
}

QList<datablock> parse_json_files(const QStringList& file_paths)
{
    QList<datablock> dataBlocks;

    for (const QString& filePath : file_paths) {
        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly)) {
            qDebug() << "Unable to open file:" << filePath;
            continue;
        }

        QJsonParseError error;
        QByteArray jsonData = file.readAll();
        file.close();

//        qDebug() << "Parsing JSON data from:" << filePath;
//        qDebug() << "JSON content:" << jsonData;

        QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData, &error);
//        qDebug() << "JSON document is an object:" << jsonDoc.isObject();
        if (error.error != QJsonParseError::NoError) {
//            qDebug() << "JSON parse error in file:" << filePath;
//            qDebug() << "Error code:" << error.error;
//            qDebug() << "Error description:" << error.errorString();
            continue;
        }

        if (jsonDoc.isArray()) {
            QJsonArray jsonArray = jsonDoc.array();
            for (const QJsonValue& value : jsonArray) {
                if (value.isObject()) {
                    QJsonObject jsonObject = value.toObject();
                    datablock dataBlock = parse_data_block(jsonObject);
                    dataBlocks.append(dataBlock);
                }
            }
        }
    }

    return dataBlocks;
}

