### Оглавление

1. [Введение](#введение)
2. [Пример использования](#пример-использования)
3. [Особенности](#особенности)
4. [Что под капотом](#что-под-капотом)
5. [Некоторые важные замечания](#некоторые-важные-замечания)

### Введение

В данном документе представлен шаблон разработки `ORM` для `PostgreSQL` на `C++` с возможностью быстрого подключения к `Qt/QML`, используя стандарт `C++20`. Этот шаблон обладает следующими ключевыми характеристиками:

#### Плюсы:

- Поддержка основных типов данных `PostgreSQL`.
- Операции `CRUD`.
- Поддержка связей между таблицами.
- Отсутствие промежуточных слоев, прямая связь с возможностью точечной кастомизации на уровне C++.
- Автоматическое обновление модели данных после операций `CUD` (Create, Update, Delete), либо режим ручного подтверждения.

#### Минусы:

- Полное остутсвие DI, все слои будут знать о друг друге, подходит лишь только для малых вспомогательных программ утилит,
  расчитанных под QML и взаимодействием с бд напрямую. Нерасширяемый код.

### Пример использования

`С++`
```c++
#include <qml_meta.hpp>

// Ваши типы данных из PostgreSQL
using color_tuple_t = std::tuple<int, std::string>;

struct ColorTuple : public BasicTypeDB<Color_Tuple, color_tuple_t> {
    static std::string tuple_info_name_override() { return "color_schemas"; }
    static std::string tuple_info_custom_select_override() { return "*"; }
    static std::string field_info_override(int field) {
        static const auto fields = std::vector{ "id", "main_color_code" };
        return fields[field];
    }
};

class ColorScheme : public MetaQmlObject<ColorTuple> {
    Q_OBJECT
public:
    INT_PRIMARY_PROPERTY(id, 0)
    STRING_NULL_PROPERTY(colorHex, 1, "")

public:
    ColorScheme(QObject* parent = nullptr) : MetaQmlObject<Color_Tuple>(parent) {};
};

class ColorSchemeModel : public MetaQmlModel<ColorScheme> {
    Q_OBJECT
    META_MODEL_QML_FUNCTIONS
public:
    ColorSchemeModel(QObject* parent = nullptr)
    : MetaQmlModel<ColorScheme>(DataBaseAccess::FilterSelectPack{ std::nullopt, "", "FIND_ME", {"title", "full_text"}}) {
        select_model();
    }
}

// Регистрация типов данных для использования в QML
qmlRegisterType<ColorScheme>(REGISTER_QML_TYPES, "ColorScheme");
qmlRegisterSingletonType<ColorSchemeModel>(REGISTER_QML_TYPES, "ColorSchemeModel", ...);
};
```

`QML`
```qml
    Repeater {
        model: ColorSchemeModel     
        Rectangle {
            width: 10
            height: 10
            color: item.colorHex // item является единицей каждой модели, colorHex это ваши Property из cpp 
        }      
    }
```

*В целом это все что нужно, чтобы ваша модель qml заработала в автоматическом режиме. Написание такого кода максимально ускоряет разработку приложения, 
особенно для простых таблиц не требующих сложных запросов или трудной логики обработки.*

### Особенности

Одной из уникальных особенностей этой ORM является отсутствие лишних промежуточных слоев в коде. Это позволяет быстро и легко создавать и поддерживать таблицы, основные операции с которыми, такие как фильтрация, поиск, обновление, удаление и вставка, становятся центральными элементами. Для более сложных операций предусмотрены кастомные селекты.

### Что под капотом

Основа всего функционала лежит в трех файлах: `orm_pqxx.hpp`, `qml_meta.hpp` и `types.hpp`, которые являются метаклассами и каждый из них выполняет свои функции в соответствии с названием.

Эти файлы имеют свои внутренние зависимости. Например, types.hpp представляет собой статически полиморфный базовый тип, от которого порождаются все остальные типы. При этом, зная его, ORM функционирует независимо от других компонентов. Синтаксический сахар в qml_meta.hpp предоставляет удобные обертки над ORM и паттерном Model-View. Здесь происходит делегирование функционала практически на два уровня вниз. Учитывая, что C++ предоставляет гибкость в кастомизации, это позволяет легко адаптировать функционал при необходимости.
```txt
    types.hpp 
        |
    orm_pqxx.hpp
        |
    qml_meta.hpp
``` 

### Некоторые важные замечания
Вся сложность заключается в том, что обработка `NULL` типов данных без указателей возможна лишь с помощью optional. Было принято решение использовать лишь базовые типы,
тем самым сделав невозможным использование `NULL`, например с типом` bool`, ведь `NULL` значениями являются узкие значения определнного типа данных. В следующих функциях это можно заметить:

```c++
namespace null_values {

	template <class T>
	T get() {
		if constexpr (std::is_same_v<T, int>)
			return std::numeric_limits<int>::min();
		if constexpr (std::is_same_v<T, std::string>)
			return "_null";
		if constexpr (std::is_same_v<T, bool>)
			return "false";
	}

	bool is_null(const auto & x) {
		return get<std::remove_cvref_t<decltype(x)>>() == x;
	}

	template<class... T>
	void _fill_tuple_null(std::tuple<T...> & tuple) {
		std::apply([&](auto&... elem) {
			((elem = get<T>()), ...);
		}, tuple);
	}

}

template <class D, class Tuple> requires std::_Is_specialization_v<Tuple, std::tuple>
struct BasicTypeDB{

	using tuple_t = Tuple;
	static constexpr int tuple_size = std::tuple_size_v<Tuple>;

	BasicTypeDB() {
		null_values::_fill_tuple_null(tp);
	}

// virtual abstract
	static std::string tuple_info_name() { return D::tuple_info_name_override(); }
	static std::string field_info(int field) { return  D::field_info_override(field); }

//just vritual static
	static std::string tuple_info_custom_select() {
		if constexpr ( requires{ D::tuple_info_custom_select_override(); })
			return D::tuple_info_custom_select_override();
		else
			return "";
	}
	static bool auto_increment_first() {
		if constexpr ( requires{ D::auto_increment_first_override(); })
			return D::auto_increment_first_override();
		else
			return true;
	}

	Tuple tp;
};
```

Пример Update из `orm_pqxx.hpp`
```c++
template <typename Tuple, std::size_t TupSize = Tuple::tuple_size> requires CustomTupleC<Tuple>
void Update(const Tuple&, const std::bitset<TupSize>&, ExceptionHandler& eh);

template<typename Tuple, std::size_t TupSize> requires CustomTupleC<Tuple>
inline void DataBaseAccess::Update(const Tuple& tp, const std::bitset<TupSize>& update_set, ExceptionHandler& eh) {
	std::string query = std::format(
		"UPDATE {} SET {} WHERE {} = {}",
		Tuple::tuple_info_name(),
		updateImpl(tp, update_set, std::make_index_sequence<TupSize>{}),
		Tuple::field_info(0),
		convertType(std::get<0>(tp.tp))
	);
	try {
		pqxx::work w(m_conn);
		w.exec(query);
		w.commit(); //TODO UNDO LIST
		return;
	} 
	catch (const std::exception& exp) {
		eh.what = std::format("update: {}",exp.what());
	}
	catch (...) {}

	eh.is_error_ = true;
}

template<typename Tuple, std::size_t ...Is>
inline std::string DataBaseAccess::updateImpl(const Tuple& tp, const std::bitset<sizeof...(Is)>& update_set, std::index_sequence<Is...>) {
	size_t index = 0;
	std::string out;
	auto printElem = [&index, &out, &update_set, this](const auto& x) {
		if (update_set[index]) 
			out += std::format("{} = {} , ", Tuple::field_info(index), convertType(x));
		index++;
	};
	(printElem(std::get<Is>(tp.tp)), ...);
	out.erase(out.end() - 2, out.end());
	return out;
}
```

Не обошлось и без макросов, ведь их применение неизбежно.
Все Q_INVOKABLE методы которые подключены к QML, невозможно наследовать, и остальные макросы QT, 
с ними возможно только явно делать "макрос на макрос". Порой приходится несколько раз перекомпилировать код,
чтобы препоцессор Qt определил это. Все что я написал выше это плата за удобность. Если вашего типа данных нет,
вам придется дописать вещи которые я описал в этом разделе, по прототипу. <br/>
PS. Либо напишите мне, всегда рад буду дописать,если кого-то заинтирисует данный проект.

```c++
#define STRING_NULL_PROPERTY(PROPERTY_NAME, INDEX_PROPERTY, DEFAULT) \
    Q_PROPERTY(QString PROPERTY_NAME READ PROPERTY_NAME WRITE set_##PROPERTY_NAME NOTIFY PROPERTY_NAME##Changed) \
    Q_SIGNAL void PROPERTY_NAME##Changed(); \
    QString PROPERTY_NAME() const { \
        auto& val = std::get<INDEX_PROPERTY>(m_data.tp); \
        return null_values::is_null(val) ? DEFAULT : QString::fromStdString(val); \
    } \
    void set_##PROPERTY_NAME(const QString &str) { \
        std::get<INDEX_PROPERTY>(m_data.tp) = str.toStdString(); \
        m_to_update_.set(INDEX_PROPERTY); \
        emit PROPERTY_NAME##Changed(); \
    }
```


