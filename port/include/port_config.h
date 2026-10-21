/* Settings, kept in dd64.ini next to the executable (port/src/config.c). */
#ifndef PORT_CONFIG_H
#define PORT_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    /* [video] */
    int windowWidth;
    int windowHeight;
    int fullscreen;          /* 0 windowed, 1 fullscreen */
    int fullscreenExclusive; /* 0 borderless desktop, 1 exclusive mode */
    int vsync;               /* 0 off, 1 on, -1 adaptive, 2 VRR */
    int maxFps;              /* frame cap, 0 = none (the display's rate with vsync) */
    int interpolation;       /* frames in between: 0 off, 1 smooth, 2 lowest latency */
    int msaa;                /* 1, 2, 4, 8 */
    int n64AA;               /* resolve MSAA like the N64's video interface */
    int anisotropy;          /* 1 = off, up to 16 */
    int internalHeight;      /* lines the game is drawn at; 0 = the window's */
    int stretch;             /* 0 = 4:3 with side bars, 1 = fill the window */
    int widescreen;          /* 1 = 3D fills the window, 2D stays 4:3 in the middle */
    int textureFilter;       /* 0 point, 1 bilinear, 2 three-point (N64) */
    int edgeFill;            /* see-through texels take their neighbours' colour (no dark cut-out edges) */
    int drawDistance;        /* scenery and fog that many times further out: 1 = N64, 2, 4, 8, 16 */
    int showFps;             /* the frame counter in the corner */
    int renderer;            /* 0 OpenGL, 1 SDL_gpu (Vulkan); read at start */
    /* [audio] */
    int volume;              /* 0-100 */
    int audioLatency;        /* output delay, ms */
    /* [input] */
    int rumble;              /* 0 off, 1 on */
    int rumbleStrength;      /* 0-100 */
    int deadzone;            /* stick dead zone, percent */
    int stickRange;          /* stick output at full tilt, 0-127 (N64 controllers give about 80) */
    int keyboardPlayer;      /* which controller the keyboard drives, 1-4; 0 = none */
    int freeCamera;          /* the right stick turns the camera (not the C buttons) */
    int cameraDeadzone;      /* the right stick's dead zone, percent */
    int analogPedals;        /* the triggers are gas and brake, by how far they are pressed */
    int raphnet;             /* read raphnet N64 adapters raw */
    int raphnetPlayer;       /* the controller the adapter's first port is, 1-4 */
    /* [game] */
    int doubleField;         /* races with twice the cars (up to 24) */
    int cpuUnlockables;      /* computer cars may drive the unlockable cars */
    /* [saves] */
    int pakOnAllPorts;       /* every controller has a Controller Pak, not only the first */
} PortConfig;

extern PortConfig gPortConfig;

void Port_ConfigLoad(void);
void Port_ConfigSave(void);
/* The folder next to the executable, ending in a separator */
const char *Port_BaseDir(void);

#ifdef __cplusplus
}
#endif

#endif
