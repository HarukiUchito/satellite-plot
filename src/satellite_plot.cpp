#include "satellite_plot.hpp"

#include "util.hpp"
#include "draw_satellites.hpp"
#include "ocam_model.hpp"

#include <iostream>

SatellitePlot::SatellitePlot(int argc, char **argv)
{
    if (argc < 2)
    {
        std::cout << "specify base directory" << std::endl;
        exit(0);
    }
    std::string base_dir(argv[1]);
    std::cout << "base dir: " << base_dir << std::endl;
    sats_ = read_satellites(base_dir + "satellites.csv");
    imus_ = read_imu(base_dir + "imu.csv");
    radius_ = calc_lens_radius();
    center_ = calc_center();

    gnss_map_angle_ = 0.48785606888;

    std::vector<std::string> filenames;
    search_dir(base_dir + "sky/", filenames);
    for (const auto &fn : filenames)
    {
        std::string sfn = getFileName(fn);
        std::string nums = sfn.substr(0, sfn.find('_'));
        try
        {
            int num = stoi(nums);
            filename_sorted_.push_back({num, fn});
        }
        catch (const std::invalid_argument &e)
        {
            ;
        }
    }
    std::sort(filename_sorted_.begin(), filename_sorted_.end());
}

void SatellitePlot::run()
{
    while (true)
    {
        auto fnp = filename_sorted_[current_frame_idx_];
        double time {parseTime(getFileName(fnp.second))};

        cv::Mat rotated {read_image(fnp.second)};
        // rotate image
        cv::Point2f pc(center_.first, center_.second);
        double rangle = rad2deg(nearest_imu_yaw(time)) - rad2deg(gnss_map_angle_) + 180.0;
        cv::Mat tmp, rot = cv::getRotationMatrix2D(pc, rangle, 1.0);
        cv::warpAffine(rotated, tmp, rot, rotated.size()); // what size I should use?
        cv::flip(tmp, rotated, 1);
        drawText(rotated, std::to_string(time), cv::Point(200, 60), 1, cv::Scalar(0, 255, 0), 4);

        drawSats(rotated, pc, radius_ + (3 - radius_ % 3), 3, nearest_sats(time));

        resize(rotated, rotated, cv::Size(), 0.7, 0.7);

        // Create a window for display.
        namedWindow("Display window", WINDOW_AUTOSIZE);
        imshow("Display window", rotated);

        // 'Enter Key' to go previous frame, 'Other Key' to next.
        processInput();
    }
}

double SatellitePlot::parseTime(std::string filename) const
{
    std::cout << std::endl << "file: " << filename;
    int lpos = filename.find_first_of('_'), rpos = filename.find_last_of('_');
    std::string stamp = filename.substr(lpos + 1, rpos - lpos - 1);
    std::string sec = stamp.substr(0, 10);
    std::string nsec = stamp.substr(10, stamp.size() - 10);
    return std::stod(sec + "." + nsec);
}

cv::Mat SatellitePlot::read_image(std::string path) const
{
    cv::Mat image;
    image = cv::imread(path, CV_LOAD_IMAGE_COLOR); // Read the file
    if (!image.data)
    {
        std::cout << "Could not open or find the image" << std::endl;
        exit(-1);
    }
    return image;
}

template <typename T>
bool compare(const T &a, const T &b)
{
    return a.first < b.first;
}

std::vector<satellite> SatellitePlot::nearest_sats(double time) const
{
    auto sat_itr = std::lower_bound(
        sats_.begin(), sats_.end(),
        std::make_pair(time, std::vector<satellite>()),
        compare<std::pair<double, std::vector<satellite>>>
    );
    if (sat_itr != sats_.end())
        std::cout << "found sat time: " << sat_itr->first << std::endl;
    return sat_itr->second;
}

double SatellitePlot::nearest_imu_yaw(double time) const
{
    auto imu_itr = std::lower_bound(
        imus_.begin(), imus_.end(),
        std::make_pair(time, 0),
        compare<std::pair<double, double>>
    );
    if (imu_itr != imus_.end())
        std::cout << "found imu time: " << imu_itr->first << std::endl;
    return imu_itr->second;
}

void SatellitePlot::processInput()
{
    char c = cv::waitKey();
    if (c == 32)
        current_frame_idx_--;
    else
        current_frame_idx_++;
    if (current_frame_idx_ < 0)
        current_frame_idx_ = filename_sorted_.size() - 1;
    if (current_frame_idx_ >= filename_sorted_.size())
        current_frame_idx_ = 0;
}