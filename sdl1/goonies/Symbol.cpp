#ifdef KITSCHY_DEBUG_MEMORY
#include "debug_memorymanager.h"
#endif

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "math.h" 
//#include "pthread.h"
#include "assert.h"

#include "List.h"

#include "Symbol.h"



List<SymbolContainer> *symbol_hash = 0;
//pthread_mutex_t symbol_hash_mutex=PTHREAD_MUTEX_INITIALIZER;


#ifndef WIN32
#include "ctype.h"
#ifndef HAVE_STRLWR

char *strlwr(char *s)
{
    if (!s)
        return NULL;
    while ( *s ) {
        *s = (char) tolower( *s );
        ++s;
    }
    return s;
}
#endif
#endif



void Symbol::reset(void)
{
    // pthread_mutex_lock(&symbol_hash_mutex);
    delete []symbol_hash;
    symbol_hash = 0;
    // pthread_mutex_unlock(&symbol_hash_mutex);
} /* Symbol::reset */


Symbol::Symbol()
{
    int hv = 0;
    List<SymbolContainer> l;
    SymbolContainer *s;

    // pthread_mutex_lock(&symbol_hash_mutex);
    if (symbol_hash == 0)
        symbol_hash = new List<SymbolContainer>[SYMBOL_HASH_SIZE];
    l.Instance(symbol_hash[hv]);
    l.Rewind();
    while (l.Iterate(s)) {
        if (s->m_name == 0) {
            s->m_reference_count++;
            //   assert(s->m_reference_count<32768);
            sym = s;
            //   pthread_mutex_unlock(&symbol_hash_mutex);
            return ;
        } /* if */
    } /* while */

    sym = new SymbolContainer(0);
    sym->m_reference_count++;
    symbol_hash[hv].Add(sym);
    // pthread_mutex_unlock(&symbol_hash_mutex);
} /* Symbol::Symbol */


Symbol::Symbol(char *str)
{
    int hv = SymbolContainer::hash_function(str);
    List<SymbolContainer> l;
    SymbolContainer *s;

    // pthread_mutex_lock(&symbol_hash_mutex);
    if (symbol_hash == 0)
        symbol_hash = new List<SymbolContainer>[SYMBOL_HASH_SIZE];
    l.Instance(symbol_hash[hv]);
    l.Rewind();
    while (l.Iterate(s)) {
        if (s->cmp(str)) {
            s->m_reference_count++;
            //   assert(s->m_reference_count<32768);
            sym = s;
            //   pthread_mutex_unlock(&symbol_hash_mutex);
            return ;
        } /* if */
    } /* while */

    sym = new SymbolContainer(str);
    sym->m_reference_count++;
    symbol_hash[hv].Add(sym);
    // pthread_mutex_unlock(&symbol_hash_mutex);
} /* Symbol::Symbol */


Symbol::Symbol(Symbol &s)
{
    sym = s.sym;
    sym->m_reference_count++;
} /* Symbol::Symbol */


Symbol::Symbol(Symbol *s)
{
    sym = s->sym;
    sym->m_reference_count++;
} /* Symbol::Symbol */


Symbol::Symbol(FILE *fp)
{
    sym = 0;

    load(fp);
} /* Symbol::Symbol */


Symbol::~Symbol()
{
    if (sym != 0) {
        assert(sym->m_reference_count > 0);
        sym->m_reference_count--;
        if (sym->m_reference_count <= 0) {
            //   pthread_mutex_lock(&symbol_hash_mutex);
            symbol_hash[SymbolContainer::hash_function(sym->m_name)].DeleteElement(sym);
            //   pthread_mutex_unlock(&symbol_hash_mutex);
            delete sym;
        } /* if */
    } /* if */

} /* Symbol::~Symbol */


char *Symbol::get
    (void)
{
    if (sym != 0)
        return sym->get
               ();
    return 0;
} /* Symbol::get */


