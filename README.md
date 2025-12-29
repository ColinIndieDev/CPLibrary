![CPLlogo](assets/images/logo.png)

# CPLibrary (CPL)
## About
CPL (named by me) is my custom framework made from scratch. This framework is entirely written \
in C++ and uses OpenGL & other low-level libraries like GLFW, GLAD, STBImage etc. \
Currently I worked on this just for around 3 months
## Example code

```
#include "../CPLibrary/CPLibrary.h"

using namespace CPL;
PRIORITIZE_GPU_BY_VENDOR // optional

int main() {
    InitWindow(800, 600, "Welcome to CPL");

    while (!WindowShouldClose()) {
        UpdateCPL();

        ClearBackground(BLACK);
        
        BeginDraw(DrawModes::SHAPE_2D, true);
        DrawRect({0, 0}, {100, 100}, RED);

        BeginDraw(DrawModes::TEXT, false);
        DrawText({GetScreenWidth() / 2, GetScreenHeight() / 2}, 
            1.0f, "Hello OpenGL", WHITE);

        EndDraw();
        glfwSwapBuffers(GetWindow());
        glfwPollEvents();
    }
    CloseWindow();

    return 0;
}
```
An actual example code for 3D can be found inside the `src/` folder and for 2D inside `example2D/`!

As you can see, the functions and naming conventions are pretty similar \
and inspired by the ones from Raylib

## Structure
+ `assets/`
+ + `fonts/`
+ + `images/`
+ + `shaders/`
+ +  + `.frag (fragment shaders)`
+ +  +  `.vert (vertex shaders)`
+ `external/`
+ + `glad/`
+ `CPLibrary/`
+ + `shapes2D/`
+ + `shapes3D/`
+ + `timers/`
+ + `CPLibrary.h`
+ + `Engine.cpp & .h`  
+ + `CPL.cpp & .h`
+ + `more .cpp & .h files for other functionalities`

The `assets/` folder is important since it contains the default font of the framework if the user has not chosen one and the shader code. In `external/` is the implementation from `glad/` downloaded from its offical website.
All code for the shaders like fragment as well as vertex are all stored inside the
`shader/` folder (in assets as mentioned before). Inside `shapes2D/`, classes of primitves (rectangle, circle etc.), textures & screen quad
are contained and in `shapes3D/` the 3D related stuff needed. `Engine.h` & `.cpp` have all implementations of the functions that are available for the framework which can be used by wrappers basically in `CPL.h` and `.cpp`. The others should be self explaining I guess by their names.

## Functionality
CPL currently supports 2D, but 3D as well which isn't fully completed yet. \
Since CPL is written in C++ and open source, you may look up the code \
and modify it potentially for personal use. Besides of making games
for Desktop (Windows & Linux), the framework + the code can be converted \
to Web with Emscripten.

2D:
- Primitives (Lines, Rectangles, Circles etc.)
- Textures
- Text + Fonts
- Tilemaps
- Post Processing
- Particle System

3D:
- 3D shapes (Cubes & Spheres)
- Cube Textures
- Cubemaps
- Shadows (Shadow Map)

Both:
- Camera
- Lighting
  
Others:
- Random number generator wrapper
- Key and Mouse Inputs
- Sounds & Music
- Timer

It is recommended to take a look at the example projects / demonstrations which will show core functionalities and how to use them correctly. Alternatively you may look up the functions inside the documentation but note that it may contains minor mistakes or is outdated. I will keep my best to refresh and update it regularily!

## Documentation

### Used libraries
- GLAD
> Provides OpenGL functions

- GLFW
> Window and Input

- STB Image
> Textures

- Freetype
> Text & Fonts

- Miniaudio
> Sounds and Music

### General stuff
`PRIORITIZE_CPU_BY_VENDOR` 
> Use these GPUs (NVIDIA or AMD) instead of integrated ones (Intel)

> [!IMPORTANT]
> Only works for Windows (OS)

`void UpdateCPL();` 
> Update framework (f.e. deltaTime, FPS, timer etc.)

`int GetFPS();` 
> Return current FPS

`float GetDeltaTime();` 
> Return current delta time

`float GetTime();`
> Return current time in seconds

`void SetTimeScale(float scale);`
> Set time scale

`void EnableVSync(bool enabled);`
> Toggle VSync

`void EnableFaceCulling(bool enabled);`
> Toggle face culling (important for 3D)

`void LockMouse(bool enabled);`
> Toggle locked and hidden mouse cursor

`Shader& GetShader(DrawModes mode);`
> Get shader by draw mode

`DrawModes& GetCurMode();`
> Get current active draw mode

`glm::vec2 GetScreenToWorld2D(glm::vec2 position);`
> Convert the position to world coordinates

### Random
> [!IMPORTANT]
> 100% = 100.0
> min & max is included

`int RandInt(int min, int max);` 
> Return random int between min & max

`float RandFloat(float min, float max);` 
> Return random float between min & max

`bool RandPercentInt(int percent);` 
> Return true with a probability of 'percent'

