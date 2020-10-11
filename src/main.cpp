#include <QApplication>
#include <QDeclarativeComponent>
#include <QDeclarativeContext>
#include <QTimer>

#include "qmlapplicationviewer.h"

#ifndef QT_SIMULATOR
#    include "authentication/Account.hpp"
#endif

#include "authentication/AuthenticationStore.hpp"
#include "chat/ChatStore.hpp"
#include "client/TdApi.hpp"

Q_DECL_EXPORT int main(int argc, char *argv[])
{
    QScopedPointer<QApplication> app(createApplication(argc, argv));

    QApplication::setApplicationName("meegram");
    QApplication::setOrganizationName("strawberry");

    TdApi controller;
    AuthenticationStore authenticationStore(&controller);
    ChatStore chatStore(&controller);

    QmlApplicationViewer viewer;
    viewer.rootContext()->setContextProperty("TdApi", &controller);
    viewer.rootContext()->setContextProperty("AuthenticationStore", &authenticationStore);
    viewer.rootContext()->setContextProperty("ChatStore", &chatStore);

    viewer.setMainQmlFile(QLatin1String("qml/main.qml"));

#ifndef QT_SIMULATOR
    if (Account account; account.init()) {
        authenticationStore.setTdlibParameters(account.tdParameters());
        viewer.showExpanded();
    } else {
        QObject::connect(&account, SIGNAL(failed()), app.data(), SLOT(quit()));
        QObject::connect(&account, SIGNAL(created(const QVariantMap &)), &viewer, SLOT(showFullScreen()));
        // 20 msec delay
        QTimer::singleShot(20, &account, SLOT(create()));
    }
#else
    viewer.showExpanded();
#endif

    return app->exec();
}
