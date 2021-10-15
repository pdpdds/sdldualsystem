//Includes
#include	 "Scene.h"

Scene::Scene(unsigned int param_second, string param_hero_ship_hash, string param_fleet_hash, string param_mothership_hash) {
	
	second = param_second;
	hero_ship_hash = param_hero_ship_hash;
	fleet_hash = param_fleet_hash;
	mothership_hash = param_mothership_hash;
}

Scene::~Scene() {
	//printf("Scene deleted:%i\n", second);
}

void Scene::Show() {
	/*cout << "***********************************************************" << endl;
	cout << "Reporte de Tabla Hash" << endl;
	cout << "Second/Id:\t" << second << endl;
	cout << "Hero Hash:\t" << hero_ship_hash << endl;
	cout << "Fleet Hash:\t" << fleet_hash << endl;
	int n = 1;
	for(list<string>::iterator invader = invaders.begin(); invader != invaders.end(); invader++) {
		
		cout << "\t" << n << " Invader Hash:\t" << (*invader) << endl;
		n++;
	}
	n = 1;
	for(list<string>::iterator kamikaze = kamikazes.begin(); kamikaze != kamikazes.end(); kamikaze++) {
		
		cout << "\t" << n << " Kamikaze Hash:\t" << (*kamikaze) << endl;
		n++;
	}
	n = 1;
	for(list<string>::iterator object = objects.begin(); object != objects.end(); object++) {
		
		cout << "\t" << n << " Object Hash:\t" << (*object) << endl;
		n++;
	}
	cout << "Mothership Hash:\t" << mothership_hash << endl;
	cout << "***********************************************************" << endl << endl;*/	
}

void Scene::AddInvader(string hash) {
	invaders.push_back(hash);
}

void Scene::AddKamikaze(string hash) {
	kamikazes.push_back(hash);
}

void Scene::AddObject(string hash) {
	objects.push_back(hash);
}

unsigned int Scene::GetSecond() {
	return second;
}

string Scene::GetHeroShipHash() {
	return hero_ship_hash;
}

string Scene::GetFleetHash() {
	return fleet_hash;
}

string Scene::GetMothershipHash() {
	return mothership_hash;
}

list<string>* Scene::GetInvaders() {
	return &invaders;
}

list<string>* Scene::GetKamikazes() {
	return &kamikazes;
}

list<string>* Scene::GetObjects() {
	return &objects;
}
