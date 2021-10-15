#ifdef KITSCHY_DEBUG_MEMORY
#include "debug_memorymanager.h"
#endif

#ifdef _WIN32
#include <windows.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include "math.h"
#include "string.h"

#include "GL/gl.h"
#include "GL/glu.h"
#include "SDL.h"
#include "SDL_mixer.h"

#include "List.h"
#include "Symbol.h"
#include "2DCMC.h"
#include "auxiliar.h"
#include "GLTile.h"
#include "2DCMC.h"
#include "keyboardstate.h"
#include "randomc.h"
#include "VirtualController.h"

#include "GLTManager.h"
#include "SoundManager.h"
#include "SFXManager.h"
#include "MusicManager.h"
#include "GObject.h"
#include "GO_character.h"
#include "GO_skulldoor.h"
#include "GO_enemy.h"
#include "GO_fratelli.h"
#include "GooniesScript.h"
#include "GMap.h"
#include "TheGoonies.h"
#include "TheGooniesApp.h"

#include "GObjectCreator.h"

#include "font_extractor.h"

#include "debug.h"


GooniesScript::GooniesScript(FILE *fp)
{
    GScriptControl *c = 0;

    do {
        c = parse_control(fp);
        if (c != 0)
            m_scripts.Add(c);
    } while (c != 0);
} /* GooniesScript::GooniesScript */


GooniesScript::~GooniesScript()
{} /* GooniesScript::~GooniesScript */


void GooniesScript::execute(GO_character *character, GMap *map, GLTManager *GLTM, SFXManager *SFXM, int m_sfx_volume)
{
    GScriptControl *c;
    List<GScriptControl> to_delete;

    m_scripts.Rewind();
    while (m_scripts.Iterate(c)) {
        if (!c->execute(character, map, GLTM, SFXM, m_sfx_volume))
            to_delete.Add(c);
    } // while

    while (!to_delete.EmptyP()) {
        c = to_delete.ExtractIni();
        m_scripts.DeleteElement(c);
    } // while
} /* GooniesScript::execute */


bool GScriptAction::execute(GO_character *character, GMap *map, GLTManager *GLTM, SFXManager *SFXM, int m_sfx_volume)
{
    switch (m_type) {
        case GSCRIPT_ACTION_NEW: {
                GObject *o;
                int *i;
                List<int> l;
                Symbol *s = new Symbol(m_string_parameters[0]);
                m_int_parameters.Rewind();
                m_int_parameters.Next();
                m_int_parameters.Next();
                while (m_int_parameters.Iterate(i))
                    l.Add(new int(*i));
                o = GObject_create(s, (*(m_int_parameters[0])) * 20, (*(m_int_parameters[1])) * 20, m_sfx_volume, &l);
                map->add_object(o, 2);
                SFXM->SFX_play("sfx/item_appear", m_sfx_volume, o->get_angle(map), o->get_distance(map));
            }
            break;
        default:
            break;
    } // switch
    return true;
} /* GScriptAction::execute */


