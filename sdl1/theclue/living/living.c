/*  _________  _______
   / ___/ __ \/ __/ _ \      Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/
#include "living\living.h"
#include "living\living_p.h"

#include "port\port.h"

void livInit(uword us_VisLScapeX,uword us_VisLScapeY,
				 uword us_VisLScapeWidth,uword us_VisLScapeHeight,
				 uword us_TotalLScapeWidth,uword us_TotalLScapeHeight,ubyte uch_FrameCount, ulong ul_StartArea)
	{
	sc = (struct SpriteControl*)MemAlloc(sizeof(struct SpriteControl));

	sc->p_Livings  = (LIST*)CreateList(0);
	sc->p_Template = (LIST*)CreateList(0);

	sc->us_VisLScapeX = us_VisLScapeX;
	sc->us_VisLScapeY = us_VisLScapeY;

	sc->us_VisLScapeWidth  = us_VisLScapeWidth;
	sc->us_VisLScapeHeight = us_VisLScapeHeight;

	sc->us_TotalLScapeWidth  = us_TotalLScapeWidth;
	sc->us_TotalLScapeHeight = us_TotalLScapeHeight;

	livSetActivAreaId(ul_StartArea);

	sc->uch_FrameCount = uch_FrameCount;

	sc->p_MemHandle = MemAlloc(STD_BUFFER1_SIZE);

	livLoadTemplates();		// jetzt alle Animphasen (Bilder) laden

	livLoadLivings();			// und die entsprechenden Lebewesen

	livSetPlayMode(LIV_PM_NORMAL);
	}

void livDone(void)
	{
	if (sc)
		{
		if (sc->p_MemHandle)
			MemFree(sc->p_MemHandle, STD_BUFFER1_SIZE);

		if (sc->p_Livings)
			{
			NODE *node;

			for (node = (NODE*) LIST_HEAD(sc->p_Livings); NODE_SUCC(node); node = (NODE*) NODE_SUCC(node))
				livRem((struct Living*) node);

			RemoveList(sc->p_Livings);
			sc->p_Livings=NULL;
			}

		if (sc->p_Template)
			{
			NODE *node;

			for (node = (NODE*) LIST_HEAD(sc->p_Template); NODE_SUCC(node); node = (NODE*) NODE_SUCC(node))
				livRemTemplate((struct AnimTemplate *)node);

			RemoveList(sc->p_Template);
			sc->p_Template=NULL;
			}

		MemFree(sc, sizeof(struct SpriteControl));

		sc = NULL;
		}
	}

void livSetActivAreaId(ulong areaId)
	{
	if (sc)
		sc->ul_ActivAreaId = areaId;
	}

void livLivesInArea(char *uch_Name, ulong areaId)
	{
	struct Living *liv = livGet(uch_Name);

	if (liv)
		liv->ul_LivesInAreaId = areaId;
	}

void livRefreshAll(void)
	{
	livDoAnims(0, 0);
	}

void livSetAllInvisible(void)
	{
	struct Living *liv;

	if (sc)
		{
		for(liv = (struct Living*)LIST_HEAD(sc->p_Livings);NODE_SUCC(liv);
			 liv = (struct Living*)NODE_SUCC(liv))
				livHide(liv);
		}
	}

void livSetPlayMode(ulong playMode)
	{
	sc->ul_SprPlayMode = playMode;

	if (sc->ul_SprPlayMode & LIV_PM_NORMAL)
		{
		sc->uch_FirstFrame   = 0;
		sc->uch_LastFrame    = sc->uch_FrameCount;
		sc->ch_PlayDirection = 1;
		}

	if (sc->ul_SprPlayMode & LIV_PM_REVERSE)
		{
		sc->uch_FirstFrame   = sc->uch_FrameCount - 1;
		sc->uch_LastFrame    = (ubyte) -1;
		sc->ch_PlayDirection = -1;
		}
	}

ulong livWhereIs(char *uch_Name)
	{
	struct Living *liv = livGet(uch_Name);
	ulong loc = 0;

	if (liv)
		loc = liv->ul_LivesInAreaId;

	return loc;
	}

void livSetPos(char *uch_Name,uword XPos,uword YPos)
	{
	struct Living *liv = livGet(uch_Name);

	if(liv)
		{
		liv->us_XPos = XPos;
		liv->us_YPos = YPos;
		}
	}

void livAnimate(char *uch_Name,ubyte uch_Action,word s_XSpeed,word s_YSpeed)
	{
	struct Living *liv = livGet(uch_Name);

	if(liv)
		{
		liv->uch_Status = LIV_ENABLED;

		liv->uch_OldAction = liv->uch_Action;

		liv->uch_Action = uch_Action;
		liv->s_XSpeed = s_XSpeed;
		liv->s_YSpeed = s_YSpeed;

		if (((ubyte)liv->ch_CurrFrameNr == sc->uch_LastFrame))
			 liv->ch_CurrFrameNr = (char) sc->uch_FirstFrame;
		}
	}

void livTurn(char *puch_Name,ubyte uch_Status)
	{
	struct Living *liv = livGet(puch_Name);

	if (liv)
		liv->uch_Status = uch_Status;  /* enable or disable */
	}

