//                      GILBERT AND THE DOORS V1.0
//               COPYRIGHT (c) Daniel Champagne 2016-2017
//                 Contact EMAIL: phoenixbozo@gmail.com
/*
  MoteurDeJeu.h is part of Gilbert and the doors.
  Gilbert and the doors is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef MOTEURDEJEU_H
#define MOTEURDEJEU_H

/*
 *TYPE of Sprite on levelgrid->spriteSimplifier
 *1=solide 0=vide
 TYPE of sprite on Levelgrid->SpriteEnemiSimplifier // modifier dans fonction StandardSpriteEnemiConvertion
 *2 sprite coquille
 *3 sprite scorpion -115
*/

#define MUSICVOLUME MIX_MAX_VOLUME/4
#define NBVIE 2

#define RETROACTIVITYSCROLLING 2 //retro action du scroolling

#define ON 1
#define OFF 0

#define COLLISIONBALAYAGE -7
#define ENEMISTEPMOVE 1//normal 1 deplacement des enemi

//sprite a exclure des map
#define EXC(EXCL) (levelgrid->sprite[x][y]==EXCL)
#define SPRITEEXCLUS ((levelgrid->sprite[x][y]>183)&&(levelgrid->sprite[x][y]<199))||(EXC(17))||(EXC(18))||(EXC(19))||(EXC(20))||(EXC(31))||(EXC(44))||(EXC(12))||(EXC(21))||(EXC(5))||(EXC(3))||(EXC(53))

//Sprite (decors) 
#define CD(COD) (levelgrid->sprite[x][y]==COD)

//Sprite actif
#define COUCHESPRITEACTIF(cc) ((cc>=356)&&(cc<=413))
#define COUCHESPRITEACTIFSCORPION(cc)((cc>=420)&&(cc<=477))
#define COUCHESPRITEACTIFBLOB(cc)((cc>=255)&&(cc<=274))


//mainsprite detect in level load
#define MS(MainSpriteComp) (levelgrid->sprite[x][y]==MainSpriteComp)
#define MAINSPRITE (MS(22))||(MS(23))||(MS(24))||(MS(25))||(MS(26))||(MS(27))||(MS(28))||(MS(29))||(MS(36))||(MS(37))\
||(MS(38))||(MS(39))||(MS(40))||(MS(41))||(MS(42))||(MS(43))||(MS(45))||(MS(46))||(MS(47))||(MS(48))||(MS(49))\
||(MS(50))||(MS(51))||(MS(52))||(MS(54))||(MS(55))||(MS(56))||(MS(57))||(MS(58))||(MS(59))||(MS(60))||(MS(61))||(MS(63))\
||(MS(64))||(MS(65))||(MS(66))||(MS(67))||(MS(68))||(MS(69))||(MS(70))||(MS(71))||(MS(72))||(MS(73))||(MS(74))||(MS(75))\
||(MS(76))||(MS(77))||(MS(78))
//mainsprite detect in level load end

#define SOLIDSPRITEINJURY        ((levelgrid->sprite[gridx+x][gridy+y]==202))
#define SOLIDSPRITEINJURYFIX     ((levelgrid->sprite[gridx+x][gridy+y]==308))
#define SOLIDSPRITEPICKUPBOMBER  ((levelgrid->sprite[gridx+x][gridy+y]==290))
#define SOLIDSPRITEPICKUPALLKEY  ((levelgrid->sprite[gridx+x][gridy+y]==109))
#define SOLIDSPRITEPICKUP100BOMB ((levelgrid->sprite[gridx+x][gridy+y]==110))

#define SOLIDSPRITEPICKUP          ((levelgrid->sprite[gridx+x][gridy+y]>79)&&(levelgrid->sprite[gridx+x][gridy+y]<96))
#define SOLIDSPRITEPICKUPENDURENCE ((levelgrid->sprite[gridx+x][gridy+y]>165)&&(levelgrid->sprite[gridx+x][gridy+y]<172))
#define SOLIDSPRITEPICKUPLIVE      ((levelgrid->sprite[gridx+x][gridy+y]>171)&&(levelgrid->sprite[gridx+x][gridy+y]<178))

