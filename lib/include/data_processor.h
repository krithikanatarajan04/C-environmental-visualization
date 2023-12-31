//
// Created by Krithika Natarajan on 11/29/23.
//

#ifndef C_FINAL_PROJECT_DATA_PROCESSOR_H
#define C_FINAL_PROJECT_DATA_PROCESSOR_H
#include <string>
#include <map>
#include <variant>
#include <vector>
#include <iostream>
#include <optional>

class data_processor {
private:
    std::vector<std::string> headers;
    std::map<std::string, std::string> COL_TYPES;
    std::map<std::string, std::pair<std::string, std::variant<int, double, std::string, std::optional<int>, std::optional<double>, std::optional<std::string>>>> REPLACEMENTS;

    void assign_data(
            std::map<std::string, std::variant<int, double, std::string, std::optional<int>, std::optional<double>, std::optional<std::string>>> &data_row,
            std::string line_cell, size_t column_index);

    std::variant<int, double, std::string, std::optional<int>, std::optional<double>, std::optional<std::string>>
    typecast_value(std::string header, const std::string value);
    std::vector<std::string> parse_csv_line(const std::string& line);
    void add_data(data_processor set, std::vector<std::string> common_headers);

public:

    //initialization
    std::vector<std::map<std::string, std::variant<int, double, std::string, std::optional<int>, std::optional<double>, std::optional<std::string>>>> data_map;
    // Copy constructor
    data_processor(const data_processor& other);
    data_processor& operator=(const data_processor& other);
    data_processor(std::vector<std::map<std::string, std::variant<int, double, std::string, std::optional<int>, std::optional<double>, std::optional<std::string>>>>& data);
    data_processor();
    ~data_processor();



    //getter functions
    std::map<std::string, std::pair<std::string, std::variant<int, double, std::string, std::optional<int>, std::optional<double>, std::optional<std::string>>>> get_replacement_map();
    std::map<std::string, std::string> get_col_types();
    std::vector<std::string> get_headers();




    //executive actions
    void read_data(std::string csv_path,std::map<std::string, std::string> col_types, const std::map<std::string, std::pair<std::string, std::variant<int, double, std::string, std::optional<int>, std::optional<double>, std::optional<std::string> >>> replacements = {});
    //map: key:column name, value: pair - (value to replace, new value(int,double,string, optional int double or string)
    void print_data();
    template <typename T>
    data_processor filter_data(const std::string& col_name, const T& col_value, bool exclude=false);
    static data_processor merge_data(data_processor set_1, data_processor set_2);
    void update_header(int header_idx, std::string new_header);

    template <typename T>
    void add_col(const std::vector<T>& vec, const std::string& col_name);

    data_processor aggregation(const std::vector<std::string>& col_names, const std::string& sum_col);
    data_processor get_subset(const std::vector<std::string>& col_names);

    template<typename T>
    std::vector<T> extract_column(std::string col_name) {
        /* This function returns a column of data
         * Parameters:
         * std::string - column name
         * Returns:
         * std::vector - column of data
         * */
        std::vector<T> col_data;

        for (const auto row: data_map) {
            auto col_iter = row.find(col_name);
            if (col_iter != row.end()) {
                if (std::holds_alternative<T>(col_iter->second)) {
                    col_data.push_back(std::get<T>(col_iter->second));
                }

            }
        }
        return col_data;
    }









};

#endif //C_FINAL_PROJECT_DATA_PROCESSOR_H
