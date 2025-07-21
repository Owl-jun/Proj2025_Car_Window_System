# Proj2025_Car_Window_System
전장 S/W 내부 네트워크 시스템 구축

# Directory

|폴더명|설명|
|:--:|:--:|
|[Arduino](https://github.com/Owl-jun/Proj2025_Car_Window_System/tree/main/Arduino)|아두이노 소스코드|
|GUIAPP|Qt GUI App 소스코드|
|MqTTPubApp|Python 시리얼 통신 및 MqTT Pub App 소스코드|


# 프로젝트 개요
![](https://velog.velcdn.com/images/owljun/post/441c1786-931c-4973-81e8-78ade89c0ac8/image.png)

## 프로젝트 구성 (H/W)

- Raspberry PI 4 (64bit OS)
- Arduino Uno
- 7inch Rasp Touch Display
- 온 습도 감지 센서 (DHT11)
- 비 감지 센서 (MD002)
- 스텝 모터 & 드라이버 (28BYJ-48)
- 자동차 목업
- USB A to B (시리얼 통신)

## 프로젝트 구성 (N/W)

- IPTIME 공유기 (AP)
- MQTT Server (Windows 11 64bit Desktop)
- MQTT Pub APP (Raspi 4 OS 64bit)
- Sensing Data (Arduino Uno)


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
