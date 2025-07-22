#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QDateTime>
#include <QDebug> 
#include <QFrame> 
#include <QJsonDocument> 
#include <QJsonObject>   
#include <QJsonValue>   
#include <QFile>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setFixedSize(800, 480);
    setWindowTitle("RainDetected Monitor");

    setupUi(); 
    setupMqtt(); 

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::updateCurrentTime);
    timer->start(1000); 
    updateCurrentTime(); 
}

MainWindow::~MainWindow()
{
    // =========================
    // m_mqttClientWrapper는 QObject 계층 구조에 의해 부모가 삭제될 때 자동으로 삭제됩니다.
    // 명시적인 delete는 필요하지 않습니다.
    // =========================
}

void MainWindow::setupUi()
{
    // =========================
    // 전체 위젯
    // =========================
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget); 

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(10, 10, 10, 10); 
    mainLayout->setSpacing(10); 

    // 상단 바 (시간 표시)
    QHBoxLayout *topBarLayout = new QHBoxLayout();
    topBarLayout->addStretch(); // 시간을 오른쪽으로.
    m_currentTimeLabel = new QLabel("19:12"); // 더미 초기 값
    m_currentTimeLabel->setStyleSheet("font-size: 28px; font-weight: bold; color: white;");
    topBarLayout->addWidget(m_currentTimeLabel);
    mainLayout->addLayout(topBarLayout);

    // 메인 콘텐츠 영역 (버튼, 데이터, 로그)
    QHBoxLayout *contentLayout = new QHBoxLayout();
    contentLayout->setSpacing(15); 

    // 왼쪽 컬럼 (버튼)
    QFrame *buttonFrame = new QFrame();
    buttonFrame->setFrameShape(QFrame::StyledPanel);
    buttonFrame->setFrameShadow(QFrame::Raised);
    buttonFrame->setStyleSheet("background-color: #34495e; border-radius: 10px;"); 

    QVBoxLayout *buttonLayout = new QVBoxLayout(buttonFrame);
    buttonLayout->setContentsMargins(20, 20, 20, 20);
    buttonLayout->setSpacing(15); 

    m_upButton = new QPushButton("UP");
    m_upButton->setFixedSize(160, 90); 
    m_upButton->setStyleSheet("QPushButton { font-size: 20px; font-weight: bold; color: #ecf0f1; background-color: #3498db; border-radius: 8px; }"
                              "QPushButton:pressed { background-color: #2980b9; }"); 
    connect(m_upButton, &QPushButton::clicked, this, &MainWindow::onUpButtonClicked);

    m_stopButton = new QPushButton("STOP");
    m_stopButton->setFixedSize(160, 90);
    m_stopButton->setStyleSheet("QPushButton { font-size: 20px; font-weight: bold; color: #ecf0f1; background-color: #e74c3c; border-radius: 8px; }"
                                "QPushButton:pressed { background-color: #c0392b; }");
    connect(m_stopButton, &QPushButton::clicked, this, &MainWindow::onStopButtonClicked);

    m_downButton = new QPushButton("DOWN");
    m_downButton->setFixedSize(160, 90);
    m_downButton->setStyleSheet("QPushButton { font-size: 20px; font-weight: bold; color: #ecf0f1; background-color: #f1c40f; border-radius: 8px; }"
                                "QPushButton:pressed { background-color: #d6b10d; }");
    connect(m_downButton, &QPushButton::clicked, this, &MainWindow::onDownButtonClicked);

    buttonLayout->addWidget(m_upButton);
    buttonLayout->addWidget(m_stopButton);
    buttonLayout->addWidget(m_downButton);
    buttonLayout->addStretch();

    contentLayout->addWidget(buttonFrame); 

    // 가운데 컬럼 (온도, 습도 데이터)
    QFrame *dataFrame = new QFrame();
    dataFrame->setFrameShape(QFrame::StyledPanel);
    dataFrame->setFrameShadow(QFrame::Raised);
    dataFrame->setStyleSheet("background-color: #ecf0f1; border-radius: 10px; color: #2c3e50;"); 

    QVBoxLayout *dataLayout = new QVBoxLayout(dataFrame);
    dataLayout->setContentsMargins(30, 30, 30, 30); 
    dataLayout->setSpacing(40); 

    // 온도 줄
    QHBoxLayout *tempRowLayout = new QHBoxLayout();
    QLabel *tempText = new QLabel("TEMP");
    tempText->setStyleSheet("font-size: 32px; font-weight: bold;");
    m_tempLabel = new QLabel("21"); // 초기 값
    m_tempLabel->setStyleSheet("font-size: 48px; font-weight: bold; color: #3498db;");
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
    m_humiLabel = new QLabel("34"); 
    m_humiLabel->setStyleSheet("font-size: 48px; font-weight: bold; color: #2ecc71;");
    QLabel *humiUnit = new QLabel("%");
    humiUnit->setStyleSheet("font-size: 32px;");
    humiRowLayout->addWidget(humiText);
    humiRowLayout->addStretch();
    humiRowLayout->addWidget(m_humiLabel);
    humiRowLayout->addWidget(humiUnit);

    // 상태 줄
    // QHBoxLayout *strLayout = new QHBoxLayout();
    // QLabel *dataLabel = new QLabel("Current : ");
    // dataLabel->setStyleSheet("font-size: 32px; font-weight: bold;");
    // strLayout->addWidget(dataLabel);

    dataLayout->addLayout(tempRowLayout);
    dataLayout->addLayout(humiRowLayout);
    dataLayout->addStretch(); // 데이터를 상단에 정렬
    contentLayout->addWidget(dataFrame, 2); 

    // 오른쪽 컬럼 (로그)
    QFrame *logFrame = new QFrame();
    logFrame->setFrameShape(QFrame::StyledPanel);
    logFrame->setFrameShadow(QFrame::Raised);
    logFrame->setStyleSheet("background-color: #bdc3c7; border-radius: 10px; color: #2c3e50;");

    QVBoxLayout *logLayout = new QVBoxLayout(logFrame);
    logLayout->setContentsMargins(15, 15, 15, 15);
    logLayout->setSpacing(10);

    QLabel *logTitle = new QLabel("LOG:");
    logTitle->setStyleSheet("font-size: 24px; font-weight: bold;");
    m_logLabel = new QLabel("OPEN : 250721 19:10"); 
    m_logLabel->setStyleSheet("font-size: 18px;");
    m_logLabel->setWordWrap(true);
    m_logLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft); 

    logLayout->addWidget(logTitle);
    logLayout->addWidget(m_logLabel);
    logLayout->addStretch(); 

    contentLayout->addWidget(logFrame, 1); 

    mainLayout->addLayout(contentLayout);

    // 하단 바 
    QHBoxLayout *bottomBarLayout = new QHBoxLayout();
    bottomBarLayout->addStretch();
    m_stateLabel = new QLabel("STATE : STOP"); // 초기 상태 메시지
    m_stateLabel->setStyleSheet("font-size: 28px; font-weight: bold; color: #e74c3c;"); 
    bottomBarLayout->addWidget(m_stateLabel);
    mainLayout->addLayout(bottomBarLayout);

    // 전체 윈도우 배경색 설정 
    centralWidget->setStyleSheet("background-color: #2c3e50; color: white;"); 
}

