/* Nework game is not supported yet in U-hexen */
#include "h2def.h"

void I_InitNetwork (void)
{ 
    doomcom=malloc(sizeof (*doomcom));
    memset(doomcom,0,sizeof(*doomcom));
    doomcom->id=DOOMCOM_ID;
    doomcom->numplayers=doomcom->numnodes=1;
    doomcom->ticdup=1;
}


void I_NetCmd (void)
{

}

