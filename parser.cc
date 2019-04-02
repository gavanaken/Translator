#include <iostream>
#include <cstdlib>  // for 'exit' function to abandon the program
#include <hc_list_helpers.h>
#include "parser.h"
#include "scanner.h"

using std::string;
using HaverfordCS::list;
using HaverfordCS::ez_list;
using std::cout;
using std::cerr;
using std::endl;


// PART 2a ANSWER:
// match this:
// START -> E   <followed by end-of-input> *PREDICT : <EOF>
// E -> number *PREDICT : INT
// E -> identifier *PREDICT : IDENTIFIER
// E -> ( E_IN_PARENS ) *PREDICT : LPAREN
// E_IN_PARENS -> OP E E *PREDICT : FIRST_OP
// E_IN_PARENS -> Minus EAndE2 *PREDICT : FIRST_MINUS
// Minus -> - *PREDICT: MINUS
// OP -> +|* *PREDICT : PLUS | TIMES
// EAndE2 -> E E *PREDICT : FIRST_E
// EAndE2 -> E *PREDICT : FIRST_E

// PART 3 ANSWER:
// match this:
// START -> E   <followed by end-of-input> *PREDICT : <EOF>
// E -> number *PREDICT : INT
// E -> identifier *PREDICT : IDENTIFIER
// E -> ( E_IN_PARENS ) *PREDICT : LPAREN
// E_IN_PARENS -> OP E Es *PREDICT : FIRST_OP
// E_IN_PARENS -> MINUS EAndE2 *PREDICT : FIRST_MINUS
// Minus -> - *PREDICT : MINUS
// OP -> +|-|* *PREDICT : PLUS | TIMES
// Es -> E *PREDICT : FIRST_E
// Es -> E Es *PREDICT : FIRST_E
// EAndE2 -> E E_2 *PREDICT : FIRST_E
// E_2 -> E *PREDICT : FIRST_E
// E_2 -> *PREDICT : ""

// Declare all functions, so they can call each other in any order
//  (note: "static" means it's restricted to this scope, e.g. this file
static Tree matchE();
static Tree matchEInParens();
static string matchOp();
static string matchMinus();
static list<Tree> matchEAndE2();
static list<Tree> matchEs(list<Tree> Es);


// FIRST and FOLLOW sets for those,
//  built with lists rather than sets, for familiarity
//  defined in terms of kindOfToken from scanner-regexp.h
static list<kindOfToken> FIRST_OP  = ez_list(PLUS, TIMES);
static list<kindOfToken> FIRST_MINUS  = ez_list(MINUS);
static list<kindOfToken> FIRST_EIP = ez_list(PLUS, MINUS, TIMES);
static list<kindOfToken> FIRST_E   = ez_list(INT, IDENTIFIER, LPAREN);
static list<kindOfToken> FIRST_EAndE2   = ez_list(INT, IDENTIFIER, LPAREN);
static list<kindOfToken> FIRST_E_2   = ez_list(INT, IDENTIFIER, LPAREN); //NULLABLE

static list<kindOfToken> FOLLOW_OP  = FIRST_E;
static list<kindOfToken> FOLLOW_EIP = ez_list(RPAREN);
static list<kindOfToken> FOLLOW_E   = ez_list(LPAREN, INT, END_OF_INPUT); //$
static list<kindOfToken> FOLLOW_EAndE2   = ez_list(RPAREN);
static list<kindOfToken> FOLLOW_E_2 = ez_list(RPAREN);

// first, a helpful function ---
//    we call this when we need to get more input,
//    i.e. when we want to move current_token along and end-of-input would be an error

void mustGetNextToken()
{
	getNextToken();
	if (!tokenAvailable()) {
		cerr << "unexpected end of input at token #" << tokenNumber() << endl;
		exit(5);
	}
}

// match a literal, assuming the token HAS been scanned already,
//  i.e. that "currentToken" is _on_ the literal we wish to match
// leave "currentToken" on the very last token of the matched pattern
static void confirmLiteral(string what)
{
	if (currentToken() != what) {
		cerr << "got " << currentToken() << " instead of " << what << " at token #" << tokenNumber() << endl;
		exit(2);
	}	
}


