/*  _________  _______
   / ___/ __ \/ __/ _ \      Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/
#include "scenes\scenes.h"

uword	CurrentBackground = BGD_LONDON;


/**************************************************
 * a little static func for presentation of data
 */

static ubyte tcDisplayInfoAboutPerson(ulong objID)
	{
	LIST *bubble = NULL;
	char name[TXT_KEY_LENGTH]; 
	ubyte ret = 0;

	if(has(Person_Matt_Stuvysunt, objID))   /* hat Matt Wissen �ber ihn? */
		ret = Present(objID,"Person",InitPersonPresent);
	else
		{
		dbGetObjectName (objID, name);
		bubble = txtGoKey (LOOK_TXT, name);

		SetBubbleType (THINK_BUBBLE);
		Bubble (bubble,0,0L,0L);
		RemoveList (bubble);
		}

	return ret;
	}


/**************************************************
 * taxi stuff
 *
 * AddTaxiLocation
 * RemTaxiLocation
 */

void	AddTaxiLocation(ulong locNr)
	{
	ulong objNr;

	if(objNr = GetObjNrOfLocation(locNr))
		taxiSet(Person_Matt_Stuvysunt, objNr);
	}

void RemTaxiLocation(ulong locNr)
	{
	ulong objNr;

	if (objNr = GetObjNrOfLocation(locNr))
		taxiUnSet(Person_Matt_Stuvysunt, objNr);
	}

/***************************************************
 * standard scenes
 * Go
 * Information
 * Look
 * Telefon
 * Wait
 */

ulong Go(LIST *succ)
	{
	struct	TCEventNode *node;
	struct 	Scene *sc;
	ulong	prob,succ_eventnr;
	NODE 	*location;
	char 	line[255];

	inpTurnFunctionKey(0);

	if(GetNrOfNodes(succ) > 1)
		{
		prob=0L;

		ShowMenuBackground();

		txtGetFirstLine(THECLOU_TXT,"Gehen",line);

		PrintStatus(line);

		for(node=(struct TCEventNode*) LIST_HEAD(succ);NODE_SUCC(node);node=(struct TCEventNode*) NODE_SUCC(node))
			{
			sc=GetScene(node->EventNr);

			location=(NODE*)GetNthNode(film->loc_names,(ulong)sc->LocationNr);

			NODE_NAME(node)=NODE_NAME(location);

			prob++;
			}

		prob=(1 << prob) - 1;

		prob=Menu(succ,prob,0,NULL,0L);

		succ_eventnr=((struct TCEventNode*)GetNthNode(succ,(ulong)prob))->EventNr;

		/* jetzt die Zuweisung wieder entfernen, damit der Name */
		/* nicht 2 mal freigegeben wird */

		for(node=(struct TCEventNode*)LIST_HEAD(succ);NODE_SUCC(node);node=(struct TCEventNode*)NODE_SUCC(node))
			NODE_NAME(node)=NULL;
		}
	else
		{
		succ_eventnr=(ulong) ((struct TCEventNode*)
			GetNthNode(succ,0L))->EventNr;
		}

	inpTurnFunctionKey(1);

	return(succ_eventnr);
	}


