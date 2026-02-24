#include <iostream>
#include <time.h>
#include <vector>
#include <fstream> // For file input/output

//MakeFile
#include "../inc/i2c.h"
#include "../inc/timer.h"
#include "../inc/stats.h"
#include "../inc/temp.h"
#include "../inc/accel.h"
#include "../inc/gyro.h"
#include "../inc/geo.h"

//Variables
#define FIFO_Threshold 1
#define Num_Reads 6
#define Acquisition_Time 65
#define IMU_Clock_Frequency 19200000.0

int read_main(int device_handle, double& out_gx, double& out_gy, double& out_gz);

double gyro_x[Num_Reads];
double gyro_y[Num_Reads];
double gyro_z[Num_Reads];

int main(int argc, char* argv[])
{
    //MPU9250 setup
    if (gpioInitialise() < 0) return -1;

    int device_handle = i2cOpen(I2C_DEVICE, I2C_DEVICE_ADDR, I2C_FLAGS);
    if (device_handle < 0) return -2;

    i2cWriteByteData(device_handle, PWR_MGMT_1_ADDR, PWR_MGMT_1_VAL);
    set_gyro(device_handle, 250);
    set_accel(device_handle, 2);

    device_wait(100);
    std::cout << "Starting Acquisition for " << Acquisition_Time << " seconds, " << Num_Reads << " times...\n";

    // --- Setup CSV File ---
    std::ofstream csv_file("imu_averages.csv");
    if (!csv_file.is_open()) {
        std::cout << "Failed to create CSV file!\n";
        return -4;
    }

    csv_file << "Read_Number,Avg_Gx,Avg_Gy,Avg_Gz\n";

    //Main Loop --> Reading Data
    for (int i = 0; i < Num_Reads; i++) {
        std::cout << "\n--- Starting Read " << (i + 1) << " of " << Num_Reads << " ---\n";

        // Variables to hold the results from this specific read
        double current_gx, current_gy, current_gz;

        if (read_main(device_handle, current_gx, current_gy, current_gz) != 0) {
            std::cout << "An error occurred during measurement.\n";
            csv_file.close();
            return -3;
        }


        //Saving Data in Array
        gyro_x[i] = current_gx;
        gyro_y[i] = current_gy;
        gyro_z[i] = current_gz;

        // Write the data to the CSV file
        csv_file << (i + 1) << ","
                 << current_gx << ","
                 << current_gy << ","
                 << current_gz << "\n";

        std::cout << "finish\n";
    }

    // --- Calculations ---
    double angular_velocity;
    angular_velocity = radius_estimation(Num_Reads, gyro_x, gyro_y, gyro_z);

    if (angular_velocity < 0) {
       std::cout << "Calculation is incorrect\n";
       return -5; 
    }
    
    std::cout << angular_velocity << " rad/s\n" << std::endl; 
    
    double sid_day = sidereal_day(angular_velocity); 
    
    std::cout << "A sidereal day is " << sid_day << " hours long.\n" << std::endl; 


    // Close the file when done
    csv_file.close();
    std::cout << "\nAll data saved successfully to imu_averages.csv\n";

    return 0;
}

//Read Data
int read_main(int device_handle, double& out_gx, double& out_gy, double& out_gz ) {
    struct timespec start, now, last_read;
    double total_time = 0;
    double g_x_sum = 0, g_y_sum = 0, g_z_sum = 0;

    clock_gettime(CLOCK_MONOTONIC, &start);
    last_read = start;

    // CountDown
    for(int i = 3; i > 0; i--) {
        std::cout << i << "...\n";
        device_wait(1000);
    }

    //Integration
    do {
        clock_gettime(CLOCK_MONOTONIC, &now);
        double dt = (now.tv_sec - last_read.tv_sec) +
                    (now.tv_nsec - last_read.tv_nsec) / 1e9;
        last_read = now;

        double gx = get_gyro_x(device_handle);
        double gy = get_gyro_y(device_handle);
        double gz = get_gyro_z(device_handle);
        
        //Deg/s --> Rad/s
        gx = deg2rad(gx);
        gy = deg2rad(gy);
        gz = deg2rad(gz);

        g_x_sum += gx * dt;
        g_y_sum += gy * dt;
        g_z_sum += gz * dt;
        total_time += dt;

        double elapsed = (now.tv_sec - start.tv_sec) +
                         (now.tv_nsec - start.tv_nsec) / 1e9;

        if ((int)elapsed % 5 == 0 && dt > 0) {
             std::cout << "Acquiring... " << (int)elapsed << "s\r" << std::flush;
        }

        if (elapsed >= Acquisition_Time) break;

    } while (true);

    // Instead of creating new local variables, we assign the answers
    // to the reference variables passed from main()
    out_gx = g_x_sum / total_time;
    out_gy = g_y_sum / total_time;
    out_gz = g_z_sum / total_time;

    std::cout << "\nAverage Gyro (rad/s): " << out_gx << ", " << out_gy << ", " << out_gz << std::endl;

    return 0;
}
