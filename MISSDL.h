#pragma once

#include <SDL2/SDL.h>
#include <functional>
#include <set>

/*
The goal of this header - MISSDL (Mutable Input System for SDL2) - is to provide a simple abstraction layer for dynamically changeable keybinds in SDL2.
PLANNED FEATURES:
    A callback-based lambda system for the keyboard and mouse and joystick/gamepad events that is somewhat UNIFIED.
    Add some way for auto-repeat to be overriden
        Like, store whether or not it has been "released" and only re-call if it has previously been released
    A simple abstraction of SDL2 scancodes so the user can see what keys are currently being pressed/held
        A way to link boolean references to these such that they become true when the button is held, etc.
*/

struct MISSDL_KeyCallback {
    // Stores a keyboard key, a key action, and a C++ lambda callback function
    SDL_Keycode key; // The keyboard key that this is referrring to
    bool onDown; // Whether the function triggers on down or on up
    bool triggered = false; // When checking to trigger, if this is false triggers to true. Opposite event triggers to false. Prevents auto-repeat
    std::function<void()> function; // A lambda function with no arguments returning void
    MISSDL_KeyCallback() {
        // Default constructor
        key = SDLK_UNKNOWN;
        onDown = true;
        function = [](){};
    }
    const operator<(const MISSDL_KeyCallback& rhs) {
        // Basic comparator, assumes no duplicate triggers
        if (key == rhs.key) {
            return onDown;
        }
        return key < rhs.key;
    }
};

struct MISSDL_MouseButtonCallback {
    // Stores a mouse key, a key action, and a C++ lambda callback function
    uint8_t button; // The mouse button, [SDL_BUTTON_LEFT, -MIDDLE, -RIGHT, -X1, -X2]
    bool onDown; // Whether the function triggers on down or on up
    bool triggered = false; // When checking to trigger, if this is false triggers to true. Opposite event triggers to false. Prevents auto-repeat
    std::function<void()> function; // A lambda function with no arguments returning void
    MISSDL_MouseButtonCallback() {
        // Default constructor
        button = 0;
        onDown = true;
        function = [](){};
    }
    const operator<(const MISSDL_MouseButtonCallback& rhs) {
        // Basic comparator, assumes no duplicate triggers
        if (button == rhs.button) {
            return onDown;
        }
        return button < rhs.button;
    }
};

class MISSDL {
    // This is the master class doodad. Instantiate one of these and use dynamic keybinds to your heart's content
public:
    // Functions
    MISSDL(); // Constructor
    ~MISSDL(); // Destructor
    bool doEvents(); // Handles all event callbacks that are a part of this header. NOTE Purges all events. Use as only event handling. If returns true, quit program
    void addKeyCallback(SDL_Keycode key, bool onDown, std::function<void()> function); // Adds a lambda keycallback function
    void addMouseButtonCallback(uint8_t button, bool onDown, std::function<void()> function); // Adds a lambda mousebuttoncallback function
    // Variables
private:
    // Functions
    // Variables
    std::set<MISSDL_KeyCallback*> keyCallbacks;
    std::set<MISSDL_MouseButtonCallback*> mouseButtonCallbacks;
};

MISSDL::MISSDL() {
    // Constructor
}

MISSDL::~MISSDL() {
    // Deallocate all callbacks
    for (MISSDL_KeyCallback* call : keyCallbacks) {
        delete call;
    }
    for (MISSDL_MouseButtonCallback* call : mouseButtonCallbacks) {
        delete call;
    }
}

bool MISSDL::doEvents() {
    // Goes through all SDL events, purges them, and handles them if they are part of this library. Prevents auto-repeat
    SDL_Event e;
    while(SDL_PollEvent(&e) != 0) {
        // User requests quit
        if (e.type == SDL_QUIT) {
            return true;
        } else if (e.type == SDL_MOUSEBUTTONDOWN) {
            for (MISSDL_MouseButtonCallback* call : mouseButtonCallbacks) {
                if (call->button == e.button.button) {
                    if (call->onDown) {
                        call->function();
                        call->triggered = true;
                    } else {
                        call->triggered = false;
                    }
                }
            }
        } else if (e.type == SDL_MOUSEBUTTONUP) {
            for (MISSDL_MouseButtonCallback* call : mouseButtonCallbacks) {
                if (call->button == e.button.button) {
                    if (!call->onDown) {
                        call->function();
                        call->triggered = true;
                    } else {
                        call->triggered = false;
                    }
                }
            }
        } else if (e.type == SDL_KEYDOWN) {
            for (MISSDL_KeyCallback* call : keyCallbacks) {
                if (call->key == e.key.keysym.sym) {
                    if (call->onDown) {
                        call->function();
                        call->triggered = true;
                    } else {
                        call->triggered = false;
                    }
                }
            }
        } else if (e.type == SDL_KEYUP) {
            for (MISSDL_KeyCallback* call : keyCallbacks) {
                if (call->key == e.key.keysym.sym) {
                    if (!call->onDown) {
                        call->function();
                        call->triggered = true;
                    } else {
                        call->triggered = false;
                    }
                }
            }
        }
    }
    return false;
}

void MISSDL::addKeyCallback(SDL_Keycode key, bool onDown, std::function<void()> function) {
    // Adds a new keyboard key callback (no-argument lambda returning void)
    // NOTE this is a really inefficient way to do this...
    MISSDL_KeyCallback* call = new MISSDL_KeyCallback();
    call->key = key;
    call->onDown = onDown;
    call->function = function;
    keyCallbacks.insert(call);
}

void MISSDL::addMouseButtonCallback(uint8_t button, bool onDown, std::function<void()> function) {
    // Adds a lambda mousebuttoncallback function
    // NOTE really inefficient...
    MISSDL_MouseButtonCallback* call = new MISSDL_MouseButtonCallback();
    call->button = button;
    call->onDown = onDown;
    call->function = function;
    mouseButtonCallbacks.insert(call);
}