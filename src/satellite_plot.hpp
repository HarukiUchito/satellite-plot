#ifndef SATELLITE_PLOT_HPP
#define SATELLITE_PLOT_HPP

#include <string>
#include <vector>

#include "read_dump_files.hpp"


class SatellitePlot {
public:
    SatellitePlot(int argc, char **argv);

    void run();
private:
    int radius_;
    std::vector<std::pair<double, std::vector<satellite>>> sats_;
    std::vector<std::pair<double, double>> imus_;
    std::vector<std::pair<int, std::string>> filename_sorted_;
};

#endif