void Symbol::set
    (char *str)
{
    int hv = SymbolContainer::hash_function(str);
    List<SymbolContainer> l;
    SymbolContainer *s;

    if (sym != 0) {
        sym->m_reference_count--;
        if (sym->m_reference_count <= 0) {
            //   pthread_mutex_lock(&symbol_hash_mutex);
            symbol_hash[SymbolContainer::hash_function(sym->m_name)].DeleteElement(sym);
            //   pthread_mutex_unlock(&symbol_hash_mutex);
            delete sym;
        } /* if */
    } /* if */

    // pthread_mutex_lock(&symbol_hash_mutex);
    if (symbol_hash == 0)
        symbol_hash = new List<SymbolContainer>[SYMBOL_HASH_SIZE];
    l.Instance(symbol_hash[hv]);
    l.Rewind();
    while (l.Iterate(s)) {
        if (s->cmp(str)) {
            s->m_reference_count++;
            sym = s;
            //   pthread_mutex_unlock(&symbol_hash_mutex);
            return ;
        } /* if */
    } /* while */

    sym = new SymbolContainer(str);
    sym->m_reference_count++;
    symbol_hash[hv].Add(sym);
    // pthread_mutex_unlock(&symbol_hash_mutex);
} /* Symbol::set */


bool Symbol::cmp(char *str)
{
    if (sym == 0)
        return false;
    if (str == sym->m_name)
        return true;
    if (str == 0 || sym->m_name == 0)
        return false;
    if (strcmp(str, sym->m_name) == 0)
        return true;
    return false;
} /* Symbol::cmp */


bool Symbol::cmp(Symbol *s)
{
    return sym == s->sym;
} /* Symbol::cmp */


bool Symbol::cmp(Symbol &s)
{
    return sym == s.sym;
} /* Symbol::cmp */


bool Symbol::operator==(Symbol &s)
{
    return sym == s.sym;
} /* Symbol::operator== */


void Symbol::arrange_string(char *str)
{
    int len;

    while (str[0] == ' ' || str[0] == '\n' || str[0] == '\r' ||
            str[0] == '\t')
        strcpy(str, str + 1);

    len = strlen(str);
    while (len > 1 &&
            (str[len - 1] == ' ' || str[len - 1] == '\n' || str[len - 1] == '\r' ||
             str[len - 1] == '\t')) {
        str[len - 1] = 0;
        len--;
    } /* while */
} /* Symbol::arrange_string */


bool Symbol::load(FILE *fp)
{
    char tmp[16];

    if (sym != 0) {
        sym->m_reference_count--;
        if (sym->m_reference_count <= 0) {
            //   pthread_mutex_lock(&symbol_hash_mutex);
            symbol_hash[SymbolContainer::hash_function(sym->m_name)].DeleteElement(sym);
            //   pthread_mutex_unlock(&symbol_hash_mutex);
            delete sym;
        } /* if */
    } /* if */
    sym = 0;

    if (1 != fscanf(fp, "%s", tmp))
        return false;
    if (strcmp(tmp, "SYM") == 0) {
        int i;
        char c;
        char str[MAXSYMBOLLENGTH];
        int hv;
        List<SymbolContainer> l;
        SymbolContainer *s;

        c = fgetc(fp);
        while (c == '\n' || c == ' ' || c == '\r')
            c = fgetc(fp);
        for (i = 0;c != '\n' && c != ' ' && c != '\r' && !feof(fp);i++, c = fgetc(fp)) {
            switch (c) {
                case '\\':
                    str[i] = fgetc(fp);
                    break;
                default:
                    str[i] = c;
            } /* switch */
        } /* for */
        str[i] = 0;

        hv = SymbolContainer::hash_function(str);

        //  pthread_mutex_lock(&symbol_hash_mutex);
        if (symbol_hash == 0)
            symbol_hash = new List<SymbolContainer>[SYMBOL_HASH_SIZE];
        l.Instance(symbol_hash[hv]);
        l.Rewind();
        while (l.Iterate(s)) {
            if (s->cmp(str)) {
                s->m_reference_count++;
                sym = s;
                //    pthread_mutex_unlock(&symbol_hash_mutex);
                return true;
            } /* if */
        } /* while */

        sym = new SymbolContainer(str);
        sym->m_reference_count++;
        symbol_hash[hv].Add(sym);
        //  pthread_mutex_unlock(&symbol_hash_mutex);
        return true;
    } /* if */

    if (strcmp(tmp, "NULLSYM") == 0) {
        sym = 0;
        return true;
    } /* if */

    return false;
} /* Symbol::load */


bool Symbol::save(FILE *fp)
{
    if (sym != 0) {
        fprintf(fp, "SYM\n");
        return sym->save(fp);
    } /* if */
    fprintf(fp, "NULLSYM\n");
    return true;
} /* Symbol::save */



