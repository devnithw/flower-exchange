#include "OrderBook.h"
#include <algorithm>
#include <iostream>

OrderBook::OrderBook(CSVHandler& handler, const std::string& inputFile, const std::string& outputFile)
    : csvHandler(handler), inputFilename(inputFile), outputFilename(outputFile) {}

void OrderBook::sortOrderbook() {
    std::cout << "Sorting the orderbook" << std::endl;

    std::sort(buyOrders.begin(), buyOrders.end(), [](const Order& a, const Order& b) {
        if (a.price != b.price) return a.price > b.price;
        else return a.ord < b.ord;
    });

    std::sort(sellOrders.begin(), sellOrders.end(), [](const Order& a, const Order& b) {
        if (a.price != b.price) return a.price < b.price;
        else return a.ord < b.ord;
    });
}

void OrderBook::processOrder(Order& input_order) {
    std::cout << "Now considering: " << input_order.ord << std::endl;

    bool isMatching = false;
    bool input_order_filled = false;
    Order processed_order = input_order;
    double input_order_price = input_order.price;

    if (input_order.isBuyOrder()) {
        std::cout << "This is a buy order" << std::endl;

        auto it = sellOrders.begin();
        while (it != sellOrders.end()) {
            Order& sell_order = *it;

            if (input_order_price < sell_order.price) break;

            if (input_order.instrument == sell_order.instrument && input_order.price >= sell_order.price && input_order.quantity >= sell_order.quantity) {
                std::cout << "Matching orders found" << std::endl;
                isMatching = true;

                if (input_order.quantity == sell_order.quantity) {
                    processed_order.status = 2;
                    sell_order.status = 2;
                    processed_order.price = sell_order.price;
                    input_order_filled = true;
                } else if (input_order.quantity > sell_order.quantity) {
                    processed_order.status = 3;
                    sell_order.status = 2;
                    processed_order.quantity = sell_order.quantity;
                    input_order.quantity = input_order.quantity - sell_order.quantity;
                    processed_order.price = sell_order.price;
                }

                csvHandler.writeOrderToCSV(outputFilename, processed_order);
                csvHandler.writeOrderToCSV(outputFilename, sell_order);

                it = sellOrders.erase(it);
            } else {
                ++it;
            }

            if (input_order_filled) break;
        }

        if (!isMatching) {
            std::cout << "No matching orders" << std::endl;
            buyOrders.push_back(input_order);
            csvHandler.writeOrderToCSV(outputFilename, input_order);
        }
    } else if (input_order.isSellOrder()) {
        std::cout << "This is a sell order" << std::endl;

        auto it = buyOrders.begin();
        while (it != buyOrders.end()) {
            Order& buy_order = *it;

            if (input_order_price > buy_order.price) break;

            if (input_order.instrument == buy_order.instrument && input_order.price <= buy_order.price) { // Changed for larger buy orders
                std::cout << "Matching orders found" << std::endl;
                isMatching = true;

                if (input_order.quantity > buy_order.quantity) {
                    processed_order.status = 3;
                    buy_order.status = 2;
                    input_order.quantity = input_order.quantity - buy_order.quantity;
                    processed_order.quantity = buy_order.quantity;
                    processed_order.price = buy_order.price;
                } 
                else if (input_order.quantity == buy_order.quantity) {
                    processed_order.status = 2;
                    buy_order.status = 2;
                    processed_order.price = buy_order.price;
                    input_order_filled = true;
                }
                else if (input_order.quantity < buy_order.quantity) {
                    processed_order.status = 2;
                    buy_order.status = 3;
                    buy_order.quantity = buy_order.quantity - input_order.quantity;
                    processed_order.quantity = input_order.quantity;
                    processed_order.price = buy_order.price;
                } 

                csvHandler.writeOrderToCSV(outputFilename, processed_order);
                csvHandler.writeOrderToCSV(outputFilename, buy_order);

                it = buyOrders.erase(it);
            } else {
                ++it;
            }

            if (input_order_filled) break;
        }

        if (!isMatching) {
            std::cout << "No matching orders" << std::endl;
            sellOrders.push_back(input_order);
            csvHandler.writeOrderToCSV(outputFilename, input_order);
        }
    }

    if (isMatching && processed_order.status == 3) {
        input_order.status = 3;

        if (input_order.isBuyOrder()) buyOrders.push_back(input_order);
        else if (input_order.isSellOrder()) sellOrders.push_back(input_order);
    }

    sortOrderbook();
    printOrderbook();
}

void OrderBook::printOrderbook() {
    std::cout << "Printing the BUY side" << std::endl;
    std::cout << "---------------------" << std::endl;
    for (const Order& order : buyOrders) {
        order.printOrder();
    }

    std::cout << "Printing the SELL side" << std::endl;
    std::cout << "---------------------" << std::endl;
    for (const Order& order : sellOrders) {
        order.printOrder();
    }
}