/*
 * Controllers: SDL3 gamepads and the keyboard, as N64 controllers.
 *
 * Gamepads take controllers 1-4 in the order they are connected; the keyboard
 * drives the controller [input] keyboard_player names, alongside any gamepad
 * on it. Controller 1 is always connected, so the game can be played with no
 * gamepad. Bindings are in dd64.ini, [keyboard] and [gamepad]: a comma
 * separated list per N64 input, of SDL key names ("Left Shift", "X") or SDL
 * gamepad button names ("a", "leftshoulder", "dpup") and axis directions
 * ("+righty", "-leftx", "+lefttrigger").
 *
 * A raphnet N64 adapter ([input] raphnet = 1) is read raw (port/src/raphnet.c):
 * its ports are controllers raphnet_player and the next, with the controller's
 * own buttons and stick values, unmapped and without a dead zone, and its
 * Rumble Pak driven directly. SDL leaves the adapter's joystick side alone then.
 */
#include <SDL3/SDL.h>
#include <string.h>

#include "port.h"
#include "port_config.h"
#include "port_ini.h"
#include "port_raphnet.h"

#define CONT_A 0x8000
#define CONT_B 0x4000
#define CONT_G 0x2000 /* Z */
#define CONT_START 0x1000
#define CONT_UP 0x0800
#define CONT_DOWN 0x0400
#define CONT_LEFT 0x0200
#define CONT_RIGHT 0x0100
#define CONT_L 0x0020
#define CONT_R 0x0010
#define CONT_E 0x0008 /* C up */
#define CONT_D 0x0004 /* C down */
#define CONT_C 0x0002 /* C left */
#define CONT_F 0x0001 /* C right */

/* Stick directions, after the button bits */
enum { IN_STICK_UP = 14, IN_STICK_DOWN, IN_STICK_LEFT, IN_STICK_RIGHT, IN_COUNT };

typedef struct {
    const char *key;
    u16 bit; /* 0 for the stick directions */
    const char *keyboard;
    const char *gamepad;
} InputDef;

static const InputDef sInputs[IN_COUNT] = {
    { "a", CONT_A, "X", "a" },
    { "b", CONT_B, "C", "x,b" },
    { "z", CONT_G, "Z", "lefttrigger" },
    { "start", CONT_START, "Return", "start" },
    { "l", CONT_L, "A", "leftshoulder" },
    { "r", CONT_R, "S", "rightshoulder,righttrigger" },
    { "dpad_up", CONT_UP, "T", "dpup" },
    { "dpad_down", CONT_DOWN, "G", "dpdown" },
    { "dpad_left", CONT_LEFT, "F", "dpleft" },
    { "dpad_right", CONT_RIGHT, "H", "dpright" },
    { "c_up", CONT_E, "I", "-righty,y" },
    { "c_down", CONT_D, "K", "+righty" },
    { "c_left", CONT_C, "J", "-rightx" },
    { "c_right", CONT_F, "L", "+rightx" },
    { "stick_up", 0, "Up", "-lefty" },
    { "stick_down", 0, "Down", "+lefty" },
    { "stick_left", 0, "Left", "-leftx" },
    { "stick_right", 0, "Right", "+leftx" },
};

#define MAX_BINDS 4

typedef struct {
    int n;
    SDL_Scancode keys[MAX_BINDS];
} KeyBind;

typedef struct {
    int n;
    struct {
        int axis;   /* SDL_GamepadAxis, or -1 for a button */
        int button; /* SDL_GamepadButton */
        int sign;   /* +1 or -1 for an axis */
    } src[MAX_BINDS];
} PadBind;

static KeyBind sKeyBinds[IN_COUNT];
static PadBind sPadBinds[IN_COUNT];

/* The gamepad on each controller, or NULL. The event pump (main thread)
   changes these while game threads read them: sLock covers both. */
