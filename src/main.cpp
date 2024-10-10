#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <set>
#include <algorithm>
#include <chrono>
#include <typeinfo>
#include <cctype>

using namespace std;

string inputFilename = "example2.csv"; // Input CSV file with orders
string outputFilename = "execution2.csv"; // Output CSV file with execution report

// Utility function to trim the whitespace from start and end of a string
void trim(string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    size_t end = s.find_last_not_of(" \t\r\n");

    if (start != string::npos && end != string::npos) {
        s = s.substr(start, end - start + 1);
    }
    else {
        s.clear();
    }
}

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

    void printOrder() const {
        cout << "Order ID: " << ord
            << ", Client Order: " << clientOrder
            << ", Instrument: " << instrument
            << ", Side: " << side
            << ", Status: " << execStatus
            << ", Quantity: " << quantity
            << ", Price: " << price
            << std::endl;
    }
};

// Class to handle CSV file operations
class CSVHandler {
public:
    vector<Order> readCSV(const string& filename) {
        vector<Order> orders;
        ifstream file(filename);

        // Raise error if file not found
        if (!file.is_open()) {
            cerr << "Error opening file: " << filename << endl;
            return orders;
        }
        string line;
        int orderCounter = 1;

        // Read all the lines from the CSV file
        while (getline(file, line)) {
            istringstream ss(line);
            string clientOrder, instrument, side, quantityStr, priceStr;
            
            getline(ss, clientOrder, ',');
            getline(ss, instrument, ',');
            getline(ss, side, ',');
            getline(ss, quantityStr, ',');
            getline(ss, priceStr, ',');

            // Trim whitespace from the side field
            trim(side);
            
            // Convert quantity and price to integers
            int quantity = stoi(quantityStr);
            int price = stoi(priceStr);

            // Generate order ID
            string ord = "ord" + to_string(orderCounter++);

            // Create order object and push to the orders vector
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

    // Print orderbook
    void printOrderbook (){
        // Print BUY side
        cout << "Printing the BUY side" << endl;
        cout << "---------------------" << endl;
        for (const Order& order : buyOrders) {
            order.printOrder();
        }

        // Print SEll side
        cout << "Printing the SELL side" << endl;
        cout << "---------------------" << endl;
        for (const Order& order : sellOrders) {
            order.printOrder();
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
            order.printOrder();
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

        orderBook.printOrderbook();
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
