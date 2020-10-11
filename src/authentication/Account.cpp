#include "Account.hpp"

Account::Account(QObject *parent)
    : QObject(parent)
{
}

QVariantMap Account::tdParameters() const
{
    QVariantMap result{};

    if (m_account) {
        result["api_id"] = m_account->valueAsInt("api_id");
        result["api_hash"] = m_account->valueAsString("api_hash");
        result["useFileDatabase"] = m_account->valueAsBool("useFileDatabase");
        result["useChatInfoDatabase"] = m_account->valueAsBool("useChatInfoDatabase");
        result["useMessageDatabase"] = m_account->valueAsBool("useMessageDatabase");
        result["useSecretChats"] = m_account->valueAsBool("useSecretChats");
        result["enableStorageOptimizer"] = m_account->valueAsBool("enableStorageOptimizer");
    }

    return result;
}

bool Account::init()
{
    for (auto id : m_manager.accountList()) {
        if (auto *account = m_manager.account(id); account->providerName() == "tg") {
            m_account = account;
            break;
        }
    }
    return m_account;
}

void Account::create()
{
    if (!m_accountSetup) {
        m_accountSetup = new AccountSetup::ProviderPluginProxy(this);

        connect(m_accountSetup, SIGNAL(finished()), this, SLOT(onAccountCreated()));
    }
    m_accountSetup->createAccount(m_manager.provider("tg"), "");
}

void Account::onAccountCreated()
{
    for (auto id : m_manager.accountList()) {
        if (auto *account = m_manager.account(id); account->providerName() == "tg") {
            m_account = account;
            break;
        }
    }

    if (m_account) {
        emit created();
    } else {
        emit failed();
    }

    m_accountSetup->deleteLater();
}
