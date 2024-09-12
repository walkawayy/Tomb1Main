#include "game/objects/general/drawbridge.h"

#include "config.h"
#include "game/items.h"
#include "game/objects/general/door.h"
#include "game/room.h"

typedef enum {
    DRAWBRIDGE_CLOSED,
    DRAWBRIDGE_OPEN,
} DRAWBRIDGE_STATE;

static bool Drawbridge_IsLaraOnTop(const ITEM_INFO *item, int32_t x, int32_t z);
static int16_t Drawbridge_GetFloorHeight(
    const ITEM_INFO *item, int32_t x, int32_t y, int32_t z, int16_t height);
static int16_t Drawbridge_GetCeilingHeight(
    const ITEM_INFO *item, int32_t x, int32_t y, int32_t z, int16_t height);
static void Drawbridge_Collision(
    int16_t item_num, ITEM_INFO *lara_item, COLL_INFO *coll);
static void Drawbridge_Control(int16_t item_num);

static bool Drawbridge_IsLaraOnTop(const ITEM_INFO *item, int32_t x, int32_t z)
{
    int32_t ix = item->pos.x >> WALL_SHIFT;
    int32_t iz = item->pos.z >> WALL_SHIFT;
    x >>= WALL_SHIFT;
    z >>= WALL_SHIFT;

    if (item->rot.y == 0 && x == ix && (z == iz - 1 || z == iz - 2)) {
        return true;
    } else if (
        item->rot.y == -PHD_180 && x == ix && (z == iz + 1 || z == iz + 2)) {
        return true;
    } else if (
        item->rot.y == PHD_90 && z == iz && (x == ix - 1 || x == ix - 2)) {
        return true;
    } else if (
        item->rot.y == -PHD_90 && z == iz && (x == ix + 1 || x == ix + 2)) {
        return true;
    }

    return false;
}

static int16_t Drawbridge_GetFloorHeight(
    const ITEM_INFO *item, const int32_t x, const int32_t y, const int32_t z,
    const int16_t height)
{
    if (item->current_anim_state != DOOR_OPEN) {
        return height;
    } else if (!Drawbridge_IsLaraOnTop(item, x, z)) {
        return height;
    } else if (y > item->pos.y) {
        return height;
    } else if (g_Config.fix_bridge_collision && item->pos.y >= height) {
        return height;
    }
    return item->pos.y;
}

static int16_t Drawbridge_GetCeilingHeight(
    const ITEM_INFO *item, const int32_t x, const int32_t y, const int32_t z,
    const int16_t height)
{
    if (item->current_anim_state != DOOR_OPEN) {
        return height;
    } else if (!Drawbridge_IsLaraOnTop(item, x, z)) {
        return height;
    } else if (y <= item->pos.y) {
        return height;
    } else if (g_Config.fix_bridge_collision && item->pos.y <= height) {
        return height;
    }
    return item->pos.y + STEP_L;
}

static void Drawbridge_Collision(
    int16_t item_num, ITEM_INFO *lara_item, COLL_INFO *coll)
{
    ITEM_INFO *item = &g_Items[item_num];
    if (item->current_anim_state == DOOR_CLOSED) {
        Door_Collision(item_num, lara_item, coll);
    }
}

static void Drawbridge_Control(int16_t item_num)
{
    ITEM_INFO *item = &g_Items[item_num];
    if (Item_IsTriggerActive(item)) {
        item->goal_anim_state = DRAWBRIDGE_OPEN;
    } else {
        item->goal_anim_state = DRAWBRIDGE_CLOSED;
    }

    Item_Animate(item);

    int16_t room_num = item->room_num;
    Room_GetSector(item->pos.x, item->pos.y, item->pos.z, &room_num);
    if (room_num != item->room_num) {
        Item_NewRoom(item_num, room_num);
    }
}

void Drawbridge_Setup(OBJECT_INFO *obj)
{
    if (!obj->loaded) {
        return;
    }
    obj->ceiling_height_func = Drawbridge_GetCeilingHeight;
    obj->collision = Drawbridge_Collision;
    obj->control = Drawbridge_Control;
    obj->save_anim = 1;
    obj->save_flags = 1;
    obj->floor_height_func = Drawbridge_GetFloorHeight;
}
