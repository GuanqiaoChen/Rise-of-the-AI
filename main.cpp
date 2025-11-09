/**
* Author: Guanqiao Chen
* Assignment: Rise of the AI
* Date due: 2025-11-08, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "CS3113/MenuScene.h"
#include "CS3113/Level1.h"
#include "CS3113/Level2.h"
#include "CS3113/Level3.h"

// Global Constants
constexpr int SCREEN_WIDTH     = 1000,
              SCREEN_HEIGHT    = 600,
              FPS              = 120,
              NUMBER_OF_LEVELS = 3,
              MAX_LIVES        = 3;

constexpr Vector2 ORIGIN      = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };
            
constexpr float FIXED_TIMESTEP = 1.0f / 60.0f;

// Global Variables
AppStatus gAppStatus   = RUNNING;
float gPreviousTicks   = 0.0f,
      gTimeAccumulator = 0.0f;

Scene *gCurrentScene = nullptr;
MenuScene *gMenuScene = nullptr;
Level1 *gLevel1 = nullptr;
Level2 *gLevel2 = nullptr;
Level3 *gLevel3 = nullptr;

int gLives = MAX_LIVES;
int gCurrentLevel = 0; 
bool gGameWon = false;
bool gGameLost = false;

Sound gWalkSound;
Sound gJumpSound;
Sound gDeathSound;
bool gWasWalking = false;

static float gOutcomeTimer = 0.0f;
static const float OUTCOME_DELAY = 2.0f;

// Function Declarations
void switchToScene(Scene *scene);
void initialise();
void processInput();
void update();
void render();
void shutdown();
void resetGame();

void switchToScene(Scene *scene)
{   
    gCurrentScene = scene;
    gCurrentScene->initialise();
}

void resetGame()
{
    gLives = MAX_LIVES;
    gCurrentLevel = 0;
    switchToScene(gMenuScene);
}

void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Adventure Quest");
    InitAudioDevice();

    // Load sound effects
    gWalkSound = LoadSound("assets/game/player_walk_sound.wav");
    gJumpSound = LoadSound("assets/game/player_jump_sound.wav");
    gDeathSound = LoadSound("assets/game/player_death_sound.wav");

    // Create scenes
    gMenuScene = new MenuScene(ORIGIN, "#1a1a2e");
    gLevel1 = new Level1(ORIGIN, "#16213e");
    gLevel2 = new Level2(ORIGIN, "#0f3460");
    gLevel3 = new Level3(ORIGIN, "#533483");

    switchToScene(gMenuScene);

    SetTargetFPS(FPS);
}

void processInput() 
{
    if (gCurrentLevel == 0) return; // Menu handles its own input

    gCurrentScene->getState().xochitl->resetMovement();

    bool isWalking = false;

    if      (IsKeyDown(KEY_A)) 
    {
        gCurrentScene->getState().xochitl->moveLeft();
        isWalking = true;
    }
    else if (IsKeyDown(KEY_D)) 
    {
        gCurrentScene->getState().xochitl->moveRight();
        isWalking = true;
    }

    if (IsKeyPressed(KEY_W) && 
        gCurrentScene->getState().xochitl->isCollidingBottom())
    {
        gCurrentScene->getState().xochitl->jump();
        PlaySound(gJumpSound);
    }

    if (GetLength(gCurrentScene->getState().xochitl->getMovement()) > 1.0f) 
        gCurrentScene->getState().xochitl->normaliseMovement();

    if (IsKeyPressed(KEY_Q) || WindowShouldClose()) gAppStatus = TERMINATED;
}

void update() 
{
    float ticks = (float) GetTime();
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks  = ticks;

    deltaTime += gTimeAccumulator;

    if (deltaTime < FIXED_TIMESTEP)
    {
        gTimeAccumulator = deltaTime;
        return;
    }

    while (deltaTime >= FIXED_TIMESTEP)
    {
        gCurrentScene->update(FIXED_TIMESTEP);
        deltaTime -= FIXED_TIMESTEP;
    }

    int stepsThisFrame = 0;

    if (gCurrentLevel != 0)
    {
        Entity* player = gCurrentScene->getState().xochitl;
        const float EPS = 1e-2f;
        
        if (player != nullptr) 
        {
            bool grounded = player->isCollidingBottom();
            bool movingX  = fabsf(player->getVelocity().x) > EPS;
        
            if (grounded && movingX) {
                if (!IsSoundPlaying(gWalkSound)) PlaySound(gWalkSound);
            } else {
                if (IsSoundPlaying(gWalkSound))  StopSound(gWalkSound);
            }
        }
    }

    // Handle scene transitions
    int nextSceneID = gCurrentScene->getState().nextSceneID;
    
    if (nextSceneID != 0)
    {
        if (IsSoundPlaying(gWalkSound)) StopSound(gWalkSound);
            gWasWalking = false;
            
        if (nextSceneID == -1) // Player lost a life
        {
            PlaySound(gDeathSound);
            gLives--;
            
            if (gLives <= 0)
            {
                if (IsSoundPlaying(gWalkSound)) StopSound(gWalkSound);
                gGameLost = true;
                gOutcomeTimer = 0.0f;
                resetGame();
            }
            else
            {
                // Restart current level
                switch (gCurrentLevel)
                {
                    case 1:
                        switchToScene(gLevel1);
                        break;
                    case 2:
                        switchToScene(gLevel2);
                        break;
                    case 3:
                        switchToScene(gLevel3);
                        break;
                }
            }
        }
        else if (nextSceneID == -2) // Player won
        {
            if (IsSoundPlaying(gWalkSound)) StopSound(gWalkSound);
            gGameWon = true;
            gOutcomeTimer = 0.0f;
            resetGame();
        }
        else if (nextSceneID == 1 && gCurrentLevel == 0) // Start game from menu
        {
            gCurrentLevel = 1;
            gLives = MAX_LIVES;
            gGameWon = false;
            gGameLost = false;
            switchToScene(gLevel1);
        }
        else if (nextSceneID > 0) // Go to next level
        {
            gCurrentLevel = nextSceneID;
            switch (nextSceneID)
            {
                case 1:
                    switchToScene(gLevel1);
                    break;
                case 2:
                    switchToScene(gLevel2);
                    break;
                case 3:
                    switchToScene(gLevel3);
                    break;
            }
        }
    }

    if (gCurrentLevel == 0 && (gGameWon || gGameLost))
    {
        gOutcomeTimer += stepsThisFrame * FIXED_TIMESTEP;

        if (gOutcomeTimer >= OUTCOME_DELAY)
        {
            gGameWon = false;
            gGameLost = false;
            gOutcomeTimer = 0.0f;
        }
    }
}

void render()
{
    BeginDrawing();
    
    if (gCurrentLevel == 0)
    {
        // Menu rendering
        gCurrentScene->render();
        
        // Show win/lose message
        if (gGameWon)
        {
            const char* message = "You Win!";
            int fontSize = 50;
            int width = MeasureText(message, fontSize);
            DrawText(message, 
                     SCREEN_WIDTH / 2 - width / 2, 
                     SCREEN_HEIGHT / 2 - 100, 
                     fontSize, 
                     GREEN);
        }
        else if (gGameLost)
        {
            const char* message = "You Lose!";
            int fontSize = 50;
            int width = MeasureText(message, fontSize);
            DrawText(message, 
                     SCREEN_WIDTH / 2 - width / 2, 
                     SCREEN_HEIGHT / 2 - 100, 
                     fontSize, 
                     RED);
        }
    }
    else
    {
        // Game rendering
        BeginMode2D(gCurrentScene->getState().camera);
        gCurrentScene->render();
        EndMode2D();
        
        // Draw lives
        const char* livesText = TextFormat("Lives: %d", gLives);
        DrawText(livesText, 10, 10, 30, WHITE);
    }
    
    EndDrawing();
}

void shutdown() 
{
    delete gMenuScene;
    delete gLevel1;
    delete gLevel2;
    delete gLevel3;

    UnloadSound(gWalkSound);
    UnloadSound(gJumpSound);
    UnloadSound(gDeathSound);

    CloseAudioDevice();
    CloseWindow();
}

int main(void)
{
    initialise();

    while (gAppStatus == RUNNING)
    {
        processInput();
        update();
        render();
    }

    shutdown();

    return 0;
}