`bool RandPercentFloat(float percent); `
> Return true with a probability of 'percent'

### Timer
> [!IMPORTANT]
> duration in s \
> if loop is true, then timer updates every frame even without being in the game loop

`void TimerManager::AddTimer(float duration, bool loop, std::function<void()> event);` 
> Add an event which should be executed in 'duration'

`void TimerManager::StopTimers();` 
> Stop all current timers and events

`void TimerManager::ClearTimers();`
> Delete all current timers and events

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

`void DestroyWindow();`
> Destroys the window while being inside the game loop

`void CloseWindow();`
> Closes the window

`GLFWwindow* GetWindow();`
> Gives the current window (for f.e. swap buffers in loop)

### 2D Camera
`Camera2D& GetCam2D();`
> 2D Camera which you can access and manipulate its position

### 3D Camera
`Camera3D& GetCam3D();`
> 3D Camera

### Post Processing
`PostProcessingModes:` \
`DEFAULT` \
`INVERSE` \
`GRAYSCALE` \
`BLUR` \
`SHARP` \
`EDGE_DETECTION`
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
> Create a point light on a certain position with a
> radius, intensity of the light and the light color

`class GlobalLight(float intensity, Color color);`
> Create a global light

`void SetGlobalLight2D(GlobalLight light);`
> Apply global light to the screen

`void SetAmbientLight(float ambientStrength);` 
> Set the ambient intensity

`void AddPointLights2D(std::vector<PointLight> pointLights);`
> Draw final point lights to the screen

### 3D Lighting
`class PointLight3D(glm::vec3 position, float intensity, float
constant, float linear, float quadratic, Color color);`
> Create a 3D point light

`class DirectionalLight(glm::vec3 direction, glm::vec3 ambient,
glm::vec3 diffuse, glm::vec3 specular);`
> Create a directional light

`void SetShininess3D(float shininess);`
> Set global shininess of all objects

`void SetDirLight3D(DirectionalLight light);`
> Set current directional light

`void AddPointLights3D(std::vector<PointLight3D> lights);`
> Draw final 3D point lights to the screen

### Drawing
`DrawModes:` \
`SHAPE_2D` \
`SHAPE_2D_LIGHT` \
`TEX` \
`TEX_LIGHT` \
`TEXT` \
`SHAPE_3D` \
`SHAPE_3D_LIGHT` \
`CUBE_TEX` \
`CUBE_TEX_LIGHT` 
> enum for draw modes if you want to draw simple shapes, textures, text and if they should be affected by lighting
and if 2D or 3D

`void ClearBackground(Color color);`
> In the next frame, the previous frame will be cleared with the chosen color

`void BeginDraw(DrawModes mode, bool mode2D);`
> Start drawing and choose a draw mode (which activates the corresponding shader), mode2D does not need to be given especially when in 3D

`void EndDraw();`
> End drawing (unbind the current active shader)

### Input
`bool IsKeyDown(int key);`
> Return if corresponding key from the keyboard is held down

`bool IsKeyUp(int key);`
> Return if correspoinding key is up

`bool IsKeyPressedOnce(int key);`
> Return if key is pressed (once)

`bool IsKeyReleased(int key);`
> Return if key is released (once)

`unsigned int GetCharPressed()`
> Returns the current char pressed as a unsigned int

`bool IsMouseDown(int button);`
> Return if corresponding mouse button is held down

`bool IsMousePressedOnce(int button);`
> Return if corresponding mouse button is pressed (once)

`bool IsMouseReleased(int button);`
> Return if corresponding mouse button is released

`glm::vec2 GetMousePosition();`
> Returns the position of the cursor on the screen / window

### Collisions
`bool CheckCollisionRects(Rectangle one, Rectangle two);`
> Return true if two rectangles collide

`bool CheckCollisionCircleRect(Circle one, Rectangle two);`
> Return true if a circle collides with a rectangle

`bool CheckCollisionVec2Rect(glm::vec2 one, Rectangle two);`
> Return true if a 2d vector collides with a rectangle

`bool CheckCollisionCircles(const Circle& one, const Circle& two);`
> Return true if two circles collide

`bool CheckCollisionVec2Circle(const glm::vec2& one, const Circle& two);`
> Return true if a 2d vector collides with a circle

### 2D Tilemap
> [!IMPORTANT]
> If using the provided functions for the tilemap you need to access them from a tilemap instance. For example:
> Tilemap map;
> map.BeginEditing();
> ...
> Also note that only textures (Texture2D) can be added to the tilemap

`class Tilemap;`
> Create a 2D tilemap

`void BeginEditing();`
> Enable editing for adding tiles (clears all tiles from before)

`void AddTile(glm::vec2 position, glm::vec2 size, const Texture2D* texture);`
> Add a texture to the tilemap

`void DeleteTile(glm::vec2 position, glm::vec2 size, const Texture2D* texture);`
> Delete a texture from the tilemap

`bool TileExist(glm::vec2 position, glm::vec2 size);`
> Returns if a tile inside the map exists at the given position and size

