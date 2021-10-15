/*  _________  _______
   / ___/ __ \/ __/ _ \      Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/
#include "data\dataappl.h"

static long tcGetWeightOfNerves(long teamMood);
static long tcIsConnectedWithEnabledAlarm(ulong lsoId);

#define tcESCAPE_MOOD         30   /* ab hier fl?chtet einer ! */
#define tcWALK_LOUDNESS       20
#define tcPATROL_ALARM        5   /* ab 5 % ver?derte Objekte schl?t Patrolie Alarm */

#define tcPIXEL_PER_SECCOND   4

#define tcX_HOTSPOT           8

ubyte TeamMood = 127;

static long tcIsPlanPerfect(ulong timer);
static long tcGetGuyState(ulong persId);
static long tcInsideSameRoom(LIST *roomsList, word polX, word polY, word livX, word livY);

ubyte tcSpendMoney(long money,ubyte breakAnim)
	{
	ubyte enough=1;

	if(money > tcGetPlayerMoney)
		{
		enough = 0;

		if(breakAnim)
			StopAnim();

		SetBubbleType(THINK_BUBBLE);
		Say(BUSINESS_TXT,0,MATT_PICTID,"NO MONEY");
		}
	else
		tcSetPlayerMoney(tcGetPlayerMoney - money);

	return(enough);
	}

/* berechnet Auswirkung eines Funkspruches
 * setzt Auswirkungen automatisch
 * pro Funkspruch sinkt Wirkung um 30 Punkte (von 255) -> ab dem 6. Spruch
 * automatisch demotivierend.
 */
void tcCalcCallValue(ulong callNr, ulong timer, ulong persId)
	{
	long callCount  = Search.CallCount;
	long callWeight = ChangeAbs(255, callCount * (-30), 0, 255);
	long perfect    = tcIsPlanPerfect(timer), situation;
	long good       = -15, nerves;
	Person guy = NULL;

	  if(persId)
			 {
			 guy = (Person)dbGetObject(persId);
		nerves = 255 - guy->Panic;
			 }

	situation = (perfect * callWeight) / 255;

	switch(callNr)
		{
		case 0:	/* hervorragend! */
			if (situation > 200)
				good = situation - 200;
			break;
		case 1:   /* Streng Dich an! */
			if (situation > 80 && situation < 200)
				good = 15;
			break;
		case 2:  /* Beeile Dich sonst... */
			if ((nerves + situation > 235) && (situation < 80))
				good = nerves + situation - 235;
			break;
		case 3:	/* Beeile Dich, Du hast genug Zeit */
			if ((situation < 80) && (nerves + situation < 180))
				good = 15;
			break;
		case 4:   /* Du Trottel! */
			if ((perfect < 100) && (nerves > 200))
				good = 50;

					if (persId)
				guy->Known = CalcValue (guy->Known, 0, 255, 0, 20);
			break;
		default:
			break;
		}

	if((good > 0) && persId)
		guy->Known = CalcValue (guy->Known, 0, 255, 0, 6);

	Search.CallValue = ChangeAbs(Search.CallValue, good, -500, 500);
	}

/* Berechnet ��berschlagsm?sig den Ausgang der Flucht */
/* basiert alleine auf den Koordinaten */
/* vernachl?sigt Stockwerke und Personen */
long tcCalcEscapeTime()
	{
	long time = 0, i;
	Building build = (Building)dbGetObject(Search.BuildingId);

	for (i = 0; i < 4; i++)
		{
		if ((Search.GuyXPos[i] != -1) && (Search.GuyYPos[i] != -1))
			{
			long distance = abs((long) Search.GuyXPos[i] - (long) build->CarXPos);

			distance += abs((long) Search.GuyYPos[i] - (long) build->CarYPos);
			distance = CalcValue (distance, 0, 0xffff, 255, 20); /* um 20% mehr! */

			distance /= tcPIXEL_PER_SECCOND;

			time = max (distance, time);
			}
		}

	time = time / 4;    /* Fluchtzeit k?rzen - war zu lang! */

	return time;
	}

/* bestimmt den Ausgang eines Kampfes */
/* Werkzeug wirkt sich nur in Geschwindigkeit und Verletzung aus */
long tcKillTheGuard(ulong guyId, ulong buildingId)
	{
	Person p = (Person)dbGetObject(guyId);
	Building b = (Building)dbGetObject (buildingId);
	ulong power = hasGet(guyId, Ability_Kampf);

	if (power >=  b->GuardStrength)
		return 1;
	else
		{
		p->OldHealth = p->Health;
		p->Health = CalcValue (p->Health, 0, 255, 0, 90);
		}

	return 0;
	}

