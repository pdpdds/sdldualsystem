#ifndef __TEXT_SYMBOL
#define __TEXT_SYMBOL

#define SYMBOL_HASH_SIZE 1024
#define MAXSYMBOLLENGTH 1024

#ifndef WIN32
#ifndef HAVE_STRLWR
char *strlwr(char *s);
#endif
#endif

/* This class must only be used from 'Symbol': */

class SymbolContainer
{

        friend class Symbol;

    public:
        SymbolContainer(char *str);
        ~SymbolContainer();

        char *get
        (void);

        bool cmp(char *str);

        bool save(FILE *fp);
        char *toString(void);

    private:
        static int hash_function(char *str);

        int m_reference_count;
        int m_hash;
        char *m_name;
}
; /* SymbolContainer */



class Symbol
{
    public:
        Symbol();
        Symbol(char *str);
        Symbol(Symbol &s);
        Symbol(Symbol *s);
        Symbol(FILE *fp);
        ~Symbol();

        static void reset(void);

        char *get
        (void);
        void set
            (char *str);

        bool cmp(char *str);
        bool cmp(Symbol *s);
        bool cmp(Symbol &s);
        bool operator==(Symbol &s);

        static void arrange_string(char *str);
        static int stats_nSymbols(void);
        static int stats_nSymbolReferences(void);
        static void stats_printSymbolStats(void);

        char *toString(void);
        int fromString(char *str);

        bool load(FILE *fp);
        bool save(FILE *fp);

        int hash_function(void);

    private:
        SymbolContainer *sym;

}
; /* Symbol */


#endif
