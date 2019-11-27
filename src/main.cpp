#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <map>
#include <iomanip>

#include "ocam_model.hpp"
#include "util.hpp"
#include "read_dump_files.hpp"
#include "draw_satellites.hpp"

using namespace cv;
using namespace std;

template <typename T>
bool compare(const T &a, const T &b)
{
    return a.first < b.first;
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        std::cout << "specify base directory" << std::endl;
        return 0;
    }
    std::string base_dir(argv[1]);
    std::cout << "base dir: " << base_dir << std::endl;
    auto sats = read_satellites(base_dir + "satellites.csv");
    auto imus = read_imu(base_dir + "imu.csv");
    int radius = calc_lens_radius();

    vector<string> filenames;
    search_dir(base_dir + "sky/", filenames);
    vector<pair<int, string>> filename_sorted;
    for (const auto &fn : filenames)
    {
        string sfn = getFileName(fn);
        string nums = sfn.substr(0, sfn.find('_'));
        try
        {
            int num = stoi(nums);
            filename_sorted.push_back({num, fn});
        }
        catch (const std::invalid_argument &e)
        {
            ;
        }
    }
    sort(filename_sorted.begin(), filename_sorted.end());

    int idx = 0;
    while (true)
    {
        auto fnp = filename_sorted[idx];
        string fn = getFileName(fnp.second);
        cout << endl
             << "file: " << fn;
        int lpos = fn.find_first_of('_'), rpos = fn.find_last_of('_');
        string stamp = fn.substr(lpos + 1, rpos - lpos - 1);
        string sec = stamp.substr(0, 10);
        string nsec = stamp.substr(10, stamp.size() - 10);
        double time = stod(sec + "." + nsec);
        cout << ", " << time << endl;

        auto imu_itr = lower_bound(
            imus.begin(), imus.end(),
            make_pair(time, 0),
            compare<pair<double, double>>);
        if (imu_itr != imus.end())
            cout << "found imu time: " << imu_itr->first << endl;

        auto sat_itr = lower_bound(
            sats.begin(), sats.end(),
            make_pair(time, vector<satellite>()),
            compare<pair<double, vector<satellite>>>);
        if (sat_itr != sats.end())
        {
            cout << "found sat time: " << sat_itr->first << endl;
        }

        Mat image, rotated;
        image = imread(fnp.second, CV_LOAD_IMAGE_COLOR); // Read the file
        if (!image.data)
        {
            cout << "Could not open or find the image" << std::endl;
            return -1;
        }

        double gnss_map_angle = 0.50785606888;
        // rotate image
        cv::Point2f pc(image.cols / 2., image.rows / 2.);
        double rangle = rad2deg(imu_itr->second) - rad2deg(gnss_map_angle) + 180.0;
        cv::Mat tmp, rot = cv::getRotationMatrix2D(pc, rangle, 1.0);
        cv::warpAffine(image, tmp, rot, image.size()); // what size I should use?
        cv::flip(tmp, rotated, 1);
        drawText(rotated, to_string(time), cv::Point(200, 60), 1, cv::Scalar(0, 255, 0), 4);

        drawSats(rotated, cv::Point(800, 600), radius + (3 - radius % 3), 3, sat_itr->second);

        resize(rotated, rotated, cv::Size(), 0.7, 0.7);

        namedWindow("Display window", WINDOW_AUTOSIZE); // Create a window for display.
        imshow("Display window", rotated);              // Show our image inside it.

        char c = waitKey();
        if (c == 32)
            idx--;
        else
            idx++;
        if (idx < 0)
            idx = filename_sorted.size() - 1;
        if (idx >= filename_sorted.size())
            idx = 0;
    }

    return 0;
}