#define HOLETOUTCH   (levelgrid->sprite[gridx+x][gridy+y]==305)
#define TOUTCHSPRITE ((levelgrid->sprite[gridx+x][gridy+y]>140)&&(levelgrid->sprite[gridx+x][gridy+y]<166))
#define TOUTCHSWITCH ((levelgrid->sprite[gridx+x][gridy+y]==203)||(levelgrid->sprite[gridx+x][gridy+y]==204))
#define TOUTCHWATER  ((levelgrid->sprite[gridx+x][gridy+y]>204)&&(levelgrid->sprite[gridx+x][gridy+y]<210))
#define TOUTCHWATERNOANIM ((levelgrid->sprite[gridx+x][gridy+y]>=210)&&(levelgrid->sprite[gridx+x][gridy+y]<=212))
#define TOUTCHSWITCHRESET (levelgrid->sprite[gridx+x][gridy+y]==113)

//Solid sprite pour detection wall pour le main sprite
#define wall(q) levelgrid->sprite[gridx+x][gridy+y]==q
#define SOLIDSPRITE (wall(126))||(wall(125))||(wall(124))||(wall(123))||(wall(122))||\
(wall(121))||(wall(120))||(wall(119))||(wall(118))||(wall(117))||(wall(116))||(wall(115))||\
(wall(114))||(wall(105))||(wall(104))||(wall(103))||(wall(102))||(wall(101))||(wall(96))||\
(wall(305))||(wall(303))||(wall(301))||(wall(299))||(wall(297))||(wall(286))||(wall(201))||\
(wall(200))||(wall(182))||(wall(181))||(wall(180))||(wall(178))||(wall(183))||(wall(179))||(wall(10))||\
(wall(33))||(wall (34))||(wall(16))||(wall(10))||(wall(15))||(wall(9))||\
(wall(32))||(wall(0))||(wall(1))||(wall(2))||(wall(4))||(wall(13))||(wall(314))\
||(wall(315))||(wall(316))||(wall(317))||(wall(318))||(wall(319))||(wall(320))||\
(wall(321))||(wall(322))||(wall(323))||(wall(324))||(wall(325))||(wall(326))||\
(wall(327))||(wall(328))||(wall(329))||(wall(330))||(wall(331))||(wall(332))\
||(wall(333))||(wall(334))||(wall(335))||(wall(336))||(wall(337))||(wall(338))\
||(wall(339))||(wall(340))||(wall(341))||(wall(342))||(wall(343))||(wall(344))\
||(wall(345))||(wall(346))||(wall(347))||(wall(348))

#define NOPRINT ((color>=314)&&(color<=321))||(color==305)
#define TEMPORARYDESABLESPRITE (color==202)


typedef enum SND{VITAL,BADABOUM,OPENDOOR,SWITCHON,SWITCHOFF,GAMEOVER,EXPLOSE,COIN,OUTCH,ALLSND,JUMP,THEEND,MUSIC1,KEY,BUMP,DOOR1,DOOR2}SND; //pour le son
typedef enum PLAYSTOP{PLAY,STOP}PLAYSTOP; //pour le son
typedef enum TypeReturn{DIFERENCE,TYPE,ADD}TypeReturn;

int SpriteTotal;
int LevelTotal;
int NumberofJoystick;
//fx et music on ou off
unsigned char bomberCount; //nombre de bombe
float speedmax; //normale 2
float speedmaxy; //normale 4
char sndmatrice[19];//pour le son
unsigned char EndurenceTrackbar;//endurence
int sndtrig;
unsigned char MainSpriteEnable;
uint32_t timer1; //pour injury mainsprite

typedef enum Direction{stop,stopgauche,stopdroite,gauche,droite,haut,bas,baisser}Direction;
typedef enum ColisionType{none,dangereux,bonus,pickup,wall}ColisionType;

typedef struct ImagePKG{
SDL_Surface** level;
SDL_Surface** spriteBMP;
int leveluse;
}ImagePKG;

typedef struct FloatRect{
float x;
float y;
float w;
float h;
}FloatRect;

typedef struct config{
int FXConfig; //fx et music on ou off
int MusicConfig;
int BIT;
int SOFTWARERENDER;
int DOUBLEBUF;
int FULLSCREEN;
int MODESANSECHEC;
int BACKGROUND;
int BACK;
int FRONT;
}config;

