#include "Order.h"
#include <iostream>

Order::Order(const std::string& ord, const std::string& clientOrder, const std::string& instrument, int side,
             int status, int quantity, double price)
    : ord(ord), clientOrder(clientOrder), instrument(instrument), side(side), 
      status(status), quantity(quantity), price(price) {}

bool Order::isBuyOrder() const {
    return side == 1;
}

bool Order::isSellOrder() const {
    return side == 2;
}

std::pair<bool, std::string> Order::isValid() const {
    bool nonempty_fields = !clientOrder.empty() && !instrument.empty();
    bool valid_quantity = quantity % 10 == 0 && quantity >= 10 && quantity < 1000; // Exclude endpoints
    bool valid_price = price > 0;
    bool valid_side = side == 1 || side == 2;
    bool valid_instrument = instrument == "Rose" || instrument == "Lavender" || instrument == "Lotus" || instrument == "Tulip" || instrument == "Orchid";  

    bool is_valid = nonempty_fields && valid_quantity && valid_price && valid_side && valid_instrument;

    std::string reason = "";
    if (!nonempty_fields) reason = "Empty fields";
    else if (!valid_instrument) reason = "Invalid instrument";
    else if (!valid_quantity) reason = "Invalid quantity";
    else if (!valid_price) reason = "Invalid price";
    else if (!valid_side) reason = "Invalid side";

    return {is_valid, reason};
}

bool Order::operator==(const Order& other) const {
    return ord == other.ord && 
           clientOrder == other.clientOrder &&
           instrument == other.instrument && 
           side == other.side &&
           quantity == other.quantity &&
           price == other.price;
}

void Order::printOrder() const {
    std::cout << "Order ID: " << ord
              << ", Client Order: " << clientOrder
              << ", Instrument: " << instrument
              << ", Side: " << side
              << ", Status: " << (status == 0 ? "New" :
                                  status == 1 ? "Rejected" :
                                  status == 2 ? "Fill" :
                                  status == 3 ? "Pfill" : "Unknown")
              << ", Quantity: " << quantity
              << ", Price: " << price
              << std::endl;
}