void livStopAll(void)
	{
	struct Living *liv;

	for (liv = (struct Living*)LIST_HEAD(sc->p_Livings);
		  NODE_SUCC(liv);liv=(struct Living*)NODE_SUCC(liv))
		{
		if (liv->uch_Status == LIV_ENABLED)
			livAnimate(NODE_NAME(liv),ANM_STAND,0,0);
		}
	}

void livCorrectViewDirection(struct Living *liv)
	{
	char *name = NODE_NAME(liv->p_OriginTemplate);

	if (liv->uch_Action <= ANM_MOVE_LEFT)
		liv->uch_ViewDirection = liv->uch_Action;
	else
		{
		if (!strcmp(name, LIV_TEMPL_BULLE_NAME) && (liv->uch_Action == ANM_WORK_CONTROL))
			liv->uch_ViewDirection = ANM_MOVE_DOWN;
		}
	}

void livPrepareAnims(void)
	{
	//xmsCopyDown(sc->p_MemHandle, 0, LIV_DECR_BUFFER, STD_BUFFER1_SIZE);
    memcpy(LIV_DECR_BUFFER, sc->p_MemHandle, STD_BUFFER1_SIZE);
	}

void livDoAnims(ubyte uch_Play, ubyte uch_Move)
	{
	struct Living *liv;

	livPrepareAnims();

	lsDoScroll();

	for (liv = (struct Living*)LIST_HEAD(sc->p_Livings);
			NODE_SUCC(liv);liv=(struct Living*)NODE_SUCC(liv))
		{
		if (liv->uch_Status == LIV_ENABLED)
			{
			if (uch_Move)
				{
				liv->us_XPos+=liv->s_XSpeed;
				liv->us_YPos+=liv->s_YSpeed;
				}

			/*
			 * die View Direction mu� hier gesetzt werden und NICHT
			 * in AnimateLiving, da die Aktion in AnimateLiving nicht
			 * stattfinden mu�, die ViewDirection zwischenzeitlich
			 * aber eine falschen Wert annimt, was z.B. im Planing
			 * zu Fehler f�hrt!
			 * mu� in jedem Fall geschehen, auch wenn Maxi unsichtbar ist
			 */

			livCorrectViewDirection(liv);

			if (livIsVisible(liv))
				{
				livShow(liv);

				// Action != ANM_STAND -> Schei� Ausnahme, weil von Marx
				// keine Stand Anim gezeichnet wurde
				if ((uch_Play) && (liv->uch_Action != ANM_STAND))
					liv->ch_CurrFrameNr += (char) sc->ch_PlayDirection;

				if ((ubyte) liv->ch_CurrFrameNr == sc->uch_LastFrame)
					livAnimate(NODE_NAME(liv),ANM_STAND,0,0);
				}
			else
				livHide(liv);
			}
		}
	}

void livSetVisLScape(uword us_VisLScapeX, uword us_VisLScapeY)
	{
	sc->us_VisLScapeX = us_VisLScapeX;
	sc->us_VisLScapeY = us_VisLScapeY;
	}

uword livGetXPos(char *Name)
	{
	struct Living *liv = livGet(Name);

	return(liv->us_XPos);
	}

