#include "Mqtt/CMainWindow.hpp"
#include "Mqtt/CMqttCallback.hpp"
#include <QVBoxLayout>
#include <QLabel>
#include <QDebug>

#include "mqtt/async_client.h"

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
    callback = new MqttCallback(this);
    client->set_callback(*callback);

    connect(callback, &MqttCallback::messageReceived,
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
        client->disconnect()->wait();
    } catch (...) {}

    delete client;
    delete callback;
}

void CMainWindow::onMessageReceived(QString topic, QString payload)
{
    QString text = QString("📥 [%1]\n%2").arg(topic).arg(payload);
    label->setText(text);
    qDebug() << "수신됨:" << text;
}
