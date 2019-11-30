#include "ocam_model.hpp"
#include "util.hpp"

#include <fstream>
#include <math.h>
#include <iostream>


int get_ocam_model(struct ocam_model *myocam_model, char *filename)
{
    double *pol = myocam_model->pol;
    double *invpol = myocam_model->invpol;
    double *xc = &(myocam_model->xc);
    double *yc = &(myocam_model->yc);
    double *c = &(myocam_model->c);
    double *d = &(myocam_model->d);
    double *e = &(myocam_model->e);
    int *width = &(myocam_model->width);
    int *height = &(myocam_model->height);
    int *length_pol = &(myocam_model->length_pol);
    int *length_invpol = &(myocam_model->length_invpol);
    FILE *f;
    char buf[CMV_MAX_BUF];
    int i;

    //Open file
    if (!(f = fopen(filename, "r")))
    {
        printf("File %s cannot be opened\n", filename);
        return -1;
    }

    //Read polynomial coefficients
    fgets(buf, CMV_MAX_BUF, f);
    fscanf(f, "\n");
    fscanf(f, "%d", length_pol);
    for (i = 0; i < *length_pol; i++)
    {
        fscanf(f, " %lf", &pol[i]);
    }

    //Read inverse polynomial coefficients
    fscanf(f, "\n");
    fgets(buf, CMV_MAX_BUF, f);
    fscanf(f, "\n");
    fscanf(f, "%d", length_invpol);
    for (i = 0; i < *length_invpol; i++)
    {
        fscanf(f, " %lf", &invpol[i]);
    }

    //Read center coordinates
    fscanf(f, "\n");
    fgets(buf, CMV_MAX_BUF, f);
    fscanf(f, "\n");
    fscanf(f, "%lf %lf\n", xc, yc);

    //Read affine coefficients
    fgets(buf, CMV_MAX_BUF, f);
    fscanf(f, "\n");
    fscanf(f, "%lf %lf %lf\n", c, d, e);

    //Read image size
    fgets(buf, CMV_MAX_BUF, f);
    fscanf(f, "\n");
    fscanf(f, "%d %d", height, width);

    fclose(f);
    return 0;
}
//------------------------------------------------------------------------------
void cam2world(double point3D[3], double point2D[2], struct ocam_model *myocam_model)
{
    double *pol = myocam_model->pol;
    double xc = (myocam_model->xc);
    double yc = (myocam_model->yc);
    double c = (myocam_model->c);
    double d = (myocam_model->d);
    double e = (myocam_model->e);
    int length_pol = (myocam_model->length_pol);
    double invdet = 1 / (c - d * e); // 1/det(A), where A = [c,d;e,1] as in the Matlab file

    double xp = invdet * ((point2D[0] - xc) - d * (point2D[1] - yc));
    double yp = invdet * (-e * (point2D[0] - xc) + c * (point2D[1] - yc));

    double r = sqrt(xp * xp + yp * yp); //distance [pixels] of  the point from the image center
    double zp = pol[0];
    double r_i = 1;
    int i;

    for (i = 1; i < length_pol; i++)
    {
        r_i *= r;
        zp += r_i * pol[i];
    }

    //normalize to unit norm
    double invnorm = 1 / sqrt(xp * xp + yp * yp + zp * zp);

    point3D[0] = invnorm * xp;
    point3D[1] = invnorm * yp;
    point3D[2] = invnorm * zp;
}
//------------------------------------------------------------------------------
void world2cam(double point2D[2], double point3D[3], struct ocam_model *myocam_model)
{
    double *invpol = myocam_model->invpol;
    double xc = (myocam_model->xc);
    double yc = (myocam_model->yc);
    double c = (myocam_model->c);
    double d = (myocam_model->d);
    double e = (myocam_model->e);
    int width = (myocam_model->width);
    int height = (myocam_model->height);
    int length_invpol = (myocam_model->length_invpol);
    double norm = sqrt(point3D[0] * point3D[0] + point3D[1] * point3D[1]);
    double theta = atan(point3D[2] / norm);
    double t, t_i;
    double rho, x, y;
    double invnorm;
    int i;

    if (norm != 0)
    {
        invnorm = 1 / norm;
        t = theta;
        rho = invpol[0];
        t_i = 1;

        for (i = 1; i < length_invpol; i++)
        {
            t_i *= t;
            rho += t_i * invpol[i];
        }

        x = point3D[0] * invnorm * rho;
        y = point3D[1] * invnorm * rho;

        point2D[0] = x * c + y * d + xc;
        point2D[1] = x * e + y + yc;
    }
    else
    {
        point2D[0] = xc;
        point2D[1] = yc;
    }
}

double calc_lens_radius()
{
    std::string ocam_param = getBaseName(__FILE__) + "/calib_results.txt";
    struct ocam_model om; // our ocam_models for the fisheye and catadioptric cameras
    std::cout << ocam_param << std::endl;
    get_ocam_model(&om, (char *)ocam_param.c_str());
    double point3D[3] = {-1, 0, 0};      // a sample 3D point
    double left[2], right[2], center[2]; // the image point in pixel coordinates
    world2cam(left, point3D, &om);
    point3D[0] = 1;
    world2cam(right, point3D, &om);
    int radius = abs(round(left[0]) - round(right[0])) / 2;
    return radius;
}

std::pair<double, double> calc_center()
{
    std::string ocam_param = getBaseName(__FILE__) + "/calib_results.txt";
    struct ocam_model om; // our ocam_models for the fisheye and catadioptric cameras
    get_ocam_model(&om, (char *)ocam_param.c_str());
    double point3D[3] = {0, 0, 1.0};      // a sample 3D point
    double center[2]; // the image point in pixel coordinates
    world2cam(center, point3D, &om);
    std::cout << center[1] << " " << center[0] << std::endl;
    return {center[1], center[0]};
}