#include "tree.h"
#include <hc_list_helpers.h>

using std::string;
using HaverfordCS::list;
using HaverfordCS::ez_list;

Tree::Tree(string s) : val(s)
{
}

Tree::Tree(string s, list<Tree> children): val(s), kids(children)
{
}


bool	 Tree::isLeaf()
{
	return empty(kids);
}

string   Tree::value()
{
	return val;
}

list<Tree> Tree::children()
{
	return kids;
}

static Tree getIthTree(int n, list<Tree> l)
{
	if (n==0)
		return head(l);
	else
		return getIthTree(n-1, rest(l));
}
Tree Tree::ithChild(int i)
{
	precondition(0 <= 0 and i < length(kids));
	return getIthTree(i, kids);
}




static int maxChildHeight(list<Tree> kids)
{
	if (empty(kids))
		return 0;
	else {
		int htHead = height(head(kids));
		int htRest = maxChildHeight(rest(kids));
		if (htHead > htRest)
			return htHead;
		else
			return htRest;
	}
}

int height(Tree t)
{
	if (t.isLeaf())
		return 1;
	else
		return 1 + maxChildHeight(t.children());
}


// This whole thing with helper functions is getting kind of tedious ...
//   a nicer way is to have 'map' and 'reduce' functions for lists,
//   and then write all of these things as map or reduce operations on trees and lists.
// However, the key is written in the easiest-to-figure out style.

static int totalChildSizes(list<Tree> kids)
{
	if (empty(kids)) {
		return 0;
	} else {
		return size(head(kids)) + totalChildSizes(rest(kids));
	}
}

int size(Tree t)
{
	if (t.isLeaf())
		return 1;
	else
		return 1 + totalChildSizes(t.children());
}


static bool listsOfTreesAreEqual(list<Tree> l1, list<Tree> l2)
{
	if (length(l1) != length(l2)) {
		return false;
	} else if (length(l1) == 0) {
		return true;
	} else {
		return (treesAreEqual(head(l1), head(l2)) and
				listsOfTreesAreEqual(rest(l1), rest(l2)));
	}
}

bool treesAreEqual(Tree t1, Tree t2)
{
	return t1.value() == t2.value() and listsOfTreesAreEqual(t1.children(), t2.children());
}

bool operator==(Tree t1, Tree t2) { return treesAreEqual(t1, t2); }




static list<Tree> mapKids(list<Tree> oldList,
			  std::function<std::string (std::string)> func)
{
	if (empty(oldList)) {
		return list<Tree>();
	} else {
		return list<Tree>(map(head(oldList), func),
				  mapKids(rest(oldList), func));
	}
}

Tree map(Tree t, std::function<std::string (std::string)> func)
{
	return Tree(func(t.value()), mapKids(t.children(), func));
}


std::string reduce(Tree t,
		   std::function<std::string (std::string rootValue,
					      HaverfordCS::list<std::string> childReductions)> func);

static list<string> reduceKids(list<Tree> oldList,
			       std::function<std::string (std::string rootValue,
							  HaverfordCS::list<std::string> childReductions)> func)
{
	if (empty(oldList)) {
		return list<string>();
	} else {
		return list<string>(reduce(head(oldList), func),
				    reduceKids(rest(oldList), func));
	}
}

std::string reduce(Tree t,
		   std::function<std::string (std::string rootValue,
					      HaverfordCS::list<std::string> childReductions)> func)
{
	if (t.isLeaf()) {
		return t.value();
	} else {
		return func(t.value(), reduceKids(t.children(), func));
	}
}
