//==============================================================================
#include "CScore.h"

//==============================================================================
CScore::CScore() {
	Surf_Score = NULL;
	Display = NULL;
};

//------------------------------------------------------------------------------        
bool CScore::OnInit() {
	if((Surf_Score = ImageLoad("media/score.png")) == NULL) {
		std::cerr << "Cannot load file: media/score.png\n";
		return false;
	}
	
	Transparency(Surf_Score, SetColor(255, 0, 255));
		
	return true;
};

//------------------------------------------------------------------------------        
void CScore::OnRender() {
	int Pos = 0;
	for(int i = 0;i < Score_Anim.size();i++) {		
		if(Score_Anim[i].SpawnTime + SCORE_ALIVETIME < SDL_GetTicks()) {
			Score_Anim.erase(Score_Anim.begin());
		}
		
		int Alpha = (SCORE_ALIVETIME / (SDL_GetTicks() - Score_Anim[i].SpawnTime)) * 255;
		
		SetAlpha(Surf_Score, Alpha);
		
		for(int j = 0;j < strlen(Score_Anim[i].Str_Score);j++) {
			Pos = ((int)(Score_Anim[i].Str_Score[j] - 49)) + 1;
			Blit(Display, Surf_Score, Score_Anim[i].X * 32 + (j * 20), Score_Anim[i].Y * 32, Pos * 20, 0, 20, 20);
		}
	}
};

//------------------------------------------------------------------------------
void CScore::OnCleanup() {
	if(Surf_Score) SDL_FreeSurface(Surf_Score);
};

//------------------------------------------------------------------------------
void CScore::AddScore(int Score, int X, int Y) {
	CScore_Anim tempAnim;
	
	tempAnim.SpawnTime = SDL_GetTicks();
	sprintf(tempAnim.Str_Score, "%d", Score);
	
	tempAnim.X = X;
	tempAnim.Y = Y;
	
	Score_Anim.push_back(tempAnim);
};

//------------------------------------------------------------------------------
void CScore::Reset() {
	Score_Anim.clear();
};

//==============================================================================
