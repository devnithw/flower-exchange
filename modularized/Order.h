#ifndef ORDER_H
#define ORDER_H

#include <string>
#include <utility>

class Order {
public:
    std::string ord;
    std::string clientOrder;
    std::string instrument;
    int side;
    int status;
    int quantity;
    double price;

    Order(const std::string& ord, const std::string& clientOrder, const std::string& instrument, int side,
          int status, int quantity, double price);

    bool isBuyOrder() const;
    bool isSellOrder() const;
    std::pair<bool, std::string> isValid() const;
    bool operator==(const Order& other) const;
    void printOrder() const;
};

#endif // ORDER_H