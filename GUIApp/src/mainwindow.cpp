#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QDateTime>
#include <QDebug> // 디버깅 출력용
#include <QFrame> // UI 요소 테두리 및 배경용

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // 이미지(800x480)에 기반하여 고정 크기 설정
    setFixedSize(800, 480);
    setWindowTitle("Smart Home Monitor"); // 윈도우 제목 설정

    setupUi(); // UI 요소 배치 및 스타일 설정
    setupMqtt(); // MQTT 클라이언트 설정

    // 현재 시간 업데이트를 위한 타이머 설정
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::updateCurrentTime);
    timer->start(1000); // 1초마다 업데이트
    updateCurrentTime(); // 초기 시간 업데이트
}

MainWindow::~MainWindow()
{
    // m_mqttClientWrapper는 QObject 계층 구조에 의해 부모가 삭제될 때 자동으로 삭제됩니다.
    // 명시적인 delete는 필요하지 않습니다.
}

void MainWindow::setupUi()
{
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget); // 중앙 위젯 설정

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(10, 10, 10, 10); // 여백 설정
    mainLayout->setSpacing(10); // 주요 섹션 간 간격 설정

    // 상단 바 (시간 표시)
    QHBoxLayout *topBarLayout = new QHBoxLayout();
    topBarLayout->addStretch(); // 시간을 오른쪽으로 밀어냅니다.
    m_currentTimeLabel = new QLabel("19:12"); // 초기 값 (업데이트됨)
    m_currentTimeLabel->setStyleSheet("font-size: 28px; font-weight: bold; color: white;");
    topBarLayout->addWidget(m_currentTimeLabel);
    mainLayout->addLayout(topBarLayout);

    // 메인 콘텐츠 영역 (버튼, 데이터, 로그)
    QHBoxLayout *contentLayout = new QHBoxLayout();
    contentLayout->setSpacing(15); // 컬럼 간 간격

    // 왼쪽 컬럼 (버튼)
    QFrame *buttonFrame = new QFrame();
    buttonFrame->setFrameShape(QFrame::StyledPanel);
    buttonFrame->setFrameShadow(QFrame::Raised);
    buttonFrame->setStyleSheet("background-color: #34495e; border-radius: 10px;"); // 어두운 배경, 둥근 모서리

    QVBoxLayout *buttonLayout = new QVBoxLayout(buttonFrame);
    buttonLayout->setContentsMargins(20, 20, 20, 20); // 내부 여백
    buttonLayout->setSpacing(15); // 버튼 간 간격

    m_upButton = new QPushButton("UP ICON");
    m_upButton->setFixedSize(160, 90); // 터치에 친화적인 크기
    m_upButton->setStyleSheet("QPushButton { font-size: 20px; font-weight: bold; color: #ecf0f1; background-color: #3498db; border-radius: 8px; }"
                              "QPushButton:pressed { background-color: #2980b9; }"); // 클릭 시 색상 변경
    connect(m_upButton, &QPushButton::clicked, this, &MainWindow::onUpButtonClicked);

    m_stopButton = new QPushButton("STOP ICON");
    m_stopButton->setFixedSize(160, 90);
    m_stopButton->setStyleSheet("QPushButton { font-size: 20px; font-weight: bold; color: #ecf0f1; background-color: #e74c3c; border-radius: 8px; }"
                                "QPushButton:pressed { background-color: #c0392b; }");
    connect(m_stopButton, &QPushButton::clicked, this, &MainWindow::onStopButtonClicked);

    m_downButton = new QPushButton("DOWN ICON");
    m_downButton->setFixedSize(160, 90);
    m_downButton->setStyleSheet("QPushButton { font-size: 20px; font-weight: bold; color: #ecf0f1; background-color: #f1c40f; border-radius: 8px; }"
                                "QPushButton:pressed { background-color: #d6b10d; }");
    connect(m_downButton, &QPushButton::clicked, this, &MainWindow::onDownButtonClicked);

    buttonLayout->addWidget(m_upButton);
    buttonLayout->addWidget(m_stopButton);
    buttonLayout->addWidget(m_downButton);
    buttonLayout->addStretch(); // 버튼을 상단에 정렬

    contentLayout->addWidget(buttonFrame); // 버튼 프레임을 콘텐츠 레이아웃에 추가

    // 가운데 컬럼 (온도, 습도 데이터)
    QFrame *dataFrame = new QFrame();
    dataFrame->setFrameShape(QFrame::StyledPanel);
    dataFrame->setFrameShadow(QFrame::Raised);
    dataFrame->setStyleSheet("background-color: #ecf0f1; border-radius: 10px; color: #2c3e50;"); // 밝은 배경, 어두운 텍스트

    QVBoxLayout *dataLayout = new QVBoxLayout(dataFrame);
    dataLayout->setContentsMargins(30, 30, 30, 30); // 내부 여백
    dataLayout->setSpacing(40); // 온도와 습도 사이 간격

    // 온도 줄
    QHBoxLayout *tempRowLayout = new QHBoxLayout();
    QLabel *tempText = new QLabel("TEMP");
    tempText->setStyleSheet("font-size: 32px; font-weight: bold;");
    m_tempLabel = new QLabel("21"); // 초기 값
    m_tempLabel->setStyleSheet("font-size: 48px; font-weight: bold; color: #3498db;"); // 더 큰 글꼴, 파란색
    QLabel *tempUnit = new QLabel("도");
    tempUnit->setStyleSheet("font-size: 32px;");
    tempRowLayout->addWidget(tempText);
    tempRowLayout->addStretch();
    tempRowLayout->addWidget(m_tempLabel);
    tempRowLayout->addWidget(tempUnit);

    // 습도 줄
    QHBoxLayout *humiRowLayout = new QHBoxLayout();
    QLabel *humiText = new QLabel("HUMI");
    humiText->setStyleSheet("font-size: 32px; font-weight: bold;");
    m_humiLabel = new QLabel("34"); // 초기 값
    m_humiLabel->setStyleSheet("font-size: 48px; font-weight: bold; color: #2ecc71;"); // 더 큰 글꼴, 초록색
    QLabel *humiUnit = new QLabel("%");
    humiUnit->setStyleSheet("font-size: 32px;");
    humiRowLayout->addWidget(humiText);
    humiRowLayout->addStretch();
    humiRowLayout->addWidget(m_humiLabel);
    humiRowLayout->addWidget(humiUnit);

    dataLayout->addLayout(tempRowLayout);
    dataLayout->addLayout(humiRowLayout);
    dataLayout->addStretch(); // 데이터를 상단에 정렬

    contentLayout->addWidget(dataFrame, 2); // 데이터 프레임에 더 많은 공간 할당 (비율 2)

    // 오른쪽 컬럼 (로그)
    QFrame *logFrame = new QFrame();
    logFrame->setFrameShape(QFrame::StyledPanel);
    logFrame->setFrameShadow(QFrame::Raised);
    logFrame->setStyleSheet("background-color: #bdc3c7; border-radius: 10px; color: #2c3e50;"); // 어두운 회색 배경, 어두운 텍스트

    QVBoxLayout *logLayout = new QVBoxLayout(logFrame);
    logLayout->setContentsMargins(15, 15, 15, 15);
    logLayout->setSpacing(10);

    QLabel *logTitle = new QLabel("LOG:");
    logTitle->setStyleSheet("font-size: 24px; font-weight: bold;");
    m_logLabel = new QLabel("OPEN : 250721 19:10"); // 초기 로그 메시지
    m_logLabel->setStyleSheet("font-size: 18px;");
    m_logLabel->setWordWrap(true); // 텍스트가 길어지면 줄 바꿈
    m_logLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft); // 텍스트를 좌측 상단에 정렬

    logLayout->addWidget(logTitle);
    logLayout->addWidget(m_logLabel);
    logLayout->addStretch(); // 나머지 공간 채우기

    contentLayout->addWidget(logFrame, 1); // 로그 프레임에 적은 공간 할당 (비율 1)

    mainLayout->addLayout(contentLayout);

    // 하단 바 (상태 표시)
    QHBoxLayout *bottomBarLayout = new QHBoxLayout();
    bottomBarLayout->addStretch();
    m_stateLabel = new QLabel("STATE : STOP"); // 초기 상태 메시지
    m_stateLabel->setStyleSheet("font-size: 28px; font-weight: bold; color: #e74c3c;"); // 기본은 빨간색 (STOP)
    bottomBarLayout->addWidget(m_stateLabel);
    mainLayout->addLayout(bottomBarLayout);

    // 전체 윈도우 배경색 설정 (선택 사항)
    centralWidget->setStyleSheet("background-color: #2c3e50; color: white;"); // 어두운 파란색 배경, 흰색 텍스트
}

