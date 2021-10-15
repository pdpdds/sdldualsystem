#define QUITSUR	    "Are you sure you want\n"\
		            "to quit this great game?"

#define CURGAME	    "You are currently in\n"\
		            "a game. Continuing will\n"\
		            "erase old game. Ok?"

#define GAMESVD	    "There's already a game\n"\
		            "saved at this position.\n"\
		            "      Overwrite?"

#define ENDGAMESTR	"Are you sure you want\n"\
					"to end the game you\n"\
					"are playing? (Y or N):"

#define STR_NG	"New Game"
#define	STR_SD	"Sound"
#define	STR_CL	"Control"
#define	STR_LG	"Load Game"
#define	STR_SG	"Save Game"
#define	STR_CV	"Change View"
#define	STR_VS	"View Scores"
#define STR_EG	"End Game"
#define	STR_BD	"Credits     "
#define STR_QT	"Quit"

#define STR_LOADING	"Loading"
#define STR_SAVING	"Saving"

#define STR_GAME	"Game"
#define STR_DEMO	"     "
#define STR_LGC		"Load Game called\n\""
#define STR_EMPTY	"empty"
#define STR_CALIB	"Calibrate"
#define STR_JOYST	"Joystick"
#define STR_MOVEJOY	"Move joystick to\nupper left and\npress button 0\n"
#define STR_MOVEJOY2 "Move joystick to\nlower right and\npress button 1\n"
#define STR_ESCEXIT	"ESC to exit"

#define STR_NONE	"None"
#define	STR_PC		"PC Speaker"
#define	STR_ALSB	"AdLib/Sound Blaster"
#define	STR_DISNEY	"Disney Sound Source"
#define	STR_SB		"Sound Blaster"

#define	STR_MOUSEEN	"Mouse Enabled"
#define	STR_JOYEN	"Joystick Enabled"
#define	STR_PORT2	"Use joystick port 2"
#define	STR_GAMEPAD	"Gravis GamePad Enabled"
#define	STR_SENS	"Mouse Sensitivity"
#define	STR_CUSTOM	"Customize controls"

#define	STR_DADDY	"Grandma."
#define	STR_HURTME	"Go Easy on me!"
#define	STR_BRINGEM	"I'm a Hero..."
#define	STR_DEATH	"Suicidal Tendencies!"
#define STR_TOURN   "Tournament!"

#define	STR_MOUSEADJ	"Adjust Mouse Sensitivity"
#define STR_SLOW	    "Slow"
#define STR_FAST	    "Fast"

#define STR_CALTSPEED "ASpd"
#define	STR_CRUN	"Run"
#define STR_COPEN	"Open"
#define STR_CFIRE	"Fire"
#define STR_CSTRAFE	"Strafe"

#define	STR_LEFT	"Left"
#define	STR_RIGHT	"Right"
#define	STR_FRWD	"Frwd"
#define	STR_BKWD	"Bkwrd"
#define	STR_THINK	"Thinking"

#define STR_SIZE1	"Use arrows to size"
#define STR_SIZE2	"ENTER to accept"
#define STR_SIZE3	"ESC to cancel"

#define STR_YOUWIN	"you win!"

#define STR_TOTALTIME	"total time"

#define STR_RATKILL		    "kill    %"
#define STR_RATSECRET  	  "secret    %"
#define STR_RATTREASURE	"treasure    %"

#define STR_BONUS	"bonus"
#define STR_TIME	"time"
#define STR_PAR		" par"

#define STR_RAT2KILL            "kill ratio    %"
#define STR_RAT2SECRET  	  "secret ratio    %"
#define STR_RAT2TREASURE	"treasure ratio    %"

#define STR_DEFEATED	"defeated!"

#define STR_CHEATER1	"You now have 100% Health,"
#define STR_CHEATER2    "99 Ammo and both Keys!"
#define STR_CHEATER3	"Note that you have basically"
#define STR_CHEATER4	"eliminated your chances of"
#define STR_CHEATER5	"getting a high score!"

#define STR_NOSPACE1	"There is not enough space"
#define STR_NOSPACE2	"on your disk to Save Game!"

#define STR_SAVECHT1	"Your Save Game file is,"
#define STR_SAVECHT2	"shall we say, \"corrupted\"."
#define STR_SAVECHT3	"But I'll let you go on and"
#define STR_SAVECHT4	"play anyway...."

#define	STR_SEEAGAIN	"Let's see that again!"

#ifdef SPEAR
#define ENDSTR1 "If you're afraid to die\nthen press " YESBUTTONNAME "\nBut you know that\nbrave men would press " NOBUTTONNAME
#define ENDSTR2 "Press " YESBUTTONNAME " if you have a hot date\nPress " NOBUTTONNAME " she's a dog let 'er wait"
#define ENDSTR3 YESBUTTONNAME " is for corwards and shame\n" NOBUTTONNAME " is for hereos and fame"
#define ENDSTR4 "Press " YESBUTTONNAME " if you must go to work\nPress " NOBUTTONNAME " if your boss is a jerk"
#define ENDSTR5 YESBUTTONNAME " = Life, work, daily humdrum\n" NOBUTTONNAME " = Death, bullets, glory, and fun"
#define ENDSTR6 "Press " YESBUTTONNAME " to return\nto your life\nPress " NOBUTTONNAME " to give\nHeinrich some strife"
#define ENDSTR7 "If your bladder is full\nthen press " YESBUTTONNAME "\nPress " NOBUTTONNAME " let it go\nit'll dry"
#define ENDSTR8 "Press " YESBUTTONNAME " to make love but not war\nPress " NOBUTTONNAME " I prefer war to a whore!"
#define ENDSTR9 "Press " NOBUTTONNAME " if you like this game\nPress " YESBUTTONNAME " if you think it's lame"
#else
#define ENDSTR1 "Dost thou wish to\nleave with such hasty\nabandon?"
#define ENDSTR2 "Chickening out...\nalready?"
#define ENDSTR3 "Press " NOBUTTONNAME " for more carnage.\nPress " YESBUTTONNAME " to be a weenie."
#define ENDSTR4 "So, you think you can\nquit this easily, huh?"
#define ENDSTR5 "Press " NOBUTTONNAME " to save the world.\nPress " YESBUTTONNAME " to abandon it in\nits hour of need."
#define ENDSTR6 "Press " NOBUTTONNAME " if you are brave.\nPress " YESBUTTONNAME " to cower in shame."
#define ENDSTR7 "Heroes, press " NOBUTTONNAME ".\nWimps, press " YESBUTTONNAME "."
#define ENDSTR8 "You are at an intersection.\nA sign says, 'Press " YESBUTTONNAME " to quit.'\n>"
#define ENDSTR9 "For guns and glory, press " NOBUTTONNAME ".\nFor work and worry, press " YESBUTTONNAME "."
#endif
