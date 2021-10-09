# MISSDL
MISSDL simplifies the code needed for full keybind and mousebind support in an SDL2 main loop, with support for gamepads and other devices planned for the future.

### Usage
```c++
MISSDL missdl; // Main missdl instance
// Callbacks: Keyboard
missdl.addKeyCallback(SDLK_SPACE, true, [](){
    std::cout << "Spacebar pressed!" << std::endl;
});
missdl.addKeyCallback(SDLK_SPACE, false, [](){
    std::cout << "Spacebar released!" << std::endl;
});
// Callbacks: Mouse
missdl.addMouseButtonCallback(SDL_BUTTON_LEFT, true, [](){
    std::cout << "Left button pressed!" << std::endl;
});
missdl.addMouseButtonCallback(SDL_BUTTON_LEFT, false, [](){
    std::cout << "Left button released!" << std::endl;
});

// Main loop
bool quit = false;
while (!quit) {
  quit = missdl.doEvents();
  
  // Other gameplay, rendering, and calculations here
}
```