static SDL_Gamepad *sPads[MAXCONTROLLERS];
static float sGas[MAXCONTROLLERS] = { 1, 1, 1, 1 }, sBrake[MAXCONTROLLERS] = { 1, 1, 1, 1 }; /* the pedals, 0-1 */
static SDL_Mutex *sLock;
static int sRumbleOn[MAXCONTROLLERS];

#define RAPHNET_VID 0x289B

/* The raphnet adapter port on a controller, or -1 */
static int raphnetChan(int channel) {
    int c = channel - (gPortConfig.raphnetPlayer - 1);

    return gPortConfig.raphnet && c >= 0 && c < RAPHNET_MAX_CHAN ? c : -1;
}

/* A controller is plugged into the raphnet port for this controller */
static int raphnetPad(int channel) {
    int c = raphnetChan(channel);

    return c >= 0 && Port_RaphnetPeek(c) == 1;
}

static void parseKeys(KeyBind *b, const char *list) {
    char buf[128], *tok, *save = NULL;

    b->n = 0;
    SDL_strlcpy(buf, list, sizeof(buf));
    for (tok = SDL_strtok_r(buf, ",", &save); tok != NULL && b->n < MAX_BINDS; tok = SDL_strtok_r(NULL, ",", &save)) {
        while (*tok == ' ') {
            tok++;
        }
        SDL_Scancode sc = SDL_GetScancodeFromName(tok);
        if (sc != SDL_SCANCODE_UNKNOWN) {
            b->keys[b->n++] = sc;
        } else if (*tok != '\0') {
            Port_Log("input: unknown key \"%s\" in dd64.ini", tok);
        }
    }
}

static void parsePad(PadBind *b, const char *list) {
    char buf[128], *tok, *save = NULL;

    b->n = 0;
    SDL_strlcpy(buf, list, sizeof(buf));
    for (tok = SDL_strtok_r(buf, ",", &save); tok != NULL && b->n < MAX_BINDS; tok = SDL_strtok_r(NULL, ",", &save)) {
        while (*tok == ' ') {
            tok++;
        }
        if (*tok == '+' || *tok == '-') {
            SDL_GamepadAxis axis = SDL_GetGamepadAxisFromString(tok + 1);
            if (axis != SDL_GAMEPAD_AXIS_INVALID) {
                b->src[b->n].axis = axis;
                b->src[b->n].sign = *tok == '+' ? 1 : -1;
                b->n++;
                continue;
            }
        } else {
            SDL_GamepadButton button = SDL_GetGamepadButtonFromString(tok);
            SDL_GamepadAxis axis = SDL_GetGamepadAxisFromString(tok);
            if (button != SDL_GAMEPAD_BUTTON_INVALID) {
                b->src[b->n].axis = -1;
                b->src[b->n].button = button;
                b->n++;
                continue;
            }
            if (axis != SDL_GAMEPAD_AXIS_INVALID) { /* a trigger, named without a sign */
                b->src[b->n].axis = axis;
                b->src[b->n].sign = 1;
                b->n++;
                continue;
            }
        }
        if (*tok != '\0') {
            Port_Log("input: unknown gamepad input \"%s\" in dd64.ini", tok);
        }
    }
}

void Port_InputInit(void) {
    int i, added = 0;

    sLock = SDL_CreateMutex();
    if (!SDL_InitSubSystem(SDL_INIT_GAMEPAD)) {
        Port_Log("input: gamepads unavailable: %s", SDL_GetError());
    }
    for (i = 0; i < IN_COUNT; i++) {
        const char *k = Port_IniGet("keyboard", sInputs[i].key, NULL);
        const char *p = Port_IniGet("gamepad", sInputs[i].key, NULL);
        if (k == NULL) {
            Port_IniSet("keyboard", sInputs[i].key, sInputs[i].keyboard);
            k = sInputs[i].keyboard;
            added = 1;
        }
        if (p == NULL) {
            Port_IniSet("gamepad", sInputs[i].key, sInputs[i].gamepad);
            p = sInputs[i].gamepad;
            added = 1;
        }
        parseKeys(&sKeyBinds[i], k);
        parsePad(&sPadBinds[i], p);
    }
    if (added) {
        Port_ConfigSave();
    }
    if (gPortConfig.raphnet) {
        Port_RaphnetStart();
    }
    /* Gamepads already connected arrive as SDL_EVENT_GAMEPAD_ADDED */
}

