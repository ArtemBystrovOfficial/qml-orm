#pragma once 

#include <iostream>
#include <stdexcept>
#include <pqxx/pqxx>
#include <tuple>
#include <types.hpp>
#include <bitset>

template <class T>
concept CustomTupleC = std::is_base_of_v<BasicTypeDB<T, typename T::tuple_t>, T>;

class DataBaseAccess {
public:
	static DataBaseAccess& Instanse(const std::string& connection_query = "") {
		static DataBaseAccess base(connection_query);
		return base;
	}

	struct FilterSelectPack {
		std::optional<bool> sort_asc;
		std::string sort_field;

		std::optional<std::string> search_text;
		std::vector<std::string> search_fields;
		bool invert_search_fields = false;
	};

	struct ExceptionHandler {
		friend class DataBaseAccess;
		operator bool() noexcept { return is_error_; }
		explicit ExceptionHandler() : is_error_(false) {};

		std::string what;
	private:
		bool is_error_;
	};

	template<class Tuple> requires CustomTupleC<Tuple>
	std::optional<std::vector<Tuple>> Select(const FilterSelectPack& pack, ExceptionHandler & eh);

	template <typename Tuple, std::size_t TupSize = Tuple::tuple_size> requires CustomTupleC<Tuple>
	void Insert(const Tuple&, ExceptionHandler& eh);

	template <typename Tuple, std::size_t TupSize = Tuple::tuple_size> requires CustomTupleC<Tuple>
	void Update(const Tuple&, const std::bitset<TupSize>&, ExceptionHandler& eh);

	template<class Tuple> requires CustomTupleC<Tuple>
	void Delete(const Tuple& tuple, ExceptionHandler& eh);

	template<class T>
	std::optional <T> specialSelect11(const std::string&, ExceptionHandler& eh);
private:
	DataBaseAccess(const std::string& connection_query);
//INSERT
	template <typename Tuple, std::size_t... Is>
	std::string insertImpl(const Tuple& tp, std::index_sequence<Is...>);
//UPDATE
	template <typename Tuple, std::size_t... Is>
	std::string updateImpl(const Tuple& tp,const std::bitset<sizeof...(Is)>&, std::index_sequence<Is...>);
//SELECT + FILTERS
	std::string searchRule(const FilterSelectPack& pack);
	std::string sortRule(const FilterSelectPack& pack);

//OTHER
	std::string convertType(const auto & x) {
		using type = std::remove_cvref_t<decltype(x)>;
	//not nullable type
		if constexpr (std::is_same_v <type, bool>)
			return (x ? "TRUE" : "FALSE");

		if (x == null_values::get<type>())
			return "NULL";
		if constexpr (std::is_same_v<type, std::string>)
			return std::format("'{}'", x);
		if constexpr (requires { x + 1; })
			return std::to_string(x);
		return "Error";
	}

	pqxx::connection m_conn;
};

//SELECT

template<class Tuple> requires CustomTupleC<Tuple>
inline std::optional<std::vector<Tuple>> DataBaseAccess::Select(const FilterSelectPack& pack, ExceptionHandler& eh) {
	try {
		auto from = Tuple::tuple_info_name();
		auto sel = Tuple::tuple_info_custom_select();
		auto query = std::format(
			"SELECT {} FROM {} {} {}",
			sel, from, searchRule(pack), sortRule(pack)
		);
		pqxx::work w(m_conn);
		pqxx::result res = w.exec(query);

		std::vector <Tuple> m_out;
		for (auto row : res) {
			m_out.push_back(Tuple{});
			row.to(m_out.back().tp);
		}

		return m_out;
	}
	catch (const std::exception& exp) {
		eh.what = std::format("select: {}", exp.what());
	}
	catch (...) {}

	eh.is_error_ = true;
	return std::nullopt;
}

//INSERT

template<typename Tuple, std::size_t TupSize> requires CustomTupleC<Tuple>
inline void DataBaseAccess::Insert(const Tuple& tp, ExceptionHandler& eh) {
	std::string query = insertImpl(tp, std::make_index_sequence<TupSize>{});
	try {
		pqxx::work w(m_conn);
		w.exec(query);
		w.commit(); //TODO UNDO LIST
		return;
	}
	catch (const std::exception& exp) {
		eh.what = std::format("insert: {}",exp.what());
	}
	catch (...) {}

	eh.is_error_ = true;
}

template<typename Tuple, std::size_t ...Is> 
inline std::string DataBaseAccess::insertImpl(const Tuple& tp, std::index_sequence<Is...>) {
	size_t index = 0;
	std::string out = "INSERT INTO ";
	out += Tuple::tuple_info_name();
	out += std::format(" VALUES {}", Tuple::auto_increment_first() ? "(DEFAULT, " : "(");
	auto printElem = [&index, &out, this](const auto& x) {
		if (!(!index && Tuple::auto_increment_first())) //id skip
			out += convertType(x) + ", ";
		index++;
	};
	(printElem(std::get<Is>(tp.tp)), ...);
	out.erase(out.end() - 2, out.end());
	out += ") ";
	return out;
}

//UPDATE 

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

//DELETE
template<class Tuple> requires CustomTupleC<Tuple>
inline void DataBaseAccess::Delete(const Tuple& tp, ExceptionHandler& eh) {
	try {
		pqxx::work w(m_conn);
		w.exec(std::format("DELETE FROM {} WHERE {} = {}",
				Tuple::tuple_info_name(),
				Tuple::field_info(0),
				convertType(std::get<0>(tp.tp))
			   )
		);
		w.commit();
		return;
	}
	catch (const std::exception& exp) {
		eh.what = std::format("delete: {}", exp.what());
	}
	catch (...) {}

	eh.is_error_ = true;
}

template<class T>
inline std::optional <T> DataBaseAccess::specialSelect11(const std::string& query, ExceptionHandler& eh) {
	try {
		pqxx::work w(m_conn);
		pqxx::row res = w.exec1(query);
		return std::move(res[0].as<T>());
	}
	catch (const std::exception& exp) {
		eh.what = std::format("special select: {}",exp.what());
	}
	catch (...) {}

	eh.is_error_ = true;
	return std::nullopt;
}