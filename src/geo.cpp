#include "../inc/geo.h"
#include <cmath> // Required for pow() and sqrt()

// Function to calculate the inverse of a 3x3 matrix
int inverse_matrix_3(double A[3][3], double B[3][3]) {
    double detA = 0;
    detA += A[0][0] * A[1][1] * A[2][2] + A[0][1] * A[1][2] * A[2][0] + A[0][2] * A[1][0] * A[2][1];
    detA -= A[0][0] * A[1][2] * A[2][1] + A[0][1] * A[1][0] * A[2][2] + A[0][2] * A[1][1] * A[2][0];

    if (detA == 0) return 1; // Singular matrix error

    B[0][0] = (A[1][1] * A[2][2] - A[2][1] * A[1][2]) / detA;
    B[0][1] = (A[2][0] * A[1][2] - A[1][0] * A[2][2]) / detA;
    B[0][2] = (A[1][0] * A[2][1] - A[2][0] * A[1][1]) / detA;

    B[1][0] = (A[2][1] * A[0][2] - A[0][1] * A[2][2]) / detA;
    B[1][1] = (A[0][0] * A[2][2] - A[2][0] * A[0][2]) / detA;
    B[1][2] = (A[2][0] * A[0][1] - A[0][0] * A[2][1]) / detA;

    B[2][0] = (A[0][1] * A[1][2] - A[1][1] * A[0][2]) / detA;
    B[2][1] = (A[1][0] * A[0][2] - A[0][0] * A[1][2]) / detA;
    B[2][2] = (A[0][0] * A[1][1] - A[1][0] * A[0][1]) / detA;

    return 0;
}

// Helper to calculate average
double average(int N, double *A) {
    double sum = 0;
    for (int i = 0; i < N; i++) {
        sum += A[i];
    }
    return sum / N;
}

// Mathematical function T(l,m,n)
double T(int l, int m, int n, double x, double y, double z) {
    return std::pow(x, l) * std::pow(y, m) * std::pow(z, n);
}

double radius_estimation(int N, double *x, double *y, double *z) {
    double ave_x = average(N, x);
    double ave_y = average(N, y);
    double ave_z = average(N, z);

    // Initialize all variables to 0 (important!)
    double T200 = 0, T110 = 0, T101 = 0;
    double T020 = 0, T011 = 0;
    double T002 = 0;

    double T300 = 0, T120 = 0, T102 = 0;
    double T210 = 0, T030 = 0, T012 = 0;
    double T201 = 0, T021 = 0, T003 = 0;

    double X, Y, Z;
    for (int i = 0; i < N; i++) {
        X = x[i] - ave_x;
        Y = y[i] - ave_y;
        Z = z[i] - ave_z;

        T200 += T(2, 0, 0, X, Y, Z);
        T110 += T(1, 1, 0, X, Y, Z);
        T101 += T(1, 0, 1, X, Y, Z);
        T020 += T(0, 2, 0, X, Y, Z);
        T011 += T(0, 1, 1, X, Y, Z);
        T002 += T(0, 0, 2, X, Y, Z);

        T300 += T(3, 0, 0, X, Y, Z);
        T120 += T(1, 2, 0, X, Y, Z);
        T102 += T(1, 0, 2, X, Y, Z);
        T210 += T(2, 1, 0, X, Y, Z);
        T030 += T(0, 3, 0, X, Y, Z);
        T012 += T(0, 1, 2, X, Y, Z);
        T201 += T(2, 0, 1, X, Y, Z);
        T021 += T(0, 2, 1, X, Y, Z);
        T003 += T(0, 0, 3, X, Y, Z);
    }

    // Renamed to 'matT' to avoid conflict with the function 'T'
    double matT[3][3] = {
        {T200, T110, T101}, 
        {T110, T020, T011}, 
        {T101, T011, T002}
    };
    
    double t[3] = {
        (T300 + T120 + T102) * 0.5, 
        (T210 + T030 + T012) * 0.5, 
        (T201 + T021 + T003) * 0.5
    };

    double T_inv[3][3];

    if (inverse_matrix_3(matT, T_inv)) {
        return -1; // Return error if matrix is singular
    }

    double C_x, C_y, C_z;

    C_x = T_inv[0][0] * t[0] + T_inv[0][1] * t[1] + T_inv[0][2] * t[2];
    C_y = T_inv[1][0] * t[0] + T_inv[1][1] * t[1] + T_inv[1][2] * t[2];
    C_z = T_inv[2][0] * t[0] + T_inv[2][1] * t[1] + T_inv[2][2] * t[2];

    // Using your preferred absolute value notation for the norm:
    // r = |C|^2 + variance
    double r_squared = (C_x * C_x + C_y * C_y + C_z * C_z) + (T200 + T020 + T002) / N;

    return std::sqrt(r_squared);
}

double sidereal_day(double ang_vec){
    return (2*M_PI)/ang_vec/3600.0;	

}

double deg2rad(double deg_per_sec){
        return deg_per_sec*(M_PI/180.0);
}

