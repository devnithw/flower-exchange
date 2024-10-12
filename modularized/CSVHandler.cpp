#include "CSVHandler.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>

void trim(std::string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    size_t end = s.find_last_not_of(" \t\r\n");

    if (start != std::string::npos && end != std::string::npos) {
        s = s.substr(start, end - start + 1);
    }
    else {
        s.clear();
    }
}

std::vector<Order> CSVHandler::readCSV(const std::string& filename) {
    std::vector<Order> orders;
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return orders;
    }
    std::string line;
    int orderCounter = 1;

    while (std::getline(file, line)) {
        std::istringstream ss(line);
        std::string clientOrder, instrument, sideStr, quantityStr, priceStr;
        
        std::getline(ss, clientOrder, ',');
        std::getline(ss, instrument, ',');
        std::getline(ss, sideStr, ',');
        std::getline(ss, quantityStr, ',');
        std::getline(ss, priceStr, ',');

        trim(sideStr);
        trim(instrument);
        
        int quantity = std::stoi(quantityStr);
        int side = std::stoi(sideStr);
        double price = std::stod(priceStr);

        std::string ord = "ord" + std::to_string(orderCounter++);

        orders.emplace_back(ord, clientOrder, instrument, side, 0, quantity, price);
    }

    file.close();
    return orders;
}

void CSVHandler::writeOrderToCSV(const std::string& filename, const Order& order, const std::string& reason) {
    std::ofstream file(filename, std::ios_base::app);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return;
    }

    std::string status = (order.status == 0 ? "New" :
                          order.status == 1 ? "Rejected" :
                          order.status == 2 ? "Fill" :
                          order.status == 3 ? "Pfill" : "Unknown");

    file << order.ord << "," << order.clientOrder << "," << order.instrument << ","
         << order.side << "," << status << "," << order.quantity << ","
         << std::fixed << std::setprecision(2) << order.price;

    if (!reason.empty()) {
        file << "," << reason;
    }

    file << std::endl;
    file.close();
}

void CSVHandler::writeHeadingToCSV(const std::string& filename) {
    std::ofstream file(filename, std::ios_base::app);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return;
    }
    file << "Order ID" << "," << "Cl. Ord. ID" << "," << "Instrument" << ","
         << "Side" << "," << "Status" << "," << "Quantity" << ","
         << "Price" << "," << "Reason" << std::endl;
    file.close();
}

void CSVHandler::writeExecutionTimeToCSV(const std::string& filename, long long executionTime) {
    std::ofstream file(filename, std::ios_base::app);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return;
    }
    file << "Execution Time (ms)," << executionTime << std::endl;
    file.close();
}