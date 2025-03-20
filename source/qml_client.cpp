#include "qml_client.hpp"

#include <iomanip>
#include <random>
#include <sstream>

LoginModel::LoginModel(QObject* parent)
    : MetaQmlModel<Login>(),
    check_is_before(false) {
    m_uuid = generateUUIDString();
    set_filter({ std::nullopt, "", std::nullopt,{} });
    registerUser();
    m_timer_poll.setInterval(300);
    connect(&m_timer_poll, &QTimer::timeout, this, &LoginModel::checkUpdates);
    m_timer_poll.start();
}

void LoginModel::updateChanges() {
    if (check_is_before)
        set_filter({ std::nullopt, "", std::nullopt,{} });
    select_model();

    for (auto& value : m_list)
        if (value->uuid().toStdString() != m_uuid)
            value->set_isUpdateData(true);
    CommitChanges();
    check_is_before = false;
}

LoginModel::~LoginModel() {
    m_timer_poll.stop();
    unRegisterUser();
}

bool LoginModel::registerUser() {
    Login login(nullptr);
    LoginBasicType login_basic;
    login_basic.tp = task_login_t(m_uuid, false);
    login.setData(login_basic);
    return Add(&login).isEmpty();
}

void LoginModel::unRegisterUser() {
    set_filter({ std::nullopt, "", m_uuid, { "user_uuid" } });
    select_model();
    Q_ASSERT((m_list.size() == 1));
    Delete(0);
}

void LoginModel::checkUpdates() {
    if(!check_is_before)
        set_filter({ std::nullopt, "", m_uuid, { "user_uuid" } });
    select_model();

    if (m_list.size() != 1)
        return;

    if (m_list.back()->isUpdateData()) {
        m_list.back()->set_isUpdateData(false);
        CommitChanges();
        emit updatedAnyData();
    }

    check_is_before = true;
}

std::string LoginModel::generateUUIDString() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<uint32_t> dis(0, 0xFFFFFFFF);

    uint32_t w1 = dis(gen);
    uint32_t w2 = dis(gen);

    std::stringstream ss;
    ss << std::hex << std::setw(8) << std::setfill('0') << w1
       << std::hex << std::setw(8) << std::setfill('0') << w2;

    return ss.str();
}
