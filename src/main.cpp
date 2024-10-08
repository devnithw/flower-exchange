#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <set>
#include <algorithm>
#include <chrono>
#include <typeinfo>

using namespace std;

// Class to represent a single order
class Order {
public:
    string ord;
    string clientOrder;
    string instrument;
    string side;
    string execStatus;
    int quantity;
    int price;

    Order(const string& ord, const string& clientOrder, const string& instrument, const string& side,
          const string& execStatus, int quantity, int price)
        : ord(ord), clientOrder(clientOrder), instrument(instrument), side(side), 
          execStatus(execStatus), quantity(quantity), price(price) {}

    bool isBuyOrder() const {
        return side == "1";
    }

    bool isSellOrder() const {
        return side == "2";
    }

    bool isValid() const {
        return !clientOrder.empty() && !instrument.empty() && !side.empty() &&
               quantity > 0 && price > 0 && quantity % 10 == 0 &&
               quantity >= 10 && quantity <= 1000 &&
               (side == "1" || side == "2");
    }
};

// Class to handle CSV file operations
class CSVHandler {
public:
    vector<Order> readCSV(const string& filename) {
        vector<Order> orders;
        ifstream file(filename);

        if (!file.is_open()) {
            cerr << "Error opening file: " << filename << endl;
            return orders;
        }

        string line;
        int orderCounter = 1;
        while (getline(file, line)) {
            istringstream ss(line);
            string clientOrder, instrument, side, quantityStr, priceStr;
            
            getline(ss, clientOrder, ',');
            getline(ss, instrument, ',');
            getline(ss, side, ',');
            getline(ss, quantityStr, ',');
            getline(ss, priceStr, ',');

            int quantity = stoi(quantityStr);
            int price = stoi(priceStr);
            string ord = "ord" + to_string(orderCounter++);

            orders.emplace_back(ord, clientOrder, instrument, side, "New", quantity, price);
        }

        file.close();
        return orders;
    }

    void writeOrderToCSV(const string& filename, const Order& order) {
        ofstream file(filename, ios_base::app);
        if (!file.is_open()) {
            cerr << "Error opening file: " << filename << endl;
            return;
        }
        file << order.ord << "," << order.clientOrder << "," << order.instrument << ","
             << order.side << "," << order.execStatus << "," << order.quantity << ","
             << order.price << endl;
        file.close();
    }

    void writeExecutionTimeToCSV(const string& filename, long long executionTime) {
        ofstream file(filename, ios_base::app);
        if (!file.is_open()) {
            cerr << "Error opening file: " << filename << endl;
            return;
        }
        file << "Execution Time (ms)," << executionTime << endl;
        file.close();
    }
};

// Class to manage the order book and matching logic
class OrderBook {
private:
    vector<Order> buyOrders;
    vector<Order> sellOrders;
    CSVHandler csvHandler;

    void matchOrders() {
        // Sort the buy orders in descensing order
        sort(buyOrders.begin(), buyOrders.end(), [](const Order& a, const Order& b) { return a.price > b.price; });
        sort(sellOrders.begin(), sellOrders.end(), [](const Order& a, const Order& b) { return a.price < b.price; });

        while (!buyOrders.empty() && !sellOrders.empty()) {
            Order& buyOrder = buyOrders.front();
            Order& sellOrder = sellOrders.front();

            if (buyOrder.price >= sellOrder.price) {
                int tradeQuantity = min(buyOrder.quantity, sellOrder.quantity);
                buyOrder.quantity -= tradeQuantity;
                sellOrder.quantity -= tradeQuantity;

                if (buyOrder.quantity == 0) buyOrders.erase(buyOrders.begin());
                if (sellOrder.quantity == 0) sellOrders.erase(sellOrders.begin());

                buyOrder.execStatus = (buyOrder.quantity == 0) ? "Fill" : "PFill";
                sellOrder.execStatus = (sellOrder.quantity == 0) ? "Fill" : "PFill";

                csvHandler.writeOrderToCSV("execution_rep.csv", buyOrder);
                csvHandler.writeOrderToCSV("execution_rep.csv", sellOrder);
            } else {
                break;
            }
        }
    }

public:
    OrderBook(CSVHandler& handler) : csvHandler(handler) {}

    // Divide the orders into buy and sell orders
    void processOrder(const Order& order) {
        if (order.isBuyOrder()) {
            buyOrders.push_back(order);
        } else if (order.isSellOrder()) {
            sellOrders.push_back(order);
        }

        // Resolve orderbook
        matchOrders();
    }
};

// Main class to manage the order processing
class OrderManager {
private:
    CSVHandler csvHandler;
    OrderBook orderBook;
    string inputFilename;
    string outputFilename;

public:
    // Constructor for the order manager
    OrderManager(const string& inputFile, const string& outputFile)
        : csvHandler(), orderBook(csvHandler), inputFilename(inputFile), outputFilename(outputFile) {}

    void processOrders() {
        // Read the orders from the input CSV file
        vector<Order> orders = csvHandler.readCSV(inputFilename);

        // Print orders
        cout << "Printing the orders" << endl;
        for (const Order& order : orders) {
        cout << "Order ID: " << order.ord
            << ", Client Order: " << order.clientOrder
            << ", Instrument: " << order.instrument
            << ", Side: " << order.side
            << ", Status: " << order.execStatus
            << ", Quantity: " << order.quantity
            << ", Price: " << order.price
            << std::endl;
        }

        for (Order& order : orders) {
            // Check for invalid orders
            if (order.isValid()) {
                orderBook.processOrder(order);
            } else {
                order.execStatus = "Reject";
                csvHandler.writeOrderToCSV(outputFilename, order);
            }
        }
    }
};

int main() {
    string inputFilename = "example1.csv"; // Input CSV file with orders
    string outputFilename = "execution1.csv"; // Output CSV file for order execution

    // Instantiate order manager
    OrderManager orderManager(inputFilename, outputFilename);

    // Start timer
    auto start = chrono::high_resolution_clock::now();

    // Process orders
    orderManager.processOrders();
    auto end = chrono::high_resolution_clock::now();

    // Calculate execution time and write to CSV
    long long executionTime = chrono::duration_cast<chrono::milliseconds>(end - start).count();
    CSVHandler().writeExecutionTimeToCSV(outputFilename, executionTime);

    return 0;
}
