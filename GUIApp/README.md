# Qt6 GUI APP
터치 디스플레이 지원 GUI APP 입니다.

# 개요


## CMake 빌드 관련

- 이전 CMakeCache 삭제하기
    ```bash
    rm -rf build/
    cmake -Bbuild -S. -DCMAKE_PREFIX_PATH="/usr/local/lib/cmake"
    ```

- 빌드
    ```bash
    cmake -build build
    ```

## paho-mqtt 환경설정 (CMake)

1. 의존성 설치

    ```bash
    sudo apt update
    sudo apt install git cmake build-essential libssl-dev
    ```

2. paho.mqtt.c (C 라이브러리) 빌드 및 설치

    ```bash
    git clone https://github.com/eclipse/paho.mqtt.c.git
    cd paho.mqtt.c

    cmake -Bbuild -DPAHO_BUILD_STATIC=TRUE -DPAHO_BUILD_SHARED=TRUE
    cmake --build build/ -- -j$(nproc)

    sudo cmake --install build
    ```

3. paho.mqtt.cpp (C++ 라이브러리) 빌드 및 설치

    ```bash
    git clone https://github.com/eclipse/paho.mqtt.cpp.git
    cd paho.mqtt.cpp

    cmake -Bbuild -H. \
    -DPAHO_BUILD_STATIC=TRUE \
    -DPAHO_BUILD_SHARED=TRUE \
    -DPAHO_MQTT_C_PATH=/usr/local

    cmake --build build/ -- -j$(nproc)
    sudo cmake --install build
    ```

4. 설치 확인

    ```bash
    # 헤더
    ls /usr/local/include | grep MQTT

    # 라이브러리
    ls /usr/local/lib | grep paho

    # cmake 설정
    ls /usr/local/lib/cmake/eclipse-paho-mqtt-c
    ls /usr/local/lib/cmake/paho-mqttpp3
    ```