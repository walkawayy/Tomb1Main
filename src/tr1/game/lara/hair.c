#include "game/lara/hair.h"

#include "game/items.h"
#include "game/output.h"
#include "game/room.h"
#include "global/const.h"
#include "global/types.h"
#include "global/vars.h"
#include "math/matrix.h"

#include <libtrx/config.h>
#include <libtrx/game/math.h>
#include <libtrx/utils.h>

#include <stdbool.h>
#include <stdint.h>

#define HAIR_SEGMENTS 6
#define HAIR_OFFSET_X (0) // left-right
#define HAIR_OFFSET_Y (20) // up-down
#define HAIR_OFFSET_Z (-45) // front-back

static bool m_FirstHair = false;
static GAME_OBJECT_ID m_LaraType = O_LARA;
static HAIR_SEGMENT m_Hair[HAIR_SEGMENTS + 1] = { 0 };
static XYZ_32 m_HVel[HAIR_SEGMENTS + 1] = { 0 };

static int16_t M_GetRoom(int32_t x, int32_t y, int32_t z);

static int16_t M_GetRoom(int32_t x, int32_t y, int32_t z)
{
    int16_t room_num = Room_GetIndexFromPos(x, y, z);
    if (room_num != NO_ROOM) {
        return room_num;
    }
    return g_LaraItem->room_num;
}

int32_t Lara_Hair_GetSegmentCount(void)
{
    return HAIR_SEGMENTS;
}

HAIR_SEGMENT *Lara_Hair_GetSegment(int32_t n)
{
    return &m_Hair[n];
}

bool Lara_Hair_IsActive(void)
{
    return g_Config.visuals.enable_braid && g_Objects[O_HAIR].loaded
        && g_Objects[m_LaraType].loaded;
}

void Lara_Hair_Initialise(void)
{
    m_FirstHair = true;
    Lara_Hair_SetLaraType(O_LARA);

    const OBJECT *const object = Object_GetObject(O_HAIR);

    m_Hair[0].rot.y = 0;
    m_Hair[0].rot.x = -PHD_90;

    for (int32_t i = 1; i < HAIR_SEGMENTS + 1; i++) {
        const ANIM_BONE *const bone = Object_GetBone(object, i - 1);
        m_Hair[i].pos = bone->pos;
        m_Hair[i].rot.x = -PHD_90;
        m_Hair[i].rot.y = 0;
        m_Hair[i].rot.z = 0;
        m_HVel[i].x = 0;
        m_HVel[i].y = 0;
        m_HVel[i].z = 0;
    }
}

void Lara_Hair_SetLaraType(GAME_OBJECT_ID lara_type)
{
    m_LaraType = lara_type;
}

