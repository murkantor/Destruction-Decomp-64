/*
 * The PC options page, drawn in the game's own menus. On the main menu (under
 * its line for Z, the options screen) and on the options screen (the pause
 * menu's too) a line at the bottom offers it, and R opens it: a panel over the
 * menu with the PC settings in pages (L or Z for the next page), in
 * the screen's own font and colours. While it is open it takes the controller
 * (port/src/os_hw.c gives the game no input); B or Start closes it. It is drawn
 * after the game's UI each frame (Render_Frame); the settings themselves are
 * port/src/options.c.
 */
#include "common.h"

typedef struct { u32 w0; u32 w1; } Gfx;
typedef struct { f32 x, y, z; } Vec3f;

/* A font as the game's UI keeps it (Font_DrawText, 5F52C.c) */
typedef struct {
    u32 flags;
    u8 *widths;
    u8 height;
    u8 fmt;
    u8 siz;
    u8 pad0B;
    u8 *tex;
    u16 perRow;
    u8 pad12[2];
    u8 numChars;
    u8 pad15[3];
    u8 *map;
} Font;

/* The UI manager's text elements (Ui_AddTextElement, 5FA90.c) */
typedef struct {
    s32 id;
    s32 x4;
    s32 x8;
    u8 xC;
    Font *font;
    s32 x14;
    u16 x18;
    u16 x1A;
    u8 x1C;
    s32 x20;
} UiText;

typedef struct { u8 pad[0x6BB0]; UiText text[200]; } UiTexts;
typedef struct { u8 pad[0x8B64]; s16 nText; } UiCount;
typedef struct { u8 pad[0x10FC]; s16 mode; } GameMode;

extern void *gUi[];
extern GameMode *gGame;
extern void *D_8011EEF8[];
extern Vec3f D_800BBF60, D_800BBF6C; /* the sounds' position and velocity, as the menus pass them */
void Sound_PlayAt(void *, Vec3f, Vec3f, s32);

void Port_Log(const char *fmt, ...);
int Port_OptPages(void);
const char *Port_OptPageName(int page);
int Port_OptCount(int page);
const char *Port_OptLabel(int page, int i);
const char *Port_OptValue(int page, int i);
int Port_OptNeedsRestart(int page, int i);
void Port_OptChange(int page, int i, int dir);
void Port_OptSetOpen(int open);
int Port_OptIsOpen(void);
void Port_OptMenuPad(u16 *buttons, s8 *x, s8 *y);
int Port_OptCapturing(void);
int Port_OptIsBindPage(int page);
extern u32 gfx_scene_4x3; /* port/fast3d/gfx_pc.cpp */

#define MODE_MAIN 0x13 /* the game mode menu: "press z-button for options menu" */
#define MODE_INTRO 0x1A   /* the course intro */
#define MODE_COUNTDOWN 0x1B
#define MODE_RACE 0x1C
#define MODE_PAUSE 0x1F   /* the pause menu */
#define MODE_OPTIONS 0x20
#define ROWS 7      /* rows shown at once */
#define ROW_H 13
#define PANEL_X0 22
#define PANEL_X1 298
#define PANEL_Y0 82
#define PANEL_Y1 230

#define BTN_A 0x8000
#define BTN_B 0x4000
#define BTN_Z 0x2000
#define BTN_START 0x1000
#define BTN_UP 0x0800
#define BTN_DOWN 0x0400
#define BTN_LEFT 0x0200
#define BTN_RIGHT 0x0100
#define BTN_L 0x0020
#define BTN_R 0x0010

/* The options screen's colours */
#define YELLOW 0xFFE628FF
#define GREEN 0x2EB043FF
#define WHITE 0xF0F0F0FF
#define GREY 0x9AA6B0FF
#define HINT 0xFFEE00FF /* the main menu's "press z-button" line */

#define GFX2(g, a, b) { Gfx *_g = (g)++; _g->w0 = (a); _g->w1 = (u32)(b); }

static s32 sPage, sSel, sTop;

static Font *menuFont(void) {
    UiTexts *ui = gUi[0];
    s32 i, n;

    if (ui == NULL) {
        return NULL;
    }
    n = ((UiCount *)ui)->nText;
    for (i = 0; i < n && i < 200; i++) {
        if (ui->text[i].font != NULL && (ui->text[i].font->siz == 2 || ui->text[i].font->siz == 3)) {
            return ui->text[i].font;
        }
    }
    return NULL;
}

