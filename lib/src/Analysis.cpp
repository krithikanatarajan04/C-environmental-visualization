//
// Created by Krithika Natarajan on 12/4/23.
//
#include <iostream>
#include <string>
#include <fstream>

#include "data_processor.h"
#include "visualization.h"
#include "../matplot/matplot.h"
#include "Analysis.h"
#include <vector>
#include <variant>
#include <map>
#include <optional>
#include <set>

Analysis::Analysis() {

}

Analysis::~Analysis(){

}

void Analysis::unit_standardization(data_processor& dataset) {
    /* This function standardizes units for GHG emissions
     * Parameter:
     * data_processor - data set to standardize
     * */

    for (auto& record : dataset.data_map) {
        auto it = record.find("GHG Emissions (t CO2e)");
        if (it != record.end() && std::holds_alternative<double>(it->second)) {
            it->second = std::get<double>(it->second) / 1000;  // Convert t CO2e to mt CO2e
        }
    }

}

data_processor Analysis::combine_dataset(data_processor set_1, data_processor set_2) {
    /* This function combines two datasets into one by looking at the common headers
     * Parameter:
     * data_processor - datasets one and two
     * Returns:
     * data_processer
     * */

    //convert the units to mtCO2E
    unit_standardization(set_1);


    //check which headers from each dataset are the same and extract those headers
    std::vector<std::string> set_1_headers = set_1.get_headers();
    std::vector<std::string> set_2_headers = set_2.get_headers();

    //change set_1 header name to match the headers
    for (size_t i = 0; i < set_1_headers.size(); ++i) {
        if (set_1_headers[i] == "GHG Emissions (t CO2e)") {
            set_1.update_header(i,"GHG Emissions (mt CO2e)");

            break; // Assuming each header name is unique and you found the header to change
        }
    }

    data_processor combine_sets = data_processor::merge_data(set_1,set_2);
    return combine_sets;
}


void Analysis::emissions_over_time_by_sector(){
    /* THis function conducts overview Analysis on the Community Greenhouse Gas CSV
     * */
    data_processor comm_greenhouse_gas;
    visualization visualizer;

    //COMMUNITY GREENHOUSE GAS DATA TYPES
    std::map<std::string, std::string> COL_TYPES = {
            {"Year", "double"},
            {"GHG Emissions (mt CO2e)", "double"}
    };

    comm_greenhouse_gas.read_data("../data/community-greenhouse-gas.csv", COL_TYPES);
    comm_greenhouse_gas = comm_greenhouse_gas.filter_data("GHG Emissions (mt CO2e)", std::optional<double>{},true);

    std::cout << comm_greenhouse_gas.data_map.size() <<std::endl;

    //LOCAL GOVERNMENT GREENHOUSE GAS
    data_processor local_gov_greenhouse_gas;
    std::map<std::string, std::string> COL_TYPES_1 = {
            {"Year", "double"},
            {"GHG Emissions (t CO2e)", "double"}
    };

    std::map<std::string, std::pair<std::string,std::variant<int, double, std::string, std::optional<int>,std::optional<double>,std::optional<std::string>>>> REPLACEMENTS = {
            {"GHG Emissions (t CO2e)", {"-", std::optional<double>{}}}
    };

    local_gov_greenhouse_gas.read_data("../data/local-government-operations-greenhouse-gas.csv", COL_TYPES_1, REPLACEMENTS);
    local_gov_greenhouse_gas = local_gov_greenhouse_gas.filter_data("GHG Emissions (t CO2e)", std::optional<double>{}, true);

    std::cout << local_gov_greenhouse_gas.data_map.size() <<std::endl;
    //LOCAL GOVERNMENT OPERATIONS FUELS
    data_processor local_gov_fuels;
    std::map<std::string, std::string> COL_TYPE_FUELS = {
            {"Year", "double"},
            {"Quantity", "double"}
    };
    std::map<std::string, std::pair<std::string,std::variant<int, double, std::string, std::optional<int>,std::optional<double>,std::optional<std::string>>>> REP_FUELS = {
            {"Quantity", {"-", std::optional<double>{}}}
    };

    local_gov_fuels.read_data("../data/local-government-operations-fuels.csv", COL_TYPE_FUELS, REP_FUELS);
    local_gov_fuels = local_gov_fuels.filter_data("Quantity", std::optional<double>{}, true);


    //TIME SERIES VISUALIZATION
    //step 1: combine dataset with greenhouse gas emissions
    data_processor greenhouse_gas;
    greenhouse_gas = combine_dataset(local_gov_greenhouse_gas,comm_greenhouse_gas);


    //step 2: data aggregation
    std::vector<std::string> col_names = {"Year","Sector"};
    greenhouse_gas = greenhouse_gas.aggregation(col_names,"GHG Emissions (mt CO2e)");


    //Step 3: filter data (remove extraneous zeros)
    greenhouse_gas = greenhouse_gas.filter_data("GHG Emissions (mt CO2e)", 0, true);
    greenhouse_gas.print_data();

    //step 3 : visualization prep
    std::vector<double> years = greenhouse_gas.extract_column<double>("Year");
    std::sort(years.begin(), years.end());
    years.erase(std::unique(years.begin(), years.end()), years.end());

    std::vector<std::string> sector = greenhouse_gas.extract_column<std::string>("Sector");
    std::sort(sector.begin(), sector.end());
    sector.erase(std::unique(sector.begin(), sector.end()), sector.end());


    //data type needed for visualization
    std::unordered_map<std::string, std::vector<double>> emission;
    std::vector<double> year;
    for (const auto& label : sector) {
        // Check if the 'Sector' and 'GHG Emissions (mt CO2e)' fields exist and have the correct type

        data_processor time_series_data;
        time_series_data = greenhouse_gas.filter_data("Sector", label);
        std::vector<double> emissions = time_series_data.extract_column<double>("GHG Emissions (mt CO2e)");
        year = time_series_data.extract_column<double>("Year");
        emission[label] = emissions; // Append to the vector
        }

    visualizer.time_series(emission,year,sector, "Emissions by Sector over time", "Time (Years)", "Emissions (mt CO2e)");

    }
    void Analysis::emissions_by_fuels() {
        data_processor comm_greenhouse_gas;
        visualization visualizer;

        //LOCAL GOVERNMENT OPERATIONS FUELS
        data_processor local_gov_fuels;
        std::map<std::string, std::string> COL_TYPE_FUELS = {
                {"Year", "double"},
                {"Quantity", "double"}
        };
        std::map<std::string, std::pair<std::string,std::variant<int, double, std::string, std::optional<int>,std::optional<double>,std::optional<std::string>>>> REP_FUELS = {
                {"Quantity", {"-", std::optional<double>{}}}
        };

        local_gov_fuels.read_data("../data/local-government-operations-fuels.csv", COL_TYPE_FUELS, REP_FUELS);
        local_gov_fuels = local_gov_fuels.filter_data("Quantity", std::optional<double>{}, true);

        //HEAT MAP
        //step 1: extract desired columns
        //local_gov_fuels.print_data();

        data_processor subset_fuel = local_gov_fuels.get_subset({"Department", "Year", "Quantity"});
        subset_fuel.print_data();

}







    //step 3:

