#include "OrderManager.h"
#include <iostream>

OrderManager::OrderManager(const std::string& inputFile, const std::string& outputFile)
    : inputFilename(inputFile), outputFilename(outputFile), csvHandler(), orderBook(csvHandler, inputFile, outputFile) {}

void OrderManager::processOrders() {
    // Read the orders from the input CSV file
    std::vector<Order> orders = csvHandler.readCSV(inputFilename);
    
    // Write the Execution report headings to CSV
    csvHandler.writeHeadingToCSV(outputFilename);

    // Process each order
    for (Order& order : orders) {
        // Check for invalid orders
        auto [is_valid, reason] = order.isValid();
        if (is_valid) {
            orderBook.processOrder(order);
        } 
        else {
            // Reject the order
            order.status = 1;
            csvHandler.writeOrderToCSV(outputFilename, order, reason);
        }
    }
    
    // Print the final orderbook
    std::cout << "---------------------" << std::endl;
    std::cout << "Printing the final orderbook" << std::endl;
    std::cout << "---------------------" << std::endl;
    orderBook.printOrderbook();
}