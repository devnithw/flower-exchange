#include "OrderManager.h"
#include <chrono>
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    // Get arguments
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <input_filename> <output_filename>" << std::endl;
        return 1;
    }

    // Get input and output filenames
    std::string inputFilename = argv[1];
    std::string outputFilename = argv[2];

    // Instantiate order manager
    OrderManager orderManager(inputFilename, outputFilename);

    // Start timer
    auto start = std::chrono::high_resolution_clock::now();

    // Process orders
    orderManager.processOrders();
    auto end = std::chrono::high_resolution_clock::now();

    // Calculate execution time and write to CSV
    long long executionTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    CSVHandler().writeExecutionTimeToCSV(outputFilename, executionTime);

    return 0;
}