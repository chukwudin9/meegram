#include "client/Client.hpp"

Client::Client(QObject *parent)
    : QObject(parent)
{
    td::Client::execute({0, td::td_api::make_object<td::td_api::setLogVerbosityLevel>()});
    m_client = std::make_unique<td::Client>();
}

void Client::sendRequest(td::td_api::object_ptr<td::td_api::Function> f, std::function<void(ObjectPtr)> handler)
{
    auto requestId = nextRequestId();
    if (handler) {
        m_handlers.emplace(requestId, std::move(handler));
    }
    m_client->send({requestId, std::move(f)});
}

void Client::listen()
{
    while (true) {
        if (auto response = m_client->receive(10); response.object) {
            if (response.object->get_id() == td::td_api::updateAuthorizationState::ID &&
                static_cast<td::td_api::updateAuthorizationState &>(*response.object).authorization_state_->get_id() ==
                    td::td_api::authorizationStateClosed::ID) {
                break;
            }

            processResponse(std::move(response));
        }
    }
}

void Client::processResponse(td::Client::Response response)
{
    if (!response.object) {
        return;
    }

    if (response.id == 0) {
        return emitSignal(std::move(response.object));
    }

    if (auto it = m_handlers.find(response.id); it != m_handlers.end()) {
        it->second(std::move(response.object));
    }
}

void Client::emitSignal(ObjectPtr update)
{
    emit processUpdates(update.release());
}

quint64 Client::nextRequestId()
{
    quint64 currentRequestId{0};
    return ++currentRequestId;
}
