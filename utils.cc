/*
 * NESCA-VIEWER
 * by oldteam & lomaster
 * license CC-BY-NC-SA 4.0
 *   Сделано от души 2023.
*/

#include "utils.h"

int count_json_files(const QString &folderPath)
{
    QDir dir(folderPath);
    QStringList filters;
    filters << "*.json";
    dir.setNameFilters(filters);

    return dir.entryList().count();
}
bool
save_binary_file(const QString& file_name, const unsigned char* buffer, size_t file_size)
{
    std::string utf8_text;
    utf8_text = file_name.toStdString();
    std::ofstream file(utf8_text, std::ios::out | std::ios::binary);

    if (!file.is_open())
    {
        std::cerr << "Failed to open file for writing." << std::endl;
        return false;
    }

    file.write(reinterpret_cast<const char*>(buffer), file_size);

    if (!file.good())
    {
        std::cerr << "Failed to write to file." << std::endl;
        return false;
    }

    file.close();
    return true;
}

QStringList
get_all_file_paths_in_folder(const QString &folder_path, const QStringList &name_filters)
{
    QDir dir(folder_path);
    dir.setNameFilters(name_filters);

    QStringList filePaths;

    QFileInfoList fileInfoList = dir.entryInfoList(QDir::Files);
    foreach (const QFileInfo &fileInfo, fileInfoList)
    {
        filePaths.append(fileInfo.filePath());
    }

    return filePaths;
}

const unsigned char*
base64_decode(const std::string &input, size_t &output_length)
{
    const std::string base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    size_t length = input.length();
    if (length % 4 != 0) {
        std::cerr << "Invalid Base64 input" << std::endl;
        return nullptr;
    }

    size_t padding = 0;
    if (length > 0 && input[length - 1] == '=') {
        padding++;
    }
    if (length > 1 && input[length - 2] == '=') {
        padding++;
    }

    output_length = (length / 4) * 3 - padding;
    unsigned char* decoded_data = new unsigned char[output_length];

    size_t i = 0, j = 0;
    while (i < length) {
        unsigned char a = input[i] == '=' ? 0 : base64_chars.find(input[i]);
        unsigned char b = input[i + 1] == '=' ? 0 : base64_chars.find(input[i + 1]);
        unsigned char c = input[i + 2] == '=' ? 0 : base64_chars.find(input[i + 2]);
        unsigned char d = input[i + 3] == '=' ? 0 : base64_chars.find(input[i + 3]);

        decoded_data[j++] = (a << 2) | (b >> 4);
        decoded_data[j++] = (b << 4) | (c >> 2);
        decoded_data[j++] = (c << 6) | d;

        i += 4;
    }

    return decoded_data;
}
