#include "Mqtt/CMainWindow.hpp"
#include "Mqtt/CMqttCallback.hpp"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    MainWindow window;
    window.resize(400, 200);
    window.setWindowTitle("MQTT GUI Subscriber");
    window.show();
    return app.exec();
}
