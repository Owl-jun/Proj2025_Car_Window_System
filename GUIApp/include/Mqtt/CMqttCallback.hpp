#pragma once

#include <QObject>
#include <QString>
#include "mqtt/async_client.h"

class CMqttCallback : public QObject, public virtual mqtt::callback {
    Q_OBJECT

public:
    explicit CMqttCallback(QObject* parent = nullptr);

signals:
    void messageReceived(QString topic, QString payload);

private:
    void message_arrived(mqtt::const_message_ptr msg) override;
};
