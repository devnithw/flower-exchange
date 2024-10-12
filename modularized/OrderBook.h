#ifndef ORDERBOOK_H
#define ORDERBOOK_H

#include <vector>
#include "Order.h"
#include "CSVHandler.h"

class OrderBook {
private:
    std::vector<Order> buyOrders;
    std::vector<Order> sellOrders;
    CSVHandler& csvHandler;
    std::string inputFilename;
    std::string outputFilename;

    void sortOrderbook();

public:
    OrderBook(CSVHandler& handler, const std::string& inputFile, const std::string& outputFile);
    void processOrder(Order& input_order);
    void printOrderbook();
};

#endif // ORDERBOOK_H