#ifndef READ_DUMP_HPP
#define READ_DUMP_HPP

#include <string>
#include <vector>
#include <tuple>

struct satellite
{
    std::string name;
    double azimuth;
    double elevation;
};

std::vector<std::pair<double, double>> read_imu(std::string filename);

std::vector<std::pair<double, std::vector<satellite>>> read_satellites(std::string filename);

#endif