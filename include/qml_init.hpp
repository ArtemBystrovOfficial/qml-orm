#pragma once
#include <string>
class TaskModel;

class QmlSingletonModels {
public:
  static QmlSingletonModels& Instanse();
  void RouteSyncModels(const std::string& table, const std::string& action, const std::string& id);
private:
	QmlSingletonModels();

	TaskModel* task_model_;
	//ColorSchemeModel* color_model_;
};