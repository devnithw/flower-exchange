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

string inputFilename = "example2.csv"; // Input CSV file with orders
string outputFilename = "execution2.csv"; // Output CSV file with execution report

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

    void writeHeadingToCSV(const string& filename) {
        ofstream file(filename, ios_base::app);
        if (!file.is_open()) {
            cerr << "Error opening file: " << filename << endl;
            return;
        }
        file << "Order ID" << "," << "Cl. Ord. ID" << "," << "Instrument" << ","
             << "Side" << "," << "Exec Status" << "," << "Quantity" << ","
             << "Price" << endl;
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

    void sortOrderbook() {
        // Sort the buy orders in descensing order
        cout << "Sorting the orderbook" << endl;
        sort(buyOrders.begin(), buyOrders.end(), [](const Order& a, const Order& b) { return a.price > b.price; });
        sort(sellOrders.begin(), sellOrders.end(), [](const Order& a, const Order& b) { return a.price < b.price; });
    }

public:
    OrderBook(CSVHandler& handler) : csvHandler(handler) {}

    // Process each order
    void processOrder(const Order& order) {

        cout << "Now considering: "<< order.ord << endl;

        // Divide into buy or sell orders
        // If the order is BUY
        if (order.isBuyOrder()) {
            cout << "This is a buy order" << endl;

            // Check if there are any matching sell orders
            cout << "No matching orders" << endl;

            // Otherwise push the order to the orderbook
            buyOrders.push_back(order);
            csvHandler.writeOrderToCSV(outputFilename, order);

        // If the order is SELL
        } else if (order.isSellOrder()) {
            cout << "This is a sell order" << endl;

            // Check if there are any matching buy orders
            cout << "No matching orders" << endl;

            // Otherwise push the order to the orderbook
            sellOrders.push_back(order);
            csvHandler.writeOrderToCSV(outputFilename, order);
        }
    }
};

// Main class to manage the order processing
class OrderManager {
private:
    CSVHandler csvHandler;
    OrderBook orderBook;

public:
    // Constructor for the order manager
    OrderManager(const string& inputFile, const string& outputFile)
        : csvHandler(), orderBook(csvHandler) {}

    void processOrders() {
        // Read the orders from the input CSV file
        vector<Order> orders = csvHandler.readCSV(inputFilename);
        // Write the Execution report headings to CSV
        csvHandler.writeHeadingToCSV(outputFilename);

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