typedef struct LevelGrid{
//variable de particularité des level
uint32_t background; //image de fond selon le niveau
uint32_t FrontPic; //image de scrooling front
int backgroudScoll; //1 horizontal 0=vertical
char music[255]; //music selon le niveau
uint32_t cloud; //nuage selon level
//fin particularité

//anim lamp
uint32_t Lamp;

//eau anim
char WaterAnim;//animation pour leau
char WaterUpDown;//pour leau 1 ou 0
char WaterSlowMove; //pour marcher lantement dans leau 1 ou 0
//eau anim fin

// variable pour switch enable desable
int Switch; //1 ou 0
char AnimSwitch; // anim switch on off
char SwitchTrigger; //pour évité les double trig
//variable pour switch enable desable

//bombe
char Explose; // 15 frame valeur 15 déclenche l'Animation a l'endrois choisi
int bombeXgrid;
int bombeYgrid;
//fin bombe

char DoorOpen; // 0=close 1=open  2=cancel  default 0 
char DoorOpenCurentFrameAnim; // 0 a 25

//variable des sprite 
int ActualFrame; // de 0 a 120 frame
int CleTotaleAmasser; //cle ramaser
int CleTotalInLevel; //cle total dans le tableau
int NombreDeVie; //vie
char VitalInjuryTimer; // temp avant retour a la normal
char VisibleMainSprite; //Gilbert visible ou non

//couche1
uint32_t** sprite; //grille sprite wall et deco couche 1
unsigned char **SpriteSimplifer; //grille sprite wall et deco couche 1
char **Decalage32x;
char **Decalage32Y;
//fin couche 1

//couche 2
uint32_t **MapSpriteEnemi; //map enemi couche 2
unsigned char **MapSpriteEnemiSimplifier;//map enemi couche simplifier
char **MapEnemiOffSetX; //enemi couche 2
char **MapEnemiOffSetY; //enemi couche 2
char **MapEnemiDirection; 
//fin couche 2

//main sprite
int MainSpriteOrigineX; //position de dépar pour le main sprite
int MainSpriteOriginey; //position de dépar pour le main sprite
int mainspritegridx; //use position sur la grille x bloc
int mainspritegridy; //use position sur la grille y bloc
float MainSpriteoffset32x; //use
float MainSpriteoffset32y; //use
int mainSpritejump;//use
int mainSpritejumpcancel;//use
char DoubleJumpCancel; //use
float speedaxex;//use
float speedaxey;//use
int mainSpritew;
int mainSpriteh;
float MainSprite_render_pos_x; //use
float MainSprite_render_pos_y; //use
Direction DirectionMainSprite; //use
uint32_t AnimFrameRate;//use
uint32_t AnimFrameRate2;//use
uint32_t AnimFrameRatejump;//use
int framecount;//use for anim sprite
int framecount2;//use for anim sprite
uint32_t AnimSprite;//use
int animjumpcount;//use

//dimention de la grille
int width; //use
int height; //use

//variable de scrooling default
float ScrollStep; //use
float ScrollStepy; //use 
float Scrollx32;//use 
float Scrolly32;//use
float Scrollxlevel;//use
float Scrollylevel;//use
float renderx;
float rendery;

//variable de scrooling back
float Scrollx256; //use

//variable de scrooling front
float Scrollxfront256; //use

//variable de scrooling background
float ScrollxBackGround; //use
float ScrollyBackGround; //use
}LevelGrid;

