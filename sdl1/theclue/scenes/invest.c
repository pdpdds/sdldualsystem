/*  _________  _______
   / ___/ __ \/ __/ _ \      Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/
#include "scenes\scenes.h"

ulong tcShowPatrol(LIST *bubble_l, char *c_time, char *patr, ubyte first, Building bui, ulong raise)
	{
	char patrolie[TXT_KEY_LENGTH];
	ulong choice = 0;

	sprintf(patrolie, "%s  %s", c_time, patr);

	CreateNode(bubble_l, 0L, patrolie);

	SetBubbleType(THINK_BUBBLE);

	Bubble(bubble_l, (ubyte) first, 0, 140L);
	choice = GetExtBubbleActionInfo();

	tcAddBuildExactlyness(bui, raise);

	ShowTime(0);

	return choice;
	}

void Investigate(char *location)
	{
	NODE	*n, *nextMsg;
	LIST 	*origin, *bubble_l;
	char	patr[TXT_KEY_LENGTH], line[TXT_KEY_LENGTH];
	char    c_time[10];
	ulong minutes=0, guarding=0, choice=0, count=0, buiID=0, first = 0, raise;
	Building  bui;
	ulong     patrolCount;

	buiID = GetObjNrOfBuilding(GetLocation);
	bui   = (Building)dbGetObject(buiID);

	if (bProfidisk)
	{
		if (buiID == Building_Buckingham_Palace)
		{
			bubble_l = txtGoKey(INVESTIGATIONS_TXT,"BuckinghamBeobachtet");
			SetBubbleType(THINK_BUBBLE);
			Bubble(bubble_l, 0 ,0L,0L);
			RemoveList(bubble_l);

			return;
		}
	}

	if(!(GamePlayMode & GP_MUSIC_OFF))
		sndPlaySound("invest.bk", 0);

	inpTurnESC(0);

	ShowMenuBackground();    /* Bildschirmaufbau */
	ShowTime(0);
	inpSetWaitTicks(50);

	gfxSetRect(0, 320);
	gfxSetPens(l_wrp, 249, GFX_SAME_PEN, 0);

	txtGetFirstLine(INVESTIGATIONS_TXT,"Abbrechen",line);
	txtGetFirstLine(INVESTIGATIONS_TXT,"Patrolie",patr);

	gfxPrint(m_wrp,line,24,GFX_PRINT_CENTER);

	/* Beobachtungstexte von Disk lesen */
	origin   = txtGoKey(INVESTIGATIONS_TXT,location);
	bubble_l = (LIST*)CreateList(0);
	count    = GetNrOfNodes(origin);
	guarding = (ulong) tcRGetGRate(bui);
	patrolCount = (270 - guarding) / 4 + 1;

	/* Wissensgewinn pro Ereignis =
	 * (255-guarding) = alle wieviel Minuten Streife kommt
	 * count = diverse andere Ereignisse
	 * 1439 / (255- guarding) = Anzahl der Streifen / Tag
	 * 3 = Konstante zur Beschleunigung (GamePlay)
	 */

	raise    = 255 / (1439 / patrolCount + count + 1) + 3;

	hasSet(Person_Matt_Stuvysunt,buiID);

	/* bis zur 1. Meldung Zeit vergehen lassen! */
	for (nextMsg = 0; nextMsg == 0;)
		{
		BuildTime(GetMinute,txtGetLanguage(),c_time);

		if (!(GetMinute % 60))
			ShowTime(0);

		for (n = (NODE*)LIST_HEAD(origin); NODE_SUCC(n); n = (NODE*) NODE_SUCC(n))
			{
			if (strncmp(NODE_NAME(n),c_time,5) == 0)
				nextMsg = n;
			}

		if (!nextMsg)
			AddVTime(1);

		if((GetMinute % patrolCount) == 0)
			choice = tcShowPatrol(bubble_l, c_time, patr, first++, bui, raise);

		if(CalcRandomNr(0L,6L) == 1)
			tcAddBuildStrike(bui,1);
		}

	while(((minutes) < MINUTES_PER_DAY) && (!(choice & INP_LBUTTONP)) && (!(choice & INP_RBUTTONP)) && (!(choice & INP_ESC)))
		{
		choice = 0;

		/* Anzeigen jeder vollen Stunde */
		if((GetMinute % 60)==0)
			ShowTime(0);

		BuildTime(GetMinute,txtGetLanguage(),c_time);

		/* je nach Bewachungsgrad Meldung : "Patrolie" einsetzen ! */
		if((GetMinute % patrolCount) == 0)
			choice = tcShowPatrol(bubble_l, c_time, patr, first++, bui, raise);

		/* �berpr�fen ob zur aktuellen Zeit (time) etwas geschieht : */
		if (!choice)
			{
			if (strncmp(NODE_NAME(nextMsg),c_time,5)==0)
				{
				if((GetMinute % 60)!=0)
					ShowTime(0);

				n = (NODE*)CreateNode(bubble_l,0L,NODE_NAME(nextMsg));

				SetBubbleType(THINK_BUBBLE);

				Bubble(bubble_l,(ubyte) first++ ,0, 140L);
				choice = GetExtBubbleActionInfo();

				tcAddBuildExactlyness(bui, raise);

				ShowTime(0);

				if (NODE_SUCC(NODE_SUCC(nextMsg)))
					nextMsg = (NODE*) NODE_SUCC(nextMsg);
				else
					nextMsg = (NODE*) LIST_HEAD(origin);
				}
			}

		/* Zeit erh�hen und nach dem Spieler sehen ! */
		if (CalcRandomNr(0L,6L) ==1)
			tcAddBuildStrike(bui,1);

		AddVTime(1L);
		minutes++;

		if(!choice)
			choice = inpWaitFor(INP_TIME|INP_LBUTTONP|INP_RBUTTONP|INP_ESC);
		}

	RemoveList(bubble_l);

	if (minutes >= 1440) /* wurde 24 Stunden lang beobachtet ? */
		{
		bubble_l = txtGoKey(INVESTIGATIONS_TXT,"24StundenBeobachtet");
		SetBubbleType(THINK_BUBBLE);
		Bubble(bubble_l, 0 ,0L,0L);
		RemoveList(bubble_l);
		}

	RemoveList(origin);

	Present(buiID,"Building",InitBuildingPresent);

	/* Im Plan dazuaddieren : wie lange beobachtet */
	/* wenn Geb�iude 2 mal beobachtet wird -> Auff�llig !! */
	/* Auff�llugkeitgrad erh�hen */
	/* Ergebnisse ! (Genauigkeit)*/
	/* Abschlu�bericht zeigen ! */

	ShowMenuBackground();
	tcRefreshLocationInTitle(GetLocation);

	inpSetWaitTicks(0);
	ShowTime(0);

	if(!(GamePlayMode & GP_MUSIC_OFF))
		tcPlayStreetSound();

	if (GamePlayMode & GP_DEMO)
		{
		ubyte colortable[768];

		SuspendAnim();
		gfxPrepareRefresh();

		gfxGetColorTableFromReg(colortable);

		gfxShow(224,GFX_NO_REFRESH|GFX_FADE_OUT|GFX_BLEND_UP|GFX_ONE_STEP,2,-1,-1);

		inpWaitFor(INP_LBUTTONP);

		gfxChangeColors(l_wrp, 0, GFX_FADE_OUT, NULL);
		gfxClearArea(l_wrp);
		gfxChangeColors(l_wrp, 0, GFX_BLEND_UP, colortable);

		gfxRefresh();

		ContinueAnim();
		}

	inpTurnESC(1);
	}

