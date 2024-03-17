// Project Identifier: C0F4DFE8B340D81183C208F70F9D2D797908754D

#include <getopt.h>
#include <iostream>
#include <string>
#include <vector>
#include <limits>
#include <algorithm>
#include <deque>
#include <unordered_map>

using namespace std;

struct Options {
    bool isQuiet = false;
};

struct Table {
    string name;
    vector<string> columnNames;
    vector<string> columnTypes;
    vector<vector<string>> data;
};

struct Index {
    string indexType;  // hash
    string columnName;
    unordered_map<string, vector<size_t>> hashIndex; // Mapping from column value to row indices
};

class Database {
private:
    unordered_map<string, Table> tables;
    unordered_map<string, Index> indices; // Optional, if you're implementing indexing
public:

    void processCommands() {
        string commandType;
        cin >> commandType;
        while (commandType != "QUIT") {
            vector<string> tokens;

            if (commandType == "QUIT") {
                cout << "% ";
                // Call quit method
                cout << "QUIT COMMAND CALLED" << endl;
                quit();

            } else if (commandType == "CREATE") {
                cout << "% ";
                // Call create method with appropriate arguments
                string token;
                string tableName;
                int numCols;
                cin >> tableName;
                cin >> numCols;
                for (int i = 0; i < numCols*2; i++) {
                    cin >> token;
                    tokens.push_back(token);
                }
                create(tableName, numCols, tokens);

            } else if (commandType == "REMOVE") {
                cout << "% ";
    
                string tableName;
                cin >> tableName;
                remove(tableName);

            } else if (commandType == "#") {
                string junk;
                getline(cin, junk);

            } else if (commandType == "PRINT") {
                cout << "% ";
                string keyword;
                cin >> keyword; // Read the next keyword after "PRINT"
                if (keyword == "FROM") {
                    // Parse the PRINT FROM command
                    string tableName;
                    int numCols;
                    string condition;

                    cin >> tableName >> numCols; // Read table name and number of columns to print
                    for (int i = 0; i < numCols; ++i) {
                        string columnName;
                        cin >> columnName; // Read each column name to print
                        tokens.push_back(columnName);
                    }
                    cin >> condition;
                    if (condition == "WHERE") {
                        printFromWhere(tableName, tokens);
                    } else { // all
                        printFromAll(tableName, tokens);
                    }

                    // Print the selected columns from the specified table
                } else if (keyword == "INSERT") {
                     cout << "% ";
                    // Call insertInto method with appropriate arguments
                    string tableName;
                    int numRows;
                    cin >> tableName >> numRows;
                    insertInto(tableName, numRows);

                } else {
                    // Invalid command syntax
                    cout << "Invalid PRINT command syntax" << endl;
                }
            } else {
                // Invalid command
                // cerr << "Invalid command: " << tokens[0] << tokens[1] << endl;
                string junk;
                getline(cin, junk);
            }
            cin >> commandType;
            tokens.clear();
        }

    }
    // Add methods to interact with the database, such as creating tables, inserting rows, executing queries, etc.
    void create(const string& tableName, int numColumns, const vector<string>& command) {

        // Create a new table and add it to the database
        Table newTable;
        newTable.name = tableName;
         // Iterate through the command vector to populate column types and names
        for (size_t i = 0; i < command.size(); ++i) {
            if (i < static_cast<size_t>(numColumns)) {
                // Populate column types
                newTable.columnTypes.push_back(command[i]);
            } else {
                // Populate column names
                newTable.columnNames.push_back(command[i]);
            }
        }
        tables[tableName] = newTable;

        // Print confirmation message
        cout << "New table " << tableName << " with column(s)";
        for (const string& columnName : newTable.columnNames) {
            cout << " " << columnName;
        }
        cout << " created\n";
    }
    // Method to insert new rows into a specified table
    void insertInto(const string& tableName, int numRows) {
        // Find the table
        auto tableIt = tables.find(tableName);
        if (tableIt == tables.end()) {
            // Table not found
            cout << "Table " << tableName << " not found in the database" << endl;
            return;
        }

        // Insert new rows into the table
        Table& table = tableIt->second;
        size_t startIndex = table.data.size(); // Index of the first row added
        for (int i = 0; i < numRows; ++i) {
            vector<string> values(table.columnNames.size());
            for (size_t j = 0; j < table.columnNames.size(); ++j) {
                cin >> values[j];
            }
            table.data.push_back(values);
        }
        size_t endIndex = table.data.size() - 1; // Index of the last row added

        // Print confirmation message
        cout << "Added " << numRows << " rows to " << tableName << " from position " << startIndex << " to " << endIndex << endl;
    }


