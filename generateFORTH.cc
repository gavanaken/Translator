#include "generateFORTH.h"
#include <hc_list_helpers.h>

using HaverfordCS::list;

using namespace std;


string makeForth(string acc, list<string> next)
{
	if (empty(rest(next)))
	{
		return head(next);
	}
	else if (acc == "negate")
	{
		return head(next) + " " + makeForth(acc, rest(next)) + acc;
	}
	else
	{
		return head(next) + " " + makeForth(acc, rest(next)) + " " + acc;
	}
}


/*std::string generateFORTH(Tree AST)
/{
	if (AST.isLeaf())
		return AST.value();
	else {
		assert(length(AST.children()) == 2); // only binary nodes and leaves OK for now
		return	generateFORTH(AST.ithChild(0)) + " " +
			generateFORTH(AST.ithChild(1)) + " " +
			AST.value();
	}
}
*/

string generateFORTH(Tree AST)
{
	return reduce(AST, makeForth);
}