long tcGetCarStrike(Car car)
	{
	long strike = car->Strike;

	strike = strike + ((car->Strike * (car->ColorIndex - 5) * 9) / 255);

	strike = CalcValue (strike, 0, 255, 127 + abs(127 - car->BodyWorkState), 30);

	return strike;
	}

long tcGuyCanEscape(Person p)
	{
	long v = (p->Intelligence + 255 - p->Panic) / 2;

	v = CalcValue (v, 0, 255, 255-  p->KnownToPolice, 15);
	v = CalcValue (v, 0, 255, 255 - p->Popularity, 25);

	return v; /* the higher the better */
	}

long tcGuyTellsAll(Person p)
	{
	long v;

	v = (512 - p->Loyality - p->Known) / 2;

	return v; /* the higher the worser */
	}

long tcGetCarTraderOffer(Car car)
	{
	long offer  = tcGetCarPrice(car);
	Person marc = (Person)dbGetObject(Person_Marc_Smith);

	offer = CalcValue (offer, 1, 0xffff, 0, 25);      /* 25% weniger */
	offer = CalcValue (offer, 1, 0xffff, 126 + (marc->Known / 2), 30);

	return offer;
	}

ulong GetObjNrOfLocation(ulong LocNr)
	{
	struct dbObject *obj;
	Location loc;
	short objHashValue;

	for (objHashValue = 0; objHashValue < OBJ_HASH_SIZE; objHashValue++)
		{
		for (obj = (struct dbObject *)LIST_HEAD(objHash[objHashValue]); NODE_SUCC(obj); obj = (struct dbObject *)NODE_SUCC(obj))
			{
			if(obj->type == Object_Location)
				{
				loc=(Location)((void*)(obj+1));

				if(loc->LocationNr == LocNr)   return(obj->nr);
				}
			}
		}

	return(0L);
	}

ulong GetObjNrOfBuilding(ulong LocNr)
	{
	struct dbObject *obj;
	Building bui;
	short objHashValue;

	for (objHashValue = 0; objHashValue < OBJ_HASH_SIZE; objHashValue++)
		{
		for (obj = (struct dbObject *)LIST_HEAD(objHash[objHashValue]); NODE_SUCC(obj); obj = (struct dbObject *)NODE_SUCC(obj))
			{
			if(obj->type == Object_Building)
				{
				bui=(Building)((void*)(obj+1));

				if(bui->LocationNr == LocNr)   return(obj->nr);
				}
			}
		}

	return(0L);
	}

ulong tcGetPersOffer(Person person,ubyte persCount)
	{
	ulong persCapability=1,mattCapability=1;
	ulong persID = dbGetObjectNr(person);
	ulong assesment, offer, i;
	Person Pers;

	Pers = (Person) dbGetObject(persID);

	/* F�higkeiten der Person */
	if((i = hasGet(persID, Ability_Autos)) != NO_PARAMETER)
		persCapability += i;
	if((i = hasGet(persID, Ability_Sprengstoff)) != NO_PARAMETER)
		persCapability += i;
	if((i = hasGet(persID, Ability_Safes)) != NO_PARAMETER)
		persCapability += i;
	if((i = hasGet(persID, Ability_Elektronik)) != NO_PARAMETER)
		persCapability += i;
	if((i = hasGet(persID, Ability_Aufpassen)) != NO_PARAMETER)
		persCapability += i;
	if((i = hasGet(persID, Ability_Schloesser)) != NO_PARAMETER)
		persCapability += i;

	/* F�higkeiten von Matt */
	if((i = hasGet(Person_Matt_Stuvysunt, Ability_Autos)) != NO_PARAMETER)
		mattCapability += i;
	if((i = hasGet(Person_Matt_Stuvysunt, Ability_Sprengstoff)) != NO_PARAMETER)
		mattCapability += i;
	if((i = hasGet(Person_Matt_Stuvysunt, Ability_Safes)) != NO_PARAMETER)
		mattCapability += i;
	if((i = hasGet(Person_Matt_Stuvysunt, Ability_Elektronik)) != NO_PARAMETER)
		mattCapability += i;
	if((i = hasGet(Person_Matt_Stuvysunt, Ability_Aufpassen)) != NO_PARAMETER)
		mattCapability += i;
	if((i = hasGet(Person_Matt_Stuvysunt, Ability_Schloesser)) != NO_PARAMETER)
		mattCapability += i;

	/* wieviel Prozent Leistung er bringt (Matt = 100 %) */
	assesment = (persCapability * 100) / mattCapability;

	/* MOD 18-02-04 HG 92 statt 100 (Marx Testspielempfehlung) */
	offer = 100 / persCount;                             /* sein Anteil bei gleichen Teilen */
	offer = (offer * assesment) / 92;                    /* ... gewichtet mit F?igkeiten   */

	/* Beispiel zu oben: 2 Personen -> offer = 50, assesment = 200 */
	/* (andere Person ist "zweimal so gut") -> offer = 50 * 300 / 200 = 75 % */
	/* wieviel Prozent Habsucht (Matt = 100 %) */
	offer = CalcValue(offer, 1, 90, Pers->Avarice, 20);
	offer = CalcValue(offer, 1, 90, 255-Pers->Known, 10);

	return(offer);
	}

