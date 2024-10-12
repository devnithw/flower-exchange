// OrderManager.h
#ifndef ORDERMANAGER_H
#define ORDERMANAGER_H

#include "OrderBook.h"
#include "CSVHandler.h"

class OrderManager {
private:
    CSVHandler csvHandler;
    OrderBook orderBook;
    std::string inputFilename;
    std::string outputFilename;

public:
    OrderManager(const std::string& inputFile, const std::string& outputFile);
    void processOrders();
};

#endif // ORDERMANAGER_H
