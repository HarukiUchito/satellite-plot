#ifndef DRAW_SATELLITES_HPP
#define DRAW_SATELLITES_HPP

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
using namespace cv;
#include <vector>

#include "read_dump_files.hpp"

#include <math.h>
#define rad2deg(a) ((a) / M_PI * 180.0)
#define deg2rad(a) ((a) / 180.0 * M_PI)

void circleWithEdge(Mat &mat, Point center, int radius, cv::Scalar color, int thickness);

void drawText(Mat &mat, std::string text, cv::Point center, double fontScale, cv::Scalar color, int thickness);

void drawSatellite(cv::Mat &mat, cv::Point center, int radius, cv::Scalar color, satellite sat);

void drawSats(cv::Mat &mat, cv::Point center, int radius, int d, std::vector<satellite> sats);

#endif