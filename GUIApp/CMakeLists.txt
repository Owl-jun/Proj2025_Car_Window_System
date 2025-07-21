cmake_minimum_required(VERSION 3.16)
project(GUIApp)

set(CMAKE_AUTOMOC ON) # Q_OBJECT 매크로 처리를 위해 필요
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # IntelliSense를 위해 권장

# 패키지 찾기
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Network)
find_package(PahoMqttCpp REQUIRED)

# 실행 파일 정의 (main.cpp만 포함)
add_executable(MyApp main.cpp)

# 실행 파일이 필요로 하는 모든 라이브러리를 직접 링크
target_link_libraries(MyApp PUBLIC
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network
    PahoMqttCpp::paho-mqttpp3 # 정확한 타겟 이름 확인 후 사용
)

