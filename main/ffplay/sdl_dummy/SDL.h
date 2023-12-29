/*
	SDL.h - 
*/

#ifndef SDL_H
#define SDL_H

#include <cstdint>

#define SDL_QUERY   -1
#define SDL_IGNORE   0
#define SDL_DISABLE  0
#define SDL_ENABLE   1

#define SDL_BYTEORDER   SDL_LIL_ENDIAN


#define SDL_reinterpret_cast(type, expression) reinterpret_cast<type>(expression)
#define SDL_static_cast(type, expression) static_cast<type>(expression)
#define SDL_const_cast(type, expression) const_cast<type>(expression)

/* Define a four character code as a Uint32 */
#define SDL_FOURCC(A, B, C, D) \
    ((SDL_static_cast(uint32_t, SDL_static_cast(uint8_t, (A))) << 0) | \
     (SDL_static_cast(uint32_t, SDL_static_cast(uint8_t, (B))) << 8) | \
     (SDL_static_cast(uint32_t, SDL_static_cast(uint8_t, (C))) << 16) | \
     (SDL_static_cast(uint32_t, SDL_static_cast(uint8_t, (D))) << 24))


#define SDL_FORCE_INLINE 

#ifdef __CC_ARM
/* ARM's compiler throws warnings if we use an enum: like "SDL_bool x = a < b;" */
#define SDL_FALSE 0
#define SDL_TRUE 1
typedef int SDL_bool;
#else
typedef enum
{
    SDL_FALSE = 0,
    SDL_TRUE = 1
} SDL_bool;
#endif


#define SDL_MIX_MAXVOLUME 128


#include "SDL_keycode.h"
#include "SDL_scancode.h"
#include "SDL_pixels.h"
#include "SDL_rect.h"
#include "SDL_blendmode.h"


#define SDL_BUTTON(X)       (1 << ((X)-1))
#define SDL_BUTTON_LEFT     1
#define SDL_BUTTON_MIDDLE   2
#define SDL_BUTTON_RIGHT    3
#define SDL_BUTTON_X1       4
#define SDL_BUTTON_X2       5
#define SDL_BUTTON_LMASK    SDL_BUTTON(SDL_BUTTON_LEFT)
#define SDL_BUTTON_MMASK    SDL_BUTTON(SDL_BUTTON_MIDDLE)
#define SDL_BUTTON_RMASK    SDL_BUTTON(SDL_BUTTON_RIGHT)
#define SDL_BUTTON_X1MASK   SDL_BUTTON(SDL_BUTTON_X1)
#define SDL_BUTTON_X2MASK   SDL_BUTTON(SDL_BUTTON_X2)


enum SDL_WindowEventID {
    SDL_WINDOWEVENT_NONE,           /**< Never used */
    SDL_WINDOWEVENT_SHOWN,          /**< Window has been shown */
    SDL_WINDOWEVENT_HIDDEN,         /**< Window has been hidden */
    SDL_WINDOWEVENT_EXPOSED,        /**< Window has been exposed and should be
                                         redrawn */
    SDL_WINDOWEVENT_MOVED,          /**< Window has been moved to data1, data2
                                     */
    SDL_WINDOWEVENT_RESIZED,        /**< Window has been resized to data1xdata2 */
    SDL_WINDOWEVENT_SIZE_CHANGED,   /**< The window size has changed, either as
                                         a result of an API call or through the
                                         system or user changing the window size. */
    SDL_WINDOWEVENT_MINIMIZED,      /**< Window has been minimized */
    SDL_WINDOWEVENT_MAXIMIZED,      /**< Window has been maximized */
    SDL_WINDOWEVENT_RESTORED,       /**< Window has been restored to normal size
                                         and position */
    SDL_WINDOWEVENT_ENTER,          /**< Window has gained mouse focus */
    SDL_WINDOWEVENT_LEAVE,          /**< Window has lost mouse focus */
    SDL_WINDOWEVENT_FOCUS_GAINED,   /**< Window has gained keyboard focus */
    SDL_WINDOWEVENT_FOCUS_LOST,     /**< Window has lost keyboard focus */
    SDL_WINDOWEVENT_CLOSE,          /**< The window manager requests that the window be closed */
    SDL_WINDOWEVENT_TAKE_FOCUS,     /**< Window is being offered a focus (should SetWindowInputFocus() on itself or a subwindow, or ignore) */
    SDL_WINDOWEVENT_HIT_TEST,       /**< Window had a hit test that wasn't SDL_HITTEST_NORMAL. */
    SDL_WINDOWEVENT_ICCPROF_CHANGED,/**< The ICC profile of the window's display has changed. */
    SDL_WINDOWEVENT_DISPLAY_CHANGED /**< Window has been moved to display data1. */
};


