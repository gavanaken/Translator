#ifndef TREE_H_
#define TREE_H_

#include <string>
#include <hc_list.h>

class Tree {
	public:
		explicit Tree(std::string s);  // singleton tree (leaf)
				 Tree(std::string s, HaverfordCS::list<Tree> children);
				 
		// predicates/accessors:

		bool	 isLeaf();

		std::string value();
		HaverfordCS::list<Tree> children();
		Tree ithChild(int i);

	private:
		std::string val;
		HaverfordCS::list<Tree> kids;
};

int height(Tree t);
int size(Tree t);

bool treesAreEqual(Tree t1, Tree t2); // could be called "operator==", since this is C++
bool operator==(Tree T1, Tree T2); // just call the above function


#include <functional>

Tree        map(Tree t, std::function<std::string (std::string)> func);
std::string reduce(Tree t,
		   std::function<std::string (std::string rootValue,
					      HaverfordCS::list<std::string> childReductions)> func);

// just for convenience:

Tree ezTree(std::string s, Tree t1);
Tree ezTree(std::string s, Tree t1, Tree t2);
Tree ezTree(std::string s, Tree t1, Tree t2, Tree t3);
Tree ezTree(std::string s, Tree t1, Tree t2, Tree t3, Tree t4);

#endif /*TREE_H_*/
