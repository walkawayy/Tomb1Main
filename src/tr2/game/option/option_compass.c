#include "decomp/stats.h"
#include "game/input.h"
#include "game/option/option.h"
#include "game/requester.h"
#include "game/sound.h"
#include "game/ui/widgets/stats_dialog.h"
#include "global/vars.h"

#include <stdio.h>

static UI_WIDGET *m_Dialog = NULL;

static void M_Init(void);

static void M_Init(void)
{
    m_Dialog = UI_StatsDialog_Create((UI_STATS_DIALOG_ARGS) {
        .mode = g_CurrentLevel == LV_GYM ? UI_STATS_DIALOG_MODE_ASSAULT_COURSE
                                         : UI_STATS_DIALOG_MODE_LEVEL,
        .level_num = g_CurrentLevel,
        .style = UI_STATS_DIALOG_STYLE_BORDERED,
    });
}

static void M_Shutdown(void)
{
    if (m_Dialog != NULL) {
        m_Dialog->free(m_Dialog);
        m_Dialog = NULL;
    }
}

void Option_Compass_Control(INVENTORY_ITEM *const item)
{
    char buffer[32];
    const int32_t sec = g_SaveGame.current_stats.timer / FRAMES_PER_SECOND;
    sprintf(buffer, "%02d:%02d:%02d", sec / 3600, sec / 60 % 60, sec % 60);

    if (m_Dialog == NULL) {
        M_Init();
    }
    m_Dialog->control(m_Dialog);

    if (g_InputDB.menu_confirm || g_InputDB.menu_back) {
        item->anim_direction = 1;
        item->goal_frame = item->frames_total - 1;
    }

    Sound_Effect(SFX_MENU_STOPWATCH, 0, SPM_ALWAYS);
}

void Option_Compass_Draw(INVENTORY_ITEM *const item)
{
    if (m_Dialog != NULL) {
        m_Dialog->draw(m_Dialog);
    }
}

void Option_Compass_Shutdown(void)
{
    M_Shutdown();
}
