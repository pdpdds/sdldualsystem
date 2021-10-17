#include "CHighScore.h"

CHighScore::CHighScore() {
	for(int i = 0;i < 18;i++) {
		sprintf(Entry[i].Name, "AAA");
		Entry[i].Score = 0;
		Entry[i].Difficulty = 'E';
	}
	
	EntryID = 0;
	EnterName = false;
	
	CursorX = 0;
	CursorY = 0;
	
	Pos = 0;
	
	Display = NULL;
	Surf_HighScore = NULL;
};

void CHighScore::OnEvent(SDL_Event* Event) {
	switch(Event->type) {            
		//A key has been pressed
    	case SDL_KEYDOWN: {	
			switch(Event->key.keysym.sym) {
				case SDLK_LEFT:		CursorX--; break;
				case SDLK_UP:		CursorY--; break;
				case SDLK_RIGHT:	CursorX++; break;
				case SDLK_DOWN:		CursorY++; break;
				
				case SDLK_RETURN: {
					//Back
					if(CursorX == 5 && CursorY == 3) {
						Pos--;
						if(Pos < 0) Pos = 0;
					}
					
					//End
					else if(CursorX == 6 && CursorY == 3) {						
						if(Pos >= 3) {
							Entry[EntryID].Name[0] = toupper(NameID[0] + 97);
							Entry[EntryID].Name[1] = toupper(NameID[1] + 97);
							Entry[EntryID].Name[2] = toupper(NameID[2] + 97);
						
							EnterName = false;
						}
					}
					
					else {
						if(Pos < 3) {		
							NameID[Pos] = CursorX + (CursorY * 7);
							Pos++;
							
							if(Pos == 3) {
								CursorX = 6;
								CursorY = 3;
							}
						}
					}
					
					break;
				}
			}
		}
	}
	
	if(CursorX < 0) CursorX = 6;
	if(CursorX > 6) CursorX = 0;
	
	if(CursorY < 0) CursorY = 3;
	if(CursorY > 3) CursorY = 0;
};

bool CHighScore::OnInit() {
	if((Surf_HighScore = ImageLoad("media/highscore.png")) == NULL) {
		cerr << "Cannot load file: media/highscore.png\n";
		return false;
	}	
	
	Transparency(Surf_HighScore, SetColor(255, 0, 255));
	
	return true;	
};

void CHighScore::OnRender() {
	Blit(Display, Surf_HighScore, 16, 32, 0, 0, 448, 320 - 64);
	
	Blit(Display, Surf_HighScore, (CursorX * 64) + 16, (CursorY * 64) + 32, 0, 320 - 64, 64, 64);
	
	for(int i = 0;i < Pos;i++) {
		int cX = (NameID[i] % 7) * 64;
		int cY = (NameID[i] / 7) * 64;
		
		Blit(Display, Surf_HighScore, (128 + 16) + (i * 64), 320 + 64, cX, cY, 64, 64);	
	}
};

void CHighScore::OnCleanup() {
	if(Surf_HighScore)		SDL_FreeSurface(Surf_HighScore);
};
		
void CHighScore::CheckScore(int Score) {
	for(int i = 0;i < 18;i++) {
		if(Score > Entry[i].Score) {
			for(int j = 17;j >= i;j--) {
				Entry[j] = Entry[j-1];
			}
			
			Entry[i].Score = Score;
			sprintf(Entry[i].Name, "   ");
			
			EntryID = i;
			
			EnterName = true;
			
			CursorX = 0;
			CursorY = 0;
			
			break;
		}
	}
};
		
void CHighScore::LoadHighScore() {
	FILE* FileHandle = fopen("highscore.dat", "r");
	
	for(int i = 0;i < 18;i++) {
		fscanf(FileHandle, "%s %d %c\n", Entry[i].Name, &Entry[i].Score, &Entry[i].Difficulty);
	}
	
	fclose(FileHandle);
};
		
void CHighScore::WriteHighScore() {
	FILE* FileHandle = fopen("highscore.dat", "w");
	
	for(int i = 0;i < 18;i++) {
		fprintf(FileHandle, "%s %d %c\n", Entry[i].Name, Entry[i].Score, Entry[i].Difficulty);
	}
	
	fclose(FileHandle);	
};
