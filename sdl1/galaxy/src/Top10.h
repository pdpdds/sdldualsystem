#ifndef TOP10_H_
#define TOP10_H_

//Includes
#include 	"Globals.h"				//Use of the globals variables of the game
#include 	"SDL.h"				//For SDL access
#include 	<iostream>				//cout use for debug
#include 	<fstream> 				//To load files
#include 	<list>					//For List use

//The node definition
class Score {
	
public:

	//Attributes
	string name;
	unsigned int score;
	unsigned int kills;
	
	//Constructor - destructor
	Score(string param_name, unsigned int param_score, unsigned int param_kills) {
		name = param_name;
		score = param_score;
		kills = param_kills;
	}
	
	//For some reason that doesn't work to sort the list :S
	bool operator < (const Score& score_object) {
    	return score_object.score < score;
  	}
};

class Top10 {
	
public:

	//Attributes
	
	//Constructor - destructor
	Top10();
	virtual ~Top10();
	
	//Methods
	bool NewScore(unsigned int score);
	void AddScore(string name, unsigned int score, unsigned int kills);
	list<Score>* GetScores();
		
private:

	//Attributes
	list<Score> scores;
	
	//Methods
	void LoadScores();
	void SaveScores();
	void PrintScores();

};

#endif /*TOP10_H_*/
