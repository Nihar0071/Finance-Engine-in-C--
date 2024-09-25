#include <iostream>
#include <vector>
#include <string>
#include <xlnt/xlnt.hpp>
using namespace std;

// Function to read data from Excel file and store in memory
std::vector<std::vector<double>> loadExcelData(const std::string &file_path, const std::string &sheet_name) {
    xlnt::workbook wb;
    wb.load(file_path);  // Load the Excel file
    xlnt::worksheet ws = wb.sheet_by_title(sheet_name);  // Select the sheet

    std::vector<std::vector<double>> data;
    
    for (auto row : ws.rows(false)) {  // Iterate through rows
        std::vector<double> row_data;
        for (auto cell : row) {  // Iterate through cells in a row
            if (cell.has_value()) {
                try {
                    row_data.push_back(cell.value<double>());  // Store cell value
                } catch (...) {
                    std::cout << "Non-numeric data detected, skipping cell." << std::endl;
                }
            }
        }
        data.push_back(row_data);  // Store row in data
    }
    return data;
}

// Function to print the loaded data (for testing)
void printData(const std::vector<std::vector<double>> &data) {
    for (const auto &row : data) {
        for (const auto &value : row) {
            std::cout << value << " ";
        }
        std::cout << std::endl;
    }
}

int main() {
    std::string file_path = "/Users/niharpatel/Desktop/Quant/C++ Quant Finance /Finance-Engine-in-C--/AAPL_1y_1d.xlsx";  // Path to your Excel file
    std::string sheet_name = "Sheet1";  // Name of the Excel sheet

    // Load the data into memory
    std::vector<std::vector<double>> stock_data = loadExcelData(file_path, sheet_name);

    // Print the data to verify it's loaded correctly
    std::cout << "Loaded Stock Data:" << std::endl;
    printData(stock_data);

    // Future extensions: Use this data for Monte Carlo simulations and reinforcement learning algorithms

    return 0;
}
