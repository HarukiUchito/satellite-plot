#ifndef UTIL_HPP
#define UTIL_HPP

#include <vector>
#include <string>

void search_dir(std::string, std::vector<std::string> &);
std::string getBaseName(const std::string &filename);
std::string getFileName(const std::string &filename);

#endif