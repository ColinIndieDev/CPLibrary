# CPLibrary (CPL)
## About
CPL (named by me) is my custom framework made from scratch. This framework is entirely written \
in C++ and uses OpenGL & other low-level libraries like GLFW, GLAD, STBImage etc. \
Currently I worked on this just for 1-2 months
## Example code

`#include "../CPLibrary/CPLibrary.h"`

`using namespace CPL;` \
`PRIORITIZE_GPU_BY_VENDOR`

`int main() {` \
    `InitWindow(800, 600, "Welcome to CPL");`

    while (!WindowShouldClose()) {
        UpdateCPL();

        ClearBackground(BLACK);
        
        BeginDrawing(SHAPE_2D, true);
        DrawRectangle({0, 0}, {100, 100}, RED);

        BeginDrawing(TEXT, false);
        DrawText({GetScreenWidth() / 2, GetScreenHeight() / 2}, 
            1.0f, "Hello OpenGL", WHITE);

        EndDrawing();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    CloseWindow();

    return 0;
`}`

As you can see, the functions and naming conventions are pretty similar \
and inspired by the ones from Raylib

## Structure
+ `assets/`
+ + `fonts/`
+ `external/`
+ + `glad/`
+ `CPLibrary/`
+ + `shaders/`
+ +  + `.frag (fragment shaders)`
+ +  +  `.vert (vertex shaders)`
+ + `shapes2D/`
+ + `timers/`
+ + `CPLibrary.h`  
+ + `CPL.cpp & .h`
+ + `more .cpp & .h files for other functionalities`

The `assets/` folder is important since it contains the default font of the framework if the user has not chosen one. In `external` is the implementation from `glad` downloaded from its offical website.
All code for the shaders like fragment as well as vertex are all stored inside the
`shader` folder. Inside `shapes2D`, classes of primitves (rectangle, circle etc.), textures & screen quad
are contained. The other files for the functionalities are below. `CPLibrary.h` just includes all header files
so that you only need to use to include `CPLibrary.h` to get all available functions. `CPL.cpp` & `.h` implements
the functionality from the files to a function or contains its own you can call.

## Functionality
CPL currently only supports 2D, but 3D is planned for the future. \
Since CPL is written in C++ and open source, you may look up the code \
and modify it potentially for personal purpose

2D:
- drawing primitives like rectangles, circles, triangles and more!
- drawing 2D textures from every image format (.png, .jpg, ...) with stb
- drawing texts with custom fonts with freetype
- 2D camera
- creating and drawing tilemaps
- 2D lighting
- post processing

Others:
- random number generator or probabilities by percent
- key and mouse inputs
- audio for playing sounds & music
- timer & cooldowns

## Documentation

### General stuff
`PRIORITIZE_CPU_BY_VENDOR` 
> Use deprecated GPU (NVIDIA or AMD) instead of integrated ones (Intel)

> [!IMPORTANT]
> Only works for Windows (OS)

`void UpdateCPL();` 
> Update framework's data (f.e. deltaTime, FPS, timer etc.)

`int GetFPS();` 
> Return current FPS

`float GetDeltaTime();` 
> Return current delta time

`float GetTime();`
> Return current time in s

### Random
> [!IMPORTANT]
> 100% = 100.0
> min & max is included

`int RandInt(int min, int max);` 
> Return random int between min & max

`float RandFloat(float min, float max);` 
> Return random float between min & max

`bool RandByPercentInt(int percent);` 
> Returns true with a probability of 'percent'

`bool RandByPercentFloat(float percent); `
> Returns true with a probability of 'percent'

### Timer
> [!IMPORTANT]
> duration in s, i
> f loop is true, then timer updates every frame even without being in the game loop

`void TimerManager::AddTimer(float duration, bool loop, std::function<void()> event);` 
> Add an event which should be executed in 'duration'

`void TimerManager::StopTimers();` 
> Stop all current timers and events

`void TimerManager::ClearTimers();`
> Deleted all current timers and events

### Audio
`Audio AudioManager::LoadAudio(std::string audioPath);` 
> Load audio from file (miniaudio supports multiple formats like .wav, .mp3 etc.)

`void AudioManager::PlaySFX(Audio audio);` 
> Play sound

`void AudioManager::PlaySFXPitch(Audio audio, float pitch);` 
> Play sound with a certain amount of pitch

`void AudioManager::PlayMusic(Audio audio);` 
> Play music (only call once)

`void AudioManager::PlayMusicPitch(Audio audio, float pitch);` 
> Play music with a certain amount of pitch

`void AudioManager::PauseMusic();` 
> Pause current playing music

`void AudioManager::ResumeMusic();` 
> Continue current playing music

`void AudioManager::StopMusic();`
> Stop current playing music

### Window
`void InitWindow(int width, int height, const char* title);` 
> Create a window with a title and a certain width & height

`void SetWindowIcon(std::string imagePath);` 
> Set the window icon with a image
> (stb image supports multiple formats as well say .png, .jpg and more)

`void CloseWindow();`
> Closes the window

### 2D Camera
`Camera2D camera;`
> Camera which you can access and manipulate its position

### Post Processing
`PostProcessingModes:` \
`PP_DEFAULT` \
`PP_INVERSE` \
`PP_GRAYSCALE` \
`PP_BLUR` \
`PP_SHARP` \
`PP_EDGE_DETECTION`
> enum for multiple implemented post processing modes
> like blur and inverse colors

`void BeginPostProcessing();` 
> Start post processing
> (everything after this call will be affected by post processing)

`void EndPostProcessing();` 
> End post processing
> (everything after this call will not be affected anymore)

`void ApplyPostProcessing(PostProcessingModes mode);`
> Apply post processing using the given modes

`void ApplyPostProcessingCustom(Shader shader);`
> Apply post processing using your custom shader

### 2D Lighting
`class PointLight(glm::vec2 position, float radius, float intensity, Color color);`
> Create a pointLight on a certain position with a
> radius, intensity of the light and the light color

`SetAmbientLight(float ambientStrength);` 
> Set the ambient intensity

`AddPointLights(std::vector<PointLight> pointLights);`
> Draw final point lights to the screen

### Drawing
`DrawModes:` \
`SHAPE_2D` \
`SHAPE_2D_LIGHT` \
`TEXTURE_2D` \
`TEXT`
> enum for draw modes if you want to draw simple shapes, textures, text and if they should be affected by lighting

`void ClearBackground(Color color);`
> In the next frame, the previous frame will be cleared with the chosen color

`void BeginDrawing(DrawModes mode, bool mode2D);`
> Start drawing and choose a draw mode (which activates the corresponding shader)

`void EndDrawing();`
> End drawing (Unbind the current active shader)

### Input
`bool IsKeyDown(int key);`
> Return if corresponding key from the keyboard is held down

`bool IsKeyUp(int key);`
> Reurn if correspoinding key is up

`bool IsKeyPressedOnce(int key);`
> Return if key is pressed (once)

`bool IsKeyReleased(int key);`
> Return if key is released (once)

`bool IsMouseDown(int button);`
> Return if corresponding mouse button is held down

`bool IsMousePressedOnce(int button);`
> Return if corresponding mouse button is pressed (once)

`bool IsMouseReleased(int button);`
> Return if corresponding mouse button is released

`glm::vec2 GetMousePosition();`
> Return the position of the cursor on the screen / window

`glm::vec2 GetMousePositionWorld();`
> Return the position of the cursor in the 2D space (affected by camera position)

### Collisions
`bool CheckCollisionRects(Rectangle one, Rectangle two);` \
`bool CheckCollisionCircleRect(Circle one, Rectangle two);` \
`bool CheckCollisionVec2Rect(glm::vec2 one, Rectangle two);`

### Drawing 2D textures
`TextureFiltering:`
`NEAREST`
`LINEAR`

`Texture2D(std::string imagePath, glm::vec2 size, TextureFiltering mode);`

`void DrawTexture2D(Texture2D* texture, glm::vec2 position, Color color);`

`void DrawTexture2DRotated(Texture2D* texture, glm::vec2 position, float angle);`

### Drawing text
`void DrawText(glm::vec2 position, float scale, std::string text, Color color);`

`void DrawTextShadow(glm::vec2 position, glm::vec2 shadowOffset, float scale, std::string text, Color color, Color shadowColor);`

`void ShowDetails(); `

`glm::vec2 GetTextSize(std::string fontName, std::string text, float scale);`

`void Text::Init(std::string fontPath, std::string fontName, TextureFiltering filteringMode);`

### Drawing primitives
!IMPORTANT! RGBA 0-255 & angle in degrees \
`void DrawRectangle(glm::vec2 position, glm::vec2 size, Color color);` \
`void DrawRectangleRotated(glm::vec2 position, glm::vec2 size, float angle, Color color);` \
`void DrawRectangle(glm::vec2 position, glm::vec2 size, Color color);` \
`void DrawRectangleRotOut(glm::vec2 position, glm::vec2 size, float angle, Color color);`