enum SDL_EventType {
    SDL_FIRSTEVENT     = 0,     /**< Unused (do not remove) */

    /* Application events */
    SDL_QUIT           = 0x100, /**< User-requested quit */

    /* These application events have special meaning on iOS, see README-ios.md for details */
    SDL_APP_TERMINATING,        /**< The application is being terminated by the OS
                                     Called on iOS in applicationWillTerminate()
                                     Called on Android in onDestroy()
                                */
    SDL_APP_LOWMEMORY,          /**< The application is low on memory, free memory if possible.
                                     Called on iOS in applicationDidReceiveMemoryWarning()
                                     Called on Android in onLowMemory()
                                */
    SDL_APP_WILLENTERBACKGROUND, /**< The application is about to enter the background
                                     Called on iOS in applicationWillResignActive()
                                     Called on Android in onPause()
                                */
    SDL_APP_DIDENTERBACKGROUND, /**< The application did enter the background and may not get CPU for some time
                                     Called on iOS in applicationDidEnterBackground()
                                     Called on Android in onPause()
                                */
    SDL_APP_WILLENTERFOREGROUND, /**< The application is about to enter the foreground
                                     Called on iOS in applicationWillEnterForeground()
                                     Called on Android in onResume()
                                */
    SDL_APP_DIDENTERFOREGROUND, /**< The application is now interactive
                                     Called on iOS in applicationDidBecomeActive()
                                     Called on Android in onResume()
                                */

    SDL_LOCALECHANGED,  /**< The user's locale preferences have changed. */

    /* Display events */
    SDL_DISPLAYEVENT   = 0x150,  /**< Display state change */

    /* Window events */
    SDL_WINDOWEVENT    = 0x200, /**< Window state change */
    SDL_SYSWMEVENT,             /**< System specific event */

    /* Keyboard events */
    SDL_KEYDOWN        = 0x300, /**< Key pressed */
    SDL_KEYUP,                  /**< Key released */
    SDL_TEXTEDITING,            /**< Keyboard text editing (composition) */
    SDL_TEXTINPUT,              /**< Keyboard text input */
    SDL_KEYMAPCHANGED,          /**< Keymap changed due to a system event such as an
                                     input language or keyboard layout change.
                                */
    SDL_TEXTEDITING_EXT,       /**< Extended keyboard text editing (composition) */

    /* Mouse events */
    SDL_MOUSEMOTION    = 0x400, /**< Mouse moved */
    SDL_MOUSEBUTTONDOWN,        /**< Mouse button pressed */
    SDL_MOUSEBUTTONUP,          /**< Mouse button released */
    SDL_MOUSEWHEEL,             /**< Mouse wheel motion */

    /* Joystick events */
    SDL_JOYAXISMOTION  = 0x600, /**< Joystick axis motion */
    SDL_JOYBALLMOTION,          /**< Joystick trackball motion */
    SDL_JOYHATMOTION,           /**< Joystick hat position change */
    SDL_JOYBUTTONDOWN,          /**< Joystick button pressed */
    SDL_JOYBUTTONUP,            /**< Joystick button released */
    SDL_JOYDEVICEADDED,         /**< A new joystick has been inserted into the system */
    SDL_JOYDEVICEREMOVED,       /**< An opened joystick has been removed */