bool GScriptCondition::execute(GO_character *character, GMap *map, GLTManager *GLTM, SFXManager *SFXM, int m_sfx_volume)
{
    switch (m_type) {
        case GSCRIPT_CONDITION_SEQUENCE:
            if (m_timer == 0)
                m_cond_parameters.Rewind();
            if (m_cond_parameters.EndP()) {
                m_timer = 0;
                return true;
            } else {
                if (m_timer < 50) {
                    GScriptCondition *c = m_cond_parameters.GetObj();
                    if (c->execute(character, map, GLTM, SFXM, m_sfx_volume)) {
                        m_cond_parameters.Next();
                        m_timer = 1;
                    } else {
                        m_timer++;
                    } // if
                } else {
                    m_timer = 0;
                } // if
            } // if
            break;
        case GSCRIPT_CONDITION_AND: {
                GScriptCondition *c;
                m_cond_parameters.Rewind();
                while (m_cond_parameters.Iterate(c))
                    if (!c->execute(character, map, GLTM, SFXM, m_sfx_volume))
                        return false;
                return true;
            }
            break;
        case GSCRIPT_CONDITION_OR: {
                GScriptCondition *c;
                m_cond_parameters.Rewind();
                while (m_cond_parameters.Iterate(c))
                    if (c->execute(character, map, GLTM, SFXM, m_sfx_volume))
                        return true;
                return false;
            }
            break;
        case GSCRIPT_CONDITION_NOT:
            if (!m_cond_parameters[0]->execute(character, map, GLTM, SFXM, m_sfx_volume))
                return true;
            break;
        case GSCRIPT_CONDITION_POSITION:
            if (character->get_x() >= (*(m_int_parameters[0]))*20 &&
                    character->get_y() >= (*(m_int_parameters[1]))*20 &&
                    character->get_x() < (*(m_int_parameters[2]))*20 &&
                    character->get_y() < (*(m_int_parameters[3]))*20)
                return true;
            break;
        case GSCRIPT_CONDITION_PICKS:
            if (character->get_last_pick() != 0 &&
                    character->get_last_pick()->cmp(m_string_parameters[0]))
                return true;
            break;
        case GSCRIPT_CONDITION_HAS:
            if (character->player_has(m_string_parameters[0]))
                return true;
            break;
        case GSCRIPT_CONDITION_JUMPS:
            if (character->get_state() != character->get_last_state() &&
                    (character->get_state() == CSTATE_JUMPING_LEFT ||
                     character->get_state() == CSTATE_JUMPING_RIGHT ||
                     character->get_state() == CSTATE_JUMPING_LEFT_LEFT ||
                     character->get_state() == CSTATE_JUMPING_RIGHT_RIGHT))
                return true;
            break;
        case GSCRIPT_CONDITION_PUNCHES:
            if (character->get_state() != character->get_last_state() &&
                    (character->get_state() == CSTATE_PUNCH_LEFT ||
                     character->get_state() == CSTATE_PUNCH_RIGHT ||
                     character->get_state() == CSTATE_JUMPPUNCH_LEFT ||
                     character->get_state() == CSTATE_JUMPPUNCH_RIGHT ||
                     character->get_state() == CSTATE_JUMPPUNCH_LEFT_LEFT ||
                     character->get_state() == CSTATE_JUMPPUNCH_RIGHT_RIGHT)) {
                if (m_string_parameters.EmptyP()) {
                    return true;
                } else {
                    if (character->get_last_hit() != 0 &&
                            character->get_last_hit()->cmp(m_string_parameters[0]))
                        return true;
                } // if
            } // if
            break;
        case GSCRIPT_CONDITION_SAVEDGOONIES:
            if (character->get_rescued_goonies() == 7)
                return true;
            break;
        case GSCRIPT_CONDITION_KILLEDENEMIES:
            if (map->get_object("OB_bat") == 0 &&
                    map->get_object("OB_skull") == 0 &&
                    map->get_object("OB_jumpingskull") == 0 &&
                    map->get_object("OB_trickyskull") == 0 &&
                    map->get_object("OB_skeleton") == 0)
                return true;
            break;
        case GSCRIPT_CONDITION_HIT:
            if (character->get_last_hit() != 0 &&
                    character->get_last_hit()->cmp(m_string_parameters[0]))
                return true;
            break;
        case GSCRIPT_CONDITION_HITBY:
            if (character->get_last_hit_by() != 0 &&
                    character->get_last_hit_by()->cmp(m_string_parameters[0]))
                return true;
            break;
        case GSCRIPT_CONDITION_CAMEFROM:
            if (character->get_camefrom() != 0 &&
                    character->get_camefrom()->cmp(m_string_parameters[0]))
                return true;
            break;
        case GSCRIPT_CONDITION_EXISTS:
            if (map->get_object(m_string_parameters[0]) != 0)
                return true;
            break;
    } // switch
    return false;
} /* GScriptCondition::execute */


bool GScriptControl::execute(GO_character *character, GMap *map, GLTManager *GLTM, SFXManager *SFXM, int m_sfx_volume)
{
    switch (m_type) {
        case GSCRIPT_CONTROL_WHEN:
            if (m_condition->execute(character, map, GLTM, SFXM, m_sfx_volume)) {
                m_action->execute(character, map, GLTM, SFXM, m_sfx_volume);
                return false;
            } // if
            break;
        case CSCRIPT_CONTROL_EVERYTIME:
            if (m_condition->execute(character, map, GLTM, SFXM, m_sfx_volume)) {
                m_action->execute(character, map, GLTM, SFXM, m_sfx_volume);
                return true;
            } // if
            break;
    } // switch
    return true;
} /* GScriptControl::execute */


