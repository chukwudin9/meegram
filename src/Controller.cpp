#include "Controller.hpp"

#include <td/telegram/td_json_client.h>

#include <iostream>

namespace meegram {

Controller::Controller(QObject *parent) : QThread (parent), client_(td_json_client_create())
{
    // disable TDLib logging
    td_json_client_execute(client_, R"({"@type":"setLogVerbosityLevel", "new_verbosity_level":0})");

    const bool test_incorrect_queries = true;
    if (test_incorrect_queries) {
        td_json_client_execute(client_, R"({"@type":"setLogVerbosityLevel", "new_verbosity_level":3})");

        td_json_client_send(client_, R"({"@type": "getTextEntities", "text": "@telegram /test_command https://telegram.org telegram.me", "@extra": ["5", 7.0]})");
        td_json_client_send(client_, R"({"@type": "getAuthorizationState", "@extra": 1.01234})");
    }
    this->setParameter();
    this->start();
}

Controller *Controller::instance()
{
    static Controller singleton;
    return &singleton;
}

nlohmann::json Controller::clientUpdate(double timeout)
{
    nlohmann::json document = nullptr;
    const char *result = td_json_client_receive(this->client_, timeout);

    if (!result) {
        return nullptr;
    }
    try {
        document = nlohmann::json::parse(result);
    } catch (nlohmann::json::parse_error &e) {
        std::cout << "[TD] JSON Parse error " << e.what() << "\n";
    }
    return document;

}

void Controller::send(const std::string &json)
{
    td_json_client_send(this->client_, json.c_str());
}

void Controller::sendTdParameter()
{
    nlohmann::json root = nlohmann::json::object();
    root["@type"] = "setTdlibParameters";
    root["parameters"] = parameters_;
    this->send(root.dump());
}

void Controller::logOut()
{
    this->send(R"({"@type": "logOut"})");
}

void Controller::setChatId()
{

}

void Controller::setParameter()
{
    nlohmann::json parameter = nlohmann::json::object();
    parameter["@type"] = "tdlibParameter";
    parameter["api_id"] = 94575;
    parameter["api_hash"] = "a3406de8d171bb422bb6ddf3bbd800e2";
    parameter["database_directory"] = "haes";
    parameter["device_model"] = "Nokia N9";
    parameter["system_language_code"] = "en";
    parameter["application_version"] = "0.0.1";
    parameter["system_version"] = "unknown";
    parameters_ = parameter;

}

void Controller::run()
{
    while (true)
    {
        if (!this->client_)
        {
            return;
        }

        try
        {
            nlohmann::json result = this->clientUpdate();
            if (result["@type"] == "updateAuthorizationState") {
                nlohmann::json auth_state = result["authorization_state"];
                if (auth_state["@type"] == "authorizationStateClosed")
                    break;

                if (auth_state["@type"] == "authorizationStateWaitTdlibParameters")
                    this->sendTdParameter();

                if (auth_state["@type"] == "authorizationStateWaitEncryptionKey")
                    this->send(R"({"@type": "checkDatabaseEncryptionKey", "key": "my_key"})");
            }

            std::cout << result << std::endl;
        }
        catch (const nlohmann::json::parse_error &ex)
        {
            std::cerr << "Error :" << ex.what();
        }

    }
}

} // namespace meegram
