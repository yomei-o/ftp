#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include "filesystem_.h"

int is_directory(const char* path)
{
	if (path == NULL)return false;
	return std::filesystem::is_directory(path);
}

std::string get_file_prop_string(const std::filesystem::directory_entry& entry)
{
#if 0
    return entry.path().filename().string();
#else
    std::string ret,tmp;
    bool f = entry.is_directory();
    long long fsz = 0;
    try{
        fsz=entry.file_size();
    }catch(...){}
    char c_fsz[32];
    sprintf(c_fsz,"%12lld",fsz);

    ret += f ? "d" : "-";
    ret += "rwxrwxrwx";
    ret += "  ";
    ret += "1";
    ret += "  ";
    ret += "ftp";
    ret += "  ";
    ret += "ftp";
    ret += "  ";
    ret += c_fsz ;
    ret += "  ";
    ret+="Jan   1  2000";
    ret += "  ";
    ret+= entry.path().filename().string();
    return ret;
#endif
}
void list_directory(const char* path, char* buffer, size_t size) {
    std::error_code ec;
    if (buffer == nullptr || size < 1)return;
    buffer[0] = 0;

    std::filesystem::path dir_path(path);

    if (!std::filesystem::exists(dir_path, ec) || !std::filesystem::is_directory(dir_path, ec)) {
        return;
    }

    size_t pos = 0;
    for (const auto& entry : std::filesystem::directory_iterator(dir_path, ec)) {
        if (ec) return;

        //std::string name = entry.path().filename().string();  // ファイル名のみ
        std::string name = get_file_prop_string(entry);
        size_t name_len = name.length();

        if (pos + name_len + 1 >= size) {
            return; // バッファが足りない
        }

        memcpy(buffer + pos, name.c_str(), name_len);
        pos += name_len;
        buffer[pos++] = '\n';  // 改行
    }

    if (pos < size) {
        buffer[pos] = '\0';  // 終端
    }
    else {
        buffer[size - 1] = '\0';  // 念のため
    }
}