`void CheckCollidableTiles(float size);`
> Used to tag the tiles "collidable" by checking if they can collide with other objects (without neighbours). When really implementing collision iterate through the tiles in the tilemap if it is tagged "collidable" and do whatever should happen in that process

`void Draw(Shader shader);`
> Draw the tilemap to the screen (select "textureShader" as the shader)

### Particle System
> [!IMPORTANT]
> If using the provided functions for the particle system you need to access them from a particle system instance as well. For example:
> ParticleSystem ps;
> ps.Update();
> ...
> Also note that only textures (Texture2D) can be added

`class ParticleSystem(glm::vec2 position);`
> Create a particle system

`glm::vec2 position;`
> Set position of the particle system

`void AddParticle(Texture* texture, Color color, float lifeTime, glm::vec2 direction, glm::vec2 offset);`
> Add particle to particle system

`void Update();`
> Update particle system (f.e. for lifetime)

`void Draw();`
> Draw particles

### Drawing 2D textures
`TextureFiltering:` \
`NEAREST` \
`LINEAR`
> enum for filtering modes

`Texture2D(std::string imagePath, glm::vec2 size, TextureFiltering mode);`
> Load a texture from an image file with a given size in pixels and a filtering mode

`void DrawTex2D(Texture2D* tex, glm::vec2 pos, Color color);`
> Draw a texture on the screen with a position and color manipulation (no manipulation -> WHITE)

`void DrawTex2DRot(Texture2D* tex, glm::vec2 pos, float angle, Color color);`
> Draw a texture with rotation

### Drawing 3D textures (Cube)
`void DrawCubeTex(Texture2D* tex, glm::vec3 pos, glm::vec3 size, Color color);`
> Draw a cube made out of a texture 2D each side

### 3D Shadows (Shadow Map)
> [!IMPORTANT]
> If using the provided functions for the shadow map you need to access them from a shadow map instance too. For example:
> ShadowMap sm(4096);
> sm.beginDepthPass(lightSpaceMatrix);
> ...

`class ShadowMap(unsigned int resolution);`
> Create a shadow map with a given shadow resolution

`void BeginDepthPass(glm::mat4 matrix);`
> After this call, every object drawn will have its own shadow inside the map stored + you need to pass the light space matrix

`void EndDepthPass();`
> Everything after will not have its shadow stored

`void BindForReading(int value);`
> Use shadow map to draw shadows into the scene (value has not to be given as a parameter)

### Drawing text
`void DrawText(glm::vec2 pos, float scale, std::string text, Color color);`
> Draw text on the screen

`void DrawTextShadow(glm::vec2 pos, glm::vec2 shadowOff, float scale, std::string text, Color color, Color shadowColor);`
> Draw text with shadow

`void ShowDetails(); `
> Display all stats (f.e. GPU Info or FPS)

`std::string GetDefaultFont();`
> Returns name of the default font

`glm::vec2 GetTextSize(std::string fontName, std::string text, float scale);`
> Get text size (width & height)

`void Text::Init(std::string fontPath, std::string fontName, TextureFiltering filteringMode);`
> Add new fonts

`void Text::Use(std::string fontName);`
> Use selected font for text drawn after this call

### Cube Map
`class CubeMap(std::string filePath);`
> Create a cube map from a cubemap image (f.e. skybox)

`void DrawCubeMap(CubeMap* map);`
> Draw cubemap onto the screen

### Drawing simple 3D shapes
`void DrawCube(glm::vec3 pos, glm::vec3 size, Color color);`
> Draw cube / quad

`void DrawSphere(glm::vec3 pos, float radius, Color color);`
> Draw sphere

### Drawing primitives
> [!IMPORTANT]
> RGBA values between 0 and 255 \
> Angles in degrees

`void DrawRect(glm::vec2 pos, glm::vec2 size, Color color);`
> Draw a rectangle

`void DrawRectRot(glm::vec2 pos, glm::vec2 size, float angle, Color color);`
> Draw a rectangle with rotation

`void DrawRectOut(glm::vec2 pos, glm::vec2 size, Color color);`
> Draw only the rectangle outline

`void DrawRectRotOut(glm::vec2 pos, glm::vec2 size, float angle, Color color);`
> Draw only the rectangle outline with rotation

`void DrawTriangle(glm::vec2 pos, glm::vec2 size, Color color);`
> Draw a triangle

`void DrawTriangleRot(glm::vec2 pos, glm::vec2 size, float angle, Color color);`
> Draw a triangle with rotation

`void DrawTriangleOut(glm::vec2 pos, glm::vec2 size, Color color);`
> Draw only the triangle outline

`void DrawTriangleRotOut(glm::vec2 pos, glm::vec2 size, float angle, Color color);`
> Draw only the triangle outline with rotation

`void DrawCircle(glm::vec2 pos, float radius, Color color);`
> Draw a circle

`void DrawCircleOut(glm::vec2 pos, float radius, Color color);`
> Draw only the circle outline

`void DrawLine(glm::vec2 startPos, glm::vec2 endPos, Color color);`
> Draw a line
