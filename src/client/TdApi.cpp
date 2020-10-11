#include "client/TdApi.hpp"

TdApi::TdApi(QObject *parent)
    : QObject(parent)
    , m_client(new Client())
{
    init();

    connect(m_client, SIGNAL(processUpdates(td::td_api::Object *)), SLOT(onProcessUpdates(td::td_api::Object *)));
    connect(this, SIGNAL(updateOption(const QString &, td::td_api::OptionValue *)),
            SLOT(handleOptions(const QString &, td::td_api::OptionValue *)));
}

TdApi::~TdApi()
{
    delete m_client;
}

void TdApi::setOption(const QString &name, const QVariant &value)
{
    switch (value.type()) {
        case QVariant::String:
            m_client->sendRequest(td::td_api::make_object<td::td_api::setOption>(
                name.toStdString(), td::td_api::make_object<td::td_api::optionValueString>(value.toString().toStdString())));
            break;
        case QVariant::Bool:
            m_client->sendRequest(td::td_api::make_object<td::td_api::setOption>(
                name.toStdString(), td::td_api::make_object<td::td_api::optionValueBoolean>(value.toBool())));
            break;
        case QVariant::Int:
            m_client->sendRequest(td::td_api::make_object<td::td_api::setOption>(
                name.toStdString(), td::td_api::make_object<td::td_api::optionValueInteger>(value.toInt())));
            break;
        default:
            m_client->sendRequest(td::td_api::make_object<td::td_api::setOption>(name.toStdString(),
                                                                                 td::td_api::make_object<td::td_api::optionValueEmpty>()));
            break;
    }
}

QVariant TdApi::getOption(const QString &name) const
{
    if (auto it = m_storeOptions.find(name); it != m_storeOptions.end()) {
        return QVariant(it->second);
    }

    return {};
}

void TdApi::close()
{
    m_client->sendRequest(td::td_api::make_object<td::td_api::close>());
}

void TdApi::destory()
{
    m_client->sendRequest(td::td_api::make_object<td::td_api::destroy>());
}

Client *TdApi::client()
{
    return m_client;
}

void TdApi::handleOptions(const QString &name, td::td_api::OptionValue *value)
{
    QVariant result;
    switch (value->get_id()) {
        case td::td_api::optionValueString::ID:
            result = QString::fromStdString(static_cast<td::td_api::optionValueString *>(value)->value_);
            break;
        case td::td_api::optionValueInteger::ID:
            result = QVariant::fromValue(static_cast<td::td_api::optionValueInteger *>(value)->value_).toLongLong();
            break;
        case td::td_api::optionValueBoolean::ID:
            result = static_cast<td::td_api::optionValueBoolean *>(value)->value_;
            break;
        default:
            break;
    }

    m_storeOptions.emplace(name, result);
}

void TdApi::onProcessUpdates(td::td_api::Object *update)
{
    auto type = update->get_id();

    if (auto it = m_events.find(type); it != m_events.end()) {
        it->second(update);
    }
}

void TdApi::init()
{
    m_clientThread = std::jthread(&Client::listen, m_client);

    m_events.emplace(td::td_api::updateAuthorizationState::ID, [this](td::td_api::Object *update) {
        auto authorizationState = static_cast<td::td_api::updateAuthorizationState *>(update);
        emit updateAuthorizationState(authorizationState->authorization_state_.release());
    });

    m_events.emplace(td::td_api::updateNewMessage::ID, [this](td::td_api::Object *update) {
        auto message = static_cast<td::td_api::updateNewMessage *>(update);
        emit updateNewMessage(message->message_.release());
    });

    m_events.emplace(td::td_api::updateNewChat::ID, [this](td::td_api::Object *update) {
        auto chat = static_cast<td::td_api::updateNewChat *>(update);
        emit updateNewChat(chat->chat_.release());
    });

    m_events.emplace(td::td_api::updateOption::ID, [this](td::td_api::Object *update) {
        auto option = static_cast<td::td_api::updateOption *>(update);
        emit updateOption(QString::fromStdString(option->name_), option->value_.release());
    });

    m_clientThread.detach();
}
