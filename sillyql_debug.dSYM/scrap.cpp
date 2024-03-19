// if (commandType == "CREATE") {
            //     cout << "% ";
            //     // Call create method with appropriate arguments
            //     string token;
            //     string tableName;
            //     int numCols;
            //     cin >> tableName;
            //     cin >> numCols;
            //     for (int i = 0; i < numCols*2; i++) {
            //         cin >> token;
            //         tokens.push_back(token);
            //     }
            //     create(tableName, numCols, tokens);

            // } else if (commandType == "REMOVE") {
            //     cout << "% ";
    
            //     string tableName;
            //     cin >> tableName;
            //     remove(tableName);

            // } else if (commandType == "#") {
            //     cout << "% ";
            //     string junk;
            //     getline(cin, junk);

            // } else if (commandType == "PRINT") { // TODO: finish this
            // // Print the selected columns from the specified table
            //     cout << "% ";
            //     string keyword;
            //     cin >> keyword; // Read the next keyword after "PRINT"
            //     cout << "KEYWORD: " << keyword << endl;
            //     assert(keyword == "FROM");
            //     // Parse the PRINT FROM command
            //     string tableName;
            //     int numCols;

            //     cin >> tableName >> numCols; // Read table name and number of columns to print
            //     for (int i = 0; i < numCols; ++i) {
            //         string columnName;
            //         cin >> columnName; // Read each column name to print
            //         tokens.push_back(columnName);
            //     }
            //     cin >> keyword;
            //     if (keyword == "WHERE") {
            //         printFromWhere(tableName, tokens);
            //     } else { // all
            //         printFromAll(tableName, tokens);
            //     }
            //     // error handling
            //     // } else {
            //     //     // Invalid command syntax
            //     //     cout << "Invalid PRINT command syntax" << endl;
            //     // }
            // } else if (commandType == "INSERT") {
            //     cout << "% ";
            //     // Call insertInto method with appropriate arguments
            //     string tableName;
            //     int numRows;
            //     string token;
            //     cin >> token;
            //     if (token == "INTO") {
            //         cin >> tableName >> numRows >> token;
            //         insertInto(tableName, numRows);
            //     }
            // } else if (commandType == "DELETE") { // TODO: optimize
            //     cout << "% ";
            //     // Call deleteFrom method with appropriate arguments
            //     string tableName, colName, op, value, keyword;
            //     cin >> keyword >> tableName;
            //     if (keyword == "FROM") {
            //         cin >> keyword >> colName >> op >> value;
            //         if (keyword == "WHERE") {
            //             deleteFrom(tableName, colName, op, value);   
            //         }
            //     }
            // } else if (commandType == "JOIN") { // TODO: finish this
            //     cout << "% ";
            //     string tableName1, tableName2, colName1, colName2, junk;
            //     int numCols;
            //     cin >> tableName1 >> junk >> tableName2 >> junk >> colName1 >> junk >> colName2 >> junk >> junk >> numCols;
            //     vector<string> printColumns;
            //     for (int i = 0; i < numCols; ++i) {
            //         string printColName;
            //         int tableIndex;
            //         cin >> tableIndex >> printColName;
            //         printColumns.push_back(printColName);
            //         cout << "printColName: " << printColName << endl;
            //     }
            //     joinTables(tableName1, tableName2, colName1, colName2, printColumns);
            // } else {
            //     // Invalid command
            //     // cerr << "Invalid command: " << tokens[0] << tokens[1] << endl;
            //     string junk;
            //     getline(cin, junk);
            // }
    // Add methods to interact with the database, such as creating tables, inserting rows, executing queries, etc.