uword livGetYPos(char *Name)
	{
	struct Living *liv = livGet(Name);

	return(liv->us_YPos);
	}

ubyte livGetViewDirection(char *uch_Name)
	{
	struct Living *liv = livGet(uch_Name);

	return(liv->uch_ViewDirection);
	}

ubyte livGetOldAction(char *uch_Name)
	{
	struct Living *liv = livGet(uch_Name);

	return(liv->uch_OldAction);
	}

ubyte livIsPositionInViewDirection(uword us_GXPos, uword us_GYPos, uword us_XPos, uword us_YPos, ubyte uch_ViewDirection)
	{
	ubyte InDirection = 1;

	switch(uch_ViewDirection)
		{
		case ANM_MOVE_LEFT:
				if (us_XPos > us_GXPos)	InDirection = 0;
				break;
		case ANM_MOVE_RIGHT:
				if (us_XPos < us_GXPos)	InDirection = 0;
				break;
		case ANM_MOVE_DOWN:
				if (us_YPos < us_GYPos)	InDirection = 0;
				break;
		case ANM_MOVE_UP:
				if (us_YPos > us_GYPos)	InDirection = 0;
				break;
		default: InDirection = 0; break;
		}

	return InDirection;
	}

ubyte livCanWalk(char *puch_Name)
	{
	struct Living *liv = livGet(puch_Name);

	if (liv)
		{
		ubyte direction;

		switch(liv->uch_Action)
			{
			case ANM_MOVE_UP:		direction = LS_SCROLL_UP; break;
			case ANM_MOVE_DOWN:	direction = LS_SCROLL_DOWN; break;
			case ANM_MOVE_LEFT:	direction = LS_SCROLL_LEFT; break;
			case ANM_MOVE_RIGHT: direction = LS_SCROLL_RIGHT; break;
			}

		if (!(lsIsCollision(liv->us_XPos + liv->s_XSpeed, liv->us_YPos + liv->s_YSpeed, direction)))
			return 1;
		}

	return 0;
	}

static struct Living *livGet(char *uch_Name)
	{
	struct Living *liv=NULL;

	liv=(struct Living*)GetNode(sc->p_Livings,uch_Name);

	if(!liv)
		NewErrorMsg (Internal_Error, __FILE__, __func__, 1);

	return(liv);
	}

static void livAdd(char *uch_Name,char *uch_TemplateName, ubyte uch_XSize,
						 ubyte uch_YSize,word s_XSpeed,word s_YSpeed)

	{
	struct Living *liv;
	struct AnimTemplate *tlt;

	liv = (struct Living*)
		 CreateNode(sc->p_Livings, (long)sizeof(struct Living), uch_Name);

	liv->uch_XSize = uch_XSize;
	liv->uch_YSize = uch_YSize;

	tlt = liv->p_OriginTemplate = (struct AnimTemplate *) GetNode(sc->p_Template, uch_TemplateName);

	liv->us_LivingNr = BobInit(LIV_TEMPLATE_BUFFER, tlt->us_Width, tlt->us_Height);

	liv->uch_OldAction  = 0;
	liv->uch_Action     = 0;

	liv->uch_ViewDirection = 0; // MOD: 15-05-94

	liv->ch_CurrFrameNr = 0;

	liv->s_XSpeed = s_XSpeed;
	liv->s_YSpeed = s_YSpeed;

	liv->us_XPos = 0;
	liv->us_YPos = 0;

	liv->ul_LivesInAreaId = sc->ul_ActivAreaId;

	liv->uch_Status = LIV_DISABLED;
	}

static void livRem(struct Living *liv)
	{
	BobDone(liv->us_LivingNr);
	}

static void livLoadTemplates(void)
	{
	uword cnt,i;
	char *line;
	LIST  *l = (LIST*)CreateList(0);
	struct AnimTemplate *tlt;
	char pathname[TXT_KEY_LENGTH];

	dskBuildPathName(TEXT_DIRECTORY, LIV_ANIM_TEMPLATE_LIST, pathname);

	if(!(cnt=ReadList(l,0L,pathname, 0)))
		NewErrorMsg(Disk_Defect, __FILE__, __func__, 3);

	for(i=0;i<cnt;i++)
		{
		line = NODE_NAME(GetNthNode(l,i));

		tlt = (struct AnimTemplate*)CreateNode(sc->p_Template,
				  (ulong)sizeof(struct AnimTemplate),txtGetKey(1,line));

		tlt->us_Width  = (uword) txtGetKeyAsULONG(2,line);
		tlt->us_Height = (uword) txtGetKeyAsULONG(3,line);

		tlt->us_FrameOffsetNr = (uword) txtGetKeyAsULONG(4, line);
		}

	livLoadData();

	RemoveList(l);
	}

