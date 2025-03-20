#pragma once

#include <qml_meta.hpp>
#include <quuid.h>
#include <QTimer>

using task_login_t = std::tuple<std::string, bool>;

struct LoginBasicType : public BasicTypeDB<LoginBasicType, task_login_t> {
    static std::string tuple_info_name_override() { return "user_login"; }
    static std::string tuple_info_custom_select_override() { return "*"; }
    static std::string field_info_override(int field) {
        static const auto fields = std::vector{ "user_uuid", "has_data_to_sync"};
        return fields[field];
    }
    static bool auto_increment_first_override() {
        return false;
    }
};

class Login : public MetaQmlObject<LoginBasicType> {
    Q_OBJECT
public:
    STRING_NULL_PROPERTY(uuid, 0, "")
    BOOL_PROPERTY(isUpdateData, 1)

    Login(QObject* parent = nullptr) : MetaQmlObject<LoginBasicType>(parent) {};
};

class LoginModel : public MetaQmlModel<Login> {
    Q_OBJECT
    META_MODEL_QML_FUNCTIONS
public:
    LoginModel(QObject* parent = nullptr);
    void updateChanges();
    ~LoginModel();

    Q_INVOKABLE void stopListen() { m_timer_poll.stop(); }
    Q_INVOKABLE void startListen() { m_timer_poll.start(); }
signals:
    void updatedAnyData();

private:
    bool registerUser();
    void unRegisterUser();
    void checkUpdates();

    std::string generateUUIDString();

    bool check_is_before = false;
    std::string m_uuid;
    QTimer m_timer_poll;
};
