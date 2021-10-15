#ifndef __GOONIES_SCRIPT
#define __GOONIES_SCRIPT

#define CSCRIPT_WRONG -1

#define GSCRIPT_CONTROL_WHEN  0
#define CSCRIPT_CONTROL_EVERYTIME 1

#define GSCRIPT_CONDITION_SEQUENCE 0
#define GSCRIPT_CONDITION_AND  1
#define GSCRIPT_CONDITION_OR  2
#define GSCRIPT_CONDITION_NOT  3
#define GSCRIPT_CONDITION_POSITION 4
#define GSCRIPT_CONDITION_PICKS  5
#define GSCRIPT_CONDITION_HAS  6
#define GSCRIPT_CONDITION_JUMPS  7
#define GSCRIPT_CONDITION_PUNCHES 8
#define GSCRIPT_CONDITION_SAVEDGOONIES 9
#define GSCRIPT_CONDITION_KILLEDENEMIES 10
#define GSCRIPT_CONDITION_HITBY  11
#define GSCRIPT_CONDITION_CAMEFROM 12
#define GSCRIPT_CONDITION_EXISTS 13
#define GSCRIPT_CONDITION_HIT  14

#define GSCRIPT_ACTION_NEW   0



/* tokens: */
#define TOKEN_LEFT_PAR  0
#define TOKEN_RIGHT_PAR  1
#define TOKEN_SYMBOL  2
#define TOKEN_INTEGER  3
#define TOKEN_STRING  4
#define TOKEN_FLOAT   5



class GScriptAction
{
    public:
        bool execute(class GO_character *character, class GMap *map, class GLTManager *GLTM, class SFXManager *SFXM, int m_sfx_volume);

        int m_type;
        List<char> m_string_parameters;
        List<int> m_int_parameters;
};

class GScriptCondition
{
    public:
        bool execute(GO_character *character, GMap *map, GLTManager *GLTM, SFXManager *SFXM, int m_sfx_volume);

        int m_type;
        int m_timer;
        List<char> m_string_parameters;
        List<int> m_int_parameters;
        List<GScriptCondition> m_cond_parameters;
};

class GScriptControl
{
    public:
        bool execute(GO_character *character, GMap *map, GLTManager *GLTM, SFXManager *SFXM, int m_sfx_volume);

        int m_type;
        GScriptCondition *m_condition;
        GScriptAction *m_action;
};


class GooniesScript
{
    public:
        GooniesScript(FILE *fp);
        ~GooniesScript();

        void execute(GO_character *character, GMap *map, GLTManager *GLTM, SFXManager *SFXM, int m_sfx_volume);

    protected:
        GScriptControl *parse_control(FILE *fp);
        GScriptCondition *parse_condition(FILE *fp);
        GScriptAction *parse_action(FILE *fp);
        char *getToken(FILE *fp, int *type);

        List<GScriptControl> m_scripts;

};

#endif