char *Symbol::toString(void)
{
    char *tmp2;

    if (sym != 0) {
        char *tmp1;
        tmp1 = sym->toString();
        tmp2 = new char[strlen(tmp1) + 5];
        sprintf(tmp2, "SYM %s", tmp1);
        delete tmp1;
        return tmp2;
    } /* if */
    tmp2 = new char[8];
    sprintf(tmp2, "NULLSYM");
    return tmp2;
} /* Symbol::toString */


int Symbol::fromString(char *str)
{
    int pos = 0;
    int i;
    char tmp[16];

    if (sym != 0) {
        sym->m_reference_count--;
        if (sym->m_reference_count <= 0) {
            //   pthread_mutex_lock(&symbol_hash_mutex);
            symbol_hash[SymbolContainer::hash_function(sym->m_name)].DeleteElement(sym);
            //   pthread_mutex_unlock(&symbol_hash_mutex);
            delete sym;
        } /* if */
    } /* if */
    sym = 0;

    while (str[pos] == ' ' || str[pos] == '\n' || str[pos] == '\r' || str[pos] == '\t')
        pos++;

    i = 0;
    while (str[pos] != ' ' && str[pos] != '\n' && str[pos] != '\r' && str[pos] != '\t')
        tmp[i++] = str[pos++];
    tmp[i] = 0;

    if (strcmp(tmp, "SYM") == 0) {
        int i;
        char c;
        char res[MAXSYMBOLLENGTH];
        int hv;
        List<SymbolContainer> l;
        SymbolContainer *s;

        c = str[pos++];
        while (c == '\n' || c == ' ' || c == '\r' || c == '\t')
            c = str[pos++];
        for (i = 0;c != '\n' && c != ' ' && c != '\r' && c != '\t';i++, c = str[pos++]) {
            switch (c) {
                case '\\':
                    res[i] = str[pos++];
                    break;
                default:
                    res[i] = c;
            } /* switch */
        } /* for */
        res[i] = 0;

        hv = SymbolContainer::hash_function(res);

        //  pthread_mutex_lock(&symbol_hash_mutex);
        if (symbol_hash == 0)
            symbol_hash = new List<SymbolContainer>[SYMBOL_HASH_SIZE];
        l.Instance(symbol_hash[hv]);
        l.Rewind();
        while (l.Iterate(s)) {
            if (s->cmp(res)) {
                s->m_reference_count++;
                sym = s;
                //    pthread_mutex_unlock(&symbol_hash_mutex);
                return pos;
            } /* if */
        } /* while */

        sym = new SymbolContainer(res);
        sym->m_reference_count++;
        symbol_hash[hv].Add(sym);
        //  pthread_mutex_unlock(&symbol_hash_mutex);
        return pos;
    } /* if */

    if (strcmp(tmp, "NULLSYM") == 0) {
        sym = 0;
        return pos;
    } /* if */

    return -1;
} /* Symbol::fromString */


int Symbol::stats_nSymbols(void)
{
    int i;
    int c = 0;

    // pthread_mutex_lock(&symbol_hash_mutex);
    for (i = 0;i < SYMBOL_HASH_SIZE;i++) {
        c += symbol_hash[i].Length();
    } /* for */
    // pthread_mutex_unlock(&symbol_hash_mutex);

    return c;
} /* Symbol::stats_nSymbols */


int Symbol::stats_nSymbolReferences(void)
{
    int i;
    int c = 0;
    List<SymbolContainer> l;
    SymbolContainer *s;

    // pthread_mutex_lock(&symbol_hash_mutex);
    for (i = 0;i < SYMBOL_HASH_SIZE;i++) {
        l.Instance(symbol_hash[i]);
        while (l.Iterate(s)) {
            c += s->m_reference_count;
        } /* while */
    } /* for */
    // pthread_mutex_unlock(&symbol_hash_mutex);

    return c;
} /* Symbol::stats_nSymbolReferences */


void Symbol::stats_printSymbolStats(void)
{
    int i;
    int c = 0;
    List<SymbolContainer> l;
    SymbolContainer *s;

    // pthread_mutex_lock(&symbol_hash_mutex);
    for (i = 0;i < SYMBOL_HASH_SIZE;i++) {
        l.Instance(symbol_hash[i]);
        while (l.Iterate(s)) {
            printf("Symbol \"%s\" -> %i\n", s->get
                   (), s->m_reference_count);
            c += s->m_reference_count;
        } /* while */
    } /* for */
    // pthread_mutex_unlock(&symbol_hash_mutex);

    printf("Total: %i\n", c);
} /* Symbol::stats_printSymbolStats */




