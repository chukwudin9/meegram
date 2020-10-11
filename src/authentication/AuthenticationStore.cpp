#include "AuthenticationStore.hpp"

#include "client/Client.hpp"
#include "util/Overloaded.hpp"

#include "authorization_adaptor.h"

#include <td/telegram/td_api.hpp>

#include <QDesktopServices>
#include <QSettings>

constexpr auto DbusServiceName = "tg.Authorization";

namespace {

QVariantMap AuthenticationCodeType(td::td_api::object_ptr<td::td_api::AuthenticationCodeType> authenticationCodeType)
{
    QVariantMap result;

    switch (authenticationCodeType->get_id()) {
        case td::td_api::authenticationCodeTypeTelegramMessage::ID: {
            auto object = td::td_api::move_object_as<td::td_api::authenticationCodeTypeTelegramMessage>(authenticationCodeType);
            result["type"] = QString("Telegram");
            result["length"] = object->length_;
            break;
        }
        case td::td_api::authenticationCodeTypeSms::ID: {
            auto object = td::td_api::move_object_as<td::td_api::authenticationCodeTypeSms>(authenticationCodeType);
            result["type"] = QString("SMS");
            result["length"] = object->length_;
            break;
        }
        case td::td_api::authenticationCodeTypeCall::ID: {
            auto object = td::td_api::move_object_as<td::td_api::authenticationCodeTypeCall>(authenticationCodeType);
            result["type"] = QString("Call");
            result["length"] = object->length_;
            break;
        }
        case td::td_api::authenticationCodeTypeFlashCall::ID: {
            auto object = td::td_api::move_object_as<td::td_api::authenticationCodeTypeFlashCall>(authenticationCodeType);
            result["type"] = QString("FlashCall");
            result["pattern"] = QString::fromStdString(object->pattern_);
            break;
        }
    }

    return result;
}
}  // namespace

AuthenticationStore::AuthenticationStore(TdApi *controller, QObject *parent)
    : QObject(parent)
    , m_client(controller->client())
{
    connect(controller, SIGNAL(updateAuthorizationState(td::td_api::AuthorizationState *)),
            SLOT(onProcessUpdates(td::td_api::AuthorizationState *)));

    checkErrors = [this](td::td_api::object_ptr<td::td_api::Object> object) {
        if (object->get_id() == td::td_api::error::ID) {
            auto err = td::td_api::move_object_as<td::td_api::error>(object);
            emit error(QString::fromStdString(err->message_));
        }
    };

    new AuthorizationAdaptor(this);
    QDBusConnection::sessionBus().registerObject("/", this);

    if (!QDBusConnection::sessionBus().registerService(DbusServiceName)) {
        exit(1);
    }
}

void AuthenticationStore::checkCode(const QString &code)
{
    m_client->sendRequest(td::td_api::make_object<td::td_api::checkAuthenticationCode>(code.toStdString()), checkErrors);
}

void AuthenticationStore::checkPassword(const QString &password)
{
    m_client->sendRequest(td::td_api::make_object<td::td_api::checkAuthenticationPassword>(password.toStdString()), checkErrors);
}

void AuthenticationStore::checkDbEncryptionKey(const QByteArray &encryptionKey)
{
    m_client->sendRequest(td::td_api::make_object<td::td_api::checkDatabaseEncryptionKey>(encryptionKey.constData()));
}

QString AuthenticationStore::getAuthorizationState() const
{
    static const std::unordered_map<std::int32_t, QString> state{
        {904720988, "WaitTdlibParameters"},
        {612103496, "WaitEncryptionKey"},
        {306402531, "WaitPhoneNumber"},
        {52643073, "WaitCode"},
        {860166378, "WaitOtherDeviceConfirmation"},
        {550350511, "WaitRegistration"},
        {187548796, "WaitPassword"},
        {-1834871737, "Ready"},
        {154449270, "LoggingOut"},
        {445855311, "Closing"},
        {1526047584, "Closed"},
    };

    if (auto it = state.find(m_currentAuthorizationState); it != state.end()) {
        return QString(it->second);
    }
    return {};
}

void AuthenticationStore::logOut()
{
    m_client->sendRequest(td::td_api::make_object<td::td_api::logOut>());
}

void AuthenticationStore::registerUser(const QString &firstName, const QString &lastName)
{
    m_client->sendRequest(td::td_api::make_object<td::td_api::registerUser>(firstName.toStdString(), lastName.toStdString()));
}

void AuthenticationStore::setPhoneNumber(const QString &phoneNumber)
{
    auto tdSettings = td::td_api::make_object<td::td_api::phoneNumberAuthenticationSettings>();

    QSettings settings;
    tdSettings->allow_flash_call_ = settings.value("PhoneNumberSettings/allowFlashCall", false).toBool();
    tdSettings->is_current_phone_number_ = settings.value("PhoneNumberSettings/isCurrentPhoneNumber", true).toBool();
    tdSettings->allow_sms_retriever_api_ = settings.value("PhoneNumberSettings/allowSmsRetrieverApi", false).toBool();

    m_client->sendRequest(
        td::td_api::make_object<td::td_api::setAuthenticationPhoneNumber>(phoneNumber.toStdString(), std::move(tdSettings)), checkErrors);
}