void MainWindow::setupMqtt()
{
    // Paho MQTT Client Wrapper 인스턴스 생성
    // 브로커 주소와 클라이언트 ID를 설정합니다.
    m_mqttClientWrapper = new MqttClientWrapper("192.168.0.2:1883", "SmartHomeMonitor_Client", this);

    // Wrapper의 시그널을 MainWindow의 슬롯에 연결
    connect(m_mqttClientWrapper, &MqttClientWrapper::connected, this, &MainWindow::onMqttConnected);
    connect(m_mqttClientWrapper, &MqttClientWrapper::disconnected, this, &MainWindow::onMqttDisconnected);
    connect(m_mqttClientWrapper, &MqttClientWrapper::messageReceived, this, &MainWindow::onMqttMessageReceived);
    connect(m_mqttClientWrapper, &MqttClientWrapper::mqttError, this, &MainWindow::onMqttError);

    // MQTT 브로커에 연결 시도
    m_mqttClientWrapper->connectToBroker();
}

void MainWindow::onMqttConnected()
{
    qDebug() << "MQTT 브로커에 연결되었습니다.";
    // 연결 성공 시 구독할 토픽들을 설정합니다.
    m_mqttClientWrapper->subscribe("sensor/rain_dht"); // 예시 토픽

    updateState("STATE : CONNECTED"); // 상태 표시 업데이트
}

