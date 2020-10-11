#pragma once

#include <AccountSetup/ProviderPluginProxy>
#include <Accounts/Account>
#include <Accounts/Manager>

#include <QObject>
#include <QVariant>

class Account : public QObject
{
    Q_OBJECT
public:
    explicit Account(QObject *parent = nullptr);

    QVariantMap tdParameters() const;

signals:
    void created();
    void failed();

public slots:
    bool init();
    void create();

private slots:
    void onAccountCreated();

private:
    Accounts::Manager m_manager{};
    Accounts::Account *m_account{};
    AccountSetup::ProviderPluginProxy *m_accountSetup{};
};
