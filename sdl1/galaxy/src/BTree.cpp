//Includes
#include	"BTree.h"		//Own header file definition

BTree::BTree() {
	count = 0;
}

BTree::~BTree() {
	scenes.clear();
}

void BTree::Insert(Scene* new_scene) {
	
	scenes.push_back(*new_scene);
	count++;
	
}

Scene* BTree::Find(int index) {
	
	if((index < 0) || (index > count)) {
		return NULL;
	} else {
		
		list<Scene>::iterator iterator = scenes.begin();
		int n = 0;
		
		while(iterator != scenes.end()) {
			if(n == index) {
				return &(*iterator);
			} else {
				iterator++;
				n++;
			}
		}
		
		//This is impossible... I suppose
		return NULL;
	}
}

void BTree::Clear() {
	count = 0;
	scenes.clear();
}

int BTree::GetCount() {
	return count;
}
