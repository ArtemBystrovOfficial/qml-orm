#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include <qml_init.hpp>
#include <orm_pqxx.hpp>

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);

    DataBaseAccess::Instanse(
        "user=root "
        "host=rc1a-6ekts2fq0umn56q6.mdb.yandexcloud.net "
        "port=6432 "
        "password=90009000 "
        "dbname=task_app2"
    );
        
    QmlSingletonModels::Instanse();

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/MainWindow.qml")));

    return app.exec();
}
