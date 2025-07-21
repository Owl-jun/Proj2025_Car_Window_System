#ifndef MQTTCLIENTWRAPPER_H
#define MQTTCLIENTWRAPPER_H

#include <QObject>
#include <QTimer>
#include <string>
#include <mqtt/async_client.h> // Paho MQTT C++ 라이브러리 헤더

class MqttClientWrapper : public QObject, public virtual mqtt::callback, public virtual mqtt::iaction_listener
{
    Q_OBJECT // Qt의 시그널/슬롯 시스템을 사용하기 위해 필수

public:
    explicit MqttClientWrapper(const std::string& serverUri, const std::string& clientId, QObject *parent = nullptr);
    ~MqttClientWrapper();

    void connectToBroker(); // 브로커에 연결 시도
    void disconnectFromBroker(); // 브로커에서 연결 해제
    void subscribe(const std::string& topic); // 토픽 구독
    void publish(const std::string& topic, const std::string& payload); // 메시지 발행

signals:
    void connected(); // 브로커에 연결되었을 때 발생
    void disconnected(); // 브로커와 연결이 끊어졌을 때 발생
    void messageReceived(const QString& topic, const QString& payload); // 메시지 수신 시 발생
    void mqttError(const QString& errorString); // MQTT 작업 중 에러 발생 시 발생

private slots:
    void reconnectAttempt(); // 재연결을 시도하는 슬롯

private:
    mqtt::async_client m_client; // Paho MQTT 비동기 클라이언트 객체
    mqtt::connect_options m_connOpts; // 연결 옵션
    std::string m_serverUri; // MQTT 브로커 URI
    std::string m_clientId; // MQTT 클라이언트 ID
    QTimer *m_reconnectTimer; // 재연결을 위한 타이머

    // Paho MQTT 콜백 함수들 (mqtt::callback 인터페이스 구현)
    void connection_lost(const std::string& cause) override; // 연결이 끊어졌을 때 호출
    void delivery_complete(mqtt::delivery_token_ptr tok) override; // 메시지 전달 완료 시 호출
    void message_arrived(mqtt::const_message_ptr msg) override; // 메시지 수신 시 호출

    // Paho MQTT 액션 리스너 콜백 함수들 (mqtt::iaction_listener 인터페이스 구현)
    void on_failure(const mqtt::token& tok) override; // 작업 실패 시 호출
    void on_success(const mqtt::token& tok) override; // 작업 성공 시 호출
};

#endif // MQTTCLIENTWRAPPER_H