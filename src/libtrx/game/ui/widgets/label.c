#include "game/ui/widgets/label.h"

#include "game/text.h"
#include "memory.h"

typedef struct {
    UI_WIDGET_VTABLE vtable;
    TEXTSTRING *text;
    int32_t width;
    int32_t height;
    bool has_frame;
} UI_LABEL;

static int32_t M_GetWidth(const UI_LABEL *self);
static int32_t M_GetHeight(const UI_LABEL *self);
static void M_SetPosition(UI_LABEL *self, int32_t x, int32_t y);
static void M_Draw(UI_LABEL *self);
static void M_Free(UI_LABEL *self);

static int32_t M_GetWidth(const UI_LABEL *const self)
{
    if (self->vtable.is_hidden) {
        return 0;
    }
    if (self->width != UI_LABEL_AUTO_SIZE) {
        return self->width;
    }
    return Text_GetWidth(self->text);
}

static int32_t M_GetHeight(const UI_LABEL *const self)
{
    if (self->vtable.is_hidden) {
        return 0;
    }
    if (self->height != UI_LABEL_AUTO_SIZE) {
        return self->height;
    }
    return Text_GetHeight(self->text);
}

static void M_SetPosition(
    UI_LABEL *const self, const int32_t x, const int32_t y)
{
    Text_SetPos(self->text, x, y + TEXT_HEIGHT_FIXED);
}

static void M_Draw(UI_LABEL *const self)
{
    if (self->vtable.is_hidden) {
        return;
    }
    Text_DrawText(self->text);
}

static void M_Free(UI_LABEL *const self)
{
    Text_Remove(self->text);
    Memory_Free(self);
}

UI_WIDGET *UI_Label_Create(
    const char *const text, const int32_t width, const int32_t height)
{
    UI_LABEL *self = Memory_Alloc(sizeof(UI_LABEL));
    self->vtable = (UI_WIDGET_VTABLE) {
        .get_width = (UI_WIDGET_GET_WIDTH)M_GetWidth,
        .get_height = (UI_WIDGET_GET_HEIGHT)M_GetHeight,
        .set_position = (UI_WIDGET_SET_POSITION)M_SetPosition,
        .control = nullptr,
        .draw = (UI_WIDGET_DRAW)M_Draw,
        .free = (UI_WIDGET_FREE)M_Free,
    };

    self->width = width;
    self->height = height;
    self->has_frame = false;

    self->text = Text_Create(0, 0, text);
    self->text->pos.z = 16;
    Text_SetMultiline(self->text, true);
    self->text->flags.manual_draw = 1;

    return (UI_WIDGET *)self;
}

void UI_Label_ChangeText(UI_WIDGET *const widget, const char *const text)
{
    UI_LABEL *const self = (UI_LABEL *)widget;
    Text_ChangeText(self->text, text);
}

const char *UI_Label_GetText(UI_WIDGET *const widget)
{
    UI_LABEL *const self = (UI_LABEL *)widget;
    return self->text->content;
}

void UI_Label_SetSize(
    UI_WIDGET *const widget, const int32_t width, const int32_t height)
{
    UI_LABEL *const self = (UI_LABEL *)widget;
    self->width = width;
    self->height = height;
}

void UI_Label_SetVisible(UI_WIDGET *const widget, const bool visible)
{
    UI_LABEL *const self = (UI_LABEL *)widget;
    Text_Hide(self->text, !visible);
}

void UI_Label_AddFrame(UI_WIDGET *const widget)
{
    UI_LABEL *const self = (UI_LABEL *)widget;
    if (!self->has_frame) {
        self->text->pos.z = 0;
        Text_AddBackground(self->text, 0, 0, 0, 0, TS_REQUESTED);
        Text_AddOutline(self->text, TS_REQUESTED);
        self->has_frame = true;
    }
}

void UI_Label_RemoveFrame(UI_WIDGET *const widget)
{
    UI_LABEL *const self = (UI_LABEL *)widget;
    if (self->has_frame) {
        Text_RemoveBackground(self->text);
        Text_RemoveOutline(self->text);
        self->text->pos.z = 16;
        self->has_frame = false;
    }
}

void UI_Label_Flash(
    UI_WIDGET *const widget, const bool enable, const int32_t rate)
{
    UI_LABEL *const self = (UI_LABEL *)widget;
    Text_Flash(self->text, enable, rate);
}

void UI_Label_SetScale(UI_WIDGET *const widget, const float scale)
{
    UI_LABEL *const self = (UI_LABEL *)widget;
    Text_SetScale(self->text, TEXT_BASE_SCALE * scale, TEXT_BASE_SCALE * scale);
}

void UI_Label_SetZIndex(UI_WIDGET *const widget, const int32_t z_index)
{
    UI_LABEL *const self = (UI_LABEL *)widget;
    self->text->pos.z = z_index;
}

int32_t UI_Label_MeasureTextWidth(UI_WIDGET *const widget)
{
    UI_LABEL *const self = (UI_LABEL *)widget;
    return Text_GetWidth(self->text);
}

int32_t UI_Label_MeasureTextHeight(UI_WIDGET *const widget)
{
    UI_LABEL *const self = (UI_LABEL *)widget;
    return Text_GetHeight(self->text);
}