/*
   - control:
  - everytime c a
  - when c a
*/

/*
 - examples:
  when and(sequence( and( punches() position(10 10 20 20)) 
         and( punches() position(10 10 20 20)))
     has(GO_lamp))
    new(GO_item 12 10 1)
  when picks(GO_key) new(GO_item 9 8 2)
  everytime sequence(punches() punches()) new(GO_item 15 8 2)
 
  */

GScriptControl *GooniesScript::parse_control(FILE *fp)
{
    int token_type;
    char *token;
    GScriptControl *res;
    GScriptCondition *c;
    GScriptAction *a;

    token = getToken(fp, &token_type);

    if (token_type != TOKEN_SYMBOL) {
        if (token != 0)
            delete []token;
        return 0;
    } // if

    if (strcmp(token, "when") == 0) {
        delete []token;
        token = 0;
        c = parse_condition(fp);
        if (c != 0) {
            a = parse_action(fp);
            if (a != 0) {
                res = new GScriptControl();
                res->m_type = GSCRIPT_CONTROL_WHEN;
                res->m_condition = c;
                res->m_action = a;
                return res;
            } // if
        } // if
    } else {
        if (strcmp(token, "everytime") == 0) {
            delete []token;
            token = 0;
            c = parse_condition(fp);
            if (c != 0) {
                a = parse_action(fp);
                if (a != 0) {
                    res = new GScriptControl();
                    res->m_type = CSCRIPT_CONTROL_EVERYTIME;
                    res->m_condition = c;
                    res->m_action = a;
                    return res;
                } // if
            } // if
        } // if
    } // if

    if (token != 0)
        delete []token;
    return 0;
} /* GooniesScript::parse_control */


/*
 - condition:
  - (c)
  - and(c1 c2 ... endc)
  - or(c1 c2 ... endc)
  - not(c)
  - sequence(c1 c2 ... ends)
  - position(x1 y1 x2 y2)
  - picks(x)
  - has(x)
  - jumps()
  - punches(x)
  - allgooniessaved()
  - allenemieskilled()
  - hitby(x)
  - hit(x)
  - camefrom(left|right|up|down)
  - exists(x)
*/

