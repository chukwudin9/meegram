#pragma once

#include "client/TdApi.hpp"

#include <QObject>

class ChatStore : public TdApi
{
    Q_OBJECT
public:
    explicit ChatStore(TdApi *controller, QObject *parent = nullptr);

    Q_INVOKABLE void closeChat(qint64 chatId);
    Q_INVOKABLE void closeSecretChat(qint32 secretChatId);
    Q_INVOKABLE void createBasicGroupChat(qint32 basicGroupId, bool force);
    Q_INVOKABLE void createNewBasicGroupChat(QList<qint32> userIds, const QString &title);
    Q_INVOKABLE void createNewSecretChat(qint32 userId);
    Q_INVOKABLE void createNewSupergroupChat(const QString &title, bool isChannel, const QString &description);
    Q_INVOKABLE void createPrivateChat(qint32 userId, bool force);
    Q_INVOKABLE void createSecretChat(qint32 secretChatId);
    Q_INVOKABLE void createSupergroupChat(qint32 supergroupId, bool force);
    Q_INVOKABLE void deleteChatHistory(qint64 chatId, bool removeFromChatList, bool revoke);
    Q_INVOKABLE void getChat(qint64 chatId);
    Q_INVOKABLE void getChats(QVariant chatList, qint64 offsetOrder, qint64 offsetChatId, qint32 limit);
    Q_INVOKABLE void getSecretChat(qint32 secretChatId);
    Q_INVOKABLE void getSupergroup(qint32 supergroupId);
    Q_INVOKABLE void getSupergroupFullInfo(qint32 supergroupId);
    Q_INVOKABLE void leaveChat(qint64 chatId);
    Q_INVOKABLE void openChat(qint64 chatId);
    // Q_INVOKABLE void reportChat(qint64 chatId, TdApi::ChatReportReason reason, QList<qint64> messageIds);
    // Q_INVOKABLE void sendChatAction(qint64 chatId, TdApi::ChatAction action);
    Q_INVOKABLE void setChatTitle(qint64 chatId, const QString &title);
    Q_INVOKABLE void setPinnedChats(QVariant chatList, QList<qint64> chatIds);

private slots:
    void onNewChatUpdate(td::td_api::chat *chat);

private:
    Client *m_client{};
};