/* ---- SymbolCotnainer ---------------------------- */

SymbolContainer::SymbolContainer(char *str)
{
    if (str == 0) {
        m_reference_count = 0;
        m_name = 0;
        return ;
    } /* if */

    /* "Limpiar" el string: */
    char *nstr;
    int state = 0, i, j;
    char byte;

    nstr = new char[strlen(str) + 1];
    i = j = 0;
    do {
        byte = str[i++];
        switch (state) {
            case 0:
                if (byte != ' ' && byte != 10 && byte != 13) {
                    nstr[j++] = byte;
                    state = 1;
                } /* if */
                break;
            case 1:
                if (byte == ' ' || byte == 10 || byte == 13) {
                    //    if (byte==' ') {
                    nstr[j++] = byte;
                    //    } /* if */
                    state = 0;
                } else {
                    nstr[j++] = byte;
                } /* if */
                break;
        } /* switch */
    } while (byte != 0);

    m_reference_count = 0;
    m_name = new char[strlen(nstr) + 1];
    m_hash = hash_function(str);
    strcpy(m_name, nstr);
    delete []nstr;
} /* SymbolContainer::SymbolContainer */


SymbolContainer::~SymbolContainer()
{
    delete []m_name;
    m_name = 0;
} /* SymbolContainer::~SymbolContainer */


char *SymbolContainer::get
    (void)
{
    assert(m_reference_count > 0);
    return m_name;
} /* SymbolContainer::get */


bool SymbolContainer::cmp(char *str)
{
    if (str == m_name)
        return true;
    if (str == 0 || m_name == 0)
        return false;
    if (strcmp(str, m_name) == 0)
        return true;
    return false;
} /* SymbolContainer::cmp */


bool SymbolContainer::save(FILE *fp)
{
    int i;

    for (i = 0;m_name[i] != 0;i++) {
        switch (m_name[i]) {
            case ' ':
                fprintf(fp, "\\ ");
                break;
            case '\\':
                fprintf(fp, "\\\\");
                break;
            default:
                fprintf(fp, "%c", m_name[i]);
        } /* switch */
    } /* for */
    fprintf(fp, "\n");
    return true;
} /* SymbolContainer::save */


char *SymbolContainer::toString(void)
{
    int i, j;
    char *tmp1, *tmp2;

    j = 0;
    tmp1 = new char[strlen(m_name) * 2 + 1];

    for (i = 0;m_name[i] != 0;i++) {
        switch (m_name[i]) {
            case ' ':
                tmp1[j++] = '\\';
                tmp1[j++] = ' ';
                break;
            case '\\':
                tmp1[j++] = '\\';
                tmp1[j++] = '\\';
                break;
            default:
                tmp1[j++] = m_name[i];
        } /* switch */
    } /* for */
    tmp1[j++] = 0;
    tmp2 = new char[strlen(tmp1) + 1];
    strcpy(tmp2, tmp1);
    delete tmp1;
    return tmp2;
} /* SymbolContainer::toString */



int SymbolContainer::hash_function(char *str)
{
    int v = 0;
    int l = 0;

    if (str == 0)
        return 0;

    l = strlen(str);

    if (l > 0) {
        v += str[0] * 3;
        if (l > 1) {
            v += str[1] * 5;
            if (l > 2) {
                v += str[2] * 7;
                if (l > 3) {
                    v += str[3] * 11;
                    if (l > 4) {
                        v += str[4] * 13;
                        if (l > 5) {
                            v += str[5] * 17;
                            if (l > 6) {
                                v += str[6] * 19;
                                if (l > 7) {
                                    v += str[7] * 23;
                                    if (l > 8) {
                                        v += str[8] * 29;
                                        if (l > 9) {
                                            v += str[9] * 31;
                                            if (l > 10) {
                                                v += str[10] * 37;
                                                if (l > 11) {
                                                    v += str[11] * 41;
                                                    if (l > 12) {
                                                        v += str[12] * 43;
                                                    } /* if */
                                                } /* if */
                                            } /* if */
                                        } /* if */
                                    } /* if */
                                } /* if */
                            } /*  if */
                        } /* if */
                    } /* if */
                } /* if */
            } /* if */
        } /* if */
    } /* If */
    return v % SYMBOL_HASH_SIZE;
} /* SymbolContainer */


int Symbol::hash_function(void)
{
    return sym->m_hash;
} /* Symbol::hash_function */

