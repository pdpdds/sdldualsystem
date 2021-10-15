#ifndef BTREE_H_
#define BTREE_H_

//Includes
#include	"Scene.h"
#include 	<list>

using namespace std; 

class BTree {

public:

	//Attributes
	
	//Contructor - destructor
	BTree();
	virtual ~BTree();
	
	//Methods
	void Insert(Scene* new_scene);
	Scene* Find(int index);
	void Clear();
	int GetCount();
	
private:
	
	//Attributes
	int count;
	list<Scene> scenes;
	
	//Methods
	
};

#endif /*BTREE_H_*/
