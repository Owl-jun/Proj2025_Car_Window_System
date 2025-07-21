#include "Mqtt/CMqttCallback.hpp"

CMqttCallback::CMqttCallback(QObject* parent)
    : QObject(parent)
{}

void CMqttCallback::message_arrived(mqtt::const_message_ptr msg)
{
    emit messageReceived(
        QString::fromStdString(msg->get_topic()),
        QString::fromStdString(msg->to_string())
    );
}
