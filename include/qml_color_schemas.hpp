#pragma once

#include <qml_meta.hpp>

struct Color_Tuple : public BasicTypeDB <Color_Tuple, std::tuple<int, std::string>> {
    static std::string tuple_info_name_override() { return "color_schemas"; }
    static std::string tuple_info_custom_select_override() { return "*"; }
    static std::string field_info_override(int field) {
        static const auto fields = std::vector{ "id", "main_color_code" };
        return fields[field];
    }
};

class ColorScheme : public MetaQmlObject<Color_Tuple> {
    Q_OBJECT
public:
    INT_PRIMARY_PROPERTY(id, 0)
    STRING_NULL_PROPERTY(color, 1, "")

public:
    ColorScheme(QObject* parent = nullptr) : MetaQmlObject<Color_Tuple>(parent) {};
};

class ColorSchemeModel : public MetaQmlModel<ColorScheme> {
    Q_OBJECT
    META_MODEL_QML_FUNCTIONS
public:
    ColorSchemeModel(QObject* parent = nullptr)
        : MetaQmlModel<ColorScheme>(DataBaseAccess::FilterSelectPack{ std::nullopt, "", std::nullopt, {}}) {
        select_model();
    }
};