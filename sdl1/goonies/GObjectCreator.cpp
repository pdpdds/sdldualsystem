#ifdef KITSCHY_DEBUG_MEMORY
#include "debug_memorymanager.h"
#endif


#ifdef _WIN32
#include "windows.h"
#endif

#include "stdlib.h"
#include "string.h"
#include "assert.h"

#include "GL/gl.h"
#include "GL/glu.h"
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_mixer.h"

#include "List.h"

#include "auxiliar.h"
#include "2DCMC.h"
#include "Symbol.h"
#include "GLTile.h"
#include "keyboardstate.h"

#include "GLTManager.h"
#include "SoundManager.h"
#include "SFXManager.h"
#include "MusicManager.h"

#include "GObject.h"
#include "GO_enemy.h"
#include "GO_key.h"
#include "GO_cagedoor.h"
#include "GO_fallingrock.h"
#include "GO_skull.h"
#include "GO_jumpingskull.h"
#include "GO_trickyskull.h"
#include "GO_drop.h"
#include "GO_dropgenerator.h"
#include "GO_skulldoor.h"
#include "GO_wateropening.h"
#include "GO_entrydoor.h"
#include "GO_exitdoor.h"
#include "GO_character.h"
#include "GO_rope.h"
#include "GO_water.h"
#include "GO_lava.h"
#include "GO_bigrock.h"
#include "GO_bat.h"
#include "GO_skeleton.h"
#include "GO_fratelli.h"
#include "GO_bullet.h"
#include "GO_item.h"
#include "GO_pipe_water.h"
#include "GO_bone.h"
#include "GO_closingwall.h"
#include "GO_flame.h"
#include "GO_ghost.h"
#include "GO_musicalnote.h"
#include "GO_coin.h"
#include "GO_bridge.h"


Symbol *key_symbol = new Symbol("GO_key");
Symbol *cagedoor_symbol = new Symbol("GO_cagedoor");
Symbol *fallingrock_symbol = new Symbol("GO_fallingrock");
Symbol *fallingrock_blue_symbol = new Symbol("GO_fallingrock_blue");
Symbol *fallingrock_green_symbol = new Symbol("GO_fallingrock_green");
Symbol *fallingrock_yellow_symbol = new Symbol("GO_fallingrock_yellow");
Symbol *skull_symbol = new Symbol("GO_skull");
Symbol *jumpingskull_symbol = new Symbol("GO_jumpingskull");
Symbol *trickyskull_symbol = new Symbol("GO_trickyskull");
Symbol *drop_symbol = new Symbol("GO_drop");
Symbol *dropgenerator_symbol = new Symbol("GO_dropgenerator");
Symbol *skulldoor_symbol = new Symbol("GO_skulldoor");
Symbol *wateropening_symbol = new Symbol("GO_wateropening");
Symbol *wateropening_blue_symbol = new Symbol("GO_wateropening_blue");
Symbol *wateropening_green_symbol = new Symbol("GO_wateropening_green");
Symbol *wateropening_yellow_symbol = new Symbol("GO_wateropening_yellow");
Symbol *water_symbol = new Symbol("GO_water");
Symbol *lava_symbol = new Symbol("GO_lava");
Symbol *entrydoor_symbol = new Symbol("GO_entrydoor");
Symbol *exitdoor_symbol = new Symbol("GO_exitdoor");
Symbol *character_symbol = new Symbol("GO_character");
Symbol *ropetop_symbol = new Symbol("GO_ropetop");
Symbol *rope_symbol = new Symbol("GO_rope");
Symbol *ropebottom_symbol = new Symbol("GO_ropebottom");
Symbol *bigrock_symbol = new Symbol("GO_bigrock");
Symbol *bigrock_blue_symbol = new Symbol("GO_bigrock_blue");
Symbol *bigrock_green_symbol = new Symbol("GO_bigrock_green");
Symbol *bigrock_yellow_symbol = new Symbol("GO_bigrock_yellow");
Symbol *bat_symbol = new Symbol("GO_bat");
Symbol *skeleton_symbol = new Symbol("GO_skeleton");
Symbol *fratelli_yellow_symbol = new Symbol("GO_fratelli_yellow");
Symbol *fratelli_blue_symbol = new Symbol("GO_fratelli_blue");
Symbol *fratelli_green_symbol = new Symbol("GO_fratelli_green");
Symbol *fratelli_red_symbol = new Symbol("GO_fratelli_red");
Symbol *fratelli_white_symbol = new Symbol("GO_fratelli_white");
Symbol *fratelli_grey_symbol = new Symbol("GO_fratelli_grey");
Symbol *bullet_symbol = new Symbol("GO_bullet");
Symbol *item_symbol = new Symbol("GO_item");
Symbol *pipewater_left_symbol = new Symbol("GO_pipe_water_left");
Symbol *pipewater_right_symbol = new Symbol("GO_pipe_water_right");
Symbol *bone_symbol = new Symbol("GO_bone");
Symbol *closingwall_symbol = new Symbol("GO_closingwall");
Symbol *closingwall_blue_symbol = new Symbol("GO_closingwall_blue");
Symbol *closingwall_green_symbol = new Symbol("GO_closingwall_green");
Symbol *closingwall_yellow_symbol = new Symbol("GO_closingwall_yellow");
Symbol *flame_symbol = new Symbol("GO_flame");
Symbol *ghost_symbol = new Symbol("GO_ghost");
Symbol *musicalnote_symbol = new Symbol("GO_musicalnote");
Symbol *coin_symbol = new Symbol("GO_coin");
Symbol *fallingwater_symbol = new Symbol("GO_fallingwater");
Symbol *watersplash_symbol = new Symbol("GO_watersplash");