void Information(void)
	{
	LIST   *bubble,*list;
	ubyte  choice=0,choice1=0, ret = 0;
	char enough[TXT_KEY_LENGTH];
	ulong  objID;

	inpTurnESC(1);

	while((choice != 6) && (ret != GET_OUT))
		{
		SetBubbleType(THINK_BUBBLE);
		bubble  = txtGoKey(THECLOU_TXT,"INFORMATION");
		choice  = Bubble(bubble,choice,0L,0L);
		RemoveList(bubble);

		choice1 = 0;

		switch(choice)
			{
			case 0:
				if ((Present(Player_Player_1,"Player",InitPlayerPresent)) != GET_OUT)
					  Present(Person_Matt_Stuvysunt,"Person",InitPersonPresent);
				break;

			case 1:
				hasAll(Person_Matt_Stuvysunt,OLF_PRIVATE_LIST|OLF_INCLUDE_NAME|OLF_INSERT_STAR, Object_Car);
				list = ObjectListPrivate;

				if(!(LIST_EMPTY(list)))
					{
					txtGetFirstLine(BUSINESS_TXT, "GENUG", enough);
					ExpandObjectList(list, enough);
					SetBubbleType(THINK_BUBBLE);

					while(ChoiceOk((choice1 = Bubble(list, choice1, 0L, 0L)), GET_OUT, list))
						{
						SetBubbleType(THINK_BUBBLE);

						objID = (ulong)(((struct ObjectNode*)GetNthNode(list ,(ulong)choice1))->nr);
						ret   = Present (objID ,"Car" ,InitCarPresent);
						}
					}
				else
					Say(BUSINESS_TXT, 0, MATT_PICTID, "POOR");

				RemoveList(list);
				break;

			case 2:
				knowsAll(Person_Matt_Stuvysunt,OLF_PRIVATE_LIST|OLF_INCLUDE_NAME|OLF_INSERT_STAR,Object_Person);
				list = ObjectListPrivate;

				if(!(LIST_EMPTY(list)))
					{
					txtGetFirstLine(BUSINESS_TXT, "GENUG", enough);
					ExpandObjectList(list, enough);
					SetBubbleType(THINK_BUBBLE);

					while(ChoiceOk((choice1 = Bubble(list, choice1, 0L, 0L)), GET_OUT, list))
						{
						SetBubbleType(THINK_BUBBLE);

						objID = (ulong)(((struct ObjectNode*)GetNthNode(list,(ulong)choice1))->nr);
						ret   = tcDisplayInfoAboutPerson(objID);
						}
					}
				else
					Say(BUSINESS_TXT, 0, MATT_PICTID, "POOR");

				RemoveList(list);
				break;

			case 3:
				hasAll(Person_Matt_Stuvysunt, OLF_PRIVATE_LIST|OLF_INCLUDE_NAME|OLF_INSERT_STAR, Object_Tool);
				list = ObjectListPrivate;

				if(!(LIST_EMPTY(list)))
					{
					SetBubbleType(THINK_BUBBLE);
					txtGetFirstLine(BUSINESS_TXT, "GENUG", enough);
					ExpandObjectList(list, enough);

					while(ChoiceOk((choice1 = Bubble(list, choice1, 0L, 0L)), GET_OUT, list))
						{
						SetBubbleType(THINK_BUBBLE);

						objID   = (ulong)(((struct ObjectNode*)GetNthNode(list,(ulong)choice1))->nr);
						ret = Present(objID,"Tool",InitToolPresent);
						}
					}
				else
					Say(BUSINESS_TXT, 0, MATT_PICTID, "POOR");

				RemoveList(list);
				break;

			case 4:
				hasAll(Person_Matt_Stuvysunt, OLF_PRIVATE_LIST|OLF_INCLUDE_NAME|OLF_INSERT_STAR, Object_Building);
				list = ObjectListPrivate;

				if(!(LIST_EMPTY(list)))
					{
					SetBubbleType(THINK_BUBBLE);
					txtGetFirstLine(BUSINESS_TXT, "GENUG", enough);
					ExpandObjectList(list, enough);

					while(ChoiceOk((choice1 = Bubble(list, choice1, 0L, 0L)), GET_OUT, list))
						{
						SetBubbleType(THINK_BUBBLE);

						objID = (ulong)(((struct ObjectNode*)GetNthNode(list,(ulong)choice1))->nr);
						ret   = Present(objID,"Building",InitBuildingPresent);
						}
					}
				else
				  Say(BUSINESS_TXT, 0, MATT_PICTID, "POOR");

				RemoveList(list);
				break;

			case 5:
				Present(0, "Beute", InitLootPresent);
				break;

			default:
				choice = 6;
				break;
			}
		}
	}


void Look(ulong locNr)
	{
	LIST  *menu = NULL, *bubble = NULL;
	ubyte choice=0 ,choice1=0;
	char enough[TXT_KEY_LENGTH];
	ulong objID;

	inpTurnESC(1);

	while(choice != 2)
		{
		menu = txtGoKey(THECLOU_TXT,"UMSEHEN");

		SetBubbleType(THINK_BUBBLE);
		choice = Bubble(menu,0,0L,0L);
		RemoveList(menu);

		choice1 = 0;

		switch(choice)
			{
			case 0:
				bubble = txtGoKey(HOUSEDESC_TXT,
								NODE_NAME (GetNthNode(film->loc_names,(locNr))));

				SetBubbleType(THINK_BUBBLE);
				choice1 = Bubble(bubble,0,0L,0L);
				RemoveList(bubble);
				break;

			case 1:
				objID = GetObjNrOfLocation(locNr);
				if(objID)
					{
					hasAll(objID , OLF_PRIVATE_LIST|OLF_INCLUDE_NAME|OLF_INSERT_STAR, Object_Person);
					bubble = ObjectListPrivate;

					if(!(LIST_EMPTY(bubble)))
						{
						txtGetFirstLine(BUSINESS_TXT, "GENUG_2", enough);
						ExpandObjectList(bubble, enough);

						SetBubbleType(THINK_BUBBLE);

						while(ChoiceOk((choice1 = Bubble(bubble,choice1,0L,0L)), GET_OUT, bubble))
							{
							objID  = ((struct ObjectNode *)GetNthNode(bubble,(ulong)choice1))->nr;

							tcDisplayInfoAboutPerson(objID);
							}
						}
					else
						Say(BUSINESS_TXT,0,MATT_PICTID,"NOBODY TO SEE");
					}
				else
					Say(BUSINESS_TXT,0,MATT_PICTID,"NOBODY TO SEE");

				RemoveList(bubble);
				break;
			default:
				choice = 2;
				break;
			}
		}
	}