GScriptCondition *GooniesScript::parse_condition(FILE *fp)
{
    int token_type;
    char *token;
    GScriptCondition *c;

    token = getToken(fp, &token_type);
    if (token_type == TOKEN_LEFT_PAR) {
        delete []token;
        token = 0;

        c = parse_condition(fp);

        token = getToken(fp, &token_type);
        if (token_type == TOKEN_RIGHT_PAR) {
            delete []token;
            return c;
        } else {
            delete c;
        } // if
    } else {
        if (token_type == TOKEN_SYMBOL) {
            if (strcmp(token, "and") == 0 ||
                    strcmp(token, "or") == 0 ||
                    strcmp(token, "not") == 0 ||
                    strcmp(token, "sequence") == 0) {
                int type = CSCRIPT_WRONG;
                if (strcmp(token, "and") == 0)
                    type = GSCRIPT_CONDITION_AND;
                if (strcmp(token, "or") == 0)
                    type = GSCRIPT_CONDITION_OR;
                if (strcmp(token, "not") == 0)
                    type = GSCRIPT_CONDITION_NOT;
                if (strcmp(token, "sequence") == 0)
                    type = GSCRIPT_CONDITION_SEQUENCE;

                if (type != CSCRIPT_WRONG) {
                    if (token != 0)
                        delete []token;
                    token = getToken(fp, &token_type);
                    if (token_type == TOKEN_LEFT_PAR) {
                        List<GScriptCondition> m_cond;
                        long pos;
                        do {
                            pos = ftell(fp);
                            c = parse_condition(fp);
                            if (c == 0)
                                fseek(fp, pos, SEEK_SET);
                            if (c != 0)
                                m_cond.Add(c);
                        } while (c != 0);
                        if (token != 0)
                            delete []token;
                        token = 0;
                        token = getToken(fp, &token_type);

                        if (token_type == TOKEN_RIGHT_PAR) {
                            if (token != 0)
                                delete []token;
                            c = new GScriptCondition();
                            c->m_type = type;
                            c->m_timer = 0;
                            while (!m_cond.EmptyP())
                                c->m_cond_parameters.Add(m_cond.ExtractIni());
                            return c;
                        } // if
                    } // if
                } // if
            } else {
                int type = CSCRIPT_WRONG;
                if (strcmp(token, "position") == 0)
                    type = GSCRIPT_CONDITION_POSITION;
                if (strcmp(token, "picks") == 0)
                    type = GSCRIPT_CONDITION_PICKS;
                if (strcmp(token, "has") == 0)
                    type = GSCRIPT_CONDITION_HAS;
                if (strcmp(token, "jumps") == 0)
                    type = GSCRIPT_CONDITION_JUMPS;
                if (strcmp(token, "punches") == 0)
                    type = GSCRIPT_CONDITION_PUNCHES;
                if (strcmp(token, "allgooniessaved") == 0)
                    type = GSCRIPT_CONDITION_SAVEDGOONIES;
                if (strcmp(token, "allenemieskilled") == 0)
                    type = GSCRIPT_CONDITION_KILLEDENEMIES;
                if (strcmp(token, "hitby") == 0)
                    type = GSCRIPT_CONDITION_HITBY;
                if (strcmp(token, "camefrom") == 0)
                    type = GSCRIPT_CONDITION_CAMEFROM;
                if (strcmp(token, "exists") == 0)
                    type = GSCRIPT_CONDITION_EXISTS;
                if (strcmp(token, "hit") == 0)
                    type = GSCRIPT_CONDITION_HIT;

                if (type != CSCRIPT_WRONG) {
                    if (token != 0)
                        delete []token;
                    token = getToken(fp, &token_type);
                    if (token_type == TOKEN_LEFT_PAR) {
                        List<int> m_int;
                        List<char> m_string;
                        do {
                            if (token != 0)
                                delete []token;
                            token = getToken(fp, &token_type);
                            if (token_type == TOKEN_INTEGER) {
                                m_int.Add(new int(atoi(token)));
                            } // if
                            if (token_type == TOKEN_FLOAT) {
                                m_int.Add(new int(int(atof(token))));
                            } // if
                            if (token_type == TOKEN_SYMBOL) {
                                char *tmp = new char[strlen(token) + 1];
                                strcpy(tmp, token);
                                m_string.Add(tmp);
                            } // if
                        } while (token != 0 && token_type != TOKEN_RIGHT_PAR);
                        if (token != 0)
                            delete []token;
                        token = 0;

                        if (token_type == TOKEN_RIGHT_PAR) {
                            if (token != 0)
                                delete []token;
                            token = 0;
                            c = new GScriptCondition();
                            c->m_type = type;
                            c->m_timer = 0;
                            while (!m_int.EmptyP())
                                c->m_int_parameters.Add(m_int.ExtractIni());
                            while (!m_string.EmptyP())
                                c->m_string_parameters.Add(m_string.ExtractIni());
                            return c;
                        } // if
                    } // if
                } // if
            } // if
        } // if
    } // if

    if (token != 0)
        delete []token;
    return 0;
} /* GooniesScript::parse_condition */


/*
 - actions
  - new(o,x,y,p1 p2 ...)
*/

