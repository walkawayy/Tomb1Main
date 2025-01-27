#pragma once

typedef enum {
    // Genuine level types
    GFL_TITLE,
    GFL_NORMAL,
    GFL_CUTSCENE,
    GFL_DEMO,
#if TR_VERSION == 1
    GFL_GYM,
    GFL_BONUS,
#endif

#if TR_VERSION == 1
    // Legacy level types to maintain savegame backwards compatibility.
    // TODO: get rid of these.
    GFL_DUMMY,
    GFL_CURRENT,
#endif
} GAME_FLOW_LEVEL_TYPE;

typedef enum {
    GFSC_NORMAL,
    GFSC_SAVED,
#if TR_VERSION == 1
    GFSC_RESTART,
    GFSC_SELECT,
#elif TR_VERSION == 2
    GFSC_STORY,
    GFSC_MID_STORY,
#endif
} GAME_FLOW_SEQUENCE_CONTEXT;

typedef enum {
    GF_NOOP = -1,
    GF_START_GAME,
    GF_START_CINE,
    GF_START_FMV,
    GF_START_DEMO,
    GF_EXIT_TO_TITLE,
    GF_LEVEL_COMPLETE,
    GF_EXIT_GAME,
    GF_START_SAVED_GAME,
#if TR_VERSION == 1
    GF_RESTART_GAME,
#endif
    GF_SELECT_GAME,
#if TR_VERSION == 1
    GF_START_GYM,
    GF_STORY_SO_FAR,
#endif
} GAME_FLOW_ACTION;

typedef enum {
    GFS_DISPLAY_PICTURE,
    GFS_PLAY_LEVEL,
    GFS_PLAY_CUTSCENE,
    GFS_PLAY_FMV,
    GFS_PLAY_MUSIC,
    GFS_EXIT_TO_TITLE,
    GFS_LEVEL_STATS,
    GFS_TOTAL_STATS,
    GFS_LEVEL_COMPLETE,
#if TR_VERSION == 1
    GFS_LOADING_SCREEN,
    GFS_LOAD_LEVEL,
#endif
    GFS_SET_CAMERA_ANGLE,
    GFS_ADD_ITEM,
    GFS_REMOVE_WEAPONS,
    GFS_REMOVE_AMMO,
#if TR_VERSION == 1
    GFS_REMOVE_MEDIPACKS,
    GFS_REMOVE_SCIONS,
    GFS_FLIP_MAP,
    GFS_MESH_SWAP,
    GFS_SETUP_BACON_LARA,
#elif TR_VERSION == 2
    GFS_SET_START_ANIM,
    GFS_SET_NUM_SECRETS,
    GFS_DISABLE_FLOOR,
    GFS_ENABLE_SUNSET,
    GFS_ADD_SECRET_REWARD,
#endif
} GAME_FLOW_SEQUENCE_EVENT_TYPE;
