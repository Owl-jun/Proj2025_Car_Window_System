# Proj2025_Car_Window_System
전장 S/W 내부 네트워크 시스템 구축

# Directory

|폴더명|설명|
|:--:|:--:|
|[Arduino](https://github.com/Owl-jun/Proj2025_Car_Window_System/tree/main/Arduino)|아두이노 소스코드|
|[GUIAPP](https://github.com/Owl-jun/Proj2025_Car_Window_System/tree/main/GUIApp)|Qt GUI App 소스코드|
|[MqTTPubApp](https://github.com/Owl-jun/Proj2025_Car_Window_System/tree/main/MqTTPubApp)|Python 시리얼 통신 및 MqTT Pub App 소스코드|
|[MotorController](https://github.com/Owl-jun/Proj2025_Car_Window_System/tree/main/MotorController)|서보모터 제어 프로그램 소스코드|

# 프로젝트 개요

- MqTT를 활용한 실시간 네트워크 시스템 구축을 목표로 합니다.
- MqTT 연동을 통해 쉽게 별도의 프로그램을 추가할 수 있습니다.

- 본 프로젝트의 구성도입니다.
![](https://velog.velcdn.com/images/owljun/post/441c1786-931c-4973-81e8-78ade89c0ac8/image.png)

- 자동차 내부 디스플레이에 출력되는 터치스크린을 가정하여 그 위에 Raspi4 를 기반으로 프로그램을 구동합니다.
- Raspi4 는 ECU의 역할을 한다고 가정합니다, Pi4 가 센서 데이터수집을 위한 전원(아두이노) , 디스플레이 구동을 위한 전원을 공급합니다.
- 모든 기기는 하나의 AP로 구성하여 중심이 되는 서버 MCU에 MqTT 서버를 구동합니다. (여기서는 Windows 데스크탑이 그 역할을 합니다.)
- 내부 기기들은 필요에 따라 서버 MCU의 MqTT 서버와 시리얼통신을 통해 통신합니다.

## 시연



## 프로젝트 구성 (H/W)

- Raspberry PI 4 (64bit OS)
- Arduino Uno
- 7inch Rasp Touch Display
- 온 습도 감지 센서 (DHT11)
- 비 감지 센서 (MD002)
- 서보모터 (sg90)
- USB A to B (시리얼 통신)
- IPTIME 공유기 (AP)


## 파이4 부팅시 자동 실행 스크립트 설정

- start.sh

  ```sh
  #!/bin/bash
  
  source /home/raspi/owljun/venv/bin/activate
  
  # Mqtt Pub App
  python /home/raspi/owljun/Proj2025_Car_Window_System/MqTTPubApp/MqTTPubApp.py &
  
  # Motor App
  python /home/raspi/owljun/Proj2025_Car_Window_System/MotorController/MotorContoller.py &
  
  # GUI
  DISPLAY=:0 /home/raspi/owljun/Proj2025_Car_Window_System/GUIApp/build/bin/MyApp
  
  exit 0
  ```

- /etc/systemed/system/myapp.service
  
  ```nano
  [Unit]
  Description=My Car Window System
  After=network.target
  
  [Service]
  Type=simple
  ExecStart=/home/raspi/startup.sh
  WorkingDirectory=/home/raspi/owljun/Proj2025_Car_Window_System/
  User=raspi
  Environment=DISPLAY=:0
  Restart=always
  
  [Install]
  WantedBy=multi-user.target
  ```

- 등록
  
  ```bash
  sudo systemctl daemon-reload
  sudo systemctl enable myapp.service
  sudo systemctl start myapp.service
  sudo systemctl status myapp.service
  ```


## 개발 환경 구축

- python : 기본 python3 + venv 활용 가상환경
- C++ Qt : VSCode + Qt6 + CMake + Pi4

```bash
sudo apt update && sudo apt upgrade -y

# venv 환경설정
mkdir 작업디렉터리
python3 -m venv venv
source .venv/bin/activate

# paho-mqtt 설치
pip install paho-mqtt

# qt 5
sudo apt install qtbase5-dev qttools5-dev-tools qttools5-dev qtchooser -y

# qt 6
sudo apt install qt6-base-dev qt6-tools-dev-tools qt6-tools-dev -y
sudo apt install qtcreator -y

# 버전 체크
qmake -v

# cmake 설치
sudo apt update
sudo apt install cmake -y

# 디렉터리 설정 및 빌드
cd ~/YourDir/build
cmake ..
make
./MyQtApp

# 디스플레이 서버 연결 실패시
export DISPLAY=:0
./MyQtApp

# VSCode Settings.json (Ctrl + Shift + P -> 기본 설정: 작업영역설정열기(JSON))
{
  "cmake.generator": "Unix Makefiles",
  "cmake.configureOnOpen": true,
  "cmake.buildDirectory": "${workspaceFolder}/build"
}

```
  

### Git 자주사용 명령어

```

# 상태확인
git status

# 추가
git add .

# 커밋 & 푸쉬
git commit -m "comment"
git push


# SSH 에서 사용하기 위한 설정

ssh-keygen -t ed25519 -C "your_email@example.com"

# Enter

# SSH 키 출력 (공개 키)
cat ~/.ssh/id_ed25519.pub

# 출력 내용 복사 이후
# Github -> 우상단 프로필 -> Settings -> SSH and GPG keys -> Add SSH Key -> 복사내용 붙혀넣기

ssh -T git@github.com
# yes -> Enter

# HTTPS -> SSH 변경
cd 작업디렉터리
git remote -v # 현재 상태 확인
git remote set-url origin git@github.com:사용자이름/저장소이름.git # 상태 변경

# git 전역설정
git config --global user.name "Your Name"
git config --global user.email "your.email@example.com"

# 확인
git config --global user.name
git config --global user.email

```
