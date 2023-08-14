/*
 * NESCA-VIEWER
 * by oldteam & lomaster
 * license CC-BY-NC-SA 4.0
 *   Сделано от души 2023.
*/

#include "readfiles.h"
#include <QXmlStreamReader>
#include <QFile>
#include <QList>
#include <QDebug>

datablock parse_data_block_nmap(QXmlStreamReader &xml)
{
    datablock block;
    __details details;
    port currentPort;
    bool no_open = false;

    while (!xml.atEnd() && !xml.hasError())
    {
        xml.readNext();
        if (xml.isStartElement())
        {
            QString elementName = xml.name().toString();
            if (elementName == "address")
            {
                if (xml.attributes().hasAttribute("addrtype") && xml.attributes().value("addrtype").toString() == "ipv4")
                {
                    block.ip_address = xml.attributes().value("addr").toString();
                }
            }
            else if (elementName == "hostname")
            {
                if (xml.attributes().hasAttribute("name"))
                {
                    details.dns_name = xml.attributes().value("name").toString();
                }
            }
            else if (elementName == "times")
            {
                if (xml.attributes().hasAttribute("rttvar"))
                {
                    QString rttvarStr = xml.attributes().value("rttvar").toString();
                    // Replace comma with period in the string and then convert to double
                    rttvarStr.replace(",", ".");
                    details.rtt = xml.attributes().value("rttvar").toString().toDouble() / 1000.0;
                }
            }
            else if (elementName == "port")
            {
                currentPort.port = xml.attributes().value("portid").toString().toInt();
                currentPort.protocol = xml.attributes().value("protocol").toString();
            }
            else if (elementName == "state")
            {
                currentPort.state = xml.attributes().value("state").toString();
            }
        }
        else if (xml.isEndElement())
        {
            QString elementName = xml.name().toString();
            if (elementName == "port")
            {
                details.ports.append(currentPort);
                currentPort = port();
            }
            else if (elementName == "host")
            {
                block._details = details;
                break; // Exit the loop after processing a host
            }
        }
    }

    return block;
}

QList<datablock> parse_xml_files(QString& file_path)
{
    QList<datablock> dataBlocks;
    QFile file(file_path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Failed to open the file:" << file_path;
        file.close();
        return {};
    }

    QXmlStreamReader xml(&file);
    while (!xml.atEnd() && !xml.hasError())
    {
        xml.readNextStartElement();
        if (xml.isStartElement() && xml.name().toString() == "host")
        {
            datablock block = parse_data_block_nmap(xml);

            // Check if the state is "open" before adding to dataBlocks
            if (!block._details.ports.isEmpty() && block._details.ports.first().state == "open")
            {
                dataBlocks.append(block);
            }
        }
    }

    file.close();

    return dataBlocks;
}

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

QList<datablock> parse_json_files(QString& file_path)
{
    QList<datablock> dataBlocks;

    QFile file(file_path);
    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "Unable to open file:" << file_path;
        file.close();
        return {};
    }

    QJsonParseError error;
    QByteArray jsonData = file.readAll();
    file.close();

    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData, &error);
    if (error.error != QJsonParseError::NoError)
    {
        file.close();
        return {};
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

    return dataBlocks;
}

