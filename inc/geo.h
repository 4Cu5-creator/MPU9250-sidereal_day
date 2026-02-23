#ifndef GEOMETRY_UTILS_HPP
#define GEOMETRY_UTILS_HPP

// We only put the function signatures here
int inverse_matrix_3(double A[3][3], double B[3][3]);
double average(int N, double *A);
double T(int l, int m, int n, double x, double y, double z);
double radius_estimation(int N, double *x, double *y, double *z);
double sidereal_day(double ang_vec);

#endif
