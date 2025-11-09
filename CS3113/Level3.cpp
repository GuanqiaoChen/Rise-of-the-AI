/**
* Author: Guanqiao Chen
* Assignment: Rise of the AI
* Date due: 2025-11-08, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "Level3.h"

Level3::Level3() : Scene { {0.0f}, nullptr } {}
Level3::Level3(Vector2 origin, const char *bgHexCode) : Scene { origin, bgHexCode } {}

Level3::~Level3() { shutdown(); }

void Level3::initialise()
{
    mGameState.nextSceneID = 0;

    // Create level data - final level
    for (int i = 0; i < LEVEL3_WIDTH * LEVEL3_HEIGHT; ++i) mLevelData[i] = 0;

    // 1) Flat ground (two rows) across the whole width
    if (LEVEL3_WIDTH > 0 && LEVEL3_HEIGHT > 1) {
        int r0 = LEVEL3_HEIGHT - 1;     // bottom row
        int r1 = LEVEL3_HEIGHT - 2;     // just above bottom
        for (int c = 0; c < LEVEL3_WIDTH; ++c) {
            mLevelData[r0 * LEVEL3_WIDTH + c] = 9;   // choose any solid tile
            mLevelData[r1 * LEVEL3_WIDTH + c] = 10;  // another solid tile
        }
    }

    // 2) One sky platform (cat’s perch), safely clamped to map size
    int skyRow = std::max(0, LEVEL3_HEIGHT - 9);         // high but safe
    int spanW  = std::max(3, LEVEL3_WIDTH / 6);          // platform width
    int c0     = (LEVEL3_WIDTH * 3) / 5;                 // start near 60% of map
    int c1     = std::min(LEVEL3_WIDTH - 1, c0 + spanW);
    c0         = std::min(c0, LEVEL3_WIDTH - 1);         // clamp start

    for (int c = c0; c <= c1; ++c) {
        mLevelData[skyRow * LEVEL3_WIDTH + c] = 14;      // pick a distinct tile
    }
    

    mGameState.bgm = LoadMusicStream("assets/game/looped_background_music.wav");
    SetMusicVolume(mGameState.bgm, 0.5f);
    PlayMusicStream(mGameState.bgm);

    mGameState.jumpSound = LoadSound("assets/game/player_jump_sound.wav");

    /*
      ----------- MAP -----------
    */
    mGameState.map = new Map(
        LEVEL3_WIDTH, LEVEL3_HEIGHT,
        (unsigned int *) mLevelData,
        "assets/game/tilemap.png",
        TILE_DIMENSION,
        16, 7,  // 16 columns, 7 rows
        mOrigin
    );

    /*
      ----------- PLAYER -----------
    */
    std::map<Direction, std::vector<int>> adventurerAnimationAtlas = {
        {LEFT,  {0, 1, 2, 3, 4, 5, 6, 7}},  // 1x8 sprite sheet
        {RIGHT, {0, 1, 2, 3, 4, 5, 6, 7}}   // 1x8 sprite sheet
    };

    mGameState.xochitl = new Entity(
        {mOrigin.x - 500.0f, mOrigin.y - 150.0f},
        {140.0f, 140.0f},
        "assets/game/adventurer_walk_right.png",
        PLAYER
    );
    mGameState.xochitl->setLeftRightTextures(
        "assets/game/adventurer_walk_left.png",
        "assets/game/adventurer_walk_right.png",
        ATLAS,
        {1.0f, 8.0f}  // 8 columns, 1 row
    );
    mGameState.xochitl->setAnimationAtlas(adventurerAnimationAtlas);
    mGameState.xochitl->setDirection(RIGHT); // Initialize animation indices
    mGameState.xochitl->setFrameSpeed(Entity::DEFAULT_FRAME_SPEED);
    mGameState.xochitl->setJumpingPower(550.0f);
    mGameState.xochitl->setColliderDimensions({80.0f, 80.0f});
    mGameState.xochitl->setAcceleration({0.0f, ACCELERATION_OF_GRAVITY});
    mGameState.xochitl->setSpeed(200);

    /*
      ----------- DEMON AI (FLYER) -----------
    */
    std::map<Direction, std::vector<int>> demonAnimationAtlas = {
        {LEFT,  {0, 1, 2, 3}},  // 1x4 sprite sheet
        {RIGHT, {0, 1, 2, 3}}   // 1x4 sprite sheet
    };

    mDemon = new Entity(
        {mOrigin.x + 150.0f, mOrigin.y - 200.0f},
        {80.0f, 80.0f},
        "assets/game/demon_fly_right.png",
        NPC
    );
    mDemon->setLeftRightTextures(
        "assets/game/demon_fly_left.png",
        "assets/game/demon_fly_right.png",
        ATLAS,
        {1.0f, 4.0f}  // 4 columns, 1 row
    );
    mDemon->setAnimationAtlas(demonAnimationAtlas);
    mDemon->setDirection(RIGHT); // Initialize animation indices
    mDemon->setFrameSpeed(Entity::DEFAULT_FRAME_SPEED);
    mDemon->setAIType(FLYER);
    mDemon->setAIState(WALKING);
    mDemon->setSpeed(120);
    mDemon->setAcceleration({0.0f, 0.0f}); // No gravity for flyer
    mDemon->setColliderDimensions({70.0f, 70.0f});

    /*
      ----------- CAMERA -----------
    */
    mGameState.camera = { 0 };
    mGameState.camera.target = mGameState.xochitl->getPosition();
    mGameState.camera.offset = mOrigin;
    mGameState.camera.rotation = 0.0f;
    mGameState.camera.zoom = 1.0f;
}

void Level3::update(float deltaTime)
{
    UpdateMusicStream(mGameState.bgm);

    // Update demon AI
    mDemon->update(
        deltaTime,
        mGameState.xochitl,
        mGameState.map,
        nullptr,
        0
    );

    // Update player
    mGameState.xochitl->update(
        deltaTime,
        nullptr,
        mGameState.map,
        nullptr,
        0
    );

    // Check collision with AI (from player's perspective)
    if (mGameState.xochitl->checkCollisionWithAI(mDemon))
    {
        // Player loses a life - handled in main.cpp
        mGameState.nextSceneID = -1; // Signal to lose a life
    }

    // Check if player reached end of level (win condition)
    if (mGameState.xochitl->getPosition().x > mOrigin.x + 800.0f)
    {
        mGameState.nextSceneID = -2; // Signal win
    }

    // Camera follows player
    Vector2 currentPlayerPosition = { mGameState.xochitl->getPosition().x, mOrigin.y };
    panCamera(&mGameState.camera, &currentPlayerPosition);
}

void Level3::render()
{
    ClearBackground(ColorFromHex(mBGColourHexCode));

    mGameState.map->render();
    mDemon->render();
    mGameState.xochitl->render();
}

void Level3::shutdown()
{
    delete mGameState.xochitl;
    delete mGameState.map;
    delete mDemon;

    UnloadMusicStream(mGameState.bgm);
    UnloadSound(mGameState.jumpSound);
}

