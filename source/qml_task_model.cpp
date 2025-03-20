#include <qml_task_model.hpp>

QString TaskModel::getFullText(int index) {
	DataBaseAccess::ExceptionHandler eh;
	auto opt = DataBaseAccess::Instanse().specialSelect11<std::string>(
		std::format("SELECT COALESCE(full_text, ''::text) AS ft FROM {} WHERE id = {}",
			TaskBasicType::tuple_info_name(),
			m_list[index]->id()), eh
		);
	return QString::fromStdString(opt.has_value() ? opt.value() : "");
}

void TaskModel::searchText(const QString& str) {
	if (str.isEmpty()) {
		mutable_filter().search_text = std::nullopt;
	}
	else {
		mutable_filter().search_text = str.toStdString();
		mutable_filter().search_fields = { "title", "full_text" };
	}
	select_model();
}
