#ifndef SATELLITE_PLOT_HPP
#define SATELLITE_PLOT_HPP

#include <string>
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "read_dump_files.hpp"


class SatellitePlot {
public:
    SatellitePlot(int argc, char **argv);

    void run();
private:
    double gnss_map_angle_ {0.0};
    int current_frame_idx_ {0};
    int radius_ {0};
    std::pair<double, double> center_;
    std::vector<std::pair<double, std::vector<satellite>>> sats_;
    std::vector<satellite> nearest_sats(double time) const;

    std::vector<std::pair<double, double>> imus_;
    double nearest_imu_yaw(double time) const;

    cv::Mat read_image(std::string path) const;

    std::vector<std::pair<int, std::string>> filename_sorted_;

    void processInput();
    double parseTime(std::string filename) const;
};

#endif