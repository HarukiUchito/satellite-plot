#ifndef OCAM_MODEL_HPP
#define OCAM_MODEL_HPP

#include <tuple>

#define CMV_MAX_BUF 1024
#define MAX_POL_LENGTH 64

struct ocam_model
{
    double pol[MAX_POL_LENGTH];    // the polynomial coefficients: pol[0] + x"pol[1] + x^2*pol[2] + ... + x^(N-1)*pol[N-1]
    int length_pol;                // length of polynomial
    double invpol[MAX_POL_LENGTH]; // the coefficients of the inverse polynomial
    int length_invpol;             // length of inverse polynomial
    double xc;                     // row coordinate of the center
    double yc;                     // column coordinate of the center
    double c;                      // affine parameter
    double d;                      // affine parameter
    double e;                      // affine parameter
    int width;                     // image width
    int height;                    // image height
};
int get_ocam_model(struct ocam_model *myocam_model, char *filename);

void cam2world(double point3D[3], double point2D[2], struct ocam_model *myocam_model);

void world2cam(double point2D[2], double point3D[3], struct ocam_model *myocam_model);

double calc_lens_radius();

std::pair<double, double> calc_center();

#endif