void Port_InputGamepadAdded(SDL_JoystickID id) {
    int i;

    SDL_LockMutex(sLock);
    for (i = 0; i < MAXCONTROLLERS; i++) {
        if (sPads[i] != NULL && SDL_GetGamepadID(sPads[i]) == id) {
            SDL_UnlockMutex(sLock);
            return;
        }
    }
    if (gPortConfig.raphnet && Port_RaphnetOnline() && SDL_GetGamepadVendorForID(id) == RAPHNET_VID) {
        Port_Log("input: %s is read raw (raphnet)", SDL_GetGamepadNameForID(id));
        SDL_UnlockMutex(sLock);
        return;
    }
    /* the first controller with no gamepad and no raphnet controller */
    for (i = 0; i < MAXCONTROLLERS && (sPads[i] != NULL || raphnetPad(i)); i++) {
    }
    if (i < MAXCONTROLLERS) {
        sPads[i] = SDL_OpenGamepad(id);
        if (sPads[i] != NULL) {
            Port_Log("input: controller %d: %s", i + 1, SDL_GetGamepadName(sPads[i]));
        }
    }
    SDL_UnlockMutex(sLock);
}

void Port_InputGamepadRemoved(SDL_JoystickID id) {
    int i;

    SDL_LockMutex(sLock);
    for (i = 0; i < MAXCONTROLLERS; i++) {
        if (sPads[i] != NULL && SDL_GetGamepadID(sPads[i]) == id) {
            Port_Log("input: controller %d disconnected", i + 1);
            SDL_CloseGamepad(sPads[i]);
            sPads[i] = NULL;
            sRumbleOn[i] = 0;
        }
    }
    SDL_UnlockMutex(sLock);
}

int Port_InputConnected(int channel) {
    int connected;

    if (channel < 0 || channel >= MAXCONTROLLERS) {
        return 0;
    }
    SDL_LockMutex(sLock);
    connected = channel == 0 || sPads[channel] != NULL || channel == gPortConfig.keyboardPlayer - 1 ||
                raphnetPad(channel);
    SDL_UnlockMutex(sLock);
    return connected;
}

/* How far an input is pressed, 0-1 */
static float keyAmount(const KeyBind *b, const bool *keys) {
    int i;

    for (i = 0; i < b->n; i++) {
        SDL_Scancode sc = b->keys[i];
        /* Alt+Enter is the window's fullscreen key, not Start */
        if (keys[sc] && !(sc == SDL_SCANCODE_RETURN && (keys[SDL_SCANCODE_LALT] || keys[SDL_SCANCODE_RALT]))) {
            return 1.0f;
        }
    }
    return 0.0f;
}

static float padAmount(const PadBind *b, SDL_Gamepad *pad) {
    float best = 0.0f;
    int i;

    for (i = 0; i < b->n; i++) {
        float v;
        if (b->src[i].axis < 0) {
            v = SDL_GetGamepadButton(pad, (SDL_GamepadButton)b->src[i].button) ? 1.0f : 0.0f;
        } else if (gPortConfig.freeCamera &&
                   (b->src[i].axis == SDL_GAMEPAD_AXIS_RIGHTX || b->src[i].axis == SDL_GAMEPAD_AXIS_RIGHTY)) {
            continue; /* the right stick turns the camera instead (port/game/freecam.c) */
        } else if (gPortConfig.analogPedals && (b->src[i].axis == SDL_GAMEPAD_AXIS_LEFT_TRIGGER ||
                                                b->src[i].axis == SDL_GAMEPAD_AXIS_RIGHT_TRIGGER)) {
            continue; /* the triggers are the pedals instead (readPad) */
        } else {
            v = SDL_GetGamepadAxis(pad, (SDL_GamepadAxis)b->src[i].axis) / 32767.0f * b->src[i].sign;
        }
        if (v > best) {
            best = v;
        }
    }
    return best > 1.0f ? 1.0f : best;
}

