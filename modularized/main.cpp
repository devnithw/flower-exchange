#include "OrderManager.h"
#include <chrono>

int main() {
    std::string inputFilename = "examples/example6.csv";
    std::string outputFilename = "execution_reports/execution6.csv";

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