/* The menu font keeps its letters under the capitals' codes (it draws them as the
   menus' lower case) */
static s32 glyph(Font *f, u8 c) {
    s32 i;

    if (c >= 'a' && c <= 'z') {
        c -= 'a' - 'A';
    }
    for (i = 0; i < f->numChars; i++) {
        if (f->map[i] == c) {
            return i;
        }
    }
    return -1;
}

static s32 textWidth(Font *f, const char *s) {
    s32 w = 0, i;

    for (; *s != 0; s++) {
        i = glyph(f, (u8)*s);
        w += i >= 0 ? f->widths[i] : 4;
    }
    return w;
}

static void playSound(s32 id) {
    Sound_PlayAt(D_8011EEF8[0], D_800BBF60, D_800BBF6C, id);
}

/* Text tinted as the game's text is (Font_DrawText's alpha mode): the glyphs'
   own colours, outline included, times the primitive colour */
static Gfx *drawText(Gfx *g, Font *f, s32 x, s32 y, const char *s, u32 rgba) {
    s32 i, j, sum, row, col;
    u8 *addr;

    GFX2(g, 0xE7000000, 0);
    GFX2(g, 0xE3000A01, 0);          /* one cycle */
    GFX2(g, 0xE3001001, 0);          /* no palette */
    GFX2(g, 0xE3001201, 0);          /* point sampled, as the game's text */
    GFX2(g, 0xE3000C00, 0);          /* no perspective */
    GFX2(g, 0xE200001C, 0x0F0A4000); /* blended, as the UI's sprites */
    GFX2(g, 0xFC119623, 0xFF2FFFFF); /* texel * primitive, colour and alpha */
    GFX2(g, 0xFA000000, rgba);
    for (; *s != 0; s++) {
        i = glyph(f, (u8)*s);
        if (i < 0) {
            x += 4; /* a space, or a character the font does not have */
            continue;
        }
        sum = 0;
        for (j = 0; j < i; j++) {
            sum += f->widths[j];
        }
        if (f->perRow == 0) {
            break;
        }
        row = sum / f->perRow;
        col = sum % f->perRow;
        addr = f->tex + (f->perRow * row * f->height + col) * (f->siz == 2 ? 2 : 4);
        if (f->siz == 2) {
            GFX2(g, 0xFD100000 | ((f->fmt & 7) << 21) | ((f->perRow - 1) & 0xFFF), addr);
            GFX2(g, 0xF5101200 | ((f->fmt & 7) << 21), 0x07080200);
            GFX2(g, 0xE6000000, 0);
            GFX2(g, 0xF4000000, 0x07080080);
            GFX2(g, 0xE7000000, 0);
            GFX2(g, 0xF5101200 | ((f->fmt & 7) << 21), 0x00080200);
            GFX2(g, 0xF2000000, 0x00080080);
        } else {
            GFX2(g, 0xFD180000 | ((f->fmt & 7) << 21) | ((f->perRow - 1) & 0xFFF), addr);
            GFX2(g, 0xF5181200 | ((f->fmt & 7) << 21), 0x07080200);
            GFX2(g, 0xE6000000, 0);
            GFX2(g, 0xF4000000, 0x07080080);
            GFX2(g, 0xE7000000, 0);
            GFX2(g, 0xF5181200 | ((f->fmt & 7) << 21), 0x00080200);
            GFX2(g, 0xF2000000, 0x00080080);
        }
        GFX2(g, 0xE4000000 | ((((x + f->widths[i]) * 4) & 0xFFF) << 12) | (((y + f->height) * 4) & 0xFFF),
             (((x * 4) & 0xFFF) << 12) | ((y * 4) & 0xFFF));
        GFX2(g, 0xE1000000, 0);
        GFX2(g, 0xF1000000, 0x04000400);
        x += f->widths[i];
    }
    return g;
}

/* With the game's drop shadow */
static Gfx *drawLabel(Gfx *g, Font *f, s32 x, s32 y, const char *s, u32 rgba) {
    g = drawText(g, f, x + 1, y + 1, s, 0x000000C0);
    return drawText(g, f, x, y, s, rgba);
}

