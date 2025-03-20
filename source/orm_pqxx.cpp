#include "orm_pqxx.hpp"

std::string DataBaseAccess::searchRule(const FilterSelectPack& pack) {
	std::string out = "WHERE ";
	if (pack.search_text.has_value()) {
		if (pack.search_fields.empty())
			return "";
		for (auto& field : pack.search_fields) 
			out += std::format("{} {} ILIKE '%' || '{}' || '%' OR ", field, pack.invert_search_fields ? "NOT" :"", pack.search_text.value());
		//invert_search_fields
		out.erase(out.end() - 3, out.end()); //Last OR
	}
	else
		return "";
	return out;
}

std::string DataBaseAccess::sortRule(const FilterSelectPack& pack) {
	return pack.sort_asc.has_value()
		? std::format("ORDER BY {} {}", pack.sort_field, pack.sort_asc.value() ? "ASC" : "DESC") : "";
}

DataBaseAccess::DataBaseAccess(const std::string& connection_query)
	: m_conn(connection_query.c_str()) {}

