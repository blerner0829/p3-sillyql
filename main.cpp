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

class Comparator {
public:
    bool operator()(const string& lhs, const string& op, const string& rhs) const {
        if (op == "=") {
            return lhs == rhs;
        } else if (op == "<") {
            return lhs < rhs;
        } else if (op == ">") {
            return lhs > rhs;
        } else {
            // Invalid operaxtor
            cout << "Invalid comparison operator: " << op << endl;
            return false;
        }
    }
};

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
    Options o;
public:

    void processCommands() {
        string commandType;
        cin >> commandType;
        while (commandType != "QUIT") {
            vector<string> tokens;
            switch (commandType[0])
            {
            case 'C': {
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
                break;
            } 
            case 'R': {
                cout << "% ";
                string tableName;
                cin >> tableName;
                remove(tableName);
                break;
            }
            case '#': {
                cout << "% ";
                string junk;
                getline(cin, junk);
                break;
            }
            case 'P': {
                // Print the selected columns from the specified table
                cout << "% ";
                string keyword;
                cin >> keyword; // Read the next keyword after "PRINT"
                if (keyword != "FROM") break;
                // Parse the PRINT FROM command
                string tableName;
                int numCols;

                cin >> tableName >> numCols; // Read table name and number of columns to print
                for (int i = 0; i < numCols; ++i) {
                    string columnName;
                    cin >> columnName; // Read each column name to print
                    tokens.push_back(columnName);
                }
                cin >> keyword;
                if (keyword == "WHERE") {
                    printFromWhere(tableName, tokens);
                } else { // all
                    printFromAll(tableName, tokens);
                }
                // error handling
                // } else {
                //     // Invalid command syntax
                //     cout << "Invalid PRINT command syntax" << endl;
                // }
                break;
            }
            case 'I': {
                cout << "% ";
                // Call insertInto method with appropriate arguments
                string tableName;
                int numRows;
                string token;
                cin >> token;
                if (token != "INTO") break; // TODO: if this happens, getline(cin, junk) will not be called
                cin >> tableName >> numRows >> token;
                if (token != "ROWS") break;
                insertInto(tableName, numRows);
                break;
                
            }
            case 'D': {
                cout << "% ";
                // Call deleteFrom method with appropriate arguments
                string tableName, colName, op, value, keyword;
                cin >> keyword >> tableName;
                if (keyword != "FROM") break;
                cin >> keyword >> colName >> op >> value;
                if (keyword != "WHERE") break;
                deleteFrom(tableName, colName, op, value);
                break;
            }
            case 'J': {
                cout << "% ";
                string tableName1, tableName2, colName1, colName2, junk;
                int numCols;
                // JOINT    table1   AND      table2       WHERE     col1       =       col2       AND      PRINT   numCols
                cin >> tableName1 >> junk >> tableName2 >> junk >> colName1 >> junk >> colName2 >> junk >> junk >> numCols;
                vector<pair<size_t, string>> printColumns;
                for (int i = 0; i < numCols; ++i) {
                    string printColName;
                    int tableIndex;
                    cin >> printColName >> tableIndex;
                    printColumns.emplace_back(tableIndex, printColName);
                }
                joinTables(tableName1, tableName2, colName1, colName2, printColumns);
                break;
            }
            case 'G': {
                cout << "% ";
                // Parse the GENERATE command
                string keyword, tableName, indexType, colName;
                cin >> keyword >> tableName >> indexType >> keyword >> keyword >> colName;
                // cout << "TABLE NAME: " << tableName << endl;
                generateIndex(tableName, indexType, colName);
                break;
            }
            default: {
                cout << "% ";
                cout << "Error: unrecognized command\n";
                string junk;
                getline(cin, junk);
                break;
            }
            }
            cin >> commandType;
            tokens.clear();
        }
        if (commandType == "QUIT") {
                cout << "% ";
                // Call quit method
                quit();
        }
    }
   
    void create(const string& tableName, int numColumns, const vector<string>& command) {
        // Check if the table already exists in the database
        if (tables.find(tableName) != tables.end()) {
            cout << "Error during CREATE: Cannot create already existing table " << tableName << endl;
            return;
        }
        
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
            cout << "Error during INSERT: " << tableName  << " `does not name a table in the database\n";
            string junk;
            for (int i = 0; i < numRows; ++i) {
                getline(cin, junk);
            }
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

    // Method to delete rows from the specified table based on conditions
    void deleteFrom(const string& tableName, const string& columnName, const string& op, const string& value) {
        // Find the table
        auto tableIt = tables.find(tableName);
        if (tableIt == tables.end()) {
            // Table not found
            cout << "Error during DELETE: " << tableName << " does not name a table in the database\n";
            return;
        }

        // Find the column index
        Table& table = tableIt->second;
        auto colIndex = find(table.columnNames.begin(), table.columnNames.end(), columnName);
        if (colIndex == table.columnNames.end()) {
            // Column not found
            cout << "Error during DELETE: " <<  columnName << " does not name a column in " << tableName << endl;
            return;
        }
        size_t columnIndex = static_cast<size_t>(distance(table.columnNames.begin(), colIndex));

        // Delete rows based on conditions
        Comparator comparator;
        int deletedRows = 0;
        for (auto it = table.data.begin(); it != table.data.end();) {
        if (comparator((*it)[columnIndex], op, value)) { // Using overloaded operator
            it = table.data.erase(it);
            ++deletedRows;
        } else {
            ++it;
        }
    }

        // Print summary
        cout << "Deleted " << deletedRows << " rows from " << tableName << endl;
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
        if (tableIt == tables.end()) {
            // Table does not exist
            cout << "Error during REMOVE: " << tableName << " does not name a table in the database\n";
            return;
        }

        // Remove the table and its associated data
        tables.erase(tableIt);

        // Remove any associated index
        indices.erase(tableName);

        // Print confirmation message
        cout << "Table " << tableName << " removed" << endl;
    }

    void printFromWhere(const string& tableName, const vector<string>& printColumns) {
        string lhs;
        string opp;
        string rhs;
        int numMatches = 0;
        cin >> lhs >> opp >> rhs;
        // Find the table
        auto tableIt = tables.find(tableName);

        const Table& table = tableIt->second;
        
        // error checking
        if (tableIt == tables.end()) {
            // Table not found
            cout << "Error during PRINT: " << tableName  << " does not name a table in the database\n";
            return;
        }

        for (const auto& columnName : printColumns) {
            // Check if the column exists in the table
            auto colIt = find(table.columnNames.begin(), table.columnNames.end(), columnName);
            // error checking
            if (colIt == table.columnNames.end()) {
                // Column not found in the table
                cout << "Error during PRINT: " << columnName << " does not name a column in " << tableName << endl;
                return;
            }
        }

        // error checking for WHERE statement
        auto colIt = find(table.columnNames.begin(), table.columnNames.end(), lhs);
        // TODO: Write a test case for this
        if (colIt == table.columnNames.end()) {
            cout << "Error during PRINT: " << lhs << " does not name a column in " << tableName << endl;
            return;
        }

        // Print the column names if !quietMode
        if (!o.isQuiet) {
            for (const auto& columnName : printColumns) {
                cout << columnName << " ";
            }
            cout << endl;
        }

        // Find the index of the column to be compared
        auto colIndex = find(table.columnNames.begin(), table.columnNames.end(), lhs);
        // Print the values from the selected columns for each row
        Comparator comparator; // Instantiate the comparator
        for (const auto& row : table.data) {
            if (comparator(row[static_cast<size_t>(distance(table.columnNames.begin(), colIndex))], opp, rhs)) {
                // Print the values from the selected columns for this row
                for (const auto& columnName : printColumns) {
                    auto colIndex = find(table.columnNames.begin(), table.columnNames.end(), columnName);
                    if (!o.isQuiet) {
                        cout << row[static_cast<size_t>(distance(table.columnNames.begin(), colIndex))] << " ";
                    }
                    numMatches++;
                }
                cout << endl;
            }
        }

        // Print summary
        cout << "Printed " << numMatches << " matching rows from " << tableName << endl;

    }

    void printFromAll(const string& tableName, const vector<string>& printColumns) {
        auto tableIt = tables.find(tableName);
        const Table& table = tableIt->second;

        // error checking
        if (tableIt == tables.end()) {
            // Table not found
            cout << "Error during PRINT: " << tableName  << " does not name a table in the database\n";
            return;
        }

        for (const auto& columnName : printColumns) {
            // Check if the column exists in the table
            auto colIt = find(table.columnNames.begin(), table.columnNames.end(), columnName);
            // error checking
            if (colIt == table.columnNames.end()) {
                // Column not found in the table
                 cout << "Error during PRINT: " << columnName << " does not name a column in " << tableName << endl;
                return;
            }
        }
        
        // Print the column names
        if (!o.isQuiet) {
            for (const auto& columnName : printColumns) {
                cout << columnName << " ";
            }
            cout << endl;
        }

        // Print the values from the selected columns for each row
        for (const auto& row : table.data) {
            for (const auto& columnName : printColumns) {
                // Find the index of the column
                auto colIndex = find(table.columnNames.begin(), table.columnNames.end(), columnName);
                // Print the value from the corresponding column
                if (!o.isQuiet) {
                    cout << row[static_cast<size_t>(distance(table.columnNames.begin(), colIndex))] << " ";
                }
            }
            cout << endl;
        }

        // Print summary
        cout << "Printed " << table.data.size() << " matching rows from " << tableName << endl;
    }

    void joinTables(const string& tableName1, const string& tableName2, const string& colName1, const string& colName2, const vector<pair<size_t, string>>& printColumns) {
        int numPrinted = 0;
        
        // Find the tables
        auto tableIt1 = tables.find(tableName1);
        auto tableIt2 = tables.find(tableName2);
        
        // Check if both tables exist
        if (tableIt1 == tables.end()) {
            cout << "Error during JOIN: " << tableName1 << " does not name a table in the database\n";
            return;
        }
        if (tableIt2 == tables.end()) {
            cout << "Error during JOIN: " << tableName2 << " does not name a table in the database\n";
            return;
        }

        // Find the columns
        const Table& table1 = tableIt1->second;
        const Table& table2 = tableIt2->second;
        auto colIndex1 = find(table1.columnNames.begin(), table1.columnNames.end(), colName1);
        auto colIndex2 = find(table2.columnNames.begin(), table2.columnNames.end(), colName2);
        
        // Check if both columns exist
        if (colIndex1 == table1.columnNames.end()) {
            cout << "Error during JOIN: " << colName1 << " does not name a column in " << tableName1 << endl;
            return;
        }
        if (colIndex2 == table2.columnNames.end()) {
            cout << "Error during JOIN: " << colName2 << " does not name a column in " << tableName2 << endl;
            return;
        }

        // Get the indices of the columns
        size_t columnIndex1 = static_cast<size_t>(distance(table1.columnNames.begin(), colIndex1));
        size_t columnIndex2 = static_cast<size_t>(distance(table2.columnNames.begin(), colIndex2));


        // iterate through the columns that will be printed
        if (!o.isQuiet) {
            for (const auto& cols : printColumns) {
                cout << cols.second << " ";
            }
        }
        cout << endl;
        // Iterate through the first table
        for (const auto& row1 : table1.data) {
            // Find rows in the second table that match the condition
            for (const auto& row2 : table2.data) {
                if (row1[columnIndex1] == row2[columnIndex2]) {
                    numPrinted++;
                    // Print the selected columns
                    for (const auto& printCol : printColumns) {
                        // size_t tableIndex = printCol.first;
                        // const string& columnName = printCol.second;
                        const Table* printTable = nullptr;
                        size_t printColumnIndex = 0;

                        if (printCol.first == 1) {
                            printTable = &table1;
                            auto colIndex = find(printTable->columnNames.begin(), printTable->columnNames.end(), printCol.second);
                            if (colIndex == printTable->columnNames.end()) {
                                cout << "Error during JOIN: " << printCol.first << " does not name a column in " << printTable->name << endl;
                            } else {
                                printColumnIndex = static_cast<size_t>(distance(printTable->columnNames.begin(), colIndex));
                                if (!o.isQuiet) {
                                    cout << row1[printColumnIndex] << " ";
                                }
                            }
                        } else if (printCol.first == 2) {
                            printTable = &table2;
                            auto colIndex = find(printTable->columnNames.begin(), printTable->columnNames.end(), printCol.second);
                            if (colIndex == printTable->columnNames.end()) {
                                cout << "Error during JOIN: " << printCol.first << " does not name a column in " << printTable->name << endl;
                            } else {
                                printColumnIndex = static_cast<size_t>(distance(printTable->columnNames.begin(), colIndex));
                                if (!o.isQuiet) {
                                    cout << row2[printColumnIndex] << " ";
                                }
                            }
                        }
                    }
                    cout << endl;
                }
            }
        }

        // Print summary
        cout << "Printed " << numPrinted << " rows from joining " << tableName1 << " to " << tableName2 << endl;
    }

    void generateIndex(const string& tableName, const string& indexType, const string& colName) {
        // Check if the specified table exists
        auto tableIt = tables.find(tableName);
        if (tableIt == tables.end()) {
            cout << "Error during GENERATE: " << tableName << " does not name a table in the database\n";
            return;
        }

        // Check if the specified column exists in the table
        const Table& table = tableIt->second;
        auto colIndex = find(table.columnNames.begin(), table.columnNames.end(), colName);
        if (colIndex == table.columnNames.end()) {
            cout << "Error during GENERATE>: " << colName << " does not name a column in " << tableName << endl;
            return;
        }

        // Create the index based on the specified type
        if (indexType == "hash") {
            // Create a hash index
            Index newIndex;
            newIndex.indexType = indexType;
            newIndex.columnName = colName;
            // Populate the hash index
            for (size_t i = 0; i < table.data.size(); ++i) {
                const string& key = table.data[i][static_cast<size_t>(distance(table.columnNames.begin(), colIndex))];
                newIndex.hashIndex[key].push_back(i);
            }
            // Update or insert the index
            indices[tableName] = newIndex;

            cout << "Created " << indexType << " index for table " << tableName << " on column " << colName << ", with "
                << newIndex.hashIndex.size() << " distinct keys" << endl;
        } else if (indexType == "bst") {
            // Create a bst index
            // Implement the code for bst index creation here (using std::map<>)
            cout << "BST index creation is not implemented yet" << endl;
        } else { // not sure if this is needed
            // Invalid index type
            cout << "Invalid index type: " << indexType << endl;
        }
    }

    Database(Options& opt) :
    o(opt) {}

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
    
    ios_base::sync_with_stdio(false);

    Database b(opt);
    b.processCommands();
    return 0;
}