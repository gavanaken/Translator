#include "scanner.h"
#include "scannerDemo.h"
#include <iostream>

using namespace std;

void scannerDemo()
{
	while (getNextToken(), tokenAvailable()) {
		cout << "Token number " << tokenNumber() << " is ";
		cout << "'" << currentToken() << "'";
		cout << ", which is token kind " << currentTokenKind() << endl;
	}
	cout << "Reached end-of-file, now stopping!" << endl;
}
