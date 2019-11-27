#include "util.hpp"

#include <dirent.h>
#include <sys/stat.h>
#include <iostream>
#include <string.h>

void search_dir(std::string path, std::vector<std::string> &fileNames)
{

    int i, dirElements;
    std::string search_path;

    struct stat stat_buf;
    struct dirent **namelist = NULL;

    // dirElements にはディレクトリ内の要素数が入る
    dirElements = scandir(path.c_str(), &namelist, NULL, NULL);

    if (dirElements == -1)
        std::cout << "ERROR" << std::endl;

    else
    {

        //ディレクトリかファイルかを順番に識別
        for (i = 0; i < dirElements; i += 1)
        {

            // "." と ".." を除く
            if ((strcmp(namelist[i]->d_name, ".\0") != 0) && (strcmp(namelist[i]->d_name, "..\0") != 0))
            {

                //search_pathには検索対象のフルパスを格納する
                search_path = path + std::string(namelist[i]->d_name);

                // ファイル情報の取得の成功
                if (stat(search_path.c_str(), &stat_buf) == 0)
                {
                    // ディレクトリだった場合の処理
                    if ((stat_buf.st_mode & S_IFMT) == S_IFDIR)
                        // 再帰によりディレクトリ内を探索
                        search_dir(path + std::string(namelist[i]->d_name) + "/", fileNames);

                    //ファイルだった場合の処理
                    else
                        fileNames.push_back(search_path);
                }

                // ファイル情報の取得の失敗
                else
                {
                    std::cout << "ERROR" << std::endl << std::endl;
                }
            }
        }
    }

    free(namelist);
    return;
}

std::string getBaseName(const std::string &filename)
{
    const std::string separator = "/";
    std::size_t last_separator = filename.find_last_of(separator);
    if (last_separator == std::string::npos)
        return std::string();
    return filename.substr(0, last_separator);
}
std::string getFileName(const std::string &filename)
{
    int length = getBaseName(filename).size();
    return filename.substr(length + 1, filename.size() - length);
}