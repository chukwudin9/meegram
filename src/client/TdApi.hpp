#pragma once

#include <td/telegram/td_api.h>

#include "client/Client.hpp"

#include <QObject>
#include <QVariant>

#include <functional>
#include <map>
#include <thread>
#include <unordered_map>

using Function = td::td_api::object_ptr<td::td_api::Function>;
using Callback = std::function<void(ObjectPtr)>;

class TdApi : public QObject
{
    Q_OBJECT
public:
    explicit TdApi(QObject *parent = nullptr);
    ~TdApi() override;

    Q_INVOKABLE void setOption(const QString &name, const QVariant &value);
    Q_INVOKABLE QVariant getOption(const QString &name) const;

    Q_INVOKABLE void close();
    Q_INVOKABLE void destory();

    Client *client();

signals:
    void updateAuthorizationState(td::td_api::AuthorizationState *state);
    void updateNewMessage(td::td_api::message *message);
    void updateNewChat(td::td_api::chat *chat);
    void updateOption(const QString &name, td::td_api::OptionValue *value);

    void error(const QString &errorString);

private slots:
    void onProcessUpdates(td::td_api::Object *update);

    void handleOptions(const QString &name, td::td_api::OptionValue *value);

private:
    void init();

    Client *m_client{};
    std::jthread m_clientThread;
    std::unordered_map<qint32, std::function<void(td::td_api::Object *)>> m_events;

    std::map<QString, QVariant> m_storeOptions;
};