void MainWindow::setupMqtt()
{
    QFile file("../secure/config.json");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning("Failed to open JSON file.");
    }
    QByteArray data = file.readAll();
    file.close();
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        qWarning("JSON parse error: %s", qUtf8Printable(parseError.errorString()));
    }

    QJsonObject config(doc.object());
    auto ip = config["MQTT"].toObject()["IP"].toString();
    m_mqttClientWrapper = new MqttClientWrapper(ip.toStdString(), "GUIAPP_Client", this);

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
    m_mqttClientWrapper->subscribe("sensor/rain_dht");
    updateState("STATE : CONNECTED");
}

void MainWindow::onMqttDisconnected()
{
    qDebug() << "MQTT 브로커와 연결이 끊어졌습니다.";
    updateState("STATE : DISCONNECTED"); 
}

void MainWindow::onMqttMessageReceived(const QString& topic, const QString& payload)
{
    qDebug() << "MQTT 메시지 수신 - 토픽:" << topic << "페이로드:" << payload;

    if (topic == "sensor/rain_dht") {
        QJsonDocument doc = QJsonDocument::fromJson(payload.toUtf8());

        if (doc.isObject()) {
            QJsonObject obj = doc.object();
            
            if (obj.contains("temperature") && obj["temperature"].isDouble()) {
                double temperature = obj["temperature"].toDouble();
                updateTemperature(QString::number(temperature, 'f', 1)); 
            }

            if (obj.contains("humidity") && obj["humidity"].isDouble()) {
                double humidity = obj["humidity"].toDouble();
                updateHumidity(QString::number(humidity, 'f', 1)); 
            }
            
            // ===============
            // 비 올때 핸들링
            // ===============
            if (obj.contains("digital_rain") && obj.contains("analog_rain") && obj["digital_rain"].toBool() != 0 && obj["analog_rain"].toInt() < 500) {
                onUpButtonClicked();
            }

        } else {
            qWarning() << "수신된 페이로드가 유효한 JSON 객체가 아닙니다:" << payload;
        }
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
    m_mqttClientWrapper->publish("control", "1"); // UP
    updateState("STATE : RUN"); 

}

void MainWindow::onStopButtonClicked()
{
    qDebug() << "STOP 버튼이 클릭되었습니다!";
    m_mqttClientWrapper->publish("control", "0"); // STOP
    updateState("STATE : STOP"); 
}

void MainWindow::onDownButtonClicked()
{
    qDebug() << "DOWN 버튼이 클릭되었습니다!";
    m_mqttClientWrapper->publish("control", "-1"); // DOWN
    updateState("STATE : RUN"); 
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