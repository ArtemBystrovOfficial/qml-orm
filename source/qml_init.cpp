#include <qml_init.hpp>
#include <qml_task_model.hpp>
#include <qml_color_schemas.hpp>
#include <qml_client.hpp>

#include <QQmlApplicationEngine>

#define REGISTER_QML_TYPES "CppObjects", 1, 0

QmlSingletonModels& QmlSingletonModels::Instanse() {
	static QmlSingletonModels model;
	return model;
}

QmlSingletonModels::QmlSingletonModels() {
//TaskModel
    task_model_ = new TaskModel;

    qmlRegisterType<Task>(REGISTER_QML_TYPES, "Task");
    qmlRegisterSingletonType<TaskModel>(REGISTER_QML_TYPES, "TaskModel",
    [this](QQmlEngine* engine, QJSEngine* scriptEngine) -> QObject* {
        Q_UNUSED(engine)
        Q_UNUSED(scriptEngine)
        return task_model_;
    });

//ColorScheme
    //static auto color_model = new ColorSchemeModel;

    //qmlRegisterType<ColorScheme>(REGISTER_QML_TYPES, "ColorScheme");
    //qmlRegisterSingletonType<ColorSchemeModel>(REGISTER_QML_TYPES, "ColorSchemeModel",
    //[&](QQmlEngine* engine, QJSEngine* scriptEngine) -> QObject* {
    //    Q_UNUSED(engine)
    //    Q_UNUSED(scriptEngine)
    //    return color_model;
    //});

}

void QmlSingletonModels::RouteSyncModels(const std::string& table, const std::string& action, const std::string& id) {
    QMetaObject::invokeMethod(
        task_model_,
        "RouteSyncModels",
        Qt::QueuedConnection,
        Q_ARG(QString, QString::fromStdString(table)),
        Q_ARG(QString, QString::fromStdString(action)),
        Q_ARG(QString, QString::fromStdString(id))
    );
    //todo auto connected for all classes
}
