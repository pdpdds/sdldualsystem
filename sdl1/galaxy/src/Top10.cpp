//Includes
#include "Top10.h"

Top10::Top10() {
	LoadScores();
}

Top10::~Top10() {
	
}

void Top10::LoadScores() {
	
	//Clear the list just in case
	scores.clear();
	
	//Open a file for reading
	ifstream scores_file(FILE_SCORES.c_str());
	
	if(scores_file.good()) {
		
		for(int n = 0; n != 10; n++) {
			
			//Temporal variables 
			string name;
			unsigned int score;
			unsigned int kills;
			
			//Load the variables
			scores_file >> name;
			scores_file >> score;
			scores_file >> kills;
			
			//Create a temporal register
			Score loaded_register(name, score, kills);
			
			//Insert the register
			scores.push_back(loaded_register);
			
			//Skip past the end of the line 
			scores_file.ignore();
		}
		
		//Close the file
		scores_file.close();
		
		//Debug
		//PrintScores();
		
		//If everything goes fine
		return;
		
	}
	
	//The default configuration
	fprintf(stderr, "Error, unable to load the scores file.\n");
	
}

bool Top10::NewScore(unsigned int score) {
	
	//Check if this score is a new high score
	for(list<Score>::const_iterator iterator = scores.begin(); iterator != scores.end(); iterator++) {
		if(score > iterator->score) {
			return true;
		}
	}
	
	return false;
} 

void Top10::AddScore(string name, unsigned int score, unsigned int kills) {
	
	//Create a temporal register
	Score new_register(name, score, kills);
			
	//Insert the register
	scores.push_back(new_register);

	//Debug :: Print the list
	//PrintScores();	//*****************************//
		
	//Sort the list
	scores.sort();
	
	//Debug :: Print the list
	//PrintScores();	//*****************************//
	
	//Removes the las element
	scores.pop_back();
	
	//Debug :: Print the list
	//PrintScores();	//*****************************//
	
	//Save the score to a file
	SaveScores();
	
}

void Top10::SaveScores() {
	
	//Open a file for writing 
	ofstream scores_file(FILE_SCORES.c_str());
	
	//Check if this score is a new high score
	for(list<Score>::const_iterator iterator = scores.begin(); iterator != scores.end(); iterator++) {
		
		//Write the data to the scores 
		scores_file << iterator->name;
		scores_file << "\t";
		scores_file << iterator->score;
		scores_file << "\t";
		scores_file << iterator->kills;
		scores_file << "\n";
		
	} 
	
	//Close the file
	scores_file.close();
}

void Top10::PrintScores() {
	//Debug :: Print the list
	int n = 0;
	list<Score>::const_iterator iterator = scores.begin();
		
	cout << "*****************************" << endl;
	
	while(iterator != scores.end()) {
		
		cout << n << " Name: " << iterator->name << endl;
		cout << n << " Score: " << iterator->score << endl;
		cout << n << " Kills: " << iterator->kills << endl;
		cout << endl;
			 	
		n++;
		iterator++;
  	}
  	
  	cout << "*****************************" << endl;
}

list<Score>* Top10::GetScores() {
	return &scores;
}