/* An axis from two directions, with the dead zone, scaled to the N64 range */
static s8 stickAxis(float pos, float neg) {
    float v = pos - neg, dz = gPortConfig.deadzone / 100.0f, mag = v < 0 ? -v : v;

    if (mag <= dz) {
        return 0;
    }
    mag = (mag - dz) / (1.0f - dz);
    if (mag > 1.0f) {
        mag = 1.0f;
    }
    v = (v < 0 ? -mag : mag) * SDL_clamp(gPortConfig.stickRange, 0, 127);
    return (s8)(v < 0 ? v - 0.5f : v + 0.5f);
}

/*
 * DD64_PRESS (testing): controller 1 input by frame, "frame:inputs ...", each
 * held from that presented frame until the next entry; inputs are the [keyboard]
 * names joined with '+' ("1900:a 1906:none 2000:start+stick_up").
 */
extern unsigned gfx_presents;
static void readPad(int channel, OSContPad *out);

static void pressNamed(const char *name, u16 *buttons, float *amount) {
    int i;

    for (i = 0; i < IN_COUNT; i++) {
        if (SDL_strcasecmp(name, sInputs[i].key) == 0) {
            amount[i] = 1.0f;
            *buttons |= sInputs[i].bit;
        }
    }
}

/* DD64_TAP (testing): "input:period ..." taps each input for 4 frames every period frames */
static int tappedPad(u16 *buttons, float *amount) {
    static const char *taps;
    static int checked;
    const char *p;
    char name[32];

    if (!checked) {
        taps = SDL_getenv("DD64_TAP");
        checked = 1;
    }
    if (taps == NULL) {
        return 0;
    }
    for (p = taps; *p != '\0';) {
        size_t n = strcspn(p, ": ");
        unsigned period;
        SDL_strlcpy(name, p, n + 1 < sizeof(name) ? n + 1 : sizeof(name));
        p += n;
        if (*p != ':') {
            break;
        }
        period = (unsigned)SDL_strtoul(p + 1, (char **)&p, 10);
        if (period > 4 && gfx_presents % period < 4) {
            pressNamed(name, buttons, amount);
        }
        p += strspn(p, " ");
    }
    return 1;
}

static int scriptedPad(u16 *buttons, float *amount) {
    static const char *script;
    static int checked;
    const char *p, *best = NULL;
    char name[32];
    int i, tapped;

    if (!checked) {
        script = SDL_getenv("DD64_PRESS");
        checked = 1;
    }
    *buttons = 0;
    for (i = 0; i < IN_COUNT; i++) {
        amount[i] = 0.0f;
    }
    tapped = tappedPad(buttons, amount);
    if (script == NULL) {
        return tapped;
    }
    for (p = script; *p != '\0';) {
        unsigned frame = (unsigned)SDL_strtoul(p, (char **)&p, 10);
        if (*p != ':') {
            break;
        }
        p++;
        if (frame <= gfx_presents) {
            best = p;
        }
        p += strcspn(p, " ");
        p += strspn(p, " ");
    }
    while (best != NULL && *best != '\0' && *best != ' ') {
        size_t n = strcspn(best, "+ ");
        SDL_strlcpy(name, best, n + 1 < sizeof(name) ? n + 1 : sizeof(name));
        pressNamed(name, buttons, amount);
        best += n;
        if (*best == '+') {
            best++;
        }
    }
    return 1;
}