void tcPersonLearns(ulong pId)
	{
	struct ObjectNode *n;
	Person pers = (Person)dbGetObject(pId);
	long ability, count, growth;

	/* Abilites */
	hasAll(pId, OLF_NORMAL, Object_Ability);

	for (n = (struct ObjectNode *) LIST_HEAD(ObjectList); NODE_SUCC(n); n = (struct ObjectNode *) NODE_SUCC(n))
		{
		ability = hasGet(pId, OL_NR(n));   /* Jetztzustand der F?igkeit */

		if(learned(pId, OL_NR(n)))         /* er hat dazugelernt ! */
			{
			count = learnedGet(pId, OL_NR(n));      /* wie oft er gelernt hat ! */

			growth = 7 + count * 4;
			learnedUnSet(pId, OL_NR(n));            /* Gelerntes nicht nocheinmal lernen! */
			}
		else
			growth = 10;

		growth = max((ability / 10), growth);   /* Wachstum um das Minimum von
																	* x Punkten oder x Prozent    */

		if(ability + growth < 255)
			hasSetP(pId, OL_NR(n), ability + growth);
		else
			hasSetP(pId, OL_NR(n), 255);
		}

	  /* other attributes ! */

	if (pers->OldHealth)
		pers->Health = pers->OldHealth;    /* Verletzung ist kuriert */

	tcImprovePanic(pers, (1 + CalcRandomNr(4, 6)));    /* 25 bis 16 % besser ! */

	growth = max((pers->Known / (1 + CalcRandomNr(8, 10))), 20);
	tcImproveKnown(pers, pers->Known + ((pers->Known * growth) / 100));     /* 10 bis 12 % */
	}

ulong tcGetBuildValues(Building bui)
	{
	ulong v, x;

	x = (255 - bui->Exactlyness) / 3;

	v = CalcValue (bui->Values, 0, bui->Values + 500000L, CalcRandomNr(0, 255), x);

	return((ulong)(Round(v,3)));
	}

/*
 * Berechnet Teamstimmung
 * zu verwenden f��r Anzeige der Teamstimmung
 * 0 - 255
 */

long tcGetTeamMood(ulong *guyId, ulong timer)   /* ptr auf 4 ulongs */
	{
	long team = 0, mood = 0, i;

	/* Summe aus Einzelstimmungen */
	for (i = 0; (i < 4) && (guyId[i]); i++)
		{
		mood = tcGetPersMood((dbGetObject (guyId[i])));          /* Stimmung eines
														Einzelnen   */
		team += mood;
		}

	team /= i;
	TeamMood = CalcValue (team, 0, 255, (tcIsPlanPerfect(timer) * 20) / 35, 100);

	return ((long) TeamMood);
	}

/*
 * retourniert neuen Ersch?fungszustand eines Einbrechers
 * Aufzurufen, wenn Person Aktion durchf?hrt
 * nur alle x Aktionsschritte aufrufen !
 */

long tcGuyInAction(ulong persId, long exhaustion)
	{
	long state = tcGetGuyState(persId);

	if(CalcRandomNr(0, 15) == 1)
		state = (255 - state) / 90;   /* Ersch?fungszuwachs = Invers von Zustand */
	else
		state = 0;

	return ChangeAbs (exhaustion, state, 0, 255);
	}

/*
 * Setzt die Ersch?fungsabnahme eines Einbrechers
 * Aufzurufen, wenn Person wartet
 * nur alle x Warteschritte aufrufen !
 */

long tcGuyIsWaiting(ulong persId, long exhaustion)
	{
	long state = tcGetGuyState(persId);

	if(CalcRandomNr(0, 4) == 1)
		state = state / 10;   /* Ersch?fungsabnahme */
	else
		state = 0;

	state = max(10, state);

	return ChangeAbs (exhaustion, state * (-1), 0, 255);
	}

/*
 * 255 : Plan wird (zeitlich) zu 100% eingehalten
 */