static void livRemTemplate(struct AnimTemplate *tlt)
	{
	// dummy function
	}

static void livLoadLivings(void)
	{
	uword cnt,i;
	char *line;
	LIST  *l = (LIST*)CreateList(0);
	char pathname[TXT_KEY_LENGTH];

	dskBuildPathName(TEXT_DIRECTORY, LIV_LIVINGS_LIST, pathname);

	if(!(cnt=ReadList(l,0L,pathname, 0)))
		NewErrorMsg(Disk_Defect, __FILE__, __func__, 2);

	for(i=0;i<cnt;i++)
		{
		char name[TXT_KEY_LENGTH], templateName[TXT_KEY_LENGTH];

		line = NODE_NAME(GetNthNode(l,i));

		strcpy(name    , txtGetKey(1,line));
		strcpy(templateName, txtGetKey(2,line));

		livAdd( name,
				templateName,
				(ubyte)  txtGetKeyAsULONG(3, line),
				(ubyte)  txtGetKeyAsULONG(4, line),
				(word)   txtGetKeyAsULONG(5, line),
				(word)   txtGetKeyAsULONG(6, line));
		}

	RemoveList(l);
	}

static void livHide(struct Living *liv)
	{
	BobInVis(liv->us_LivingNr);
	}

static void livShow(struct Living *liv)
	{
	uword frameNr, action;
	uword srcX, srcY, offset;
	struct AnimTemplate *tlt = liv->p_OriginTemplate;

	// Scheiss Ausnahme, weil von marx kein ANM_STAND gezeichnet wurde
	if (liv->uch_Action == ANM_STAND)
		{
		action = liv->uch_ViewDirection;
		liv->ch_CurrFrameNr = 4;
		}
	else
		action = liv->uch_Action;

	frameNr = action * sc->uch_FrameCount + liv->ch_CurrFrameNr;

	frameNr = frameNr + tlt->us_FrameOffsetNr;

	offset = frameNr * tlt->us_Width;

	srcY = (offset / LIV_COLL_WIDTH) * tlt->us_Height;
	srcX = (offset % LIV_COLL_WIDTH);

	// COSP
	BobSet(liv->us_LivingNr, liv->us_XPos, liv->us_YPos, srcX, srcY);
	BobVis(liv->us_LivingNr);
	}

static void livLoadData(void)
{
	char filename[TXT_KEY_LENGTH];

	dskBuildPathName(PICTURE_DIRECTORY,LIV_COLL_NAME,filename);

	dskLoad(filename, LIV_LOAD_BUFFER, 0);

	tcClearStdBuffer(LIV_DECR_BUFFER);

	gfxILBMToRAW((ubyte*)LIV_LOAD_BUFFER, (ubyte*)LIV_DECR_BUFFER);

	//xmsCopyUp(sc->p_MemHandle, 0, LIV_DECR_BUFFER, STD_BUFFER1_SIZE);
    memcpy(sc->p_MemHandle, LIV_DECR_BUFFER, STD_BUFFER1_SIZE);
}

static ubyte livIsVisible(struct Living *liv)
	{
	uword left,right,up,down;
	ubyte visible = 0;

	left  = liv->us_XPos;
	right = left+liv->uch_XSize;
	up    = liv->us_YPos;
	down  = up + liv->uch_YSize;

	if(liv->ul_LivesInAreaId == sc->ul_ActivAreaId)
		if(right>sc->us_VisLScapeX)
			if(left<(sc->us_VisLScapeX + sc->us_VisLScapeWidth))
				if(down>sc->us_VisLScapeY)
					if(up<(sc->us_VisLScapeY + sc->us_VisLScapeHeight))
						visible=1;

	return(visible);
	}