static Gfx *drawPanel(Gfx *g, s32 x0, s32 y0, s32 x1, s32 y1, u32 rgba) {
    GFX2(g, 0xE7000000, 0);
    GFX2(g, 0xE3000A01, 0);
    GFX2(g, 0xE200001C, 0x0F0A4000);
    GFX2(g, 0xFCFFFFFF, 0xFFFDF6FB); /* colour and alpha = primitive */
    GFX2(g, 0xFA000000, rgba);
    GFX2(g, 0xF6000000 | ((x1 * 4) << 12) | (y1 * 4), ((x0 * 4) << 12) | (y0 * 4));
    return g;
}

/* A small triangle pointing up or down (the font has no arrows), centred on x */
static Gfx *drawArrow(Gfx *g, s32 x, s32 y, s32 up, u32 rgba) {
    s32 r;

    for (r = 0; r < 4; r++) {
        s32 half = up ? r : 3 - r;
        g = drawPanel(g, x - half, y + r, x + half + 1, y + r + 1, rgba);
    }
    return g;
}

/* Edges and held-direction repeats, once a game frame */
static u16 sPrev;
static s32 sHeld, sHeldFrames;

static u16 pressed(void) {
    u16 b, out;
    s8 x, y;
    s32 dir;

    Port_OptMenuPad(&b, &x, &y);
    if (y > 50) b |= BTN_UP;
    if (y < -50) b |= BTN_DOWN;
    if (x < -50) b |= BTN_LEFT;
    if (x > 50) b |= BTN_RIGHT;
    out = b & ~sPrev;
    dir = b & (BTN_UP | BTN_DOWN | BTN_LEFT | BTN_RIGHT);
    if (dir != 0 && dir == sHeld) {
        if (++sHeldFrames >= 18 && (sHeldFrames - 18) % 4 == 0) {
            out |= dir; /* held: repeats */
        }
    } else {
        sHeld = dir;
        sHeldFrames = 0;
    }
    sPrev = b;
    return out;
}

static s32 sWaitRelease; /* after a capture: until every button is let go */

static void input(void) {
    u16 p, b;
    s8 x, y;
    s32 n = Port_OptCount(sPage);

    if (Port_OptCapturing()) {
        sWaitRelease = 1;
        pressed();
        return;
    }
    if (sWaitRelease) {
        Port_OptMenuPad(&b, &x, &y);
        pressed();
        if (b == 0 && x > -30 && x < 30 && y > -30 && y < 30) {
            sWaitRelease = 0;
        }
        return;
    }
    p = pressed();

    if (p & (BTN_B | BTN_START)) {
        Port_OptSetOpen(0);
        playSound(5);
        return;
    }
    if (p & (BTN_L | BTN_Z | BTN_R)) {
        sPage = (sPage + ((p & BTN_L) ? Port_OptPages() - 1 : 1)) % Port_OptPages();
        sSel = sTop = 0;
        playSound(7);
        return;
    }
    if (p & BTN_UP) {
        sSel = (sSel + n - 1) % n;
        playSound(7);
    } else if (p & BTN_DOWN) {
        sSel = (sSel + 1) % n;
        playSound(7);
    } else if (p & (BTN_LEFT | BTN_RIGHT | BTN_A)) {
        Port_OptChange(sPage, sSel, (p & BTN_LEFT) ? -1 : 1);
        playSound(7);
    }
    if (sSel < sTop) {
        sTop = sSel;
    } else if (sSel >= sTop + ROWS) {
        sTop = sSel - ROWS + 1;
    }
}

