#include <iostream>
#include <iomanip>
#include <cmath>

#include "./inc/geo.h"
// These tell the compiler that the logic lives in your geo.cpp file
double radius_estimation(int num_reads, double x[], double y[], double z[]);
double sidereal_day(double angular_velocity);

int main() {
    // 1. Your specific data points
    const int Num_Reads = 6;
    double gyro_x[Num_Reads] = {-6.876499e-05, 3.940242e-05, -1.354063e-05, 1.760444e-05, 8.612564e-06, 4.854159e-06};
    double gyro_y[Num_Reads] = {6.967326e-05, 5.252688e-05, -2.771865e-05, 4.332510e-05, 7.057308e-05, 4.411307e-06};
    double gyro_z[Num_Reads] = {-9.210947e-06, -1.610237e-05, -1.999408e-05, -8.630062e-07, -1.141718e-04, -9.701927e-05};

    std::cout << "==========================================" << std::endl;
    std::cout << "   IMU GEOLOCATION CALCULATION TEST       " << std::endl;
    std::cout << "==========================================" << std::endl;

    // 2. Call your radius_estimation function
    // This should process the arrays and return avg angular velocity
    double ang_vel = radius_estimation(Num_Reads, gyro_x, gyro_y, gyro_z);

    // 3. Call your sidereal_day function
    double hours = sidereal_day(ang_vel);

    // 4. Output Results
    std::cout << std::scientific << std::setprecision(8);
    std::cout << "Average Angular Velocity : " << ang_vel << " rad/s" << std::endl;
    
    std::cout << std::fixed << std::setprecision(4);
    std::cout << "Calculated Sidereal Day  : " << hours << " hours" << std::endl;

    // 5. Comparison with Earth's True Constant
    // Earth's rotation is approx 7.2921159e-5 rad/s
    const double earth_true_rads = 7.2921159e-5;
    const double earth_true_hours = 23.9344;

    std::cout << "------------------------------------------" << std::endl;
    std::cout << "Theoretical Earth Day    : " << earth_true_hours << " hours" << std::endl;
    
    double error = std::abs(hours - earth_true_hours);
    std::cout << "Absolute Error           : " << error << " hours" << std::endl;
    std::cout << "==========================================" << std::endl;

    return 0;
}
