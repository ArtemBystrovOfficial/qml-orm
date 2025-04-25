#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include <qml_init.hpp>
#include <orm_pqxx.hpp>

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);

    DataBaseAccess::Instanse(
        "user=postgres "
        "host=127.0.0.1 "
        "port=5432 "
        "password=9000 "
        "dbname=task_app"
    );

    QmlSingletonModels::Instanse();

    DataBaseAccess::Instanse().SetSyncCallback([](std::string a1, std::string a2, std::string a3) {
        QmlSingletonModels::Instanse().RouteSyncModels(a1, a2, a3);
    });

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/MainWindow.qml")));

    return app.exec();
}
