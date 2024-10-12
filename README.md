# Flower Exchange Application using C++
## Developed for High Performance Computing with C++ workshop by LSEG Technologies

## Base version
The originally developed code is available in the `src` folder as `main.cpp` file. This file contains all the program logic and implementation for the exchange application in a single .cpp file. The code is compiled into `main` executable file. This program handles all the 7 examples discussed in the workshop session. Compile the code and run using the following commands.

```bash
clang++ -std=c++17 -o main main.cpp
./main
```

This version of the code is not modularized and optimized. Also the filepaths for the input and output CSV files need to be hardcoded into the C++ script before compiling.


## Modularized version (Improved)
This is a modularized and optimized version of the `src/main.cpp` code. The individual objects are divided into their own modules. Also includes the funcitonality to give input and output filepaths using command line arguments.

- `CSVHandler` class - Handles the reading and writing rows a CSV file. Has seperate methods for reading an entire CSV, writing headings to CSV and writing each order executed to a line in CSV.
- `Order` class - This module contains the code for the Order object. Each row in the input CSV file is converted to an Order object and sent to the OrderManager for execution.
- `OrderBook` class - Contains the code for the OrderBook object. Includes two private tables for BUY orders and SELL orders. Handles sorting the orderbook according to the price. It also includes the main logic for executing a matched order.
- `OrderManager` class - This module manages reading inputs, creating a Order vector and executing each Order using the OrderBook object.
- `main.cpp` file - This file reads the filepaths from the command line arguments and tracks execution time.

All above classes are seperately implemented in `.h` and `.cpp` files and compiled into `flower_trader` executable file using the command below.

```bash
g++ -std=c++17 main.cpp Order.cpp CSVHandler.cpp OrderBook.cpp OrderManager.cpp -o flower_trader
```
A given example can be run using the `flower_trader` application using the below command format.

```bash
./flower_trader examples/example1.csv execution_reports/execution1.csv                                            
```

The first arguments specifies the input CSV file path while the second argument denotes the output CSV.


## How to run
1. Clone this repository to your local machine
```bash
git clone https://github.com/devnithw/flower-exchange.git                                     
```
2. Move to the modularized folder
```bash
cd flower-exchange/modularized                                  
```
3. Compile the C++ files into an executable using a suitable compiler (g++).
```bash
g++ -std=c++17 main.cpp Order.cpp CSVHandler.cpp OrderBook.cpp OrderManager.cpp -o flower_trader
```
4. Run the executable file by passing in the correct filepaths.
```bash
./flower_trader examples/example1.csv execution_reports/execution1.csv                                            
```

## Features
- C++17 standard code
- OOP implementation
- Modularized implementation
- Compact lightweight code

## Group members
- Devnith Wijesinghe ([devnithw](https://github.com/devnithw))
- Nadun Rajapaksha ([nadunnr](https://github.com/nadunnr))

## Acknowledgements
We would like to express our gratitude to the LSEG C++ Workshop Series for their invaluable insights.