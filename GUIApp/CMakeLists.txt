cmake_minimum_required(VERSION 3.16)
project(GUIApp)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 패키지 찾기
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)
find_package(PahoMqttCpp REQUIRED)

# 하위 디렉터리
add_subdirectory(src)    # 라이브러리 정의
add_subdirectory(main)   # 실행파일 정의
