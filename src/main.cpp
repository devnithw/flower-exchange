#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <set>
#include <iomanip>
#include <algorithm>
#include <chrono>
#include <typeinfo>
#include <cctype>

using namespace std;

string inputFilename = "examples/example6.csv"; // Input CSV file with orders
string outputFilename = "execution_reports/execution_test.csv"; // Output CSV file with execution report

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
    int side;
    int status;
    int quantity;
    double price;

    // order constructor
    Order(const string& ord, const string& clientOrder, const string& instrument, int side,
        int status, int quantity, double price)
        : ord(ord), clientOrder(clientOrder), instrument(instrument), side(side), 
          status(status), quantity(quantity), price(price) {}

    bool isBuyOrder() const {
        return side == 1;
    }

    bool isSellOrder() const {
        return side == 2;
    }

    pair<bool, string> isValid() const {

        bool nonempty_fields = !clientOrder.empty() && !instrument.empty();
        bool valid_quantity = quantity % 10 == 0 && quantity >= 10 && quantity <= 1000;
        bool valid_price = price > 0;
        bool valid_side = side == 1 || side == 2;
        bool valid_instrument = instrument == "Rose" || instrument == "Lavender" || instrument == "Lotus" || instrument == "Tulip" || instrument == "Orchid";  

        // validity of the order
        bool is_valid = nonempty_fields && valid_quantity && valid_price && valid_side && valid_instrument;

        // Reason string generation for invalid string
        string reason = "";
        if (!nonempty_fields) reason = "Empty fields";
        else if (!valid_instrument) reason = "Invalid instrument";
        else if (!valid_quantity) reason = "Invalid quantity";
        else if (!valid_price) reason = "Invalid price";
        else if (!valid_side) reason = "Invalid side";

        return {is_valid, reason};
    }

    // Overload the == operator to compare two orders
    bool operator==(const Order& other) const {
    return ord == other.ord && 
           clientOrder == other.clientOrder &&
           instrument == other.instrument && 
           side == other.side &&
           quantity == other.quantity &&
           price == other.price;
    }

    void printOrder() const {
        cout << "Order ID: " << ord
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
            string clientOrder, instrument, sideStr, quantityStr, priceStr;
            
            getline(ss, clientOrder, ',');
            getline(ss, instrument, ',');
            getline(ss, sideStr, ',');
            getline(ss, quantityStr, ',');
            getline(ss, priceStr, ',');

            // Trim whitespace from the side and instrument fields
            trim(sideStr);
            trim(instrument);
            
            // Convert quantity and price to integers
            int quantity = stoi(quantityStr);
            int side = stoi(sideStr);
            double price = stod(priceStr);

            // Generate order ID
            string ord = "ord" + to_string(orderCounter++);

            // Create order object and push to the orders vector
            orders.emplace_back(ord, clientOrder, instrument, side, 0, quantity, price);
        }

        file.close();
        return orders;
    }

    void writeOrderToCSV(const string& filename, const Order& order, const string& reason = "") {
        ofstream file(filename, ios_base::app);
        if (!file.is_open()) {
            cerr << "Error opening file: " << filename << endl;
            return;
        }

        // Status string generation
        string status = (order.status == 0 ? "New" :
                         order.status == 1 ? "Rejected" :
                         order.status == 2 ? "Fill" :
                         order.status == 3 ? "Pfill" : "Unknown");

        // Write the order details to the CSV file
        file << order.ord << "," << order.clientOrder << "," << order.instrument << ","
             << order.side << "," << status << "," << order.quantity << ","
             << fixed << setprecision(2) << order.price;

        // Add a reason if the order is invalid
        if (!reason.empty()) {
            file << "," << reason;
        }

        file << endl;
        file.close();
    }

    void writeHeadingToCSV(const string& filename) {
        ofstream file(filename, ios_base::app);
        if (!file.is_open()) {
            cerr << "Error opening file: " << filename << endl;
            return;
        }
        file << "Order ID" << "," << "Cl. Ord. ID" << "," << "Instrument" << ","
             << "Side" << "," << "Status" << "," << "Quantity" << ","
             << "Price" << "," << "Reason" << endl;
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
        cout << "Sorting the orderbook" << endl;

        // Sort the buy orders in descensing order of price, and in case of a tie, by 'ord' (FIFO)
        sort(buyOrders.begin(), buyOrders.end(), [](const Order& a, const Order& b) {
            if (a.price != b.price) return a.price > b.price;  // Higher price comes first for buy orders
            else return a.ord < b.ord; // For the same price, earlier 'ord' comes first
        });

        // Sort the sell orders in ascending order of price, and in case of a tie, by 'ord' (first come first serve)
        sort(sellOrders.begin(), sellOrders.end(), [](const Order& a, const Order& b) {
            if (a.price != b.price) return a.price < b.price;  // Lower price comes first for sell orders
            else return a.ord < b.ord; // For the same price, earlier 'ord' comes first
        });
    }

