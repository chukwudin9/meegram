#include "Controller.hpp"

#include <QApplication>
#include <QtDeclarative>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    meegram::Controller nst;
    return QApplication::exec();
}
