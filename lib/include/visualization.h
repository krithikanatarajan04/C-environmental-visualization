//
// Created by Krithika Natarajan on 12/3/23.
//

#ifndef C_FINAL_PROJECT_VISUALIZATION_H
#define C_FINAL_PROJECT_VISUALIZATION_H
#include <vector>
#include <string>




class visualization {
private:
public:
    visualization();
    ~visualization();
    void scatter_plot(std::vector<double> x_col, std::vector<double> y_col, const std::string& title = "", const std::string& x_label = "", const std::string& y_label = "");

    void histogram(std::vector<double> data, const std::string& title = "", const std::string& x_label = "", const std::string& y_label = "");
    void bar_chart(std::vector<double> x , std::vector<double> y,const std::string& title = "", const std::string& x_label = "", const std::string& y_label = "" , std::vector<std::string> category={});

    static void time_series(
            const std::unordered_map<std::string, std::vector<double>>& data,
            const std::vector<double>& x_values,
            const std::vector<std::string>& labels,
            const std::string& title ="",
            const std::string& x_label="",
            const std::string& y_label=""
    );

};


#endif //C_FINAL_PROJECT_VISUALIZATION_H