public:
    OrderBook(CSVHandler& handler) : csvHandler(handler) {}

    // Process each order
    void processOrder(Order& input_order) {

        cout << "Now considering: "<< input_order.ord << endl;

        bool isMatching = false; // flag to check if the order is matching
        Order processed_order = input_order; // generating order object for the processing order

        // Divide into buy or sell orders
        // If the order is BUY
        if (input_order.isBuyOrder()) {
            cout << "This is a buy order" << endl;

            // Check if there are any matching buy orders
            auto it = sellOrders.begin(); // iterator for the sell orders
            while (it != sellOrders.end()) {
                
                //current sell order
                Order& sell_order = *it;
            
                if (input_order.instrument == sell_order.instrument && input_order.price >= sell_order.price && input_order.quantity >= sell_order.quantity){

                    //Matching orders found
                    cout << "Matching orders found" << endl;
                    isMatching = true;

                    //Checking whether if the order is filling or partial-filling
                    //Fill
                    if (input_order.quantity == sell_order.quantity){
                        processed_order.status = 2;
                        sell_order.status = 2;
                        processed_order.price = sell_order.price;
                    }
                    
                    //Pfill
                    else if (input_order.quantity > sell_order.quantity){
                        processed_order.status = 3;
                        sell_order.status = 2;
                        processed_order.quantity = sell_order.quantity;
                        input_order.quantity = input_order.quantity - sell_order.quantity;
                        processed_order.price = sell_order.price;
                    }

                    csvHandler.writeOrderToCSV(outputFilename, processed_order); // writing the processed order to the CSV
                    csvHandler.writeOrderToCSV(outputFilename, sell_order); // writing the sell order to the CSV

                    //deleting the sell order from the orderbook
                    it = sellOrders.erase(it);
                }
                else{
                    ++it;
                }
            }
            

            if (!isMatching){
                // There are no matching orders
                cout << "No matching orders" << endl;
                    
                // Push the order to the orderbook
                buyOrders.push_back(input_order);
                csvHandler.writeOrderToCSV(outputFilename, input_order);
            }       
        }
    

       // If the order is SELL
        else if (input_order.isSellOrder()) { 
            cout << "This is a sell order" << endl;

            // Check if there are any matching buy orders by iterating through the buy orders
            auto it = buyOrders.begin();
            while (it != buyOrders.end()){

                //current buy order
                Order& buy_order = *it;

                if (input_order.instrument == buy_order.instrument  && input_order.price <= buy_order.price && input_order.quantity >= buy_order.quantity){

                    //Matching orders found
                    cout << "Matching orders found" << endl;
                    isMatching = true;

                    //Checking whether if the order is filling or partial-filling
                    //Pfill
                    if (input_order.quantity > buy_order.quantity){
                        processed_order.status = 3;
                        buy_order.status = 2;
                        input_order.quantity = input_order.quantity - buy_order.quantity;
                        processed_order.quantity = buy_order.quantity;
                        processed_order.price = buy_order.price; 
                    }

                    //Fill
                    else if (input_order.quantity == buy_order.quantity){
                        processed_order.status = 2;
                        buy_order.status = 2;
                        processed_order.price = buy_order.price;
                    }

                    csvHandler.writeOrderToCSV(outputFilename, processed_order); // writing the processed order to the CSV
                    csvHandler.writeOrderToCSV(outputFilename, buy_order); // writing the buy order to the CSV
                    
                    //deleting the buy order from the orderbook
                    it = buyOrders.erase(it);
                }
                else{
                    ++it;
                }
            }

            
            if (!isMatching){
                // There are no matching orders
                cout << "No matching orders" << endl;

                // Otherwise push the order to the orderbook
                sellOrders.push_back(input_order);
                csvHandler.writeOrderToCSV(outputFilename, input_order);
            }

        }

        // Adding the Pfill input order to the orderbook
        if (isMatching && processed_order.status == 3){

            input_order.status = 3; // setting the status of the input order to Pfill

            if (input_order.isBuyOrder()) buyOrders.push_back(input_order); // adding the Pfill order to the buy orderbook
            else if (input_order.isSellOrder()) sellOrders.push_back(input_order); // adding the Pfill order to the sell orderbook
        }
        
        // Sort the orderbook at the end of the processing of one order
        sortOrderbook();
        printOrderbook();
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



        // Check for invalid ordes
        for (Order& order : orders) {
            // Check for invalid orders
            auto [is_valid, reason] = order.isValid();
            if (is_valid) {
                orderBook.processOrder(order);
            } 
            else {
                //reject the order
                order.status = 1;
                csvHandler.writeOrderToCSV(outputFilename, order, reason);
            }
        }
        
        // Print the final orderbook
        cout << "---------------------" << endl;
        cout << "Printing the final orderbook" << endl;
        cout << "---------------------" << endl;
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