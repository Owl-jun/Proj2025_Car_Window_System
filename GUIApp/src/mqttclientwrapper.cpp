#include "mqttclientwrapper.h"
#include <QDebug> 
#include <QHostInfo> // 호스트 정보 획득용 (현재 사용되지는 않음)

// std::string을 QString으로 변환하는 헬퍼 함수
inline QString toQString(const std::string& s) {
    return QString::fromStdString(s);
}

MqttClientWrapper::MqttClientWrapper(const std::string& serverUri, const std::string& clientId, QObject *parent)
    : QObject(parent),
      m_client(serverUri, clientId), // Paho MQTT 클라이언트 초기화
      m_serverUri(serverUri),
      m_clientId(clientId)
{
    m_client.set_callback(*this); // 이 클래스를 Paho MQTT 콜백 핸들러로 설정

    // 연결 옵션 설정
    m_connOpts.set_keep_alive_interval(20); // 20초마다 keep-alive 메시지 전송
    m_connOpts.set_clean_session(true);    // 새로운 연결 시 이전 세션 정보 제거

    // 재연결 타이머 설정
    m_reconnectTimer = new QTimer(this);
    m_reconnectTimer->setInterval(5000); // 5초마다 재연결 시도
    m_reconnectTimer->setSingleShot(true); // 한 번만 실행되도록 설정 (연결 끊김 시 재시작)
    connect(m_reconnectTimer, &QTimer::timeout, this, &MqttClientWrapper::reconnectAttempt);
}

MqttClientWrapper::~MqttClientWrapper()
{
    // 소멸 시 브로커에서 연결 해제
    disconnectFromBroker();
}

void MqttClientWrapper::connectToBroker()
{
    qDebug() << "MQTT 브로커에 연결 시도 중:" << toQString(m_serverUri);
    try {
        // connect()는 토큰을 반환
        // wait() 은 BLOCKING 방식
        m_client.connect(m_connOpts, nullptr, *this)->wait();
        // wait()가 성공하면 on_success가 호출, connected() 시그널 쏨
    } catch (const mqtt::exception& exc) {
        qCritical() << "MQTT 연결 에러:" << exc.what();
        emit mqttError(QString("연결 실패: ") + exc.what());
        reconnectAttempt(); // 연결 실패 시 재연결 시도 예약
    }
}

void MqttClientWrapper::disconnectFromBroker()
{
    if (m_client.is_connected()) {
        qDebug() << "MQTT 브로커에서 연결 해제 중.";
        try {
            m_client.disconnect()->wait();
            emit disconnected();
        } catch (const mqtt::exception& exc) {
            qCritical() << "MQTT 연결 해제 에러:" << exc.what();
            emit mqttError(QString("연결 해제 실패: ") + exc.what());
        }
    }
    m_reconnectTimer->stop(); // 재연결 시도 중이라면 타이머 중지
}

void MqttClientWrapper::subscribe(const std::string& topic)
{
    if (m_client.is_connected()) {
        qDebug() << "토픽 구독 중:" << toQString(topic);
        try {
            m_client.subscribe(topic, 1)->wait(); // QoS 1로 구독
        } catch (const mqtt::exception& exc) {
            qCritical() << "MQTT 구독 에러:" << exc.what();
            emit mqttError(QString("구독 실패: ") + exc.what());
        }
    } else {
        qWarning() << "연결되지 않아 토픽을 구독할 수 없습니다.";
    }
}

void MqttClientWrapper::publish(const std::string& topic, const std::string& payload)
{
    if (m_client.is_connected()) {
        qDebug() << "토픽에 발행 중:" << toQString(topic) << "페이로드:" << toQString(payload);
        try {
            mqtt::message_ptr pubmsg = mqtt::make_message(topic, payload);
            pubmsg->set_qos(1); // QoS 1로 메시지 설정
            m_client.publish(pubmsg)->wait();
        } catch (const mqtt::exception& exc) {
            qCritical() << "MQTT 발행 에러:" << exc.what();
            emit mqttError(QString("발행 실패: ") + exc.what());
        }
    } else {
        qWarning() << "연결되지 않아 메시지를 발행할 수 없습니다.";
    }
}

void MqttClientWrapper::reconnectAttempt()
{
    if (!m_client.is_connected()) {
        qDebug() << "MQTT 재연결 시도 중...";
        try {
            m_client.connect(m_connOpts, nullptr, *this); // 비동기 연결 시도
        } catch (const mqtt::exception& exc) {
            qCritical() << "MQTT 재연결 시도 실패:" << exc.what();
            emit mqttError(QString("재연결 시도 실패: ") + exc.what());
            m_reconnectTimer->start(); // 다음 재연결 시도 예약
        }
    }
}

// Paho MQTT 콜백 구현
void MqttClientWrapper::connection_lost(const std::string& cause)
{
    qWarning() << "MQTT 연결 끊김:" << (cause.empty() ? "원인 없음" : toQString(cause));
    emit disconnected();
    m_reconnectTimer->start(); // 연결 끊김 시 재연결 타이머 시작
}

void MqttClientWrapper::delivery_complete(mqtt::delivery_token_ptr tok)
{
    // 메시지 전달 성공 후 콜백임 
    Q_UNUSED(tok);
}

void MqttClientWrapper::message_arrived(mqtt::const_message_ptr msg) 
{
    QString topic = toQString(msg->get_topic());
    QString payload = toQString(msg->to_string());
    // qDebug() << "MQTT 메시지 도착 - 토픽:" << topic << "페이로드:" << payload;
    emit messageReceived(topic, payload); // Qt 시그널로 메시지 전달
}

// Paho MQTT 액션 리스너 콜백 구현
void MqttClientWrapper::on_failure(const mqtt::token& tok)
{
    // 초기 연결 토큰(message_id가 0)이 아닌 다른 작업(구독, 발행 등)의 실패를 처리
    if (tok.get_message_id() != 0) {
        qCritical() << "MQTT 작업 실패, 토큰 ID:" << tok.get_message_id();
        emit mqttError(QString("MQTT 작업 실패: "));
    }
    // 초기 연결 실패인 경우
    if (tok.get_message_id() == m_client.connect()->get_message_id()) {
         qCritical() << "MQTT 초기 연결 실패!";
         emit mqttError("초기 MQTT 연결 실패.");
         m_reconnectTimer->start(); // 초기 연결 실패 시 재연결 시도 예약
    }
}

void MqttClientWrapper::on_success(const mqtt::token& tok)
{
    // 초기 연결 토큰(message_id가 0)의 성공을 처리
    if (tok.get_message_id() == 0 && tok.get_user_context() == nullptr) {
         qDebug() << "MQTT 연결 성공!";
         m_reconnectTimer->stop(); // 연결 성공 시 재연결 타이머 중지
         emit connected(); // Qt 시그널 방출
    } else {
        // TODO : 발행 혹은 구독 시 로직~
    }
}