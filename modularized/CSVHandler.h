#ifndef CSVHANDLER_H
#define CSVHANDLER_H

#include <vector>
#include <string>
#include "Order.h"

class CSVHandler {
public:
    std::vector<Order> readCSV(const std::string& filename);
    void writeOrderToCSV(const std::string& filename, const Order& order, const std::string& reason = "");
    void writeHeadingToCSV(const std::string& filename);
    void writeExecutionTimeToCSV(const std::string& filename, long long executionTime);
};

#endif // CSVHANDLER_H