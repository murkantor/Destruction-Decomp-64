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
    while (SDL_PollEvent(&e)) {
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