/* bei jeder Warnung eines Aufpasser , wird dieser Wert nach unten */
/* korrigiert */
/* R?ckgabewert steigt mit Zunahme der Zeit (Einbrecher werden immer
	selbstsicherer
	und sinkt mit der Differenz der Realit? zum Plan!
	durch Funkspr?che diesen R?ckgabewert beeinflussen!
*/

static long tcIsPlanPerfect(ulong timer)
	{
	long perfect = (255 * ((long)(timer + 1) - (long)Search.DeriTime)) / ((long)(timer + 1));

	perfect = max(perfect, 0);
	perfect = ChangeAbs (perfect, Search.CallValue, 0, 255);
	perfect = ChangeAbs (perfect, Search.WarningCount * (-35), 0, 255);

	return perfect;
	}

/*
 * berechnet die Spuren
 *
 */

long tcGetTrail(Person p, ubyte which)
	{
	long nerves = tcGetWeightOfNerves(TeamMood) / 7;
	long trail;

	switch(which)
		{
		case 0:   /* Walk */
			trail = (255 - p->Skill) / 4;
			trail = CalcValue (trail, 0, 255, 255 - p->Stamina, 20);
			trail = CalcValue (trail, 0, 255, 255 - p->Health , 10);
			break;
		case 1:   /* Wait */
			trail = (p->Panic) / 4;   /* umso gr?ser Panik umso mehr Spuren! */
			break;
		case 2:   /* Work */
			trail = (255 - p->Skill) / 4;
			trail = CalcValue (trail, 0, 255, 255 - p->Stamina, 15);
			trail = CalcValue (trail, 0, 255, 255 - p->Health,  15);
			trail = CalcValue (trail, 0, 255, 255 - p->Strength, 5);

			if(!(has(Person_Matt_Stuvysunt, Tool_Handschuhe)))
				trail = CalcValue (trail, 0, 255, 255, 50);  /* um 50 % mehr ! */
			break;

		case 3:   /* Kill */
			trail = (255 - p->Strength) / 4;
			trail = CalcValue (trail, 0, 255, 255 - p->Stamina, 10);
			trail = CalcValue (trail, 0, 255, 255 - p->Health,  10);
			trail = CalcValue (trail, 0, 255, 255 - p->Skill, 20);
			break;

		default:
			break;
		}

	trail = CalcValue (trail, 0, 255, p->Panic, nerves);

	return trail;
	}

static ulong tcGetNecessaryAbility(ulong persId, ulong toolId)
	{
	ulong ability = 255;  /* andere Arbeiten optimal! */

	switch(toolId)
		{
		case Tool_Fusz:
		case Tool_Chloroform:
		case Tool_Hand:
			  ability =  hasGet(persId, Ability_Kampf);
			  break;
		case Tool_Kernbohrer:
		case Tool_Winkelschleifer:
		case Tool_Schneidbrenner:
		case Tool_Sauerstofflanze:
		case Tool_Stethoskop:
		case Tool_Elektrohammer:
			  ability = hasGet(persId, Ability_Safes);
			  break;
		case Tool_Bohrmaschine:
		case Tool_Bohrwinde:
		case Tool_Dietrich:
			  ability = hasGet(persId, Ability_Schloesser);
			  break;
		case Tool_Dynamit:
			  ability = hasGet(persId, Ability_Sprengstoff);
			  break;
		case Tool_Elektroset:
			  ability = hasGet(persId, Ability_Elektronik);
			  break;
		}

	return ability;
	}

/*
 * Berechnet Zeit, die Einbrecher f?r eine Aktion ben?igt
 */

ulong tcGuyUsesToolInPlayer(ulong persId, Building b, ulong toolId, ulong itemId, ulong needTime)
	{
	ulong time    = tcGuyUsesTool(persId, b, toolId, itemId);
	ulong ability = tcGetNecessaryAbility(persId, toolId);

	if (ability < (CalcRandomNr(0,230)))
		if (CalcRandomNr(0, ability / 20) == 1)
			time = CalcValue (time, 0, time * 4, ability / 2, 10);

	if (time < needTime)
		time = needTime;

	return time;
	}

