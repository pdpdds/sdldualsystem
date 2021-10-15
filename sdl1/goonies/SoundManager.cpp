#ifdef KITSCHY_DEBUG_MEMORY
#include "debug_memorymanager.h"
#endif


#ifdef _WIN32
#include "windows.h"
#else
#include "dirent.h"
#endif

#include "GL/gl.h"
#include "GL/glu.h"
#include "SDL.h"
#include "SDL_mixer.h"

#include "stdlib.h"
#include "string.h"
#include "List.h"

#include "Symbol.h"
#include "sound.h"

#include "SoundManager.h"


List<SoundManager> m_sound_managers;


void channel_finished_cb(int channel)
{
	SoundManager *sm;

	m_sound_managers.Rewind();
 	while(m_sound_managers.Iterate(sm)) sm->channel_finished(channel);

} /* channel_finished_cb */ 


void SoundManager::channel_finished(int channel)
{
} /* SoundManager::channel_finished */ 




SoundManager::SoundManager()
{
    m_hash = new List<SoundManagerNode>[SYMBOL_HASH_SIZE];

	Mix_ChannelFinished(&channel_finished_cb);

	m_sound_managers.Add(this);

} /* SoundManager::SoundManager */


SoundManager::~SoundManager()
{
	Mix_ChannelFinished(0);

	m_sound_managers.DeleteElement(this);
} /* SoundManager::~SoundManager */


Mix_Chunk *SoundManager::get
    (char *name)
{
    Symbol *s = new Symbol(name);
    Mix_Chunk *t = get
                       (s);
    delete s;
    return t;
} /* SoundManager::get */


Mix_Chunk *SoundManager::get
    (Symbol *name)
{
    SoundManagerNode *n;
    int h = name->hash_function();

    m_hash[h].Rewind();
    while (m_hash[h].Iterate(n)) {
        if (n->m_name->cmp(name)) {
            return n->m_sfx;
        } // if
    } /* while */

    // load the sfx:
    {
        char filename[256];
        Mix_Chunk *t;

        sprintf(filename, "sound/%s", name->get());
        t = load_sound(filename);
        n = new SoundManagerNode();
        n->m_sfx = t;
        n->m_name = new Symbol(name);
        m_hash[h].Add(n);

        return t;
    }
} /* SoundManager::get */


// helper function for the cache
// filter the dir specified for only ogg/wav/mp3
#ifndef _WIN32
#ifdef __APPLE__
int filter_sfx(struct dirent *de)
{
#else
int filter_sfx(const struct dirent *de)
{
#endif //__APPLE__
    if (
        strcmp(".ogg", de->d_name + strlen(de->d_name) - 4) == 0 ||
        strcmp(".wav", de->d_name + strlen(de->d_name) - 4) == 0 ||
        strcmp(".mp3", de->d_name + strlen(de->d_name) - 4) == 0
    )
    {
        return (1);
    } else
    {
        return (0);
    }
}
#endif

// helper function for the cache;
// remove file extensions, since Santi's Sound.cpp doesn't like 'em
char *SoundManager::remove_extension(char *filename)
{
    char *p = strrchr(filename, '\\');
    if (p != NULL) {
        p = strrchr(p + 1, '.');
    } else {
        p = strrchr(filename, '.');
    }
    if (p != NULL) {
        *p = '\0';
    }
    return filename;
}

// read specified dir for sfx and cache them
void SoundManager::cache(char *localpath)
{
    // FIXME: unfortunately the entire game has a hardcoded use of the sound/ dir...
    char path[256];
    char *filename;
    sprintf(path, "sound/%s", localpath);

#ifdef _WIN32

    WIN32_FIND_DATA finfo;
    HANDLE h;
    char pattern[256], tmp[256];
    sprintf(pattern, "%s/*.*", path);

    h = FindFirstFile(pattern, &finfo);
    if (h != INVALID_HANDLE_VALUE) {
        strcpy(tmp, finfo.cFileName);
        while (FindNextFile(h, &finfo) == TRUE) {
            if (
                (
                    finfo.cFileName[strlen(finfo.cFileName) - 1] == 'g' &&
                    finfo.cFileName[strlen(finfo.cFileName) - 2] == 'g' &&
                    finfo.cFileName[strlen(finfo.cFileName) - 3] == 'o'
                ) ||
                (
                    finfo.cFileName[strlen(finfo.cFileName) - 1] == '3' &&
                    finfo.cFileName[strlen(finfo.cFileName) - 2] == 'p' &&
                    finfo.cFileName[strlen(finfo.cFileName) - 3] == 'm'
                ) ||
                (
                    finfo.cFileName[strlen(finfo.cFileName) - 1] == 'v' &&
                    finfo.cFileName[strlen(finfo.cFileName) - 2] == 'a' &&
                    finfo.cFileName[strlen(finfo.cFileName) - 3] == 'w'
                )
                &&
                finfo.cFileName[strlen(finfo.cFileName) - 4] == '.'
            ) {
                filename = remove_extension(finfo.cFileName);
                sprintf(tmp, "%s/%s", localpath, filename);
                get
                    (tmp);
            }
        }
    }
#else
    struct dirent **namelist;
    char tmp[512];
    int i, n;

    // get alpha sorted list of sfx names from "path"
    if ((n = scandir(path, &namelist, filter_sfx, alphasort)) > 0) {
        for (i = 0; i < n; i++) {
            filename = remove_extension(namelist[i]->d_name);
            snprintf(tmp, 512, "%s/%s", localpath, filename);
            get
                (tmp);
        }
        free(namelist);
    }
#endif
}


void SoundManager::next_cycle(void)
{
} /* SoundManager::next_cycle */ 


void SoundManager::object_destroyed_notification(void *object)
{
}

Mix_Chunk *SoundManager::load_sound(char *file)
{
    int n_ext = 6;
    char *ext[6] = {".WAV", ".OGG", ".MP3", ".wav", ".ogg", ".mp3"};
    char name[256];
    int i;

    for (i = 0; i < n_ext; i++) {
        strcpy(name, file);
        strcat(name, ext[i]);
        if (file_check(name)) {
            return Mix_LoadWAV(name);
        }
    }
#ifdef __DEBUG_MESSAGES
        output_debug_message("ERROR in Sound_create_sound(): Could not load sound file: %s.(wav|ogg|mp3)\n", file);
#endif
    exit(1);
} /* SoundManager::load_sound */ 


/* a check to see if file is readable and greater than zero */
int SoundManager::file_check(char *fname)
{
    FILE *fp;

    if ((fp = fopen(fname, "r")) != NULL) {
        if (fseek(fp, 0L, SEEK_END) == 0 && ftell(fp) > 0) {
            fclose(fp);
            return true;
        }
        /* either the file could not be read (== -1) or size was zero (== 0) */
#ifdef __DEBUG_MESSAGES
        output_debug_message("ERROR in file_check(): the file %s is corrupted.\n", fname);
#endif

        fclose(fp);
        exit(1);
    }
    return false;
} /* SoundManager::file_check */  


SoundManagerNode::~SoundManagerNode()
{
	if (m_sfx!=0) Mix_FreeChunk(m_sfx);
	m_sfx=0;
	if (m_name!=0) delete m_name;
	m_name=0;
} /* SoundManagerNode::~SoundManagerNode */ 