/* DD64_FAKEGAS=frame:amount (testing): controller 1's gas held there from that frame */
static void fakeGas(int channel, u16 *buttons, float *gas) {
    static const char *fake = (const char *)1;
    extern unsigned gfx_presents;

    if (fake == (const char *)1) {
        fake = SDL_getenv("DD64_FAKEGAS");
    }
    if (fake != NULL && channel == 0 && gfx_presents >= (unsigned)SDL_atoi(fake) && SDL_strchr(fake, ':') != NULL) {
        *gas = (float)SDL_atof(SDL_strchr(fake, ':') + 1);
        *buttons |= CONT_A;
    }
}

void Port_InputGetPad(int channel, OSContPad *out) {
    if (channel == 0) {
        float amount[IN_COUNT];
        u16 buttons;
        if (scriptedPad(&buttons, amount)) {
            float gas = 1.0f;
            fakeGas(channel, &buttons, &gas);
            sGas[0] = gas;
            sBrake[0] = 1.0f;
            out->button = buttons;
            out->stick_x = stickAxis(amount[IN_STICK_RIGHT], amount[IN_STICK_LEFT]);
            out->stick_y = stickAxis(amount[IN_STICK_UP], amount[IN_STICK_DOWN]);
            return;
        }
    }
    readPad(channel, out);
}

static void readPad(int channel, OSContPad *out) {
    float amount[IN_COUNT];
    const bool *keys = SDL_GetKeyboardState(NULL);
    int useKeys = channel == gPortConfig.keyboardPlayer - 1;
    SDL_Gamepad *pad;
    u16 buttons = 0;
    int i;

    SDL_LockMutex(sLock);
    pad = channel >= 0 && channel < MAXCONTROLLERS ? sPads[channel] : NULL;
    for (i = 0; i < IN_COUNT; i++) {
        float a = 0.0f;
        if (useKeys && keys != NULL) {
            a = keyAmount(&sKeyBinds[i], keys);
        }
        if (pad != NULL) {
            float p = padAmount(&sPadBinds[i], pad);
            if (p > a) {
                a = p;
            }
        }
        amount[i] = a;
        if (sInputs[i].bit != 0 && a >= 0.5f) {
            buttons |= sInputs[i].bit;
        }
    }
    {
        /* Analog pedals: the right trigger is A (gas) and the left B (brake), and how far
           each is pressed scales the car's engine and brakes (port/game/pedals.c). A
           button, or a trigger pressed all the way, is the N64's full press. */
        float gas = amount[0] >= 0.5f ? 1.0f : 0.0f, brake = amount[1] >= 0.5f ? 1.0f : 0.0f;
        if (gPortConfig.analogPedals && pad != NULL) {
            float rt = SDL_GetGamepadAxis(pad, SDL_GAMEPAD_AXIS_RIGHT_TRIGGER) / 32767.0f;
            float lt = SDL_GetGamepadAxis(pad, SDL_GAMEPAD_AXIS_LEFT_TRIGGER) / 32767.0f;
            if (rt > 0.06f && gas < 1.0f) {
                gas = rt > 0.95f ? 1.0f : rt;
                buttons |= CONT_A;
            }
            if (lt > 0.06f && brake < 1.0f) {
                brake = lt > 0.95f ? 1.0f : lt;
                buttons |= CONT_B;
            }
        }
        fakeGas(channel, &buttons, &gas);
        if (channel >= 0 && channel < MAXCONTROLLERS) {
            sGas[channel] = gas;
            sBrake[channel] = brake;
        }
    }
    SDL_UnlockMutex(sLock);
    out->button = buttons;
    out->stick_x = stickAxis(amount[IN_STICK_RIGHT], amount[IN_STICK_LEFT]);
    out->stick_y = stickAxis(amount[IN_STICK_UP], amount[IN_STICK_DOWN]);
    if (raphnetChan(channel) >= 0) {
        /* a real N64 controller: its own values, with the keyboard's buttons added */
        u16 rb;
        s8 rx, ry;
        if (Port_RaphnetPoll(raphnetChan(channel), &rb, &rx, &ry) == 1) {
            out->button |= rb;
            if (out->stick_x == 0 && out->stick_y == 0) {
                out->stick_x = rx;
                out->stick_y = ry;
            }
        }
    }
}