    /* Game controller events */
    SDL_CONTROLLERAXISMOTION  = 0x650, /**< Game controller axis motion */
    SDL_CONTROLLERBUTTONDOWN,          /**< Game controller button pressed */
    SDL_CONTROLLERBUTTONUP,            /**< Game controller button released */
    SDL_CONTROLLERDEVICEADDED,         /**< A new Game controller has been inserted into the system */
    SDL_CONTROLLERDEVICEREMOVED,       /**< An opened Game controller has been removed */
    SDL_CONTROLLERDEVICEREMAPPED,      /**< The controller mapping was updated */
    SDL_CONTROLLERTOUCHPADDOWN,        /**< Game controller touchpad was touched */
    SDL_CONTROLLERTOUCHPADMOTION,      /**< Game controller touchpad finger was moved */
    SDL_CONTROLLERTOUCHPADUP,          /**< Game controller touchpad finger was lifted */
    SDL_CONTROLLERSENSORUPDATE,        /**< Game controller sensor was updated */

    /* Touch events */
    SDL_FINGERDOWN      = 0x700,
    SDL_FINGERUP,
    SDL_FINGERMOTION,

    /* Gesture events */
    SDL_DOLLARGESTURE   = 0x800,
    SDL_DOLLARRECORD,
    SDL_MULTIGESTURE,

    /* Clipboard events */
    SDL_CLIPBOARDUPDATE = 0x900, /**< The clipboard changed */

    /* Drag and drop events */
    SDL_DROPFILE        = 0x1000, /**< The system requests a file open */
    SDL_DROPTEXT,                 /**< text/plain drag-and-drop event */
    SDL_DROPBEGIN,                /**< A new set of drops is beginning (NULL filename) */
    SDL_DROPCOMPLETE,             /**< Current set of drops is now complete (NULL filename) */

    /* Audio hotplug events */
    SDL_AUDIODEVICEADDED = 0x1100, /**< A new audio device is available */
    SDL_AUDIODEVICEREMOVED,        /**< An audio device has been removed. */

    /* Sensor events */
    SDL_SENSORUPDATE = 0x1200,     /**< A sensor was updated */

    /* Render events */
    SDL_RENDER_TARGETS_RESET = 0x2000, /**< The render targets have been reset and their contents need to be updated */
    SDL_RENDER_DEVICE_RESET, /**< The device has been reset and all textures need to be recreated */

    /* Internal events */
    SDL_POLLSENTINEL = 0x7F00, /**< Signals the end of an event poll cycle */

    /** Events ::SDL_USEREVENT through ::SDL_LASTEVENT are for your use,
     *  and should be allocated with SDL_RegisterEvents()
     */
    SDL_USEREVENT    = 0x8000,

    /**
     *  This last event is only for bounding internal arrays
     */
    SDL_LASTEVENT    = 0xFFFF
};


struct SDL_Keysym {
    SDL_Scancode scancode;      /**< SDL physical key code - see ::SDL_Scancode for details */
    SDL_Keycode sym;            /**< SDL virtual key code - see ::SDL_Keycode for details */
    uint16_t mod;                 /**< current key modifiers */
    uint32_t unused;
};


struct SDL_KeyboardEvent {
    uint32_t type;        /**< ::SDL_KEYDOWN or ::SDL_KEYUP */
    uint32_t timestamp;   /**< In milliseconds, populated using SDL_GetTicks() */
    uint32_t windowID;    /**< The window with keyboard focus, if any */
    uint8_t state;        /**< ::SDL_PRESSED or ::SDL_RELEASED */
    uint8_t repeat;       /**< Non-zero if this is a key repeat */
    uint8_t padding2;
    uint8_t padding3;
    SDL_Keysym keysym;  /**< The key that was pressed or released */
};


struct SDL_UserEvent {
    uint32_t type;        /**< ::SDL_USEREVENT through ::SDL_LASTEVENT-1 */
    uint32_t timestamp;   /**< In milliseconds, populated using SDL_GetTicks() */
    uint32_t windowID;    /**< The associated window if any */
    int32_t code;        /**< User defined event code */
    void *data1;        /**< User defined data pointer */
    void *data2;        /**< User defined data pointer */
};


