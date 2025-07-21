#pragma once

#include <QWidget>
#include <QString>

class QLabel;
class QVBoxLayout;
class MqttCallback;

namespace mqtt {
    class async_client;
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
    MqttCallback* callback;
};
