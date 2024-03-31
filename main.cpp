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
#include "database.h"

using namespace std;


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

    ios_base::sync_with_stdio(false);
    cin >> std::boolalpha;  // add these two lines
    cout << std::boolalpha;

    Options opt;
    getMode(argc, argv, opt);


    Database b(opt);
    b.processCommands();
    return 0;
}