void MainWindow::onMqttDisconnected()
{
    qDebug() << "MQTT 브로커와 연결이 끊어졌습니다.";
    updateState("STATE : DISCONNECTED"); // 상태 표시 업데이트
}

void MainWindow::onMqttMessageReceived(const QString& topic, const QString& payload)
{
    qDebug() << "MQTT 메시지 수신 - 토픽:" << topic << "페이로드:" << payload;

    if (topic == "sensor/temperature") {
        updateTemperature(payload);
    } else if (topic == "sensor/humidity") {
        updateHumidity(payload);
    } else if (topic == "system/log") {
        updateLog(payload);
    } else if (topic == "system/state") {
        updateState(payload);
    }
}

void MainWindow::onMqttError(const QString& errorString)
{
    qCritical() << "MQTT 에러 발생:" << errorString;
    updateLog("MQTT Error: " + errorString); // 로그에 에러 메시지 추가
}

void MainWindow::onUpButtonClicked()
{
    qDebug() << "UP 버튼이 클릭되었습니다!";
    // MQTT 메시지 발행 (예시)
    m_mqttClientWrapper->publish("control/up", "1");
    updateLog("UP 버튼이 눌렸습니다.");
}

void MainWindow::onStopButtonClicked()
{
    qDebug() << "STOP 버튼이 클릭되었습니다!";
    // MQTT 메시지 발행 (예시)
    m_mqttClientWrapper->publish("control/stop", "1");
    updateLog("STOP 버튼이 눌렸습니다.");
    updateState("STATE : STOP"); // 예시: 상태를 STOP으로 변경
}

void MainWindow::onDownButtonClicked()
{
    qDebug() << "DOWN 버튼이 클릭되었습니다!";
    // MQTT 메시지 발행 (예시)
    m_mqttClientWrapper->publish("control/down", "1");
    updateLog("DOWN 버튼이 눌렸습니다.");
}

void MainWindow::updateCurrentTime()
{
    m_currentTimeLabel->setText(QDateTime::currentDateTime().toString("HH:mm"));
}

void MainWindow::updateTemperature(const QString &temp)
{
    m_tempLabel->setText(temp);
}

void MainWindow::updateHumidity(const QString &humi)
{
    m_humiLabel->setText(humi);
}

void MainWindow::updateLog(const QString &logEntry)
{
    QString currentLog = m_logLabel->text();
    QString newLog = QDateTime::currentDateTime().toString("HH:mm:ss") + " " + logEntry;

    if (currentLog.isEmpty()) {
        m_logLabel->setText(newLog);
    } else {
        // 현재 로그에 새 로그를 추가합니다.
        // QScrollArea와 QTextEdit을 사용하면 더 많은 로그를 효율적으로 보여줄 수 있습니다.
        // 현재는 QLabel의 제한으로 인해 너무 많은 줄은 잘릴 수 있습니다.
        m_logLabel->setText(currentLog + "\n" + newLog);
    }
}

void MainWindow::updateState(const QString &state)
{
    m_stateLabel->setText(state);
    // 상태 문자열에 따라 텍스트 색상을 변경하여 시각적 피드백 제공
    if (state.contains("STOP", Qt::CaseInsensitive) || state.contains("DISCONNECTED", Qt::CaseInsensitive)) {
        m_stateLabel->setStyleSheet("font-size: 28px; font-weight: bold; color: #e74c3c;"); // 빨간색
    } else if (state.contains("CONNECTED", Qt::CaseInsensitive) || state.contains("RUN", Qt::CaseInsensitive)) {
        m_stateLabel->setStyleSheet("font-size: 28px; font-weight: bold; color: #2ecc71;"); // 초록색
    } else {
        m_stateLabel->setStyleSheet("font-size: 28px; font-weight: bold; color: #f39c12;"); // 주황색
    }
}