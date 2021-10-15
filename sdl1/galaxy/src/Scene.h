#ifndef SCENE_H_
#define SCENE_H_

//Includes
#include 	<string>				//For string use
#include 	<list>					//For list use
#include 	<iostream>				//cout used for debug

using namespace std;

class Scene {
	
public:

	//Attributes
	
	//Constructor - destructor
	Scene(unsigned int param_second, string param_hero_ship_hash, string param_fleet_hash, string param_mothership_hash);
	virtual ~Scene();
	
	//Methods
	void Show();
	void AddInvader(string hash);
	void AddKamikaze(string hash);
	void AddObject(string hash);
	//Getters
	unsigned int GetSecond();
	string GetHeroShipHash();
	string GetFleetHash();
	string GetMothershipHash();
	list<string>* GetInvaders();
	list<string>* GetKamikazes();
	list<string>* GetObjects();
	
private:

	//Attributtes
	unsigned int second;
	string hero_ship_hash;
	string fleet_hash;
	string mothership_hash;
	list<string> invaders;
	list<string> kamikazes;
	list<string> objects;
	
	//Methods

};

#endif /*SCENE_H_*/
