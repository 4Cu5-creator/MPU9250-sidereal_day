<<<<<<< HEAD
# MPU9250-sidereal_day

9軸センサ-MPU9250から得られる地球の角速度データを用いて 「恒星日（地球が1回転する時間）」を推定する。

## 詳細

未投稿

## はじめ

### Dependencies

* OS: Trixie (64-bit)
* デバイス：Raspberry Pi 4, Raspberry Pi Zero(未確認)

### Installing

* How/where to download your program
* Any modifications needed to be made to files/folders

### Executing program

* How to run the program
* Step-by-step bullets
```
code blocks for commands
```

## Help

Any advise for common problems or issues.
```
command to run if program contains helper info
```

## Authors

フォーカス


## 参考リンク

* [mpu9250-with-raspberrypi](https://github.com/Alexgichamba/mpu9250-with-raspberrypi/tree/main)
* [Pigpiod not installing under new official trixie release](https://github.com/joan2937/pigpio/issues/632)
* [それでも地球は回っている!](https://elchika.com/article/2e2a5d67-8ae6-4c7c-b0fe-ec6be2e10566/)
* [PurpleBooth](https://gist.github.com/PurpleBooth/109311bb0361f32d87a2)
* [dbader](https://github.com/dbader/readme-template)
* [zenorocha](https://gist.github.com/zenorocha/4526327)
* [fvcproductions](https://gist.github.com/fvcproductions/1bfc2d4aecb01a834b46)

=======
# MPU-9250 with Raspberry Pi
This program provides basic functionality of the MPU-9250 with a Raspberry Pi over an I<sup>2</sup>C interface. The program uses the [pigpio](https://abyz.me.uk/rpi/pigpio/index.html) library for the I<sup>2</sup>C read and write functionality.
<p>This program was tested on a Raspberry Pi Zero W running Raspbian GNU/Linux 10 (buster).</p>

## To install
You would first need to check if you have pigpio installed. To do so, run the following line of code in the terminal.
```bash
pigpiod -v
```
If the command is not recognized you can [download and install pigpio](https://abyz.me.uk/rpi/pigpio/download.html).
Once pigpio is installed, you may download and build this program. To do so, run the following lines in the terminal:
```bash
git clone https://github.com/Alexgichamba/mpu9250-with-raspberrypi.git
cd mpu9250-with-raspberrypi/
make
```
Then you may run the program:
```bash
sudo ./bin/mpu9250
```

## Functionality
* [X] Read and output inertial measurment unit data
* [X] Read and output temperature data
* [X] Calculate and log maximums and minimums to a file
* [ ] Read and output magnetometer data
* [ ] Plot the data on graphs
>>>>>>> 01eb6c2 (First commit)