void AuthenticationStore::setTdlibParameters(const QVariantMap &parameters)
{
    QString filesDirectory = QDesktopServices::storageLocation(QDesktopServices::HomeLocation) + "/MyDocs/Telegram";
    QString dbDirectory = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
    auto tdParameters = td::td_api::make_object<td::td_api::tdlibParameters>();

    tdParameters->database_directory_ = dbDirectory.toStdString();
    tdParameters->files_directory_ = filesDirectory.toStdString();
    tdParameters->use_file_database_ = parameters["useFileDatabase"].toBool();
    tdParameters->use_chat_info_database_ = parameters["useChatInfoDatabase"].toBool();
    tdParameters->use_message_database_ = parameters["useMessageDatabase"].toBool();
    tdParameters->use_secret_chats_ = parameters["useSecretChats"].toBool();
    tdParameters->api_id_ = parameters["api_id"].toInt();
    tdParameters->api_hash_ = parameters["api_hash"].toString().toStdString();
    tdParameters->system_language_code_ = "en";
    tdParameters->device_model_ = "Nokia N9";
    tdParameters->system_version_ = "MeeGo 1.2 Harmattan";
    tdParameters->application_version_ = "0.5.0";
    tdParameters->enable_storage_optimizer_ = parameters["enableStorageOptimizer"].toBool();

    m_client->sendRequest(td::td_api::make_object<td::td_api::setTdlibParameters>(std::move(tdParameters)));
}

void AuthenticationStore::resendCode()
{
    m_client->sendRequest(td::td_api::make_object<td::td_api::resendAuthenticationCode>());
}

void AuthenticationStore::deleteAccount(const QString &reason)
{
    m_client->sendRequest(td::td_api::make_object<td::td_api::deleteAccount>(reason.toStdString()));
}

void AuthenticationStore::onProcessUpdates(td::td_api::AuthorizationState *authorizationState)
{
    td::td_api::downcast_call(
        *authorizationState,
        detail::Overloaded{
            [this](td::td_api::authorizationStateReady &) { emit ready(); },
            [this](td::td_api::authorizationStateLoggingOut &) { m_client->sendRequest(td::td_api::make_object<td::td_api::destroy>()); },
            [](td::td_api::authorizationStateClosing &) {}, [](td::td_api::authorizationStateClosed &) {},
            [this](td::td_api::authorizationStateWaitEncryptionKey &) { checkDbEncryptionKey("PASSWD"); },
#ifdef QT_SIMULATOR
            [this](td::td_api::authorizationStateWaitTdlibParameters &) {
                QVariantMap parameters{};
                QSettings settings("strawberry", "tgaccount");
                parameters["api_id"] = settings.value("api_id");
                parameters["api_hash"] = settings.value("api_hash");
                parameters["useFileDatabase"] = settings.value("useFileDatabase");
                parameters["useChatInfoDatabase"] = settings.value("useChatInfoDatabase");
                parameters["useMessageDatabase"] = settings.value("useMessageDatabase");
                parameters["useSecretChats"] = settings.value("useSecretChats");
                parameters["enableStorageOptimizer"] = settings.value("enableStorageOptimizer");

                setTdlibParameters(parameters);
            },
#else
            [](td::td_api::authorizationStateWaitTdlibParameters &) {},
#endif
            [this](td::td_api::authorizationStateWaitPhoneNumber &) { emit phoneNumberRequested(); },
            [this](td::td_api::authorizationStateWaitCode &object) {
                auto phoneNumber = QString::fromStdString(object.code_info_->phone_number_);
                auto type = AuthenticationCodeType(std::move(object.code_info_->type_));
                QVariantMap nextType;
                if (object.code_info_->next_type_) {
                    nextType = AuthenticationCodeType(std::move(object.code_info_->next_type_));
                }

                auto timeout = object.code_info_->timeout_;

                emit codeRequested(phoneNumber, type, nextType, timeout);
            },
            [this](td::td_api::authorizationStateWaitPassword &object) {
                auto passwordHint = QString::fromStdString(object.password_hint_);
                auto hasRecoveryEmailAddress = object.has_recovery_email_address_;
                auto recoveryEmailAddressPattern = QString::fromStdString(object.recovery_email_address_pattern_);
                emit passwordRequested(passwordHint, hasRecoveryEmailAddress, recoveryEmailAddressPattern);
            },
            [this](td::td_api::authorizationStateWaitOtherDeviceConfirmation &object) {
                emit qrCodeRequested(QString::fromStdString(object.link_));
            },
            [this](td::td_api::authorizationStateWaitRegistration &object) {
                auto text = QString::fromStdString(object.terms_of_service_->text_->text_);
                auto minUserAge = object.terms_of_service_->min_user_age_;
                auto showPopup = object.terms_of_service_->show_popup_;
                emit registrationRequested(text, minUserAge, showPopup);
            }});

    m_currentAuthorizationState = authorizationState->get_id();
}
