add_library(MyLib
    CMainWindow.cpp
    CMqttCallback.cpp 
)

# 헤더 포함 경로
target_include_directories(MyLib PUBLIC ${CMAKE_SOURCE_DIR}/include)

# 외부 패키지 링크
target_link_libraries(MyLib
    Qt6::Core
    Qt6::Widgets
    PahoMqttCpp::paho-mqttpp3
)