ulong tcGuyUsesTool(ulong persId, Building b, ulong toolId, ulong itemId)
	/*
	 * diese Funktion darf keine Zuf?ligkeit enthalten -> Sync!!
	 */
	{
	ulong origin, time;
	Person p = (Person)dbGetObject(persId);

	origin = time = breakGet(itemId, toolId);

	if (time != -1)
		{
		switch(toolId)  /* Zeit varieren */
			{
			case Tool_Elektrohammer:
			case Tool_Hammer:
			case Tool_Axt:
				  time = CalcValue (time, 0, origin * 4, 127 + (255 - p->Strength) / 2, 5);
				  time = CalcValue (time, 0, origin * 4, 127 + (255 - p->Stamina)  / 2, 10);
				  break;
			case Tool_Hand:
			case Tool_Fusz:
			case Tool_Chloroform:
				  time = CalcValue (time, 0, origin * 4, 127 + (255 - p->Skill) / 2, 5);
				  time = CalcValue (time, 0, origin * 4, 127 + (255 - p->Strength) / 2, 10);
				  break;
			case Tool_Bohrwinde:
				  time = CalcValue (time, 0, origin * 4, 127 + (255 - p->Stamina) / 2, 5);
				  time = CalcValue (time, 0, origin * 4, 127 + (255 - p->Skill)   / 2, 10);
				  break;
			case Tool_Schloszstecher:
			case Tool_Glasschneider:
			case Tool_Bohrmaschine:
			case Tool_Brecheisen:
				  time = CalcValue (time, 0, origin * 4, 127 + (255 - p->Strength) / 2, 10);
				  time = CalcValue (time, 0, origin * 4, 127 + (255 - p->Skill) / 2, 10);
				  break;
			case Tool_Winkelschleifer:
			case Tool_Schneidbrenner:
			case Tool_Sauerstofflanze:
			case Tool_Kernbohrer:
				  time = CalcValue (time, 0, origin * 4, 127 + (255 - p->Strength) / 2, 0);
				  time = CalcValue (time, 0, origin * 4, 127 + (255 - p->Skill) / 2, 10);
				  break;
			case Tool_Dietrich:
			case Tool_Strickleiter:
				  time = CalcValue (time, 0, origin * 4, 127 + (255 - p->Skill) / 2, 10);
				  break;
			case Tool_Stethoskop:
			case Tool_Elektroset:
			case Tool_Dynamit:
				  time = CalcValue (time, 0, origin * 4, 127 + (255 - p->Intelligence) / 2, 10);
				  time = CalcValue (time, 0, origin * 4, 127 + (255 - p->Skill)        / 2 , 10);
				  break;
			default:break;
			}

		time = CalcValue(time, 0, origin * 4, 127 + ((255 - (tcGetNecessaryAbility(persId, toolId))) / 2), 50);

		switch(itemId)  /* Alarmanlagen bonus! */
			{
			case Item_Alarmanlage_Z3:
				  break;
			case Item_Alarmanlage_X3:
				  time = CalcValue (time, 0, origin * 4, 255, 30); break;
			case Item_Alarmanlage_Top:
				  time = CalcValue (time, 0, origin * 4, 255, 50); break;
			default:
				  break;
			}

		time = CalcValue(time, 0, origin * 4, 120 + (255 - b->Exactlyness) / 2, 20);
		time = CalcValue(time, 0, origin * 4, 127 + (p->Panic / 2), 10);

		time = max(origin, time);     /* Schneller kann man nicht sein! */
		}
	else
		time = 0; /* kein Tool oder kein Item angegeben! */

	return (time);
	}

/*
 * Berechnet Verletzungsgefahr sowie Verletzung
 * bei > 0 -> Funkspruch "Au! Ich habe mich verletzt!"
 */

long tcGetDanger(ulong persId, ulong toolId, ulong itemId)
	{
	Person p = (Person)dbGetObject(persId);
	long danger = hurtGet(itemId, toolId);

	danger = CalcValue (danger, 0, 255, 255 - p->Skill, 30);
	danger = CalcValue (danger, 0, 255, 255 - p->Stamina, 10);
	danger = CalcValue (danger, 0, 255, p->Panic, 5);

	if (danger > CalcRandomNr (40, 255))          /* Verletzt ! */
		{
		if (CalcRandomNr (0, 10) == 1)           /* ...oder vielleicht doch nicht */
			{
			p->OldHealth = p->Health;
			p->Health = CalcValue (p->Health, 0, 255, 127 - danger, 90);
			}
		else
			danger = 0;
		}
	else
		danger = 0;

	return danger;
	}

/*
 * berechnet die Lautst?ke einer Aktion
 *
 */

long tcGetToolLoudness(ulong persId, ulong toolId, ulong itemId)
	{
	Person p = (Person)dbGetObject(persId);
	long loudness = soundGet(itemId, toolId);

	loudness = CalcValue (loudness, 0, 255, 255 - p->Skill, 10);
	loudness = CalcValue (loudness, 0, 255, p->Panic, 5);

	return loudness;
	}

/*
 * berechnet die Lautst?ke vom Gehen
 */

