#pragma once

#include <QThread>
#include <nlohmann/json.hpp>

namespace meegram {

class Controller : public QThread
{
    Q_OBJECT
public:
    Controller(QObject *parent = nullptr);

    static Controller *instance();

    nlohmann::json clientUpdate(double timeout = 10);

    void send(const std::string &json);
    void sendTdParameter();

    void logOut();

    void setChatId();
    void setParameter();

protected:
    void run() override;

private:
    void *client_ = nullptr;
    nlohmann::json parameters_;
};

} // namespace meegram
