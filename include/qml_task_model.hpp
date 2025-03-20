#pragma once

#include <qml_meta.hpp>

using task_tuple_t = std::tuple<int, std::string, std::string, std::string, int, bool>;

struct TaskBasicType : public BasicTypeDB<TaskBasicType, task_tuple_t> {
    static std::string tuple_info_name_override() { return "tasks"; }
    static std::string tuple_info_custom_select_override() {
        return std::format("id, "
            "COALESCE(title, '{}'::text) AS title, "
            "COALESCE(to_char(updated_at, 'DD Month YYYY HH24:MI'::text), '{}'::text) AS format_updated_at, "
            "COALESCE(\"left\"(full_text, 30), '{}'::text) AS formated_full_text,"
            "COALESCE(color_schemas_id, {}),"
            "is_busy",
            null_values::get<std::string>(),
            null_values::get<std::string>(),
            null_values::get<std::string>(),
            null_values::get<int>()
        );
    }
    static std::string field_info_override(int field) {
        static const auto fields = std::vector{ "id", "title", "created_at", "full_text", "color_schemas_id", "is_busy" };
        return fields[field];
    }
};

class Task : public MetaQmlObject<TaskBasicType> {
    Q_OBJECT
public:
    INT_PRIMARY_PROPERTY(id, 0)
    STRING_NULL_PROPERTY(title, 1, "")
    STRING_NULL_PROPERTY(updatedAt, 2, "")
    STRING_NULL_PROPERTY(desc, 3, "")
    INT_NULL_PROPERTY(colorSchemeId, 4, -1)
    BOOL_PROPERTY(isBusy, 5)

    Task(QObject* parent = nullptr) : MetaQmlObject<TaskBasicType>(parent) {};
};

class TaskModel : public MetaQmlModel<Task> {
    Q_OBJECT
    META_MODEL_QML_FUNCTIONS
public:
    TaskModel(QObject* parent = nullptr) 
    : MetaQmlModel<Task>(DataBaseAccess::FilterSelectPack{ false, "updated_at", std::nullopt, {} }) {
        select_model();
    }

    void updateNow() { select_model(); }
    Q_INVOKABLE QString getFullText(int index);
    Q_INVOKABLE void searchText(const QString& str);
};
