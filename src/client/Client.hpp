#pragma once

#include <td/telegram/Client.h>

#include <QObject>

#include <functional>
#include <map>

using ObjectPtr = td::td_api::object_ptr<td::td_api::Object>;

class Client : public QObject
{
    Q_OBJECT
public:
    explicit Client(QObject *parent = nullptr);

    void sendRequest(td::td_api::object_ptr<td::td_api::Function> f, std::function<void(ObjectPtr)> handler = {});

    void listen();

signals:
    void processUpdates(td::td_api::Object *update);

private:
    void processResponse(td::Client::Response response);
    void emitSignal(ObjectPtr update);
    quint64 nextRequestId();

    std::unique_ptr<td::Client> m_client{};
    std::map<quint64, std::function<void(ObjectPtr)>> m_handlers;
};
