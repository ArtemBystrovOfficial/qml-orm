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
    static auto task_model = new TaskModel;

    qmlRegisterType<Task>(REGISTER_QML_TYPES, "Task");
    qmlRegisterSingletonType<TaskModel>(REGISTER_QML_TYPES, "TaskModel",
    [&](QQmlEngine* engine, QJSEngine* scriptEngine) -> QObject* {
        Q_UNUSED(engine)
        Q_UNUSED(scriptEngine)
        return task_model;
    });

//ColorScheme
    static auto color_model = new ColorSchemeModel;

    qmlRegisterType<ColorScheme>(REGISTER_QML_TYPES, "ColorScheme");
    qmlRegisterSingletonType<ColorSchemeModel>(REGISTER_QML_TYPES, "ColorSchemeModel",
    [&](QQmlEngine* engine, QJSEngine* scriptEngine) -> QObject* {
        Q_UNUSED(engine)
        Q_UNUSED(scriptEngine)
        return color_model;
    });

}
