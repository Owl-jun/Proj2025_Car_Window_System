# cmake_minimum_required(VERSION 3.16)
# project(GUIApp)

# set(CMAKE_AUTOMOC ON) # Q_OBJECT 매크로 처리를 위해 필요
# set(CMAKE_AUTOUIC ON)
# set(CMAKE_AUTORCC ON)
# set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # IntelliSense를 위해 권장

# # 패키지 찾기
# find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Network)
# find_package(PahoMqttCpp REQUIRED)

# # 실행 파일 정의 (main.cpp만 포함)
# add_executable(MyApp main.cpp)

# # 실행 파일이 필요로 하는 모든 라이브러리를 직접 링크
# target_link_libraries(MyApp PUBLIC
#     Qt6::Core
#     Qt6::Gui
#     Qt6::Widgets
#     Qt6::Network
#     PahoMqttCpp::paho-mqttpp3 # 정확한 타겟 이름 확인 후 사용
# )


# ===============

cmake_minimum_required(VERSION 3.16...3.29)

project(Proj2025_Car_Window_System LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 빌드 출력 디렉토리 설정:
# 이 CMakeLists.txt 파일이 있는 디렉토리 안에 'build' 폴더를 생성하고 싶다면,
# CMAKE_BINARY_DIR는 'cmake -B' 명령으로 지정한 디렉토리입니다.
# 만약 'cmake -S /path/to/GUIApp -B /path/to/GUIApp/build'로 빌드한다면,
# CMAKE_BINARY_DIR는 /path/to/GUIApp/build가 됩니다.
# 여기서는 실행 파일이 바로 GUIApp/build/bin 에 생기도록 설정합니다.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin") # <-- 이 부분이 중요

# 패키지 찾기
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Network)
find_package(PahoMqttCpp REQUIRED)

# 소스 파일 목록 정의
set(PROJECT_SOURCES
    src/main.cpp
    src/mainwindow.h
    src/mainwindow.cpp
    src/mqttclientwrapper.h
    src/mqttclientwrapper.cpp
)

# 실행 파일 정의
add_executable(MyApp ${PROJECT_SOURCES})

# 실행 파일이 필요로 하는 모든 라이브러리를 링크
target_link_libraries(MyApp PUBLIC
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network
    PahoMqttCpp::paho-mqttpp3
)

# Optional: Set RPATH for Linux/macOS
# set_target_properties(MyApp PROPERTIES
#     BUILD_RPATH "$ORIGIN"
#     INSTALL_RPATH "$ORIGIN"
# )