/*
 * SDL events, pumped on the main thread once a host frame: quit, gamepads
 * coming and going, and the window keys (Alt+Enter or F11: fullscreen).
 */
#include <SDL3/SDL.h>

#include "port.h"
#include "port_config.h"

void Port_InputGamepadAdded(SDL_JoystickID id);
void Port_InputGamepadRemoved(SDL_JoystickID id);
void Port_VideoToggleFullscreen(void);

/* DD64_FAKEKEY=frame:Key (testing): a real key press and release at that presented frame */
static void fakeKey(void) {
    extern unsigned gfx_presents;
    static const char *spec = (const char *)1;
    static unsigned at, phase;
    char name[64];

    if (spec == (const char *)1) {
        spec = SDL_getenv("DD64_FAKEKEY");
        if (spec != NULL) {
            at = (unsigned)SDL_strtoul(spec, NULL, 10);
        }
    }
    if (spec == NULL || phase >= 2 || gfx_presents < at + phase * 6) {
        return;
    }
    SDL_strlcpy(name, SDL_strchr(spec, ':') != NULL ? SDL_strchr(spec, ':') + 1 : "", sizeof(name));
    {
        SDL_Event e;
        SDL_zero(e);
        e.type = phase == 0 ? SDL_EVENT_KEY_DOWN : SDL_EVENT_KEY_UP;
        e.key.scancode = SDL_GetScancodeFromName(name);
        e.key.down = phase == 0;
        SDL_PushEvent(&e);
    }
    phase++;
}

void Port_PumpEvents(void) {
    SDL_Event e;

    fakeKey();
    /* one pump (the window's messages, the controllers polled), then what it queued:
       SDL_PollEvent pumps again on every call, the last, empty, one too */
    {
        static int log = -1;
        static Uint64 since, spent;
        static unsigned calls;
        Uint64 t0 = SDL_GetTicksNS();

        if (log < 0) {
            log = SDL_getenv("DD64_GFXLOG") != NULL;
            since = t0;
        }
        SDL_PumpEvents();
        if (log) {
            spent += SDL_GetTicksNS() - t0;
            calls++;
            if (t0 - since > 5000000000ull) {
                Port_Log("events: pumped %.0f times/s, %.1f us each", calls / ((t0 - since) / 1e9), spent / 1e3 / calls);
                since = t0;
                spent = 0;
                calls = 0;
            }
        }
    }
    while (SDL_PeepEvents(&e, 1, SDL_GETEVENT, SDL_EVENT_FIRST, SDL_EVENT_LAST) == 1) {
        if (e.type != SDL_EVENT_KEY_DOWN && Port_InputCaptureEvent(&e)) {
            continue;
        }
        switch (e.type) {
            case SDL_EVENT_QUIT:
                Port_RequestQuit();
                break;
            case SDL_EVENT_GAMEPAD_ADDED:
                Port_InputGamepadAdded(e.gdevice.which);
                break;
            case SDL_EVENT_GAMEPAD_REMOVED:
                Port_InputGamepadRemoved(e.gdevice.which);
                break;
            case SDL_EVENT_KEY_DOWN:
                if (Port_InputCaptureEvent(&e)) {
                    break;
                }
                if (e.key.repeat) {
                    break;
                }
                if (e.key.scancode == SDL_SCANCODE_F11 ||
                    (e.key.scancode == SDL_SCANCODE_RETURN && (e.key.mod & SDL_KMOD_ALT))) {
                    Port_VideoToggleFullscreen();
                }
                break;
            default:
                break;
        }
    }
}