void Lara_Hair_Control(void)
{
    if (!Lara_Hair_IsActive()) {
        return;
    }

    bool in_cutscene;
    OBJECT *object;
    int32_t distance;
    ANIM_FRAME *frame;
    const OBJECT_MESH *mesh;
    int16_t room_num;
    ANIM_FRAME *frmptr[2];
    XYZ_32 pos;
    const SECTOR *sector;
    int32_t i;
    int32_t water_level;
    int32_t height;
    int32_t frac;
    int32_t rate;
    SPHERE sphere[5];
    int32_t j;
    int32_t x;
    int32_t y;
    int32_t z;

    in_cutscene = m_LaraType != O_LARA;
    object = &g_Objects[m_LaraType];

    if (!in_cutscene && g_Lara.hit_direction >= 0) {
        LARA_ANIMATION hit_anim;
        switch (g_Lara.hit_direction) {
        case DIR_NORTH:
            hit_anim = LA_SPAZ_FORWARD;
            break;

        case DIR_SOUTH:
            hit_anim = LA_SPAZ_BACK;
            break;

        case DIR_EAST:
            hit_anim = LA_SPAZ_RIGHT;
            break;

        default:
            hit_anim = LA_SPAZ_LEFT;
            break;
        }

        frame = Object_GetAnim(object, hit_anim)->frame_ptr;
        frame += g_Lara.hit_frame;

        frac = 0;
    } else {
        frame = Item_GetBestFrame(g_LaraItem);
        frac = Item_GetFrames(g_LaraItem, frmptr, &rate);
    }

    Matrix_PushUnit();
    Matrix_TranslateSet(
        g_LaraItem->pos.x, g_LaraItem->pos.y, g_LaraItem->pos.z);
    Matrix_RotYXZ(g_LaraItem->rot.y, g_LaraItem->rot.x, g_LaraItem->rot.z);

    const ANIM_BONE *bone = Object_GetBone(object, 0);
    if (frac) {
        Matrix_InitInterpolate(frac, rate);
        int32_t *packed_rotation1 = frmptr[0]->mesh_rots;
        int32_t *packed_rotation2 = frmptr[1]->mesh_rots;
        Matrix_TranslateRel_ID(
            frmptr[0]->offset.x, frmptr[0]->offset.y, frmptr[0]->offset.z,
            frmptr[1]->offset.x, frmptr[1]->offset.y, frmptr[1]->offset.z);
        Matrix_RotYXZpack_I(
            packed_rotation1[LM_HIPS], packed_rotation2[LM_HIPS]);

        // hips
        Matrix_Push_I();
        mesh = Object_GetMesh(object->mesh_idx + LM_HIPS);
        Matrix_TranslateRel_I(mesh->center.x, mesh->center.y, mesh->center.z);
        Matrix_Interpolate();
        sphere[0].x = g_MatrixPtr->_03 >> W2V_SHIFT;
        sphere[0].y = g_MatrixPtr->_13 >> W2V_SHIFT;
        sphere[0].z = g_MatrixPtr->_23 >> W2V_SHIFT;
        sphere[0].r = mesh->radius;
        Matrix_Pop_I();

        // torso
        Matrix_TranslateRel_I(
            bone[LM_TORSO - 1].pos.x, bone[LM_TORSO - 1].pos.y,
            bone[LM_TORSO - 1].pos.z);
        Matrix_RotYXZpack_I(
            packed_rotation1[LM_TORSO], packed_rotation2[LM_TORSO]);
        Matrix_RotYXZ_I(
            g_Lara.interp.result.torso_rot.y, g_Lara.interp.result.torso_rot.x,
            g_Lara.interp.result.torso_rot.z);
        Matrix_Push_I();
        mesh = Object_GetMesh(object->mesh_idx + LM_TORSO);
        Matrix_TranslateRel_I(mesh->center.x, mesh->center.y, mesh->center.z);
        Matrix_Interpolate();
        sphere[1].x = g_MatrixPtr->_03 >> W2V_SHIFT;
        sphere[1].y = g_MatrixPtr->_13 >> W2V_SHIFT;
        sphere[1].z = g_MatrixPtr->_23 >> W2V_SHIFT;
        sphere[1].r = mesh->radius;
        Matrix_Pop_I();

        // right arm
        Matrix_Push_I();
        Matrix_TranslateRel_I(
            bone[LM_UARM_R - 1].pos.x, bone[LM_UARM_R - 1].pos.y,
            bone[LM_UARM_R - 1].pos.z);
        Matrix_RotYXZpack_I(
            packed_rotation1[LM_UARM_R], packed_rotation2[LM_UARM_R]);
        mesh = Object_GetMesh(object->mesh_idx + LM_UARM_R);
        Matrix_TranslateRel_I(mesh->center.x, mesh->center.y, mesh->center.z);
        Matrix_Interpolate();
        sphere[3].x = g_MatrixPtr->_03 >> W2V_SHIFT;
        sphere[3].y = g_MatrixPtr->_13 >> W2V_SHIFT;
        sphere[3].z = g_MatrixPtr->_23 >> W2V_SHIFT;
        sphere[3].r = mesh->radius * 3 / 2;
        Matrix_Pop_I();

        // left arm
        Matrix_Push_I();
        Matrix_TranslateRel_I(
            bone[LM_UARM_L - 1].pos.x, bone[LM_UARM_L - 1].pos.y,
            bone[LM_UARM_L - 1].pos.z);
        Matrix_RotYXZpack_I(
            packed_rotation1[LM_UARM_L], packed_rotation2[LM_UARM_L]);
        mesh = Object_GetMesh(object->mesh_idx + LM_UARM_L);
        Matrix_TranslateRel_I(mesh->center.x, mesh->center.y, mesh->center.z);
        Matrix_Interpolate();
        sphere[4].x = g_MatrixPtr->_03 >> W2V_SHIFT;
        sphere[4].y = g_MatrixPtr->_13 >> W2V_SHIFT;
        sphere[4].z = g_MatrixPtr->_23 >> W2V_SHIFT;
        sphere[4].r = mesh->radius * 3 / 2;
        Matrix_Pop_I();

        // head
        Matrix_TranslateRel_I(
            bone[LM_HEAD - 1].pos.x, bone[LM_HEAD - 1].pos.y,
            bone[LM_HEAD - 1].pos.z);
        Matrix_RotYXZpack_I(
            packed_rotation1[LM_HEAD], packed_rotation2[LM_HEAD]);
        Matrix_RotYXZ_I(
            g_Lara.interp.result.head_rot.y, g_Lara.interp.result.head_rot.x,
            g_Lara.interp.result.head_rot.z);
        Matrix_Push_I();
        mesh = Object_GetMesh(object->mesh_idx + LM_HEAD);
        Matrix_TranslateRel_I(mesh->center.x, mesh->center.y, mesh->center.z);
        Matrix_Interpolate();
        sphere[2].x = g_MatrixPtr->_03 >> W2V_SHIFT;
        sphere[2].y = g_MatrixPtr->_13 >> W2V_SHIFT;
        sphere[2].z = g_MatrixPtr->_23 >> W2V_SHIFT;
        sphere[2].r = mesh->radius;
        Matrix_Pop_I();

        Matrix_TranslateRel_I(HAIR_OFFSET_X, HAIR_OFFSET_Y, HAIR_OFFSET_Z);
        Matrix_Interpolate();

    } else {
        Matrix_TranslateRel(frame->offset.x, frame->offset.y, frame->offset.z);
        int32_t *packed_rotation = frame->mesh_rots;
        Matrix_RotYXZpack(packed_rotation[LM_HIPS]);

        // hips
        Matrix_Push();
        mesh = Object_GetMesh(object->mesh_idx + LM_HIPS);
        Matrix_TranslateRel(mesh->center.x, mesh->center.y, mesh->center.z);
        sphere[0].x = g_MatrixPtr->_03 >> W2V_SHIFT;
        sphere[0].y = g_MatrixPtr->_13 >> W2V_SHIFT;
        sphere[0].z = g_MatrixPtr->_23 >> W2V_SHIFT;
        sphere[0].r = mesh->radius;
        Matrix_Pop();

        // torso
        Matrix_TranslateRel(
            bone[LM_TORSO - 1].pos.x, bone[LM_TORSO - 1].pos.y,
            bone[LM_TORSO - 1].pos.z);
        Matrix_RotYXZpack(packed_rotation[LM_TORSO]);
        Matrix_RotYXZ(
            g_Lara.interp.result.torso_rot.y, g_Lara.interp.result.torso_rot.x,
            g_Lara.interp.result.torso_rot.z);
        Matrix_Push();
        mesh = Object_GetMesh(object->mesh_idx + LM_TORSO);
        Matrix_TranslateRel(mesh->center.x, mesh->center.y, mesh->center.z);
        sphere[1].x = g_MatrixPtr->_03 >> W2V_SHIFT;
        sphere[1].y = g_MatrixPtr->_13 >> W2V_SHIFT;
        sphere[1].z = g_MatrixPtr->_23 >> W2V_SHIFT;
        sphere[1].r = mesh->radius;
        Matrix_Pop();

        // right arm
        Matrix_Push();
        Matrix_TranslateRel(
            bone[LM_UARM_R - 1].pos.x, bone[LM_UARM_R - 1].pos.y,
            bone[LM_UARM_R - 1].pos.z);
        Matrix_RotYXZpack(packed_rotation[LM_UARM_R]);
        mesh = Object_GetMesh(object->mesh_idx + LM_UARM_R);
        Matrix_TranslateRel(mesh->center.x, mesh->center.y, mesh->center.z);
        sphere[3].x = g_MatrixPtr->_03 >> W2V_SHIFT;
        sphere[3].y = g_MatrixPtr->_13 >> W2V_SHIFT;
        sphere[3].z = g_MatrixPtr->_23 >> W2V_SHIFT;
        sphere[3].r = mesh->radius * 3 / 2;
        Matrix_Pop();

        // left arm
        Matrix_Push();
        Matrix_TranslateRel(
            bone[LM_UARM_L - 1].pos.x, bone[LM_UARM_L - 1].pos.y,
            bone[LM_UARM_L - 1].pos.z);
        Matrix_RotYXZpack(packed_rotation[LM_UARM_L]);
        mesh = Object_GetMesh(object->mesh_idx + LM_UARM_L);
        Matrix_TranslateRel(mesh->center.x, mesh->center.y, mesh->center.z);
        sphere[4].x = g_MatrixPtr->_03 >> W2V_SHIFT;
        sphere[4].y = g_MatrixPtr->_13 >> W2V_SHIFT;
        sphere[4].z = g_MatrixPtr->_23 >> W2V_SHIFT;
        sphere[4].r = mesh->radius * 3 / 2;
        Matrix_Pop();

        // head
        Matrix_TranslateRel(
            bone[LM_HEAD - 1].pos.x, bone[LM_HEAD - 1].pos.y,
            bone[LM_HEAD - 1].pos.z);
        Matrix_RotYXZpack(packed_rotation[LM_HEAD]);
        Matrix_RotYXZ(
            g_Lara.interp.result.head_rot.y, g_Lara.interp.result.head_rot.x,
            g_Lara.interp.result.head_rot.z);
        Matrix_Push();
        mesh = Object_GetMesh(object->mesh_idx + LM_HEAD);
        Matrix_TranslateRel(mesh->center.x, mesh->center.y, mesh->center.z);
        sphere[2].x = g_MatrixPtr->_03 >> W2V_SHIFT;
        sphere[2].y = g_MatrixPtr->_13 >> W2V_SHIFT;
        sphere[2].z = g_MatrixPtr->_23 >> W2V_SHIFT;
        sphere[2].r = mesh->radius;
        Matrix_Pop();

        Matrix_TranslateRel(HAIR_OFFSET_X, HAIR_OFFSET_Y, HAIR_OFFSET_Z);
    }

    pos.x = g_MatrixPtr->_03 >> W2V_SHIFT;
    pos.y = g_MatrixPtr->_13 >> W2V_SHIFT;
    pos.z = g_MatrixPtr->_23 >> W2V_SHIFT;
    Matrix_Pop();

    const OBJECT *const hair_object = Object_GetObject(O_HAIR);

    m_Hair[0].pos = pos;

    if (m_FirstHair) {
        m_FirstHair = false;

        for (i = 0; i < HAIR_SEGMENTS; i++) {
            const ANIM_BONE *const hair_bone = Object_GetBone(hair_object, i);
            Matrix_PushUnit();
            Matrix_TranslateSet(
                m_Hair[i].pos.x, m_Hair[i].pos.y, m_Hair[i].pos.z);
            Matrix_RotYXZ(m_Hair[i].rot.y, m_Hair[i].rot.x, 0);
            Matrix_TranslateRel(
                hair_bone->pos.x, hair_bone->pos.y, hair_bone->pos.z);

            m_Hair[i + 1].pos.x = g_MatrixPtr->_03 >> W2V_SHIFT;
            m_Hair[i + 1].pos.y = g_MatrixPtr->_13 >> W2V_SHIFT;
            m_Hair[i + 1].pos.z = g_MatrixPtr->_23 >> W2V_SHIFT;

            Matrix_Pop();
        }
    } else {
        if (in_cutscene) {
            room_num = M_GetRoom(pos.x, pos.y, pos.z);
            water_level = NO_HEIGHT;
        } else {
            room_num = g_LaraItem->room_num;
            x = g_LaraItem->pos.x
                + (frame->bounds.min.x + frame->bounds.max.x) / 2;
            y = g_LaraItem->pos.y
                + (frame->bounds.min.y + frame->bounds.max.y) / 2;
            z = g_LaraItem->pos.z
                + (frame->bounds.min.z + frame->bounds.max.z) / 2;
            water_level = Room_GetWaterHeight(x, y, z, room_num);
        }

        for (i = 1; i < HAIR_SEGMENTS + 1; i++) {
            const ANIM_BONE *const hair_bone =
                Object_GetBone(hair_object, i - 1);
            m_HVel[0] = m_Hair[i].pos;

            sector = Room_GetSector(
                m_Hair[i].pos.x, m_Hair[i].pos.y, m_Hair[i].pos.z, &room_num);
            height = Room_GetHeight(
                sector, m_Hair[i].pos.x, m_Hair[i].pos.y, m_Hair[i].pos.z);

            m_Hair[i].pos.x += m_HVel[i].x * 3 / 4;
            m_Hair[i].pos.y += m_HVel[i].y * 3 / 4;
            m_Hair[i].pos.z += m_HVel[i].z * 3 / 4;

            switch (g_Lara.water_status) {
            case LWS_ABOVE_WATER:
            case LWS_WADE:
                m_Hair[i].pos.y += 10;
                if (water_level != NO_HEIGHT && m_Hair[i].pos.y > water_level)
                    m_Hair[i].pos.y = water_level;
                else if (m_Hair[i].pos.y > height) {
                    m_Hair[i].pos.x = m_HVel[0].x;
                    m_Hair[i].pos.z = m_HVel[0].z;
                    if (m_Hair[i].pos.y - height <= STEP_L) {
                        m_Hair[i].pos.y = height;
                    }
                }
                break;

            case LWS_UNDERWATER:
            case LWS_SURFACE:
            case LWS_CHEAT:
                if (m_Hair[i].pos.y < water_level) {
                    m_Hair[i].pos.y = water_level;
                } else if (m_Hair[i].pos.y > height) {
                    m_Hair[i].pos.y = height;
                }
                break;
            }

            for (j = 0; j < 5; j++) {
                x = m_Hair[i].pos.x - sphere[j].x;
                y = m_Hair[i].pos.y - sphere[j].y;
                z = m_Hair[i].pos.z - sphere[j].z;

                distance = x * x + y * y + z * z;

                if (distance < SQUARE(sphere[j].r)) {
                    distance = Math_Sqrt(distance);

                    if (distance == 0)
                        distance = 1;

                    m_Hair[i].pos.x = sphere[j].x + x * sphere[j].r / distance;
                    m_Hair[i].pos.y = sphere[j].y + y * sphere[j].r / distance;
                    m_Hair[i].pos.z = sphere[j].z + z * sphere[j].r / distance;
                }
            }

            distance = Math_Sqrt(
                SQUARE(m_Hair[i].pos.z - m_Hair[i - 1].pos.z)
                + SQUARE(m_Hair[i].pos.x - m_Hair[i - 1].pos.x));
            m_Hair[i - 1].rot.y = Math_Atan(
                m_Hair[i].pos.z - m_Hair[i - 1].pos.z,
                m_Hair[i].pos.x - m_Hair[i - 1].pos.x);
            m_Hair[i - 1].rot.x =
                -Math_Atan(distance, m_Hair[i].pos.y - m_Hair[i - 1].pos.y);

            Matrix_PushUnit();
            Matrix_TranslateSet(
                m_Hair[i - 1].pos.x, m_Hair[i - 1].pos.y, m_Hair[i - 1].pos.z);
            Matrix_RotYXZ(m_Hair[i - 1].rot.y, m_Hair[i - 1].rot.x, 0);

            if (i == HAIR_SEGMENTS) {
                const ANIM_BONE *const last_bone = hair_bone - 1;
                Matrix_TranslateRel(
                    last_bone->pos.x, last_bone->pos.y, last_bone->pos.z);
            } else {
                Matrix_TranslateRel(
                    hair_bone->pos.x, hair_bone->pos.y, hair_bone->pos.z);
            }

            m_Hair[i].pos.x = g_MatrixPtr->_03 >> W2V_SHIFT;
            m_Hair[i].pos.y = g_MatrixPtr->_13 >> W2V_SHIFT;
            m_Hair[i].pos.z = g_MatrixPtr->_23 >> W2V_SHIFT;

            m_HVel[i].x = m_Hair[i].pos.x - m_HVel[0].x;
            m_HVel[i].y = m_Hair[i].pos.y - m_HVel[0].y;
            m_HVel[i].z = m_Hair[i].pos.z - m_HVel[0].z;

            Matrix_Pop();
        }
    }
}

void Lara_Hair_Draw(void)
{
    if (!Lara_Hair_IsActive()) {
        return;
    }

    const OBJECT *const object = Object_GetObject(O_HAIR);
    int16_t mesh_idx = object->mesh_idx;
    if ((g_Lara.mesh_effects & (1 << LM_HEAD))
        && object->nmeshes >= HAIR_SEGMENTS * 2) {
        mesh_idx += HAIR_SEGMENTS;
    }

    for (int i = 0; i < HAIR_SEGMENTS; i++) {
        Matrix_Push();

        Matrix_TranslateAbs(
            m_Hair[i].interp.result.pos.x, m_Hair[i].interp.result.pos.y,
            m_Hair[i].interp.result.pos.z);
        Matrix_RotY(m_Hair[i].interp.result.rot.y);
        Matrix_RotX(m_Hair[i].interp.result.rot.x);
        Object_DrawMesh(mesh_idx + i, 1, false);

        Matrix_Pop();
    }
}
