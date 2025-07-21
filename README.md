# Proj2025_Car_Window_System
전장 S/W 내부 네트워크 시스템 구축

# 프로젝트 개요
<img width="1715" height="1037" alt="image" src="https://github.com/user-attachments/assets/7fb0bd89-5bc2-4af2-8d71-f4545f968cac" />

## 프로젝트 구성 (H/W)

- Raspberry PI 4 (64bit OS)
- Arduino Uno
- 7inch Rasp Touch Display
- 비 감지 센서
- 스텝 모터 & 드라이버
- 자동차 목업
- USB A to B (시리얼 통신)


## 개발 환경 구축

- python : 기본 python3 + venv 활용 가상환경
- C++ Qt : VSCode + Qt6 + CMake + Pi4

```bash
sudo apt update && sudo apt upgrade -y

# venv 환경설정
mkdir 작업디렉터리
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
  