// match an "E", i.e, anything on the right hand side of any "E-->..." production
//  assume the first token of the E has been scanned
//   (i.e., assuming currentToken is the first token of the "E" we're matching)
//  leave "currentToken" on the very last token of the matched pattern
static Tree matchE()
{
	if (currentTokenKind() == INT) {		// check to see if we're at the start of "E -> number"
	        return Tree(currentToken());	// we're already on the number, which is last token of the to-be-matched pattern "E"
	} else if (currentTokenKind() == IDENTIFIER) {	// check to see if we're at the start of "E -> identifier"
	    	return Tree(currentToken());	// we're already on the id, which is last token of the to-be-matched pattern "E"
	} else if (currentTokenKind() == LPAREN) {	// check to see if we're at the start of "E -> ( E_IN_PARENS )"
		confirmLiteral("(");
		mustGetNextToken();  // advance current token to be the operator, or fail if there isn't one
		if (currentTokenKind() == PLUS or currentTokenKind() == TIMES)
		{
			Tree it = matchEInParens();
			// if that left off at the end of the E_IN_PARENS, we still need a ")" in the E we're matching
			confirmLiteral(")");
			return it;
		}
		else
		{
			assert(currentTokenKind() == MINUS);
			Tree it = matchEInParens();
			confirmLiteral(")");
			return it;
		}
	} else {				// if we're not in either of those "E -> " productions, we're in trouble :-(
		std::cerr << "Illegal token (" << currentToken() << ") at token #" << tokenNumber() << endl;
		exit(3);
	}
}


// match an "E_IN_PARENS",
//  assuming that the currentToken is at the start of the E_IN_PARENS, e.g. a "+"
//  leave the currentToken at the last part of what was matched,
//  i.e. *before* the ")" that should come after the E_IN_PARENS
static Tree matchEInParens() {
	if (find(currentTokenKind(), FIRST_OP)) {
		string theOp = matchOp();
		mustGetNextToken(); // get the first of the first E
		Tree firstChild = matchE();
		mustGetNextToken();  // get to first of the second E
		Tree secondChild = matchE();
		return Tree(theOp, matchEs(ez_list(firstChild, secondChild)));
	} else if (find(currentTokenKind(), FIRST_MINUS)) {
		string theMinus = matchMinus();
		mustGetNextToken();
		list<Tree> children = matchEAndE2();
		if (head(rest(children)).value() != "") //note this nested if is okay because we are just building the AST
		{										// with "-" if we are doing sub and matchMinus() if we are doing negate
			theMinus = "-";
		}
		return Tree(theMinus, children);
	}
	else {
		std::cerr << "Illegal token (" << currentToken() << ") at token #" << tokenNumber() << endl;
		exit(3);
	}
}

// Matches an E first, and then might go on to match another, or we are doing negation
// We leave when we hit a RPAREN
static list<Tree> matchEAndE2() {
	Tree firstChild = matchE(); // Always E first
	mustGetNextToken();

	if (find(currentTokenKind(), FIRST_E)) {
		Tree secondChild = matchE();
		mustGetNextToken();
		return ez_list(firstChild, secondChild);
	}
	else { // there is no second E! we are doing negation
		assert(currentTokenKind() == RPAREN);
		Tree secondChild = Tree("");
		return ez_list(firstChild, secondChild);
	}
}


// match an operator, assuming that it is the currentToken
//  leave the currentToken at the last part of what was matched, i.e. unchanged
static string matchOp()
{
	// could do three cases here, but that's so tedious...
	assert (find(currentTokenKind(), FIRST_OP));
	return currentToken();
}

// match a minus, useful only for negation
// consumes the currentToken in this case, confirming a "-"
static string matchMinus()
{
	assert (find(currentTokenKind(), FIRST_MINUS));
	confirmLiteral("-");
	return "negate";
}

// match the Es, returning a list of Trees which are used in matchEINParens
// each matched thing is an E
// we leave when we hit a RPAREN
static list<Tree> matchEs(list<Tree> Es)
{
	mustGetNextToken();

	if (find(currentTokenKind(), FOLLOW_EIP))
		{
			return Es;
		}

	// Note that these while loops are allowed because they are just building up the AST... we are not matching anything
	else
	{
		assert(find(currentTokenKind(), FIRST_E));
		list<Tree> newEs = list<Tree>(matchE(), list<Tree>());
		list<Tree> tempEs = list<Tree>();
		while(!empty(Es))
		{
			tempEs = list<Tree>(head(Es), tempEs);
			Es = rest(Es);
		}
		while(!empty(tempEs))
		{
			newEs = list<Tree>(head(tempEs), newEs);
			tempEs = rest(tempEs);
		}
		return matchEs(newEs);
	}

}

translatedResult matchStartSymbolAndEOF()
{
	getNextToken();
	if (!tokenAvailable()) {
		cerr << "Illegal end of input" << endl;
		exit(2);
	}

	Tree fullExpression = Tree(matchE());

	// now make sure there isn't anything else!
	getNextToken();
	if (tokenAvailable()) {
		cerr << "Warning: extra input after end: " << currentToken() << endl;
		exit (1);
	}

	return fullExpression;
}
