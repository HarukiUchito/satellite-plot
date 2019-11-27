#include "read_dump_files.hpp"
#include <iostream>
#include <fstream>
#include <tf/tf.h>

using namespace std;

vector<pair<double, double>> read_imu(string filename)
{
    fstream fin(filename, ios::in);
    vector<pair<double, double>> ret;
    string line, seg;
    bool got_first = false;
    double first_yaw;
    int seqcnt = 0;
    getline(fin, line); // ignore header
    while (getline(fin, line))
    {
        stringstream liness(line);
        string stamp;
        double x, y, z, w;
        for (int i = 0; i < 11; ++i)
        {
            getline(liness, seg, ',');
            if (i < 2)
                continue;
            if (i == 2)
                stamp += seg + ".";
            if (i == 3)
                stamp += seg;
            else if (i > 6)
            {
                double v = stod(seg);
                if (i == 7)
                    x = v;
                if (i == 8)
                    y = v;
                if (i == 9)
                    z = v;
                if (i == 10)
                    w = v;
            }
        }
        tf::Quaternion tq(x, y, z, w);
        double er = 0, ep = 0, ey = 0;
        tf::Matrix3x3(tq).getRPY(er, ep, ey);
        if (not got_first)
        {
            got_first = true;
            first_yaw = ey;
        }
        ret.push_back({stod(stamp), ey - first_yaw});
        seqcnt++;
    }
    cout << "read " << seqcnt << " lines." << endl;
    return ret;
}

vector<pair<double, vector<satellite>>> read_satellites(string filename)
{
    fstream fin(filename, ios::in);
    vector<pair<double, vector<satellite>>> ret;
    string line, seg;
    getline(fin, line); // ignore header
    int seqcnt = 0;
    while (getline(fin, line))
    {
        string stamp;
        stringstream liness(line);
        for (int i = 0; i < 5; ++i)
        {
            getline(liness, seg, ',');
            if (i < 2)
                continue;
            if (i == 2)
                stamp += seg + ".";
            if (i == 3)
                stamp += seg;
            if (i == 5)
                cout << seg << endl;
        }
        string satstr;
        getline(liness, satstr);
        satstr = satstr.substr(2, satstr.size() - 4);

        vector<satellite> sats;
        string elm;
        cout << setprecision(20);
        while (true)
        {
            satellite sat;
            int pos = satstr.find("name: ");
            if (pos == std::string::npos)
                break;
            satstr = satstr.substr(6, satstr.size() - 6); // delete "name: "

            pos = satstr.find("azimuth: ");
            sat.name = satstr.substr(0, pos);
            sat.name = sat.name.substr(1, sat.name.size() - 2);
            satstr = satstr.substr(pos, satstr.size() - pos);
            satstr = satstr.substr(9, satstr.size() - 9); // delete "azimuth: "

            pos = satstr.find("elevation: ");
            sat.azimuth = stod(satstr.substr(0, pos));
            satstr = satstr.substr(pos, satstr.size() - pos);
            satstr = satstr.substr(11, satstr.size() - 11);

            pos = satstr.find(", ");
            if (pos == std::string::npos)
            {
                sat.elevation = stod(satstr);
                sats.push_back(sat);
                break;
            }
            else
            {
                sat.elevation = stod(satstr.substr(0, pos));
                sats.push_back(sat);
                satstr = satstr.substr(pos, satstr.size() - pos);
                satstr = satstr.substr(2, satstr.size() - 2);
            }
        }
        ret.push_back({stod(stamp), sats});

        seqcnt++;
    }
    cout << "read " << seqcnt << " lines." << endl;
    return ret;
}
