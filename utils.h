/*
 * NESCA-VIEWER
 * by oldteam & lomaster
 * license CC-BY-NC-SA 4.0
 *   Сделано от души 2023.
*/

#ifndef UTILS_H
#define UTILS_H
#include <QDir>
#include <QStringList>
#include <iostream>
#include <fstream>
#include <vector>
#include <QtCore/QByteArray>
#include <QtCore/QDebug>

int count_json_files(const QString &folderPath);

QStringList
get_all_file_paths_in_folder(const QString &folder_path, const QStringList &name_filters);

bool
save_binary_file(const QString& file_name, const unsigned char* buffer, size_t file_size);

bool
file_found(const QString& folder_path, const QString& file_name);

const unsigned char*
base64_decode(const std::string &input, size_t &output_length);
#endif
