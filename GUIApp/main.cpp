#include <QApplication>
#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QDebug>
#include <QString>

#include "mqtt/async_client.h" // paho-mqtt 클라이언트 라이브러리 헤더

// CMqttCallback 클래스 정의
// CMqttCallback.hpp와 CMqttCallback.cpp의 내용을 여기에 합칩니다.
// #pragma once 와 같은 헤더 가드는 제거합니다.
class CMqttCallback : public QObject, public virtual mqtt::callback {
    Q_OBJECT

public:
    explicit CMqttCallback(QObject* parent = nullptr);

signals:
    void messageReceived(QString topic, QString payload);

private:
    void connection_lost(const std::string& cause) override;
    void delivery_complete(mqtt::delivery_token_ptr tok) override;
    void message_arrived(mqtt::const_message_ptr msg) override;
};

CMqttCallback::CMqttCallback(QObject* parent)
    : QObject(parent)
{}

void CMqttCallback::connection_lost(const std::string& cause) {
    qDebug() << "MQTT 연결 끊김:" << QString::fromStdString(cause);
}

void CMqttCallback::delivery_complete(mqtt::delivery_token_ptr tok) {
    qDebug() << "MQTT 메시지 발행 완료 (토큰):" << tok->get_message_id();
}

void CMqttCallback::message_arrived(mqtt::const_message_ptr msg)
{
    emit messageReceived(
        QString::fromStdString(msg->get_topic()),
        QString::fromStdString(msg->to_string())
    );
}

// CMainWindow 클래스 정의
// CMainWindow.hpp와 CMainWindow.cpp의 내용을 여기에 합칩니다.
// #pragma once 와 같은 헤더 가드는 제거합니다.
namespace mqtt {
    class async_client; // async_client 전방 선언 (이젠 필요 없을 수도 있지만 안전하게 유지)
}

// 메인 위젯 클래스
class CMainWindow : public QWidget {
    Q_OBJECT

public:
    explicit CMainWindow(QWidget* parent = nullptr);
    ~CMainWindow();

private slots:
    void onMessageReceived(QString topic, QString payload);

private:
    QLabel* label;
    mqtt::async_client* client;
    CMqttCallback* callback; 
};

// MQTT 설정
const std::string ADDRESS = "192.168.0.2";
const std::string CLIENT_ID = "qt_gui_client";
const std::string TOPIC = "sensor/rain_dht";

CMainWindow::CMainWindow(QWidget* parent)
    : QWidget(parent)
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    label = new QLabel("Waiting for MQTT messages...");
    layout->addWidget(label);
    setLayout(layout);

    client = new mqtt::async_client(ADDRESS, CLIENT_ID);
    callback = new CMqttCallback(this); 
    client->set_callback(*callback);

    connect(callback, &CMqttCallback::messageReceived, 
            this, &CMainWindow::onMessageReceived);

    mqtt::connect_options connOpts;
    connOpts.set_clean_session(true);

    try {
        client->connect(connOpts)->wait();
        client->subscribe(TOPIC, 1)->wait();
    }
    catch (const mqtt::exception& e) {
        label->setText("MQTT 연결 실패: " + QString::fromStdString(e.what()));
    }
}

CMainWindow::~CMainWindow()
{
    try {
        if (client->is_connected()) {
            client->unsubscribe(TOPIC)->wait();
            client->disconnect()->wait();
        }
    } catch (const mqtt::exception& e) {
        qDebug() << "MQTT Disconnect Error:" << e.what();
    } catch (...) {
        qDebug() << "Unknown error during MQTT disconnect.";
    }

    delete client;
    delete callback;
}

void CMainWindow::onMessageReceived(QString topic, QString payload)
{
    QString text = QString("📥 [%1]\n%2").arg(topic).arg(payload);
    label->setText(text);
    qDebug() << "수신됨:" << text;
}

#include "main.moc"
// 메인 함수
int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    CMainWindow window; 
    window.resize(400, 200);
    window.setWindowTitle("MQTT GUI Subscriber");
    window.show();
    return app.exec();
}