long tcGetWalkLoudness(void)
	{
	long loudness = tcWALK_LOUDNESS;

	if(has(Person_Matt_Stuvysunt, Tool_Schuhe))  loudness /= 2;

	return loudness;
	}

/*
 * berechnet Lautst?kenpegel aller 4 Einbrecher
 */

long tcGetTotalLoudness(long loudp0, long loudp1, long loudp2, long loudp3)
	{
	long total;

	total = max(loudp0, loudp1);
	total = max(total, loudp2);
	total = max(total, loudp3);

	total = CalcValue (total, 0, 255, 255, 20);   /* um ca 20 % erh?en */

	return (total);
	}

/*
 * stellen fest, ob ein Funkspruch aufgefangen oder
 * ob durch die Lautst?ke ein Passant, Nachbar die Polizei alarmiert wurde
 * wenn diese Funktionen 1 zur?ckliefern, ab jetzt st?dig WatchDog
 * aufrufen
 */

/* am besten jede Sekunde aufrufen (loudness) */

long tcAlarmByLoudness(Building b, long totalLoudness)
	{
	if(totalLoudness > b->MaxVolume) return 1;
	else return 0;
	}

/* nach jedem Funkspruch aufrufen */
long tcAlarmByRadio(Building b)
	{
	long random = CalcRandomNr(0, 2500) + CalcRandomNr(0, 2500);         /* 10 mal funken bei Guarding = 250 -> Alarm */

	if(random < b->RadioGuarding)
		return 1;
	else
		return 0;
	}

long tcAlarmByPatrol(Building b, uword objChangedCount, uword totalCount, ubyte patrolCount)
	{
	/* Patrolien entdecken nun schneller etwas: 100 -> 125 */

	if(((totalCount * tcPATROL_ALARM) / (125 * patrolCount)) < objChangedCount)
		return 1;
	else
		return 0;
	}

/* f?r jedes Objekt, das vom W?hter kontrolliert wird, aufrufen! */
/* wenn 1 -> richtiger Alarm! */

long tcGuardChecksObject(LSObject lso)
{
	/* hier darf NICHT das OPEN_CLOSE_BIT sonst schl?t der W?hter */
	/* Alarm, wenn er eine T��r ?fnet!                              */
	if ((lso->ul_Status & (1<<Const_tcIN_PROGRESS_BIT)) ||
		(lso->ul_Status & (1<<Const_tcLOCK_UNLOCK_BIT)))
		return 1;
	else
	{
		switch(lso->Type)
		{
		case Item_Alarmanlage_Z3:
		case Item_Alarmanlage_X3:
		case Item_Alarmanlage_Top:
		case Item_Steuerkasten:
			/* hier darf NICHT das OPEN_CLOSE_BIT gepr?ft werden */
			/* sonst schl?t der W?hter Alarm, wenn er es ?fnet! */
			if (lso->ul_Status & (1 << Const_tcON_OFF))
				return 1;
			break;
		case Item_Bild:
		case Item_Vase:
		case Item_Gemaelde:
		case Item_Statue:
			if (lso->uch_Visible == 0)
				return 1;
			break;
		default:
			break;
		}
		if (bProfidisk)
		{
			switch(lso->Type)
			{
			case Item_Heiligenstatue:
			case Item_Kerzenstaender:
			case Item_Kirchenkreuz:

			case Item_Hottentotten_Figur:
			case Item_Batman_Figur:
			case Item_Dicker_Man:
			case Item_Unbekannter:
			case Item_Jack_the_Ripper_Figur:
			case Item_Koenigs_Figur:
			case Item_Wache_Figur:
			case Item_Miss_World_1952:
				if (lso->uch_Visible == 0)
					return 1;
				break;
			default:
				break;
			}
		}

	}

	return 0;
}

/* vor Beginn jeder Arbeit oder alle 32 Pixel die ein Einbrecher geht,
 * aufrufen!
 * loudness = tcGetToolLoudness oder tcWalkLoudness
 */

long tcAlarmByMicro(uword us_XPos, uword us_YPos, long loudness)
	{
	if (loudness > lsGetLoudness(us_XPos, us_YPos))   return 1;
	else return 0;
	}

/*
 * sobald ein Alarm Marke "Nachbar" existiert, f��r jeden Aufpasser, der gerade
 * wartet, diese Funktion aufrufen
 * wenn diese Funktion 1 retourniert -> Aufpasser hat etwas bemerkt ->
 * Warnung an Matt !
 */