GScriptAction *GooniesScript::parse_action(FILE *fp)
{
    int token_type;
    char *token;
    GScriptAction *a;

    token = getToken(fp, &token_type);

    if (token_type != TOKEN_SYMBOL) {
        if (token != 0)
            delete []token;
        return 0;
    } // if

    if (strcmp(token, "new") == 0) {
        if (token != 0)
            delete []token;
        token = getToken(fp, &token_type);
        if (token_type == TOKEN_LEFT_PAR) {
            List<int> m_int;
            List<char> m_string;
            do {
                if (token != 0)
                    delete []token;
                token = getToken(fp, &token_type);
                if (token_type == TOKEN_INTEGER) {
                    m_int.Add(new int(atoi(token)));
                } // if
                if (token_type == TOKEN_FLOAT) {
                    m_int.Add(new int(int(atof(token))));
                } // if
                if (token_type == TOKEN_SYMBOL) {
                    char *tmp = new char[strlen(token) + 1];
                    strcpy(tmp, token);
                    m_string.Add(tmp);
                } // if
            } while (token != 0 && token_type != TOKEN_RIGHT_PAR);
            if (token != 0)
                delete []token;
            token = 0;

            if (token_type == TOKEN_RIGHT_PAR) {
                if (token != 0)
                    delete []token;
                token = 0;
                a = new GScriptAction();
                a->m_type = GSCRIPT_ACTION_NEW;
                while (!m_int.EmptyP())
                    a->m_int_parameters.Add(m_int.ExtractIni());
                while (!m_string.EmptyP())
                    a->m_string_parameters.Add(m_string.ExtractIni());
                return a;
            } // if
        } // if
    } // if

    if (token != 0)
        delete []token;
    return 0;
} /* GooniesScript::parse_action */



char *GooniesScript::getToken(FILE *fp, int *type)
{
    char tmp[256], *token;
    int i = 0;
    char c;
    bool end = false;
    int state;

    *type = -1;

    state = 0;

    while (!end) {
        c = fgetc(fp);

        if (!feof(fp)) {
            switch (state) {
                case - 2:  /* block comment: */
                    if (c == '|') {
                        c = fgetc(fp);
                        if (c == '#')
                            state = 0;
                    } /* if */
                    break;
                case - 1:  /* comment */
                    if (c == '\r' || c == '\n')
                        state = 0;
                    break;
                case 0:
                    if (c == ';') {
                        state = -1;
                    } else if (c == '#') {
                        c = fgetc(fp);
                        if (c == '|') {
                            state = -2;
                        } else {
                            *type = -1;
                            return 0;
                        } /* if */
                    } else if (c == '(' || c == ')') {
                        tmp[i++] = c;
                        if (c == '(')
                            *type = TOKEN_LEFT_PAR;
                        else
                            *type = TOKEN_RIGHT_PAR;
                        end = true;
                    } else if (c == '\"') {
                        state = 3;
                    } else if ((c >= '0' && c <= '9') || c == '-') {
                        *type = TOKEN_INTEGER;
                        tmp[i++] = c;
                        state = 2;
                    } else if (c != ' ' && c != '\r' && c != '\n' && c != '\t') {
                        tmp[i++] = c;
                        state = 1;
                    } /* if */
                    break;
                case 1:
                    if (c != ' ' && c != '\r' && c != '\n' && c != '\t' &&
                            c != '(' && c != ')' && c != '\"') {
                        tmp[i++] = c;
                    } else {
                        ungetc(c, fp);
                        *type = TOKEN_SYMBOL;
                        end = true;
                    } /* if */
                    break;
                case 2:
                    if ((c >= '0' && c <= '9') || c == '.') {
                        tmp[i++] = c;
                        if (c == '.')
                            *type = TOKEN_FLOAT;
                    } else {
                        ungetc(c, fp);
                        end = true;
                    } /* if */
                    break;
                case 3:
                    if (c != '\"') {
                        tmp[i++] = c;
                    } else {
                        *type = TOKEN_STRING;
                        end = true;
                    } /* if */
                    break;
                case 4:
                    if (c != ' ' && c != '\r' && c != '\n' && c != '\t' &&
                            c != '(' && c != ')' && c != '\"') {
                        tmp[i++] = c;
                    } else {
                        ungetc(c, fp);
                        end = true;
                    } /* if */
                    break;
            } /* switch */
        } else {
            end = true;
        } /* if */
    } /* while */

    tmp[i] = 0;
    if (strlen(tmp) == 0 && *type != TOKEN_STRING) {
        *type = -1;
        return 0;
    } /* if */

    token = new char[strlen(tmp) + 1];
    strcpy(token, tmp);
    // if (*type!=TOKEN_STRING) strlwr(token);

    return token;
} /* GooniesScript::getTokenNOOS */

