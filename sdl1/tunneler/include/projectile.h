#ifndef _PROJECTILE_H_
#define _PROJECTILE_H_


typedef struct Projectile Projectile;
typedef struct PList PList;
typedef struct PListNode PListNode;


#include <level.h>
#include <tank.h>
#include <tanklist.h>

PList *plist_new() ;
void plist_destroy(PList *pl) ;

void plist_push_bullet(PList *pl, Tank *t) ;
void plist_push_explosion(PList *pl, unsigned x, unsigned y, unsigned count, unsigned r, unsigned ttl);

void plist_step(PList *pl, Level *b, TankList *tl) ;

void plist_clear(PList *pl, DrawBuffer *b) ;
void plist_draw(PList *pl, DrawBuffer *b) ;

#endif /* _PROJECTILE_H_ */

