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
`DrawModes:` 
`SHAPE_2D` \
`SHAPE_2D_LIGHT` \
`TEXTURE_2D` \
`TEXT`

`void ClearBackground(Color color);` \
`void BeginDrawing(DrawModes mode, bool mode2D);` \
`void EndDrawing();`

### Input
`bool IsKeyDown(int key);` \
`bool IsKeyUp(int key);` \
`bool IsKeyPressedOnce(int key);` \
`bool IsKeyReleased(int key);`

`bool IsMouseDown(int button);` \
`bool IsMousePressedOnce(int button);` \
`bool IsMouseReleased(int button);` \
`glm::vec2 GetMousePosition();` \
`glm::vec2 GetMousePositionWorld();`

### Collisions
`bool CheckCollisionRects(Rectangle one, Rectangle two);` \
`bool CheckCollisionCircleRect(Circle one, Rectangle two);` \
`bool CheckCollisionVec2Rect(glm::vec2 one, Rectangle two);`

### Drawing text
`void DrawText(glm::vec2 position, float scale, std::string text, Color color);` \
`void DrawTextShadow(glm::vec2 position, glm::vec2 shadowOffset, float scale, std::string text, Color color, Color shadowColor);` \
`void ShowDetails(); `

`glm::vec2 GetTextSize(std::string fontName, std::string text, float scale);` \
`void Text::Init(std::string fontPath, std::string fontName, TextureFiltering filteringMode);`

### Drawing primitives
!IMPORTANT! RGBA 0-255 & angle in degrees \
`void DrawRectangle(glm::vec2 position, glm::vec2 size, Color color);` \
`void DrawRectangleRotated(glm::vec2 position, glm::vec2 size, float angle, Color color);` \
`void DrawRectangle(glm::vec2 position, glm::vec2 size, Color color);` \
`void DrawRectangleRotOut(glm::vec2 position, glm::vec2 size, float angle, Color color);`
