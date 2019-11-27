#include "draw_satellites.hpp"

void circleWithEdge(Mat &mat, Point center, int radius, cv::Scalar color, int thickness)
{
    circle(mat, center, radius, Scalar(255, 255, 255), thickness + 1, CV_AA);
    circle(mat, center, radius, color, thickness, CV_AA);
}

void drawText(Mat &mat, std::string text, cv::Point center, double fontScale, cv::Scalar color, int thickness)
{
    int baseline = 0;
    Size textSize = getTextSize(text, FONT_HERSHEY_SIMPLEX,
                                fontScale, thickness, &baseline);
    baseline += thickness;

    // テキストをセンタリングします．
    Point textOrg = center - Point(textSize.width / 2, textSize.height / 2);

    // 矩形を描画します．
    //rectangle(mat, textOrg + Point(0, baseline, textOrg + Point(textSize.width, -textSize.height), Scalar(0,0,255));
    // ... 次に，まずベースラインを描画します．
    //line(mat, textOrg + Point(0, thickness), textOrg + Point(textSize.width, thickness), Scalar(0, 0, 255));

    putText(mat, text, textOrg, FONT_HERSHEY_SIMPLEX, fontScale, color, thickness, CV_AA);
}

void drawSatellite(cv::Mat &mat, cv::Point center, int radius, cv::Scalar color, satellite sat)
{
    if (sat.azimuth == 0.0 and sat.elevation == 0.0) return;
    //std::cout << radius << " elev: " << sat.elevation << std::endl;
    double d = double(radius) * (90.0 - sat.elevation) / 90.0; // distance to center
    double rad = deg2rad(sat.azimuth + 270.0);
    double x = d * cos(rad), y = d * sin(rad);
    //ROS_INFO("x = %f, y = %f", x, y);
    circle(mat, center + cv::Point(x, y), 10, Scalar(255, 255, 255), -1, CV_AA);
    circle(mat, center + cv::Point(x, y), 13, Scalar(0, 0, 0), 6, CV_AA);
    //    circleWithEdge(mat, center + cv::Point(x, y), 34, Scalar(255, 255, 255), 30);
    drawText(mat, sat.name, center + cv::Point(x, y + 20 - 30), 1.2, Scalar(255, 0, 0), 2.0); //color, 1.5);
}

void drawSats(cv::Mat &mat, cv::Point center, int radius, int d, std::vector<satellite> sats)
{
    assert(radius % d == 0);
    int unit = radius / d;
    cv::Scalar col(0, 255, 0);
    circleWithEdge(mat, center, 1, col, 0.3);
    for (int i = 0; i < d; ++i)
    {
        circleWithEdge(mat, center, unit * (i + 1), col, 0.3);
        drawText(mat, std::to_string(90 - 90 / d * (i + 1)), center - cv::Point(0, unit * (i + 1) - 20), 0.5, col, 1);
    }
    drawText(mat, "N", center - cv::Point(0, radius + 10), 2, col, 4);
    drawText(mat, "S", center + cv::Point(0, radius + 95), 2, col, 4);
    drawText(mat, "W", center - cv::Point(radius + 40, 0), 2, col, 4);
    drawText(mat, "E", center + cv::Point(radius + 40, 0), 2, col, 4);

    for (auto sat : sats)
    {
        //std::cout << sat.name << " " << sat.elevation << " " << sat.azimuth << std::endl;
        drawSatellite(mat, center, radius, Scalar(0, 0, 255), sat);
    }
}