long tcWatchDogWarning(ulong persId)
	{
	long watch = hasGet(persId, Ability_Aufpassen);
	long random;

	random = CalcRandomNr(0, 200) +    /* Joe soll nicht gleich in der ersten */
				CalcRandomNr(0, 200) +    /* Sekunde etwas bemerken!             */
				CalcRandomNr(0, 200);     /* Risiko wird durch Addition GR?SER!! */

	if ((watch > random) && (CalcRandomNr(0, 40) == 1))
		return 1;

	return 0;
	}

/*
 * sobald KEIN Alarm Marke "Nachbar" existiert, f?r jeden Aufpasser, der
 * gerade wartet, diese Funktion aufrufen (Fehlalarm)
 */

long tcWrongWatchDogWarning(ulong persId)
	{
	long watch = (long) hasGet(persId, Ability_Aufpassen);

	if(CalcRandomNr(0, 255) >  watch)   /* Irrtum */
		{
		if(CalcRandomNr(0, watch * 50) == 1)    /* umso besser, umso kleiner Wahrscheinlichkeit */
			return 1;
		}

	return 0;
	}

long tcIsCarRecognised(Car car, long time)
	{
	long strike = tcGetCarStrike(car);
	long weight;

	/* Zeit spielt eine Rolle ! */
	/* nach einer Viertelstunde mit Alarm, ist Strike == 255 */

	weight = 127 + CalcValue(time / 8, 0, 127, 0, 0); /* nur wegen Bereichs�berpr�fung */
	strike = CalcValue(strike, 0, 255, weight, 100);

	if ((strike > 220) || ((strike > (CalcRandomNr(10, 220) + CalcRandomNr(20, 220)))))
		return 1;
	else
		return 0;
	}

static long tcGetGuyState(ulong persId)
	{
	Person p = (Person)dbGetObject(persId);
	long state;

	state = tcGetPersHealth(p);
	state = CalcValue (state, 0, 255, p->Stamina,  50);
	state = CalcValue (state, 0, 255, p->Strength, 15);

	return state;
	}

long tcCalcMattsPart(void)
	{
	LIST *guys;
	NODE *node;
	long count = 0, part = 0;

	joined_byAll(Person_Matt_Stuvysunt, OLF_INCLUDE_NAME|OLF_PRIVATE_LIST, Object_Person);
	guys = ObjectListPrivate;

	count = GetNrOfNodes(guys);

	for(node=(NODE*)LIST_HEAD(guys);NODE_SUCC(node);node = (NODE*)NODE_SUCC(node))
		{
		if(OL_NR(node) != Person_Matt_Stuvysunt)
			part += tcGetPersOffer((Person)(OL_DATA(node)), count);
		}

	if(part > 99 )
		part = 99;

	RemoveList (guys);

	return 100 - part;
	}

/* immer wenn eine Stechuhr bet?igt wird */

void tcRefreshTimeClock(ulong buildId, ulong timerId)
	{
	ulong time = hasClockGet(buildId, timerId);

     ClockTimerSetP(timerId, timerId, time);
	}

/* jedes Sekunde aufrufen
 * wenn 1 -> Alarm durch Stechuhr!
 */

long tcCheckTimeClocks(ulong buildId)
	{
	NODE *n;
	long alarm = 0;

	/* alle Stechuhren holen */

     hasClockAll(buildId, OLF_NORMAL, Object_LSObject);

	/* und kontrollieren, ob die Zeit abgelaufen ist */

	for (n = (NODE *) LIST_HEAD(ObjectList); NODE_SUCC(n); n = (NODE *) NODE_SUCC(n))
		{
		ulong timerId = OL_NR(n);
		long time = (long) ClockTimerGet (timerId, timerId);

		if (time - 1)
               ClockTimerSetP(timerId, timerId, time - 1);
		else
			alarm = 1;
		}

	return alarm;
	}

/* stellt fest, ob 2 Positionen innerhalb des selben Raumes sind */

static long tcInsideSameRoom(LIST *roomsList, word polX, word polY, word livX, word livY)
     {
     NODE   *node;
	  long   detected = 0;
     LSRoom room;

     for (node = (NODE*) LIST_HEAD(roomsList); NODE_SUCC(node) && detected==0; node = (NODE*) NODE_SUCC(node))
          {
          room = (LSRoom)OL_DATA(node);

          if ((polX >= room->us_LeftEdge) && (polX <= room->us_LeftEdge + room->us_Width) &&
              (polY >= room->us_TopEdge ) && (polY <= room->us_TopEdge  + room->us_Height))
               {
               if ((livX >= room->us_LeftEdge) && (livX <= room->us_LeftEdge + room->us_Width) &&
						 (livY >= room->us_TopEdge ) && (livY <= room->us_TopEdge  + room->us_Height))
						  detected = 1;
               }
          }

     return detected;
     }

