#include <iostream>
#include <cstdlib>
#include "generateFORTH.h"
#include "parser.h"
#include "scannerDemo.h"

using namespace std;

int main()
{

	bool testScannerInstead = false;

	if (testScannerInstead) {
		cout << "Demonstrating lexical scanner. Enter tokens followed by <EOF>." << endl;
		scannerDemo();
	} else {
		Tree AST = matchStartSymbolAndEOF();;
		string translated = generateFORTH(AST);
		cout << translated << endl;
	}

	return 0;  // indicate successful translation if no exit with non-0 before this
}

