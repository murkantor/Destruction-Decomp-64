/* raphnet N64-to-USB adapters, read raw (port/src/raphnet.c). */
#ifndef PORT_RAPHNET_H
#define PORT_RAPHNET_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define RAPHNET_MAX_CHAN 2 /* controller ports on one adapter */

/* Open the adapter (if one is plugged in) and start the poller. Main thread,
   before the game's threads read controllers. */
void Port_RaphnetStart(void);
/* Stop the poller and hand the adapter back to its own polling: before SDL_Quit */
void Port_RaphnetStop(void);
/* An adapter answers raw commands (or is being reconnected) */
int Port_RaphnetOnline(void);
/* A port's controller: 1 with its buttons (CONT_* layout) and stick (N64
   units), 0 if no controller is plugged in, -1 on an adapter error. Any thread. */
int Port_RaphnetPoll(int chan, uint16_t *btn, int8_t *x, int8_t *y);
/* The same answer, without marking the port as being read */
int Port_RaphnetPeek(int chan);
/* The Rumble Pak's motor on or off */
void Port_RaphnetRumble(int chan, int on);

#ifdef __cplusplus
}
#endif

#endif