void Port_InputRumble(int channel, int on) {
    SDL_Gamepad *pad;
    Uint16 strength;

    if (channel < 0 || channel >= MAXCONTROLLERS) {
        return;
    }
    if (raphnetPad(channel)) {
        Port_RaphnetRumble(raphnetChan(channel), on && gPortConfig.rumble);
        return;
    }
    SDL_LockMutex(sLock);
    pad = sPads[channel];
    on = on && gPortConfig.rumble;
    if (pad != NULL && (on || sRumbleOn[channel])) {
        sRumbleOn[channel] = on;
        strength = (Uint16)(0xFFFF * gPortConfig.rumbleStrength / 100);
        /* the game switches the motor on and off itself; the duration is a safety net */
        SDL_RumbleGamepad(pad, on ? strength : 0, on ? strength : 0, on ? 500 : 0);
    }
    SDL_UnlockMutex(sLock);
}

/* ---- Rebinding (the PC options page's keys and pad pages) ---- */

static const char *const sInputLabels[IN_COUNT] = {
    "a", "b", "z", "start", "l", "r", "d-pad up", "d-pad down", "d-pad left", "d-pad right",
    "c up", "c down", "c left", "c right", "stick up", "stick down", "stick left", "stick right",
};

static int sCapDevice = -1; /* 0 keyboard, 1 gamepad, -1 not capturing */
static int sCapInput;
static u64 sCapUntilNs;
static Sint16 sCapRest[SDL_GAMEPAD_AXIS_COUNT]; /* each axis when the capture began */

int Port_InputCount(void) {
    return IN_COUNT;
}

const char *Port_InputLabel(int i) {
    return sInputLabels[i];
}

/* The bindings of an input as dd64.ini has them ("Left Shift,X") */
const char *Port_InputBinding(int device, int i) {
    return Port_IniGet(device == 0 ? "keyboard" : "gamepad", sInputs[i].key, "");
}

static void setBinding(int device, int i, const char *list) {
    SDL_LockMutex(sLock);
    Port_IniSet(device == 0 ? "keyboard" : "gamepad", sInputs[i].key, list);
    if (device == 0) {
        parseKeys(&sKeyBinds[i], list);
    } else {
        parsePad(&sPadBinds[i], list);
    }
    SDL_UnlockMutex(sLock);
}

void Port_InputResetBinding(int device, int i) {
    setBinding(device, i, device == 0 ? sInputs[i].keyboard : sInputs[i].gamepad);
}

/* Wait for the next key (device 0) or gamepad button or axis (1) for input i */
void Port_InputBeginCapture(int device, int i) {
    int a;

    SDL_LockMutex(sLock);
    for (a = 0; a < SDL_GAMEPAD_AXIS_COUNT; a++) {
        sCapRest[a] = 0;
        for (int c = 0; c < MAXCONTROLLERS; c++) {
            if (sPads[c] != NULL) {
                sCapRest[a] = SDL_GetGamepadAxis(sPads[c], (SDL_GamepadAxis)a);
                break;
            }
        }
    }
    SDL_UnlockMutex(sLock);
    sCapInput = i;
    sCapUntilNs = SDL_GetTicksNS() + 6000000000ULL;
    sCapDevice = device;
}

int Port_InputCapturing(void) {
    if (sCapDevice >= 0 && SDL_GetTicksNS() > sCapUntilNs) {
        sCapDevice = -1; /* nothing pressed: left as it was */
    }
    return sCapDevice >= 0;
}

