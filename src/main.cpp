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
    enable_fifo(device_handle);

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
int read_main(int device_handle, double& out_gx, double& out_gy, double& out_gz) {
    struct timespec start, now, last_read;
    double total_time = 0;
    double g_x_sum = 0, g_y_sum = 0, g_z_sum = 0;

    // We need to know the sensitivity to convert raw bits to deg/s
    // For 250 deg/s scale, the divisor is 131.0
    const double GYRO_SENS = 131.0; 

    clock_gettime(CLOCK_MONOTONIC, &start);
    last_read = start;

    do {
        // Check how many bytes are in the FIFO
        int countH = i2cReadByteData(device_handle, 0x72); // FIFO_COUNTH
        int countL = i2cReadByteData(device_handle, 0x73); // FIFO_COUNTL
        int count = (countH << 8) | countL;

        // Each packet (Accel+Gyro) is 12 bytes. Let's wait for at least one packet.
        if (count >= 12) {
            uint8_t buffer[12];
            // Read 12 bytes in one burst from FIFO_R_W register (0x74)
            i2cReadI2CBlockData(device_handle, 0x74, (char*)buffer, 12);

            // Extract Gyro Data (Bytes 6-11 in the 12-byte packet)
            int16_t raw_gx = (buffer[6] << 8) | buffer[7];
            int16_t raw_gy = (buffer[8] << 8) | buffer[9];
            int16_t raw_gz = (buffer[10] << 8) | buffer[11];

            // Convert raw to rad/s
            double gx = deg2rad((double)raw_gx / GYRO_SENS);
            double gy = deg2rad((double)raw_gy / GYRO_SENS);
            double gz = deg2rad((double)raw_gz / GYRO_SENS);

            // Timing
            clock_gettime(CLOCK_MONOTONIC, &now);
            double dt = (now.tv_sec - last_read.tv_sec) + (now.tv_nsec - last_read.tv_nsec) / 1e9;
            last_read = now;

            // Integrate
            g_x_sum += gx * dt;
            g_y_sum += gy * dt;
            g_z_sum += gz * dt;
            total_time += dt;
        }

        double elapsed = (now.tv_sec - start.tv_sec) + (now.tv_nsec - start.tv_nsec) / 1e9;
        if (elapsed >= Acquisition_Time) break;

    } while (true);

    out_gx = g_x_sum / total_time;
    out_gy = g_y_sum / total_time;
    out_gz = g_z_sum / total_time;

    return 0;
}


void enable_fifo(int device_handle) {
    // 1. Reset FIFO and Signal Paths
    i2cWriteByteData(device_handle, USER_CTRL, 0x04); // FIFO Reset
    i2cWriteByteData(device_handle, FIFO_EN, 0x78);   // Enable Accel and Gyro (X,Y,Z) to FIFO
    i2cWriteByteData(device_handle, USER_CTRL, 0x40); // Enable FIFO mode
}
