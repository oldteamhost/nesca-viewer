/*
 * NESCA-VIEWER
 * by oldteam & lomaster
 * license CC-BY-NC-SA 4.0
 *   Сделано от души 2023.
*/

#ifndef CONFIG_H
#define CONFIG_H
#include <QFile>
#include <QTextStream>
#include <QHash>
#include <QString>
#include <QDebug>

#define CONFIG_PATH "resources/nescaviewer.cfg"
#define CONFIG_DATA "data_files_path"
#define CONFIG_TEMP "temp_files_path"
#define CONFIG_LOAD "blocks_loading"

class config
{
public:
    config() {}
    void load_from_file(const QString& file_path);
    QString get_value(const QString& key);
    void set_value(const QString& key, const QString& value);
    void save_to_file(const QString& file_path);
private:
    QHash<QString, QString> config_data;
};

#endif // CONFIG_H
