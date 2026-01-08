#include "Game.h"

void Game::Init() {
    m_Skybox = std::make_unique<CubeMap>("assets/images/sky.png");

    Audio music = AudioManager::LoadAudio("assets/sounds/aria_math_music.mp3");
    AudioManager::PlayMusic(music);

    TextureLoader::Init();

    m_InitAtlases();

    const glm::ivec2 viewDistance(16); // Default: 16 (chunks) -> each chunk takes 2ms to generate and 7-8 ms to mesh

    m_WorldGen = std::make_unique<WorldGen>(RandInt(0, 999999999), viewDistance, 30, Chunk::s_Height, 60);
    m_WorldGen->Init();
    m_WorldGen->GenMap();

    m_SetSpawnPoint();
}

void Game::Run() {
    while (!WindowShouldClose()) {
        UpdateCPL();

        m_Update();
        m_Draw();

        glfwSwapBuffers(GetWindow());
        glfwPollEvents();
    }
}

void Game::m_SetSpawnPoint() {
    const glm::ivec2 spawnPoint(0);

    const int height = m_WorldGen->GetTerrainHeight(spawnPoint.x, spawnPoint.y) + 3;
    GetCam3D().position.y = static_cast<float>(height) * 0.2f;
}

void UpdateControls() {
    float aspect = GetScreenWidth() / GetScreenHeight();
    Camera3D &cam = GetCam3D();
    cam.UpdateFrustum(aspect);

    float speed = 3.0f * GetDeltaTime();
    if (IsKeyDown(KEY_LEFT_CONTROL))
        speed *= 6;

    if (IsKeyDown(KEY_W))
        cam.position += speed * cam.front;
    if (IsKeyDown(KEY_S))
        cam.position -= speed * cam.front;
    if (IsKeyDown(KEY_A))
        cam.position -= glm::normalize(glm::cross(cam.front, cam.up)) * speed;
    if (IsKeyDown(KEY_D))
        cam.position += glm::normalize(glm::cross(cam.front, cam.up)) * speed;
    if (IsKeyDown(KEY_LEFT_SHIFT))
        cam.position.y -= speed;
    if (IsKeyDown(KEY_SPACE))
        cam.position.y += speed;

    if (IsKeyPressedOnce(KEY_ESCAPE))
        DestroyWindow();
}

void Game::m_Update() { 
    UpdateControls(); 
    m_WorldGen->manager.ProcessFinishedChunks();
}

float NormalizeYaw(float yaw) {
    yaw = static_cast<float>(fmod(yaw, 360.0));

    if (yaw < 0)
        yaw += 360.0f;

    return yaw;
}

std::string GetCardinalDir(float yaw) {
    yaw = NormalizeYaw(yaw);

    if (yaw >= 337.5f || yaw < 22.5f) {
        return "N";
    }
    if (yaw >= 22.5f && yaw < 67.5f) {
        return "NE";
    }
    if (yaw >= 67.5f && yaw < 112.5f) {
        return "E";
    }
    if (yaw >= 112.5f && yaw < 157.5f) {
        return "SE";
    }
    if (yaw >= 157.5f && yaw < 202.5f) {
        return "S";
    }
    if (yaw >= 202.5f && yaw < 247.5f) {
        return "SW";
    }
    if (yaw >= 247.5f && yaw < 292.5f) {
        return "W";
    }
    return "NW";
}

void Game::m_Draw() {
    ClearBackground(SKY_BLUE);

    DrawCubeMap(m_Skybox.get());

    EnableFaceCulling(true);

    BeginDraw(DrawModes::CUBE_TEX_LIGHT);

    SetShininess3D(6);

    SetDirLight3D(
        DirectionalLight(glm::normalize(glm::vec3(-0.2f, -1.0f, -0.3f)),
                         glm::vec3(0.3f), glm::vec3(1.0f), glm::vec3(1.0f)));

    m_WorldGen->manager.DrawChunks(GetShader(DrawModes::CUBE_TEX_LIGHT), m_TexAtlases);

    EnableFaceCulling(false);

    BeginDraw(DrawModes::TEX, false);

    Texture2D *logoTex = TextureLoader::GetCPLLogo();
    DrawTex2D(logoTex,
              {GetScreenWidth() - logoTex->size.x,
               GetScreenHeight() - logoTex->size.y},
              WHITE);

    BeginDraw(DrawModes::TEXT, false);

    DrawTextShadow(
        {0, 0}, {3, -3}, 0.5f,
        "X: " + std::to_string(static_cast<int>(GetCam3D().position.x * 5)) +
            ", Y: " +
            std::to_string(static_cast<int>(GetCam3D().position.y * 5)) +
            ", Z: " +
            std::to_string(static_cast<int>(GetCam3D().position.z * 5)),
        WHITE, DARK_GRAY);
    DrawTextShadow({0, 25}, {3, -3}, 0.5f, "Seed: " + std::to_string(m_WorldGen->seed),
             WHITE, DARK_GRAY);

    DrawTextShadow({GetScreenWidth() / 2, 0}, {7, -7}, 1.0f, GetCardinalDir(GetCam3D().yaw),
             WHITE, DARK_GRAY);

    ShowDetails();

    EndDraw();
}

void Game::m_InitAtlases() {
    m_TexAtlases[BlockType::BEDROCK] =
        TextureLoader::GetBlockTex(TextureLoader::BlockType::BEDROCK);
    m_TexAtlases[BlockType::GRASS_BLOCK] =
        TextureLoader::GetBlockTex(TextureLoader::BlockType::GRASS_BLOCK);
    m_TexAtlases[BlockType::DIRT] =
        TextureLoader::GetBlockTex(TextureLoader::BlockType::DIRT);
    m_TexAtlases[BlockType::STONE] =
        TextureLoader::GetBlockTex(TextureLoader::BlockType::STONE);
    m_TexAtlases[BlockType::OAK_LOG] =
        TextureLoader::GetBlockTex(TextureLoader::BlockType::OAK_LOG);
    m_TexAtlases[BlockType::OAK_LEAVES] =
        TextureLoader::GetBlockTex(TextureLoader::BlockType::OAK_LEAVES);
    m_TexAtlases[BlockType::SNOW] =
        TextureLoader::GetBlockTex(TextureLoader::BlockType::SNOW);
}
