#include "satellite_plot.hpp"

#include "util.hpp"
#include "draw_satellites.hpp"
#include "ocam_model.hpp"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
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

template <typename T>
bool compare(const T &a, const T &b)
{
    return a.first < b.first;
}

void SatellitePlot::run()
{
    int idx = 0;
    while (true)
    {
        auto fnp = filename_sorted_[idx];
        std::string fn = getFileName(fnp.second);
        std::cout << std::endl << "file: " << fn;
        int lpos = fn.find_first_of('_'), rpos = fn.find_last_of('_');
        std::string stamp = fn.substr(lpos + 1, rpos - lpos - 1);
        std::string sec = stamp.substr(0, 10);
        std::string nsec = stamp.substr(10, stamp.size() - 10);
        double time = std::stod(sec + "." + nsec);
        std::cout << ", " << time << std::endl;

        auto imu_itr = std::lower_bound(
            imus_.begin(), imus_.end(),
            std::make_pair(time, 0),
            compare<std::pair<double, double>>);
        if (imu_itr != imus_.end())
            std::cout << "found imu time: " << imu_itr->first << std::endl;

        std::cout << "sat time: " << time << std::endl;
        auto sat_itr = std::lower_bound(
            sats_.begin(), sats_.end(),
            std::make_pair(time, std::vector<satellite>()),
            compare<std::pair<double, std::vector<satellite>>>);
        if (sat_itr != sats_.end())
        {
            std::cout << "found sat time: " << sat_itr->first << std::endl;
        }

        cv::Mat image, rotated;
        image = cv::imread(fnp.second, CV_LOAD_IMAGE_COLOR); // Read the file
        if (!image.data)
        {
            std::cout << "Could not open or find the image" << std::endl;
            exit(-1);
        }

        double gnss_map_angle = 0.50785606888;
        // rotate image
        cv::Point2f pc(image.cols / 2., image.rows / 2.);
        double rangle = rad2deg(imu_itr->second) - rad2deg(gnss_map_angle) + 180.0;
        cv::Mat tmp, rot = cv::getRotationMatrix2D(pc, rangle, 1.0);
        cv::warpAffine(image, tmp, rot, image.size()); // what size I should use?
        cv::flip(tmp, rotated, 1);
        drawText(rotated, std::to_string(time), cv::Point(200, 60), 1, cv::Scalar(0, 255, 0), 4);

        drawSats(rotated, cv::Point(800, 600), radius_ + (3 - radius_ % 3), 3, sat_itr->second);

        resize(rotated, rotated, cv::Size(), 0.7, 0.7);

        namedWindow("Display window", WINDOW_AUTOSIZE); // Create a window for display.
        imshow("Display window", rotated);              // Show our image inside it.

        char c = cv::waitKey();
        if (c == 32)
            idx--;
        else
            idx++;
        if (idx < 0)
            idx = filename_sorted_.size() - 1;
        if (idx >= filename_sorted_.size())
            idx = 0;
    }
}