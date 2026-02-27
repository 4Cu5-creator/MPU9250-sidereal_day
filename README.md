# MPU9250-sidereal_day

9軸センサ-MPU9250から得られる地球の角速度データを用いて 「恒星日（地球が1回転する時間）」を推定する。

## はじめ

### 開発環境

* OS: Trixie (64-bit)
* デバイス：Raspberry Pi 4, Raspberry Pi Zero(未確認)

### インストール

#### pigpioのインストール
初めに、`pigpio`をインストールする。しかし、`TRIXIE`には提供されていないために、自分自身で`build`する。
`pigpio`のパッケージをインストールするために、次のコマンドを実行する。
```bash
sudo apt install -y python3-setuptools python3-full
wget https://github.com/joan2937/pigpio/archive/refs/tags/v79.tar.gz
tar zxf v79.tar.gz
cd pigpio-79
make
sudo make install
sudo ldconfig
```
次に、`pigpio.service`のテンプレートを作成する。次のコマンドを実行する。（全部コピー・ペストしてください）
```bash
DOLLAR="\$"

cat << EOF > config_pigpiod_service.sh
#! /bin/bash
PIGPIOD_SERVICE="/lib/systemd/system/pigpiod.service"

if [[ ! -f "${DOLLAR}{PIGPIOD_SERVICE}" ]] ; then

  # The configuration of pigpiod appears to be different on 32 bit and 64 bit systems
  # pigpiod must be run with -t0 on a 64 bit system, and -t1 on a 32 bit system.
  # The 32 bit behaviour contradicts what is documented in the pigpiod web documentation ;-(
  ARCH=${DOLLAR}(uname -m)

  if [[ "${DOLLAR}{ARCH}" == "aarch64" ]] ; then
    T_COMMAND="-t0"
  elif [[ "${DOLLAR}{ARCH}" == "armv7l" ]] ; then
    T_COMMAND="-t1"
  elif [[ "${DOLLAR}{ARCH}" == "armv6l" ]] ; then
    T_COMMAND="-t1"
  else
    echo "uname -m returns '${DOLLAR}{ARCH}' which is an unexpected value. Time to debug..."
    echo
    echo "I have not seen armv8 on my Pi3, but I believe this might happen if running"
    echo "64 bits. I am not sure whether pigpiod requires -t0 or -t1 in this case."
    echo "You'll need to experiment."
    echo
    echo "I believe armv6 is a possibility on a Pi2 or earlier?"
    echo "I guess use same settings as for armv6l?"
    exit 1
  fi

  # We are going to overwrite the systemd configuration file for pigpiod
  # because we are going to change the launch options
  cat << END > "${DOLLAR}{PIGPIOD_SERVICE}"
[Unit]
Description=Daemon required to control GPIO pins via pigpio

[Service]
Type=forking
ExecStart=/usr/local/bin/pigpiod -l -s10 ${DOLLAR}{T_COMMAND} -x0xFFFFFFF
Restart=always
ExecStop=/bin/systemctl kill pigpiod

[Install]
WantedBy=multi-user.target
END
else
  echo "${DOLLAR}{PIGPIOD_SERVICE} already exists. I refuse to change it."
fi
EOF

chmod 755 config_pigpiod_service.sh
sudo ./config_pigpiod_service.sh
```
最後に`daemon`を再起動し、`pigpio.service`を起動する。
```bash
sudo systemctl daemon-reload
sudo systemctl enable --now pigpiod.service
```
`pigpio`のバーションを確認する。
```bash
pigpiod -v
```
[重要] 次に進む前に `pigpio.service`を一時停止する。
```bash
sudo systemctl stop pigpiod
sudo systemctl disable pigpiod
sudo fuser -k 8888/tcp
sudo rm -f /var/run/pigpio.pid
```

#### プロクラム本体
このリポジトリを`clone`する。または、`.tar`のファイルをダウンロードし、展開する。
```
git clone https://github.com/4Cu5-creator/MPU9250-sidereal_day.git
cd mpu9250-with-raspberrypi/
make
```
### プロクラムの実行

`~\MPU9250-sidereal_day`に入り、次のコマンドを実行する。
```
sudo ./bin/mpu9250
```
## 開発

### コーディング
ソースコードは`src\`にある。
オリジナルコードから　
  * `main.cpp`を変更した
  * `geo.cpp`を追加した ==> 計算しているとコード</br>
  
もし、`.cpp`のコードに新しい関数を追加しする時、`inc\`にあるライブラリに宣言しなければならない。

## Authors

フォーカス


## 参考リンク

* [mpu9250-with-raspberrypi](https://github.com/Alexgichamba/mpu9250-with-raspberrypi/tree/main)
* [Pigpiod not installing under new official trixie release](https://github.com/joan2937/pigpio/issues/632)
* [それでも地球は回っている!](https://elchika.com/article/2e2a5d67-8ae6-4c7c-b0fe-ec6be2e10566/)

テンプレート
* [PurpleBooth](https://gist.github.com/PurpleBooth/109311bb0361f32d87a2)
* [dbader](https://github.com/dbader/readme-template)
* [zenorocha](https://gist.github.com/zenorocha/4526327)
* [fvcproductions](https://gist.github.com/fvcproductions/1bfc2d4aecb01a834b46)

