// Project Identifier: C0F4DFE8B340D81183C208F70F9D2D797908754D

#include <getopt.h>
#include <iostream>
#include <string>
#include <vector>
#include <limits>
#include <algorithm>
#include <deque>
#include <unordered_map>
#include <map>
#include "TableEntry.h"


using namespace std;
// preds for deleteHelper
struct predEqual {
    TableEntry val;
    size_t colIndex;
    bool operator()(const vector<TableEntry>& input) {
        return input[colIndex] == val;
    }
    predEqual(TableEntry& val1, size_t& colIndex1) : 
        val(val1),
        colIndex(colIndex1)
    {} 
};
struct predGreater {
    TableEntry val;
    size_t colIndex;
    bool operator()(const vector<TableEntry>& input) {
        return input[colIndex] > val;
    }
    predGreater(TableEntry& val1, size_t& colIndex1) : 
        val(val1),
        colIndex(colIndex1)
    {} 
    
};
struct predLess {
        TableEntry val;
        size_t colIndex;
        bool operator()(const vector<TableEntry>& input) {
            return input[colIndex] < val;
        }
        predLess(TableEntry& val1, size_t& colIndex1) : 
            val(val1),
            colIndex(colIndex1)
        {} 
    };
// Functor for the '==' operator
struct EqualTo {
    TableEntry value;
    EqualTo(const TableEntry& value) : value(value) {}

    bool operator()(const TableEntry& entry) const {
        return value == entry;
    }
};

// Functor for the '>' operator
struct GreaterThan {
    TableEntry value;
    GreaterThan(const TableEntry& value) : value(value) {}

    bool operator()(const TableEntry& entry) const {
        return entry > value;
    }
};

// Functor for the '<' operator
struct LessThan {
    TableEntry value;
    LessThan(const TableEntry& value) : value(value) {}

    bool operator()(const TableEntry& entry) const {
        return entry < value;
    }
};

struct Options {
    bool isQuiet = false;
};

enum IndexType {
    NONE,
    BST,
    HASH
};

struct Index {
    IndexType indexType = NONE;
    string columnName = "";
    unordered_map<TableEntry, vector<size_t>> hashIndex; // Mapping from column value to row indices
    map<TableEntry, vector<size_t>> bstIndex;
};

struct Table {
    string name;
    vector<string> columnNames;
    vector<EntryType> columnTypes; // use entry type enum
    vector<vector<TableEntry>> data;
    Index index; // string column name
    unordered_map<string, size_t> colNameIndex;
};

class Database {
private:
    unordered_map<string, Table> tables;
    Options o;
public:

    void processCommands() {
        string commandType;
        cin >> commandType;
        do {
            vector<string> tokens;
            if (cin.fail()) {
                cout << "Error: Reading from cin has failed\n";
                return;
            }
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
                    string lhs;
                    char opp;
                    string rhs;
                    cin >> lhs >> opp >> rhs;
                    printFromWhere(tableName, tokens, lhs, opp, rhs);
                } else { // all
                    printFromAll(tableName, tokens);
                }
                break;
            }
            case 'I': {
                cout << "% ";
                // Call insertInto method with appropriate arguments
                string tableName;
                int numRows;
                string token;
                cin >> token >> tableName >> numRows >> token;
                insertInto(tableName, numRows);
                break;
            }
            case 'D': {
                cout << "% ";
                // Call deleteFrom method with appropriate arguments
                string tableName, colName,value, keyword;
                char op;
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
                cout << "Error: unknown command line option" << endl;
                string junk;
                getline(cin, junk);
                break;
            }
            }
            tokens.clear();
        } while (cin >> commandType && commandType != "QUIT");
        quit();
    }
   
    void create(const string& tableName, int numColumns, const vector<string>& columnSpecs) {
        // Check if the table already exists in the database
        if (tables.find(tableName) != tables.end()) {
            cout << "Error during CREATE: Cannot create already existing table " << tableName << endl;
            return;
        }
        
        // Create a new table and add it to the database
        Table newTable;
        newTable.name = tableName;
         // Iterate through the command vector to populate column types and names
        for (size_t i = 0; i < columnSpecs.size(); ++i) {
            if (i < static_cast<size_t>(numColumns)) {
                
                // Populate column types
                switch (columnSpecs[i][0])
                {
                case 'b': newTable.columnTypes.push_back(EntryType::Bool); break;
                case 'd': newTable.columnTypes.push_back(EntryType::Double); break;
                case 'i': newTable.columnTypes.push_back(EntryType::Int); break;
                case 's': newTable.columnTypes.push_back(EntryType::String); break;
                default: break;
                }
            } else {
                // Populate column names
                newTable.columnNames.push_back(columnSpecs[i]);
                newTable.colNameIndex[columnSpecs[i]] = i - static_cast<size_t>(numColumns);
            }
        }

        tables.emplace(tableName, move(newTable));

        // Print confirmation message
        cout << "New table " << tableName << " with column(s)";
        for (const string& columnName : tables[tableName].columnNames) {
            cout << " " << columnName;
        }
        cout << " created\n";
    }

    // Method to insert new rows into a specified table
    void insertInto(const string& tableName, int numRows) {
        // Find the table
        auto tableIt = tables.find(tableName);
        if (tableIt == tables.end()) {
            cout << "Error during INSERT: " << tableName  << " does not name a table in the database\n";
            // Table not found
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
            vector<TableEntry> values;
            values.reserve(table.columnNames.size());
            for (size_t j = 0; j < table.columnNames.size(); j++) {
                switch (table.columnTypes[j])
                {
                case EntryType::Bool:
                {
                    string in;
                    bool val;
                    cin >> in;
                    if (in == "true") val = true;
                    else val = false;
                    values.push_back(TableEntry(val));
                    break;
                }
                case EntryType::Double:
                {
                    double val;
                    cin >> val;
                    values.push_back(TableEntry(val));
                    break;
                }
                case EntryType::Int:
                {
                    int val;
                    cin >> val;
                    values.push_back(TableEntry(val));
                    break;
                }
                case EntryType::String :
                {
                    string val;
                    cin >> val;
                    values.push_back(TableEntry(val));
                    break;
                }
                default:
                    break;
                }
                if (table.columnNames[j] == table.index.columnName) {
                    switch (table.index.indexType)
                    {
                    case IndexType::HASH:
                        table.index.hashIndex[values[j]].push_back(startIndex + static_cast<size_t>(i));
                        break;
                    case IndexType::BST:
                        table.index.bstIndex[values[j]].push_back(startIndex + static_cast<size_t>(i));
                        break;
                    default:
                        break;
                    }
                }
            }
            table.data.push_back(values);
        }


        // might have runtime errors for pushing back to vecotr that does not exist
        size_t endIndex = table.data.size() - 1; // Index of the last row added

        // Print confirmation message
        cout << "Added " << numRows << " rows to " << tableName << " from position " << startIndex << " to " << endIndex << endl;
    }

    // Method to delete rows from the specified table based on conditions
    void deleteFrom(const string& tableName, const string& columnName, const char& op, string& filterValueString) {
        // Find the table
        int deletedRows = 0;
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
        EntryType valueType = table.columnTypes[columnIndex];
        // TableEntry *valueEntry = makeEntry(filterValueString, valueType);
        if (valueType == EntryType::String) {
            TableEntry valueEntry(filterValueString);
            deletedRows = deleteHelper(valueEntry, table, columnIndex, op);
        }
        else if (valueType == EntryType::Int) {
            TableEntry valueEntry(stoi(filterValueString));
            deletedRows = deleteHelper(valueEntry, table, columnIndex, op);
        }
        else if (valueType == EntryType::Bool) {
            TableEntry valueEntry(stringToBool(filterValueString));
            deletedRows = deleteHelper(valueEntry, table, columnIndex, op);

        }
        else if (valueType == EntryType::Double) {
            TableEntry valueEntry(stod(filterValueString));
            deletedRows = deleteHelper(valueEntry, table, columnIndex, op);
        }

        // regenerate index
        switch (table.index.indexType)
        {
        case IndexType::BST:
            generateIndex(table.name, "bdelete", table.index.columnName);
            break;
        case IndexType::HASH:
            generateIndex(table.name, "hdelete", table.index.columnName);
        default:
            break;
        }

        cout << "Deleted " << deletedRows << " rows from " << tableName << endl;

    }
    // Method to quit the program and clean up all data
    void quit() {
        // Clean up all internal data
        tables.clear();

        // Print goodbye message
        cout << "% Thanks for being silly!" << endl;
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
        // Do I need to do this?
        // Print confirmation message
        cout << "Table " << tableName << " removed" << endl;
    }
    
    void printFromWhere(const string& tableName, vector<string>& printColumns, const string& filterColumn, char& opp, string& filterValueString) {
        int numMatches = 0;
        // Find the table
        auto tableIt = tables.find(tableName);

        const Table& table = tableIt->second;
        
        // error checking
        if (tableIt == tables.end()) {
            // Table not found
            cout << "Error during PRINT: " << tableName  << " does not name a table in the database\n";
            return;
        }

        auto filterColIt = find(table.columnNames.begin(), table.columnNames.end(), filterColumn);
        // TODO: Write a test case for this
        // error checking for WHERE statement
        if (filterColIt == table.columnNames.end()) {
            cout << "Error during PRINT: " << filterColumn << " does not name a column in " << tableName << endl;
            return;
        }
        for (const auto& columnName : printColumns) {
            auto colItCheck = find(table.columnNames.begin(), table.columnNames.end(), columnName);
            if (colItCheck == table.columnNames.end()) {
                cout << "Error during PRINT: " << columnName << " does not name a column in " << tableName << endl;
                cout << "Printed 0 matching rows from " << tableName << endl;
                return;
            }
        }
        // Print the column names if !quietMode
        if (!o.isQuiet) {
            for (const auto& columnName : printColumns) {
                cout << columnName << " ";
            }
            cout << endl;
        }

        // Find the index of the column to be compared
        // auto colIndex = find(table.columnNames.begin(), table.columnNames.end(), lhs);
        size_t colIndex = static_cast<size_t>(distance(table.columnNames.begin(), filterColIt));
        EntryType valueType = table.columnTypes[colIndex];

        if (valueType == EntryType::String) {
            TableEntry valueEntry(filterValueString);
            numMatches = printFromHelper(filterColumn, table, printColumns, valueEntry, opp, colIndex);
        }
        else if (valueType == EntryType::Int) {
            TableEntry valueEntry(stoi(filterValueString));
            numMatches = printFromHelper(filterColumn, table, printColumns, valueEntry, opp, colIndex);
        }
        else if (valueType == EntryType::Bool) {
            TableEntry valueEntry(stringToBool(filterValueString));
            numMatches = printFromHelper(filterColumn, table, printColumns, valueEntry, opp, colIndex);

        }
        else if (valueType == EntryType::Double) {
            TableEntry valueEntry(stod(filterValueString));
            numMatches = printFromHelper(filterColumn, table, printColumns, valueEntry, opp, colIndex);
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
                    const auto& index = static_cast<size_t>(distance(table.columnNames.begin(), colIndex));
                    // const TableEntry& value = row[index];
                    cout << row[index] << " ";
                }
            }
            if (!o.isQuiet) cout << endl;
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

        // Print the column names
        if (!o.isQuiet) {
            for (const auto& col : printColumns) {
                cout << col.second << " ";
            }
            cout << endl;
        }
        
        // Iterate through the first table
        for (const auto& row1 : table1.data) {
            // Find the matching rows in the second table
            for (const auto& row2 : table2.data) {
                if (row1[static_cast<size_t>(distance(table1.columnNames.begin(), colIndex1))] == row2[static_cast<size_t>(distance(table2.columnNames.begin(), colIndex2))]) {
                    // Print the values from the selected columns for each matching row
                    numPrinted++;
                    if (o.isQuiet) continue;
                    for (const auto& col : printColumns) {
                        if (col.first == 1) {
                            // find column name in the table number specified in printColumns
                            auto idx = find(table1.columnNames.begin(), table1.columnNames.end(), col.second);
                            if (idx != table1.columnNames.end()) {
                                auto val = static_cast<size_t>(distance(table1.columnNames.begin(), idx));
                                cout << row1[val] << " ";
                                // cout << row1[static_cast<size_t>(distance(table1.columnNames.begin(), idx))] << " ";

                            } else {
                                cout << "Error during JOIN: " << col.second << " does not name a column in " << tableName1 << endl;
                                return;
                            }
                        } else if (col.first == 2) {
                            auto idx = find(table2.columnNames.begin(), table2.columnNames.end(), col.second);
                            if (idx != table2.columnNames.end()) {
                                auto val = static_cast<size_t>(distance(table2.columnNames.begin(), idx));
                                cout << row2[val] << " ";

                            } else {
                                cout << "Error during JOIN: " << col.second << " does not name a column in " << tableName2 << endl;
                                return;
                            }
                        }
                    }
                    cout << endl;
                }
            }
        }

        // Iterate through the first table <tablename1> from beginning to end.
        // For each row’s respective <colname1> value in <tablename1>, find matching <colname2> values in <tablename2>, if any exist.
        // For each match found, print the column values in the matching rows in the order specified by the JOIN command.
        // The matching rows from the second table must be selected in the order of insertion into that table.
        // If no rows in the second table match a row in the first table, that row is ignored from the join.


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
        Table& table = tableIt->second;
        auto colIndex = find(table.columnNames.begin(), table.columnNames.end(), colName);
        if (colIndex == table.columnNames.end()) {
            cout << "Error during GENERATE>: " << colName << " does not name a column in " << tableName << endl;
            return;
        }

        table.index.bstIndex.clear();
        table.index.hashIndex.clear();
        table.index.columnName = colName;
        
        switch (indexType.at(0))
        {
        case 'h':
            table.index.indexType = IndexType::HASH;
            for (size_t i = 0; i < table.data.size(); i++) {
                table.index.hashIndex[(table.data[i][table.colNameIndex[table.index.columnName]])].push_back(i);
            }
            break;
        case 'b':
            table.index.indexType = IndexType::BST;
            for (size_t i = 0; i < table.data.size(); i++) {
                table.index.bstIndex[(table.data[i][table.colNameIndex[table.index.columnName]])].push_back(i);
            }
            break;
        default:
            break;
        
        }
        if (indexType.at(1) != 'd') cout << "Created " << indexType << " index for table " << tableName << " on column " << colName << ", with " << max(table.index.hashIndex.size(), table.index.bstIndex.size()) << " distinct keys" << endl;
    }

    bool stringToBool(string val) {
        return val == "true";
    }

    int deleteHelper(TableEntry& valueEntry, Table& table, size_t columnIndex, const char& op) {
        // Delete rows based on conditions
        int size = static_cast<int>(table.data.size());
        for (size_t i = 0; i < table.data.size(); i++) {
            bool deleteRow = false;
            vector<TableEntry> row = table.data[i];
            switch (op)
            {
            case '=':
                deleteRow = EqualTo(row[columnIndex])(valueEntry);
                break;
            case '<':
                deleteRow = GreaterThan(row[columnIndex])(valueEntry);
                break;
            case '>':
                deleteRow = LessThan(row[columnIndex])(valueEntry);
                break;
            default:
                break;
            }
            // deleteRow = operatorCheck(row, columnIndex, valueEntry, op);
            if (deleteRow) {
                if (op == '=') {
                    predEqual pred(valueEntry, columnIndex);
                    auto remove_begin = remove_if(table.data.begin(), table.data.end(), pred);
                    table.data.erase(remove_begin, table.data.end());
                }
                else if (op == '>') {
                    predGreater pred(valueEntry, columnIndex);
                    auto remove_begin = remove_if(table.data.begin(), table.data.end(), pred);
                    table.data.erase(remove_begin, table.data.end());
                }
                else if (op == '<') {
                    predLess pred(valueEntry, columnIndex);
                    auto remove_begin = remove_if(table.data.begin(), table.data.end(), pred);
                    table.data.erase(remove_begin, table.data.end());
                }
            }
        }
        // Print summary
        // removeif
        return (size - static_cast<int>(table.data.size()));
    }

    int printFromHelper(const string& filterColumn, const Table& table, vector<string>& printColumns, TableEntry& valueEntry, char& opp, size_t& colIndex) {
        // BST
        int numMatches = 0;
        // if (table.index.indexType == IndexType::BST) cout << "\n\nBST\n\n"; // testing
        if (table.index.indexType == IndexType::BST && table.index.columnName == filterColumn) {

            if (opp == '=') {
                auto bstItChecker = table.index.bstIndex.find(valueEntry);
                if (bstItChecker != table.index.bstIndex.end()) {
                    for (size_t i = 0; i < (table.index.bstIndex).at(valueEntry).size(); i++) {
                        for (const auto& columnName : printColumns) {
                            size_t colIdx = table.colNameIndex.at(columnName);
                            if (!o.isQuiet) {
                                cout << table.data[table.index.bstIndex.at(valueEntry)[i]][colIdx] << " ";
                            }
                        }
                        ++numMatches;
                        if (!o.isQuiet) cout << '\n';
                    }
                }
            }
            else if (opp == '<') {
                for (auto bstIt = table.index.bstIndex.begin(); bstIt != table.index.bstIndex.lower_bound(valueEntry); bstIt++) {
                    for (size_t i = 0; i < (*bstIt).second.size(); i++) {
                        for (const auto& columnName : printColumns) {
                            size_t colIdx = table.colNameIndex.at(columnName);
                            if (!o.isQuiet) {
                                cout << table.data[(*bstIt).second[i]][colIdx] << " ";
                            }
                        }
                        ++numMatches;
                        if (!o.isQuiet) cout << '\n';
                    }
                }
            }
            else if (opp == '>') {
                // check 
                for (auto bstIt = table.index.bstIndex.upper_bound(valueEntry); bstIt != table.index.bstIndex.end(); bstIt++) {
                    for (size_t i = 0; i < (*bstIt).second.size(); i++) {
                        for (const auto& columnName : printColumns) {
                            size_t colIdx = table.colNameIndex.at(columnName);
                            if (!o.isQuiet) {
                                cout << table.data[(*bstIt).second[i]][colIdx] << " ";
                            }
                        }
                        ++numMatches;
                        if (!o.isQuiet) cout << '\n';
                    }
                }
            }

        }
        else { // if IndexType is HASH or NONE, prints in insertion order
            // Print the values from the selected columns for each row
            // vector<TableEntry, row, size_t& colIndex, TableEntry* valueEntry, const char& opp
            for (const auto& row : table.data) {
                bool print = false;
                // const TableEntry& entry = row[colIndex];
                switch (opp)
                {
                case '=':
                    print = EqualTo(row[colIndex])(valueEntry);
                    break;
                case '<':
                    print = GreaterThan(row[colIndex])(valueEntry);
                    break;
                case '>':
                    print = LessThan(row[colIndex])(valueEntry);
                    break;
                default:
                    break;
                }
                // print = operatorCheck(row, colIndex, valueEntry, opp);
                if (print) {
                    // cout << "COMPARED: " << row[colIndex] << " AND " << *valueEntry << endl;
                    // Print the values from the selected columns for this row
                    for (const auto& columnName : printColumns) {
                        auto colIndex = find(table.columnNames.begin(), table.columnNames.end(), columnName);
                        if (!o.isQuiet) {
                            cout << row[static_cast<size_t>(distance(table.columnNames.begin(), colIndex))] << " ";
                        }
                    }
                    if (!o.isQuiet) cout << endl;
                    numMatches++;
                }
            }
        }
        return numMatches;
    }
    
    Database(Options& opt) :
    o(opt) {}

};