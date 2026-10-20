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

void Port_PumpEvents(void) {
    SDL_Event e;

    while (SDL_PollEvent(&e)) {
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