    // Method to quit the program and clean up all data
    void quit() {
        // Clean up all internal data
        tables.clear();
        indices.clear();

        // Print goodbye message
        cout << "Thanks for being silly!" << std::endl;
        // Exit the program
        exit(0);
    }

     // Method to remove an existing table from the database
    void remove(const string& tableName) {
        // Check if the table exists
        auto tableIt = tables.find(tableName);
        // error checking
        // if (tableIt == tables.end()) {
        //     // Table does not exist
        //     cout << "Table " << tableName << " not found in the database" << endl;
        //     return;
        // }

        // Remove the table and its associated data
        tables.erase(tableIt);

        // Remove any associated index
        indices.erase(tableName);

        // Print confirmation message
        cout << "Table " << tableName << " removed" << std::endl;
    }

    void printFromWhere(const string& tableName, const vector<string>& printColumns) {
        string lhs;
        string opp;
        string rhs;
        cin >> lhs >> opp >> rhs;
        // Find the table
        auto tableIt = tables.find(tableName);
        // error checking
        // if (tableIt == tables.end()) {
        //     // Table not found
        //     cout << "Table " << tableName << " not found in the database" << endl;
        //     return;
        // }

        // Print the selected columns
        const Table& table = tableIt->second;
        // for (const auto& columnName : printColumns) {
        //     // Check if the column exists in the table
        //     auto colIt = find(table.columnNames.begin(), table.columnNames.end(), columnName);
        //     // error checking
        //     if (colIt == table.columnNames.end()) {
        //         // Column not found in the table
        //         cout << "Column " << columnName << " not found in table " << tableName << endl;
        //         return;
        //     }
        // }

        // Print the column names
        for (const auto& columnName : printColumns) {
            cout << columnName << " ";
        }
        cout << endl;

        // Print the values from the selected columns for each row
        for (const auto& row : table.data) {
            for (const auto& columnName : printColumns) {
                // Find the index of the column
                auto colIndex = find(table.columnNames.begin(), table.columnNames.end(), columnName);
                // Print the value from the corresponding column
                cout << row[static_cast<size_t>(distance(table.columnNames.begin(), colIndex))] << " ";
            }
            cout << endl;
        }

        // Print summary
        cout << "Printed " << table.data.size() << " matching rows from " << tableName << endl;
    }


    void printFromAll(const string& tableName, const vector<string>& printColumns) {
        auto tableIt = tables.find(tableName);


        const Table& table = tableIt->second;
        // Print the column names
        for (const auto& columnName : printColumns) {
            cout << columnName << " ";
        }
        cout << endl;

        // Print the values from the selected columns for each row
        for (const auto& row : table.data) {
            for (const auto& columnName : printColumns) {
                // Find the index of the column
                auto colIndex = find(table.columnNames.begin(), table.columnNames.end(), columnName);
                // Print the value from the corresponding column
                cout << row[static_cast<size_t>(distance(table.columnNames.begin(), colIndex))] << " ";
            }
            cout << endl;
        }

        // Print summary
        cout << "Printed " << table.data.size() << " matching rows from " << tableName << endl;
    }
};

void printHelp(char *argv[])
{
    cout << "Usage: " << argv[0] << " [-m resize|reserve|nosize] | -h\n";
    cout << "This program is to help you learn command-line processing,\n";
    cout << "reading data into a vector, the difference between resize and\n";
    cout << "reserve and how to properly read until end-of-file." << endl;
} // printHelp()

void getMode(int argc, char *argv[], Options &opt)
{
    // These are used with getopt_long()
    opterr = false; // Let us handle all error output for command line options
    int choice;
    int index = 0;
    option long_options[] = {
        // TODO: Fill in two lines, for the "mode" ('m') and
        // the "help" ('h') options.
        {"help", no_argument, nullptr, 'h'},
        {"quiet", no_argument, nullptr, 'q'},
        
    }; // long_options[]

    // TODO: Fill in the double quotes, to match the mode and help options.
    while ((choice = getopt_long(argc, argv, "hq", long_options, &index)) != -1)
    {
        switch (choice)
        {
        case 'h':
            printHelp(argv);
            exit(0);
        case 'q':
            opt.isQuiet = true;
            break;
        default:
            break;

        }
    }
} // getMode()

int main(int argc, char *argv[]) {
    Options opt;
    getMode(argc, argv, opt);
    Database b;
    b.processCommands();
    return 0;
}