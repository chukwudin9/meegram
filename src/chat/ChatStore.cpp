#include "ChatStore.hpp"

#include <QDebug>

namespace {

td::td_api::object_ptr<td::td_api::ChatList> asChatList(QVariant chatList)
{
    if (chatList.canConvert(QVariant::String)) {
        return td::td_api::make_object<td::td_api::chatListArchive>();
    }
    if (chatList.canConvert(QVariant::Int)) {
        return td::td_api::make_object<td::td_api::chatListFilter>(chatList.toInt());
    }
    return td::td_api::make_object<td::td_api::chatListMain>();
}
}  // namespace

ChatStore::ChatStore(TdApi *controller, QObject *parent)
    : TdApi(parent)
    , m_client(controller->client())
{
    connect(controller, SIGNAL(updateNewChat(td::td_api::chat *)), SLOT(onNewChatUpdate(td::td_api::chat *)));
}

void ChatStore::closeChat(qint64 chatId)
{
    m_client->sendRequest(td::td_api::make_object<td::td_api::closeChat>(chatId));
}

void ChatStore::closeSecretChat(qint32 secretChatId)
{
    m_client->sendRequest(td::td_api::make_object<td::td_api::closeSecretChat>(secretChatId));
}

void ChatStore::createBasicGroupChat(qint32 basicGroupId, bool force)
{
    m_client->sendRequest(td::td_api::make_object<td::td_api::createBasicGroupChat>(basicGroupId, force));
}

void ChatStore::createNewBasicGroupChat(QList<qint32> userIds, const QString &title)
{
    std::vector<int32_t> result;
    for (auto userId : userIds) {
        result.emplace_back(userId);
    }

    m_client->sendRequest(td::td_api::make_object<td::td_api::createNewBasicGroupChat>(std::move(result), title.toStdString()));
}

void ChatStore::createNewSecretChat(qint32 userId)
{
    m_client->sendRequest(td::td_api::make_object<td::td_api::createNewSecretChat>(userId));
}

void ChatStore::createNewSupergroupChat(const QString &title, bool isChannel, const QString &description /*ChatLocation *location*/)
{
    m_client->sendRequest(
        td::td_api::make_object<td::td_api::createNewSupergroupChat>(title.toStdString(), isChannel, description.toStdString(), nullptr));
}

void ChatStore::createPrivateChat(qint32 userId, bool force)
{
    m_client->sendRequest(td::td_api::make_object<td::td_api::createPrivateChat>(userId, force));
}

void ChatStore::createSecretChat(qint32 secretChatId)
{
    m_client->sendRequest(td::td_api::make_object<td::td_api::createSecretChat>(secretChatId));
}

void ChatStore::createSupergroupChat(qint32 supergroupId, bool force)
{
    m_client->sendRequest(td::td_api::make_object<td::td_api::createSupergroupChat>(supergroupId, force));
}

void ChatStore::deleteChatHistory(qint64 chatId, bool removeFromChatList, bool revoke)
{
    m_client->sendRequest(td::td_api::make_object<td::td_api::deleteChatHistory>(chatId, removeFromChatList, revoke));
}

void ChatStore::getChat(qint64 chatId)
{
    m_client->sendRequest(td::td_api::make_object<td::td_api::getChat>(chatId));
}

void ChatStore::getChats(QVariant chatList, qint64 offsetOrder, qint64 offsetChatId, qint32 limit)
{
    m_client->sendRequest(td::td_api::make_object<td::td_api::getChats>(asChatList(chatList), offsetOrder, offsetChatId, limit));
}

void ChatStore::getSecretChat(qint32 secretChatId)
{
    m_client->sendRequest(td::td_api::make_object<td::td_api::getSecretChat>(secretChatId));
}

void ChatStore::getSupergroup(qint32 supergroupId)
{
    m_client->sendRequest(td::td_api::make_object<td::td_api::getSupergroup>(supergroupId));
}

void ChatStore::getSupergroupFullInfo(qint32 supergroupId)
{
    m_client->sendRequest(td::td_api::make_object<td::td_api::getSupergroupFullInfo>(supergroupId));
}

void ChatStore::leaveChat(qint64 chatId)
{
    m_client->sendRequest(td::td_api::make_object<td::td_api::leaveChat>(chatId));
}

void ChatStore::openChat(qint64 chatId)
{
    m_client->sendRequest(td::td_api::make_object<td::td_api::openChat>(chatId));
}

void ChatStore::setChatTitle(qint64 chatId, const QString &title)
{
    m_client->sendRequest(td::td_api::make_object<td::td_api::setChatTitle>(chatId, title.toStdString()));
}

void ChatStore::setPinnedChats(QVariant chatList, QList<qint64> chatIds)
{
}

void ChatStore::onNewChatUpdate(td::td_api::chat *chat)
{
    qDebug() << "[ Chat ]" << QString::fromStdString(chat->title_) << chat->unread_count_;
}