void DEVNOTE(ImagePKG *PKG,Widjet *widjet);
void InfoTextGPL(ImagePKG *PKG,Widjet *widjet);
void SplashScreenGPL(ImagePKG *PKG,Widjet *widjet);
void InfoText(ImagePKG *PKG,Widjet *widjet);
void INFO(ImagePKG *PKG,Widjet *widjet);
void LicenceGPL(ImagePKG *PKG,Widjet *widjet);
void ResetAllControl(LevelGrid *levelgrid,Widjet *widjet);
void FadeInFadeOutNoSoundModif(SDL_Surface *INSurface,int FadeStep,int FrameRate,SDL_Surface *OUTSurface,Widjet *widjet);
void GreenScreen(LevelGrid *levelgrid,ImagePKG *PKG,Widjet *widjet);
int StandardSpriteEnemiConvertion(TypeReturn DIFERENCE_TYPE_ADD,int SpriteCheck,LevelGrid *levelgrid,ImagePKG *PKG);
char TheEnd(SDL_Joystick** joyst,LevelGrid *levelgrid,ImagePKG *PKG,Widjet *widjet);
int NumberOfLevel();
int NumberOfSprite();
void ParticularityOfLevel(LevelGrid *levelgrid,ImagePKG *PKG);
void SplashScreen(ImagePKG *PKG,Widjet *widjet);
void FadeInFadeOut(SDL_Surface *INSurface,int FadeStep,int FrameRate,SDL_Surface *OUTSurface,Widjet *widjet);
int JoystickUpdateEvents(SDL_Joystick** joyst,Input* in,LevelGrid *levelgrid);
void SetupConfig(config *Config,LevelGrid *levelgrid,ImagePKG *PKG,Widjet *widjet);
void ResetGridInit(LevelGrid *levelgrid,ImagePKG *PKG);
void FullFrameRoutine(LevelGrid *levelgrid,ImagePKG *PKG,Widjet *widjet);
void AnimLamp(LevelGrid *levelgrid,ImagePKG *PKG,Widjet *widjet);
void VitalInjury(LevelGrid *levelgrid,Widjet *widjet);
void PlaySND(PLAYSTOP playstop,SND snd);
int RestartGame(LevelGrid *levelgrid,ImagePKG *PKG,Widjet *widjet);
int NextLevel(LevelGrid *levelgrid,ImagePKG *PKG,Widjet *widjet);
int OpenDoorProcedure(LevelGrid *levelgrid,ImagePKG *PKG,Widjet *widjet);
int MoveEnemi(int ScanXX,int ScanYY,LevelGrid *levelgrid,ImagePKG *PKG,Widjet *widjet);
void ResetLevel(LevelGrid *levelgrid,ImagePKG *PKG,Widjet *widjet);
void MainBoot();
int FichierPresent(const char* NomDuFichier);
int LoadConfig(config *cfg);
int RenduDeCouche(int Couche,LevelGrid *levelgrid,ImagePKG *PKG,Widjet *widjet);
void RenduDeFond(LevelGrid *levelgrid,ImagePKG *PKG,Widjet *widjet);
void RenduDeFront(LevelGrid *levelgrid,ImagePKG *PKG,Widjet *widjet);
void RenduDeBackGround(LevelGrid *levelgrid,ImagePKG *PKG,Widjet *widjet);
void LevelGridInit(LevelGrid *levelgrid,ImagePKG *PKG);
void PartielLevelGridFree(LevelGrid *levelgrid);
void LevelGridFree(LevelGrid *levelgrid);
void ImagePKGInit(config *cfg,ImagePKG *PKG);
void ImagePKGFree(ImagePKG *PKG);
void UpdateScroll(LevelGrid *levelgrid,Widjet *widjet);
void ControlUpdate(LevelGrid *levelgrid,ImagePKG *PKG,Widjet *widjet);
void ReafectSprite(int gridpositionx,int gridpositiony,uint32_t sprite,int Decalage32x,int Decalage32Y,LevelGrid *levelgrid);
unsigned char colisiontestmaster(LevelGrid *levelgrid,ImagePKG *PKG,Widjet *widjet);
void MainSpriteAnim(LevelGrid *lg,Widjet *widjet);
int colisiondetect(uint32_t spritetester,int x1,int  y1,int w1,int h1,int x2,int  y2,int w2,int h2,LevelGrid *levelgrid,ImagePKG *PKG,Widjet *widjet);
int colisiononeVSone(uint32_t sprite1,float x1,float  y1,float w1,float h1,uint32_t sprite2,float x2,float  y2,float w2,float h2,LevelGrid *levelgrid,ImagePKG *PKG,Widjet *widjet);
int GameOver(LevelGrid *levelgrid,ImagePKG *PKG,Widjet *widjet);
#endif