Symbol *enemy_symbol = new Symbol("GO_enemy");
Symbol *object_symbol = new Symbol("GObject");
Symbol *pipe_water_symbol = new Symbol("GO_pipewater");
Symbol *fratelli_symbol = new Symbol("GO_fratelli");
Symbol *bridge_symbol = new Symbol("GO_bridge");



GObject *GObject_create(Symbol *name, int x, int y, int sfx_volume, List<int> *parameters)
{
    if (name->cmp(key_symbol))
        return new GO_key(x, y, sfx_volume);
    if (name->cmp(cagedoor_symbol))
        return new GO_cagedoor(x, y, sfx_volume, *(parameters->operator [](0)),*(parameters->operator [](1)));
    if (name->cmp(fallingrock_symbol))
        return new GO_fallingrock(x, y, sfx_volume, 0);
    if (name->cmp(fallingrock_blue_symbol))
        return new GO_fallingrock(x, y, sfx_volume, 1);
    if (name->cmp(fallingrock_green_symbol))
        return new GO_fallingrock(x, y, sfx_volume, 2);
    if (name->cmp(fallingrock_yellow_symbol))
        return new GO_fallingrock(x, y, sfx_volume, 3);
    if (name->cmp(skull_symbol))
        return new GO_skull(x, y, sfx_volume, *(parameters->operator [](0)));
    if (name->cmp(jumpingskull_symbol))
        return new GO_jumpingskull(x, y, sfx_volume, *(parameters->operator [](0)));
    if (name->cmp(trickyskull_symbol))
        return new GO_trickyskull(x, y, sfx_volume, *(parameters->operator [](0)));
    if (name->cmp(drop_symbol))
        return new GO_drop(x, y, sfx_volume);
    if (name->cmp(dropgenerator_symbol))
        return new GO_dropgenerator(x, y, sfx_volume);
    if (name->cmp(skulldoor_symbol)) {
        if (parameters != 0 && parameters->Length() == 3) {
            return new GO_skulldoor(x, y, sfx_volume, *(parameters->operator [](0)),*(parameters->operator [](1)),*(parameters->operator [](2)));
        } else {
            return new GO_skulldoor(x, y, sfx_volume);
        } // if
    } // if
    if (name->cmp(wateropening_symbol))
        return new GO_wateropening(x, y, sfx_volume, 0);
    if (name->cmp(wateropening_blue_symbol))
        return new GO_wateropening(x, y, sfx_volume, 1);
    if (name->cmp(wateropening_green_symbol))
        return new GO_wateropening(x, y, sfx_volume, 2);
    if (name->cmp(wateropening_yellow_symbol))
        return new GO_wateropening(x, y, sfx_volume, 3);
    if (name->cmp(water_symbol))
        return new GO_water(x, y, sfx_volume);
    if (name->cmp(lava_symbol))
        return new GO_lava(x, y, sfx_volume);
    if (name->cmp(entrydoor_symbol))
        return new GO_entrydoor(x, y, sfx_volume);
    if (name->cmp(exitdoor_symbol))
        return new GO_exitdoor(x, y, sfx_volume);
    if (name->cmp(character_symbol))
        return new GO_character(x, y, sfx_volume, *(parameters->operator [](0)));
    if (name->cmp(ropetop_symbol))
        return new GO_rope(x, y, sfx_volume, 0);
    if (name->cmp(rope_symbol))
        return new GO_rope(x, y, sfx_volume, 1);
    if (name->cmp(ropebottom_symbol))
        return new GO_rope(x, y, sfx_volume, 2);
    if (name->cmp(bigrock_symbol))
        return new GO_bigrock(x, y, sfx_volume, 0);
    if (name->cmp(bigrock_blue_symbol))
        return new GO_bigrock(x, y, sfx_volume, 1);
    if (name->cmp(bigrock_green_symbol))
        return new GO_bigrock(x, y, sfx_volume, 2);
    if (name->cmp(bigrock_yellow_symbol))
        return new GO_bigrock(x, y, sfx_volume, 3);
    if (name->cmp(bat_symbol))
        return new GO_bat(x, y, sfx_volume);
    if (name->cmp(skeleton_symbol))
        return new GO_skeleton(x, y, sfx_volume, *(parameters->operator [](0)));
    if (name->cmp(fratelli_yellow_symbol))
        return new GO_fratelli(x, y, sfx_volume, 0);
    if (name->cmp(fratelli_blue_symbol))
        return new GO_fratelli(x, y, sfx_volume, 1);
    if (name->cmp(fratelli_green_symbol))
        return new GO_fratelli(x, y, sfx_volume, 2);
    if (name->cmp(fratelli_red_symbol))
        return new GO_fratelli(x, y, sfx_volume, 3);
    if (name->cmp(fratelli_white_symbol))
        return new GO_fratelli(x, y, sfx_volume, 4);
    if (name->cmp(fratelli_grey_symbol))
        return new GO_fratelli(x, y, sfx_volume, 5);
    if (name->cmp(bullet_symbol))
        return new GO_bullet(x, y, sfx_volume, *(parameters->operator [](0)));
    if (name->cmp(item_symbol))
        return new GO_item(x, y, sfx_volume, *(parameters->operator [](0)));
    if (name->cmp(pipewater_left_symbol))
        return new GO_pipe_water(x, y, sfx_volume, 0);
    if (name->cmp(pipewater_right_symbol))
        return new GO_pipe_water(x, y, sfx_volume, 1);
    if (name->cmp(bone_symbol))
        return new GO_bone(x, y, sfx_volume, *(parameters->operator [](0)));
    if (name->cmp(closingwall_symbol))
        return new GO_closingwall(x, y, sfx_volume, 0);
    if (name->cmp(closingwall_blue_symbol))
        return new GO_closingwall(x, y, sfx_volume, 1);
    if (name->cmp(closingwall_green_symbol))
        return new GO_closingwall(x, y, sfx_volume, 2);
    if (name->cmp(closingwall_yellow_symbol))
        return new GO_closingwall(x, y, sfx_volume, 3);
    if (name->cmp(flame_symbol))
        return new GO_flame(x, y, sfx_volume);
    if (name->cmp(ghost_symbol))
        return new GO_ghost(x, y, sfx_volume);
    if (name->cmp(musicalnote_symbol))
        return new GO_musicalnote(x, y, sfx_volume, float(*(parameters->operator [](0))),float(*(parameters->operator [](1))));
    if (name->cmp(coin_symbol))
        return new GO_coin(x, y, sfx_volume);

    if (name->cmp(bridge_symbol)) {
       return new GO_bridge(x, y, sfx_volume, *(parameters->operator [](0)));
    }

    assert(false);

    return 0;
} /* GObject_create */