ulong tcTelefon(void)
	{
	char connect[TXT_KEY_LENGTH];
	ulong eventNr = 0L;
	Person ben = (Person)dbGetObject(Person_Ben_Riggley);

	gfxShow(175, GFX_NO_REFRESH|GFX_ONE_STEP, 0, -1, -1);

	knowsAll(Person_Matt_Stuvysunt,OLF_INCLUDE_NAME|OLF_INSERT_STAR,Object_Person);

	if(!(LIST_EMPTY(ObjectList)))
		{
		ubyte choice = 0;
		ulong persID;

		txtGetFirstLine(BUSINESS_TXT, "DONT_CONNECT_ME", connect);
		ExpandObjectList(ObjectList, connect);

		inpTurnESC (0);
		choice = Say(THECLOU_TXT,0,MATT_PICTID,"CONNECT_ME");
		inpTurnESC (1);

		if (choice != GET_OUT)
			{
			if (ChoiceOk((choice = Bubble(ObjectList, 0, 0L, 0L)), GET_OUT, ObjectList))
				{
				persID =  (ulong)(((struct ObjectNode*)GetNthNode(ObjectList,(ulong)choice))->nr);

				if(persID == Person_Ben_Riggley)
					Say(BUSINESS_TXT,0,ben->PictID, "ALREADY_PHONING");

				if(livesIn(London_London_1, persID))
					{
					if(CalcRandomNr(0L,9L) == 3)
						Say(BUSINESS_TXT,0,ben->PictID, "OCCUPIED");
					else
						{
						inpTurnESC(0);
						DynamicTalk(Person_Matt_Stuvysunt,persID, DLG_TALKMODE_STANDARD);
						inpTurnESC(1);
						}
					}
				else
					Say(BUSINESS_TXT,0,ben->PictID, "NOBODY_AT_HOME");
				}
			}
		else
			choice = GET_OUT;	// 2014-06-28 LucyG : ??? Compiler warning "choice is assigned a value that is never used"
		}
	else
		Say(THECLOU_TXT,0,MATT_PICTID,"POOR_MATT");

	gfxShow(173, GFX_ONE_STEP|GFX_NO_REFRESH, 0, -1, -1);

	return(eventNr);
	}

void tcWait(void)
	{
	char line[TXT_KEY_LENGTH];
	ubyte ende = 0;
	ulong action=0L, minutes=0L, locNr = GetObjNrOfLocation(GetLocation);
	ulong newCount=0, oldCount=0;

	inpTurnESC(0);

	ShowMenuBackground();
	ShowTime(0);
	inpSetWaitTicks(25L);

	gfxSetRect(0, 320);
	gfxSetPens(m_wrp, 249, 0, GFX_SAME_PEN);
	txtGetFirstLine(THECLOU_TXT,"WAIT",line);
	gfxPrint(m_wrp,line,22,GFX_PRINT_CENTER);

	while((!ende) && (minutes<960))
		{
		minutes+=60;

		AddVTime(60L);
		ShowTime(0);

		if(locNr)
			{
			hasAll(locNr, OLF_NORMAL, Object_Person);

			newCount = GetNrOfNodes(ObjectList);

			if((newCount > oldCount) && (oldCount != 0))
				{
				SetBubbleType(THINK_BUBBLE);
				Say(THECLOU_TXT,0,MATT_PICTID,"NEW_PEOPLE");
				ShowTime(0);
				}

			oldCount = newCount;
			}

		action =  inpWaitFor(INP_TIME|INP_LBUTTONP|INP_RBUTTONP|INP_ESC);

		if ((action & INP_LBUTTONP) || (action & INP_RBUTTONP) ||
			 (action & INP_ESC))
				ende = 1;
		}

	if(minutes>=960)
		{
		SetBubbleType(THINK_BUBBLE);
		Say(THECLOU_TXT,0,MATT_PICTID,"WURZEL_SEPP");
		}

	inpSetWaitTicks(0L);
	ShowMenuBackground();
	tcRefreshLocationInTitle(GetLocation);

	inpTurnESC(1);
	ShowTime(0);
	}