/* The event pump, for each event: 1 if a capture took it */
int Port_InputCaptureEvent(const SDL_Event *e) {
    char name[64];

    if (sCapDevice < 0) {
        return 0;
    }
    name[0] = '\0';
    if (e->type == SDL_EVENT_KEY_DOWN) {
        if (e->key.repeat) {
            return 1;
        }
        if (e->key.scancode == SDL_SCANCODE_ESCAPE) {
            sCapDevice = -1;
            return 1;
        }
        if (sCapDevice != 0) {
            return 1;
        }
        SDL_strlcpy(name, SDL_GetScancodeName(e->key.scancode), sizeof(name));
    } else if (e->type == SDL_EVENT_GAMEPAD_BUTTON_DOWN && sCapDevice == 1) {
        SDL_strlcpy(name, SDL_GetGamepadStringForButton((SDL_GamepadButton)e->gbutton.button), sizeof(name));
    } else if (e->type == SDL_EVENT_GAMEPAD_AXIS_MOTION && sCapDevice == 1) {
        int a = e->gaxis.axis, v = e->gaxis.value, moved = v - sCapRest[a];
        if (moved > 20000 || moved < -20000) {
            SDL_snprintf(name, sizeof(name), "%s%s", moved > 0 ? "+" : "-",
                         SDL_GetGamepadStringForAxis((SDL_GamepadAxis)a));
        }
    } else {
        return 0;
    }
    if (name[0] != '\0') {
        setBinding(sCapDevice, sCapInput, name);
        Port_Log("input: %s %s = %s", sCapDevice == 0 ? "keyboard" : "gamepad", sInputs[sCapInput].key, name);
        sCapDevice = -1;
        Port_ConfigSave();
    }
    return 1;
}

/* The free camera: a player's right stick, -1 to 1 each way (right, up), with the
   dead zone. 0 if the free camera is off or the player has no gamepad. */
int Port_InputRightStick(int player, float *x, float *y) {
    float dz = gPortConfig.deadzone / 100.0f, vx = 0.0f, vy = 0.0f;
    int ok = 0;

    *x = *y = 0.0f;
    if (!gPortConfig.freeCamera || player < 0 || player >= MAXCONTROLLERS) {
        return 0;
    }
    SDL_LockMutex(sLock);
    if (sPads[player] != NULL) {
        vx = SDL_GetGamepadAxis(sPads[player], SDL_GAMEPAD_AXIS_RIGHTX) / 32767.0f;
        vy = -SDL_GetGamepadAxis(sPads[player], SDL_GAMEPAD_AXIS_RIGHTY) / 32767.0f;
        ok = 1;
    }
    SDL_UnlockMutex(sLock);
    {
        /* DD64_FAKERSTICK=frame:x,y (testing): the stick held there from that frame on */
        static const char *fake = (const char *)1;
        extern unsigned gfx_presents;
        if (fake == (const char *)1) {
            fake = SDL_getenv("DD64_FAKERSTICK");
        }
        if (fake != NULL && player == 0 && gfx_presents >= (unsigned)SDL_atoi(fake)) {
            const char *c = SDL_strchr(fake, ':');
            if (c != NULL) {
                vx = (float)SDL_atof(c + 1);
                c = SDL_strchr(c, ',');
                vy = c != NULL ? (float)SDL_atof(c + 1) : 0.0f;
                ok = 1;
            }
        }
    }
    if (vx * vx + vy * vy > dz * dz) {
        *x = vx > 1.0f ? 1.0f : vx < -1.0f ? -1.0f : vx;
        *y = vy > 1.0f ? 1.0f : vy < -1.0f ? -1.0f : vy;
    }
    return ok;
}

/* How far a controller's gas and brake are pressed, 0-1 (1 with buttons) */
float Port_InputGas(int channel) {
    return channel >= 0 && channel < MAXCONTROLLERS ? sGas[channel] : 1.0f;
}

float Port_InputBrake(int channel) {
    return channel >= 0 && channel < MAXCONTROLLERS ? sBrake[channel] : 1.0f;
}
