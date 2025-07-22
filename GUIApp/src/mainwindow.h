#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QTimer> // 시간 업데이트용
#include "mqttclientwrapper.h" // Paho MQTT 래퍼 포함

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // MQTT 관련 슬롯
    void onMqttConnected();
    void onMqttDisconnected();
    void onMqttMessageReceived(const QString& topic, const QString& payload);
    void onMqttError(const QString& errorString); // MQTT 에러 처리 슬롯

    // UI 버튼 클릭 슬롯
    void onUpButtonClicked();
    void onStopButtonClicked();
    void onDownButtonClicked();

    // UI 업데이트 관련 슬롯
    void updateCurrentTime(); // 현재 시간 업데이트 슬롯
private:
    MqttClientWrapper *m_mqttClientWrapper; // Paho MQTT 래퍼 객체

    // UI 요소들
    QLabel *m_currentTimeLabel;
    QLabel *m_tempLabel;
    QLabel *m_humiLabel;
    QLabel *m_logLabel;
    QLabel *m_stateLabel;

    QPushButton *m_upButton;
    QPushButton *m_stopButton;
    QPushButton *m_downButton;

    // UI 및 MQTT 설정 함수
    void setupUi();
    void setupMqtt();

    // 데이터 업데이트 헬퍼 함수
    void updateTemperature(const QString &temp);
    void updateHumidity(const QString &humi);
    void updateLog(const QString &logEntry);
    void updateState(const QString &state);
};

#endif // MAINWINDOW_H