/* F�r jeden(?) Schritt einer f�r JEDEN Einbrecher aufrufen */
/* LivingName = names des Einbrechers */
/* XPos, YPos = Position des W�chters */
/* wenn 1 -> Alarm! */

long tcGuardDetectsGuy(LIST *roomsList, uword us_XPos, uword us_YPos, ubyte uch_ViewDirection, char *puch_GuardName, char *puch_LivingName)
	{
	ubyte detected = 0;
	uword livXPos = livGetXPos(puch_LivingName);
	uword livYPos = livGetYPos(puch_LivingName);

	if (livWhereIs(puch_GuardName) == livWhereIs(puch_LivingName))
		if (livIsPositionInViewDirection (us_XPos, us_YPos, livXPos, livYPos, uch_ViewDirection))
			if (tcInsideSameRoom(roomsList, us_XPos + tcX_HOTSPOT, us_YPos, livXPos + tcX_HOTSPOT, livYPos))
				detected = 1;

	return detected;
	}

/* jedesmal aufrufen, wenn ein Objekt bearbeitet wird */
/* �berpr�ft, ob dieses Objekt mit einer Alarmanlafe verbunden ist */
/* 1 -> Alarm */

long tcAlarmByTouch(ulong lsoId)
	{
	LSObject lso = (LSObject)dbGetObject(lsoId);

	if (lso->uch_Chained & Const_tcCHAINED_TO_ALARM)
		if (tcIsConnectedWithEnabledAlarm(lsoId)) return 1;

	return 0;
	}


/* kontrolliert, ob durch Stromverlust ein Alarm ausgel�st wird
 * jedesmal aufrufen, wenn ein Steuerkasten ausgeschaltet wird!
 * wenn 1 -> Alarm
 */

long tcAlarmByPowerLoss (ulong powerId)
	{
	LIST *friendlyList;
	NODE *n;

	/* alle Objekte, mit denen diese Stromversorgung verbunden ist */

	SetObjectListAttr (OLF_PRIVATE_LIST, Object_LSObject);
	AskAll (dbGetObject(powerId), hasPowerRelationID, BuildObjectList);
	friendlyList = ObjectListPrivate;

	/* eine davon mit einer Alarmanlage verbunden? */

	for (n = (NODE *) LIST_HEAD(friendlyList); NODE_SUCC(n); n = (NODE*) NODE_SUCC(n))
		{
		LSObject lso = (LSObject)OL_DATA(n);

		if (lso->ul_Status & Const_tcCHAINED_TO_ALARM)
			if (tcIsConnectedWithEnabledAlarm (OL_NR(n)))
				  {
						  RemoveList(friendlyList);
						  return 1;
				  }
		}

	  RemoveList(friendlyList);
	return 0;
	}

static long tcIsConnectedWithEnabledAlarm(ulong lsoId)
	{
	NODE *n;

	/* alle Alarmanlagen, mit denen dieser Gegenstand verbunden ist */

	SetObjectListAttr (OLF_NORMAL, Object_LSObject);
	AskAll (dbGetObject(lsoId), hasAlarmRelationID, BuildObjectList);

	/* ist eine davon eingeschalten? */

	for (n = (NODE *) LIST_HEAD(ObjectList); NODE_SUCC(n); n = (NODE*) NODE_SUCC(n))
		{
		LSObject alarm = (LSObject)OL_DATA(n);

			 if (!(alarm->ul_Status & (1L<<Const_tcON_OFF)))
			return 1;
		}

	return 0;
	}

static long tcGetWeightOfNerves(long teamMood)
	{
	return (255 - teamMood);
	}

void tcInsertGuard(LIST *list, LIST *roomsList, uword x, uword y, uword width, uword height, ulong guardId, ubyte livId, ulong areaId)
	{
	char name[TXT_KEY_LENGTH] = {0};
	uword gx, gy;
	ulong guardedArea = isGuardedbyGet(lsGetCurrBuildingID(), guardId);

	sprintf ((char*)name, "Police_%d", livId);

	gx = livGetXPos(name);
	gy = livGetYPos(name);

	if (guardedArea == areaId)
		if ((x <= (gx + 16))  && ((x+width) >= gx) && (y <= (gy + 23)) && ((y+height) >= gy))
			if (tcInsideSameRoom(roomsList, gx + tcX_HOTSPOT, gy, x + tcX_HOTSPOT, y))
				dbAddObjectNode (list, guardId, OLF_INCLUDE_NAME|OLF_INSERT_STAR);
	}