struct SDL_MouseMotionEvent {
    uint32_t type;        /**< ::SDL_MOUSEMOTION */
    uint32_t timestamp;   /**< In milliseconds, populated using SDL_GetTicks() */
    uint32_t windowID;    /**< The window with mouse focus, if any */
    uint32_t which;       /**< The mouse instance id, or SDL_TOUCH_MOUSEID */
    uint32_t state;       /**< The current button state */
    int32_t x;           /**< X coordinate, relative to window */
    int32_t y;           /**< Y coordinate, relative to window */
    int32_t xrel;        /**< The relative motion in the X direction */
    int32_t yrel;        /**< The relative motion in the Y direction */
};


struct SDL_MouseButtonEvent {
    uint32_t type;        /**< ::SDL_MOUSEBUTTONDOWN or ::SDL_MOUSEBUTTONUP */
    uint32_t timestamp;   /**< In milliseconds, populated using SDL_GetTicks() */
    uint32_t windowID;    /**< The window with mouse focus, if any */
    uint32_t which;       /**< The mouse instance id, or SDL_TOUCH_MOUSEID */
    uint8_t button;       /**< The mouse button index */
    uint8_t state;        /**< ::SDL_PRESSED or ::SDL_RELEASED */
    uint8_t clicks;       /**< 1 for single-click, 2 for double-click, etc. */
    uint8_t padding1;
    int32_t x;           /**< X coordinate, relative to window */
    int32_t y;           /**< Y coordinate, relative to window */
};


struct SDL_WindowEvent {
    uint32_t type;        /**< ::SDL_WINDOWEVENT */
    uint32_t timestamp;   /**< In milliseconds, populated using SDL_GetTicks() */
    uint32_t windowID;    /**< The associated window */
    int8_t event;        /**< ::SDL_WindowEventID */
    int8_t padding1;
    int8_t padding2;
    int8_t padding3;
    int32_t data1;       /**< event dependent data */
    int32_t data2;       /**< event dependent data */
};


union SDL_Event {
    uint32_t type;                            /**< Event type, shared with all events */
    SDL_KeyboardEvent key;                  /**< Keyboard event data */
    SDL_UserEvent user;                     /**< Custom event data */
    SDL_MouseMotionEvent motion;            /**< Mouse motion event data */
    SDL_MouseButtonEvent button;            /**< Mouse button event data */
    SDL_WindowEvent window;                 /**< Window event data */

    /* This is necessary for ABI compatibility between Visual C++ and GCC.
       Visual C++ will respect the push pack pragma and use 52 bytes (size of
       SDL_TextEditingEvent, the largest structure for 32-bit and 64-bit
       architectures) for this union, and GCC will use the alignment of the
       largest datatype within the union, which is 8 bytes on 64-bit
       architectures.

       So... we'll add padding to force the size to be 56 bytes for both.

       On architectures where pointers are 16 bytes, this needs rounding up to
       the next multiple of 16, 64, and on architectures where pointers are
       even larger the size of SDL_UserEvent will dominate as being 3 pointers.
    */
    uint8_t padding[sizeof(void *) <= 8 ? 56 : sizeof(void *) == 16 ? 64 : 3 * sizeof(void *)];
};


struct SDL_Texture {
	void* foo;
};


struct SDL_RendererInfo {
    const char *name;           /**< The name of the renderer */
    uint32_t flags;               /**< Supported ::SDL_RendererFlags */
    uint32_t num_texture_formats; /**< The number of available texture formats */
    uint32_t texture_formats[16]; /**< The available texture formats */
    int max_texture_width;      /**< The maximum texture width */
    int max_texture_height;     /**< The maximum texture height */
};


#include <queue>
extern std::queue<SDL_Event> sdl_events;


int SDL_PollEvent(SDL_Event* event);
void SDL_PumpEvents();
/* int  SDL_PeepEvents(SDL_Event* events, int numevents, SDL_eventaction action,
												Uint32 minType, Uint32 maxType); */
int SDL_PushEvent(SDL_Event* event);
int SDL_ShowCursor(int toggle);
int SDL_LockTexture(SDL_Texture * texture, const SDL_Rect * rect, void **pixels, int *pitch);
void SDL_UnlockTexture(SDL_Texture * texture);
uint32_t SDL_RegisterEvents(int numevents);

#endif
