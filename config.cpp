/*
 * NESCA-VIEWER
 * by oldteam & lomaster
 * license CC-BY-NC-SA 4.0
 *   Сделано от души 2023.
*/

#include "config.h"

void config::load_from_file(const QString& file_path)
{
    QFile configFile(file_path);
    if (!configFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Error: Could not open config file" << file_path;
        return;
    }

    QTextStream in(&configFile);
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty() || line.startsWith("#") || line.startsWith(";")) {
            continue;
        }

        QStringList parts = line.split("=");
        if (parts.size() == 2) {
            config_data[parts[0].trimmed()] = parts[1].trimmed();
        }
    }
    configFile.close();
}

QString config::get_value(const QString& key)
{
    return config_data.value(key, "");
}

void config::set_value(const QString& key, const QString& value)
{
    config_data[key] = value;
}

void config::save_to_file(const QString& file_path)
{
    QFile config_file(file_path);
    if (!config_file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Error: Could not open config file for writing" << file_path;
        return;
    }

    QTextStream out(&config_file);
    for (const QString& key : config_data.keys())
    {
        out << key << "=" << config_data.value(key) << "\n";
    }

    config_file.close();
}