Gfx *Port_PcMenuDraw(Gfx *g) {
    Font *f;
    s32 i, x, y, n;

    /* Widescreen shows more of the 3D only while racing (the course intro, the
       countdown, the race) and on the pause menu's first screen. Everywhere else
       (the menus, the title screen's demo, the screens reached from pause, the
       results) the 3D keeps to the 4:3 area and the sides are black, as the N64's
       overscan. */
    gfx_scene_4x3 = gGame == NULL || !(gGame->mode == MODE_INTRO || gGame->mode == MODE_COUNTDOWN ||
                                       gGame->mode == MODE_RACE || gGame->mode == MODE_PAUSE);
    if (gGame == NULL || (gGame->mode != MODE_OPTIONS && gGame->mode != MODE_MAIN)) {
        if (Port_OptIsOpen()) {
            Port_OptSetOpen(0);
        }
        pressed(); /* keeps the edges current */
        return g;
    }
    f = menuFont();
    if (f == NULL) {
        return g;
    }
    if (!Port_OptIsOpen()) {
        if (pressed() & BTN_R) {
            Port_OptSetOpen(1);
            sSel = sTop = 0;
            playSound(6);
        } else {
            /* on the main menu under the line for Z, in its yellow */
            static const char hint[] = "press r-button for pc options";
            return drawLabel(g, f, 160 - textWidth(f, hint) / 2, gGame->mode == MODE_MAIN ? 222 : 214, hint, HINT);
        }
    } else {
        input();
        if (!Port_OptIsOpen()) {
            return g;
        }
    }

    for (i = 0; i < 3; i++) { /* layered: the rows behind barely show */
        g = drawPanel(g, PANEL_X0, PANEL_Y0, PANEL_X1, PANEL_Y1, 0x060C20C0);
    }
    /* the pages, the current one lit, spaced to fit the panel */
    {
        s32 total = 0, gap, pages = Port_OptPages();
        for (i = 0; i < pages; i++) {
            total += textWidth(f, Port_OptPageName(i));
        }
        gap = pages > 1 ? (PANEL_X1 - PANEL_X0 - 20 - total) / (pages - 1) : 0;
        gap = gap > 12 ? 12 : gap < 3 ? 3 : gap;
        x = PANEL_X0 + 10 + (PANEL_X1 - PANEL_X0 - 20 - total - gap * (pages - 1)) / 2;
        for (i = 0; i < pages; i++) {
            g = drawLabel(g, f, x, PANEL_Y0 + 4, Port_OptPageName(i), i == sPage ? YELLOW : GREEN);
            x += textWidth(f, Port_OptPageName(i)) + gap;
        }
    }
    g = drawPanel(g, PANEL_X0 + 6, PANEL_Y0 + 19, PANEL_X1 - 6, PANEL_Y0 + 20, 0x2EB04380);
    n = Port_OptCount(sPage);
    for (i = sTop; i < n && i < sTop + ROWS; i++) {
        char v[48];
        s32 room = PANEL_X1 - 12 - (PANEL_X0 + 12) - textWidth(f, Port_OptLabel(sPage, i)) - 8, len;
        y = PANEL_Y0 + 24 + (i - sTop) * ROW_H;
        /* a value too long for what the label leaves of the row (a list of bindings)
           loses its end */
        for (len = 0; len < 47 && Port_OptValue(sPage, i)[len] != 0; len++) {
            v[len] = Port_OptValue(sPage, i)[len];
        }
        v[len] = 0;
        while (len > 3 && textWidth(f, v) > room) {
            v[--len] = 0;
            v[len - 1] = '.';
        }
        g = drawLabel(g, f, PANEL_X0 + 12, y, Port_OptLabel(sPage, i), i == sSel ? YELLOW : GREEN);
        g = drawLabel(g, f, PANEL_X1 - 12 - textWidth(f, v), y, v, i == sSel ? YELLOW : WHITE);
    }
    if (sTop > 0) {
        g = drawArrow(g, 154, PANEL_Y0 + 27 + ROWS * ROW_H, 1, GREY); /* more above */
    }
    if (sTop + ROWS < n) {
        g = drawArrow(g, 166, PANEL_Y0 + 27 + ROWS * ROW_H, 0, GREY); /* more below */
    }
    if (Port_OptCapturing()) {
        static const char note[] = "esc to cancel";
        g = drawLabel(g, f, 160 - textWidth(f, note) / 2, PANEL_Y1 - 14, note, GREY);
    } else if (Port_OptIsBindPage(sPage)) {
        static const char help[] = "a set   left default   b back";
        g = drawLabel(g, f, 160 - textWidth(f, help) / 2, PANEL_Y1 - 14, help, GREY);
    } else if (Port_OptNeedsRestart(sPage, sSel)) {
        static const char note[] = "takes effect when the game restarts";
        g = drawLabel(g, f, 160 - textWidth(f, note) / 2, PANEL_Y1 - 14, note, GREY);
    } else {
        static const char help[] = "l/z page   b back";
        g = drawLabel(g, f, 160 - textWidth(f, help) / 2, PANEL_Y1 - 14, help, GREY);
    }
    return g;
}
