#pragma once

#include "client/TdApi.hpp"

#include <td/telegram/td_api.h>

#include <QObject>
#include <QVariant>

#include <functional>

class Client;

class AuthenticationStore : public QObject
{
    Q_OBJECT
public:
    explicit AuthenticationStore(TdApi *controller, QObject *parent = nullptr);

    Q_INVOKABLE void checkCode(const QString &code);
    Q_INVOKABLE void checkPassword(const QString &password);
    Q_INVOKABLE void checkDbEncryptionKey(const QByteArray &encryptionKey);
    Q_INVOKABLE QString getAuthorizationState() const;
    Q_INVOKABLE void logOut();
    Q_INVOKABLE void registerUser(const QString &firstName, const QString &lastName);
    Q_INVOKABLE void setPhoneNumber(const QString &phoneNumber);
    Q_INVOKABLE void setTdlibParameters(const QVariantMap &parameters);
    Q_INVOKABLE void resendCode();
    Q_INVOKABLE void deleteAccount(const QString &reason);

signals:
    void phoneNumberRequested();
    void codeRequested(const QString &phoneNumber, const QVariantMap &type, const QVariantMap &nextType, int timeout);
    void registrationRequested(const QString &text, int minUserAge, bool showPopup);
    void passwordRequested(const QString &passwordHint, bool hasRecoveryEmailAddress, const QString &recoveryEmailAddressPattern);
    void qrCodeRequested(const QString &link);
    void ready();

    void error(const QString &errorString);

private slots:
    void onProcessUpdates(td::td_api::AuthorizationState *authorizationState);

private:
    Client *m_client{};

    std::function<void(td::td_api::object_ptr<td::td_api::Object>)> checkErrors;
    std::int32_t m_currentAuthorizationState{};
};
