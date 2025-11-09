#include "Level2.h"

Level2::Level2() : Scene { {0.0f}, nullptr } {}
Level2::Level2(Vector2 origin, const char *bgHexCode) : Scene { origin, bgHexCode } {}

Level2::~Level2() { shutdown(); }

void Level2::initialise()
{
    mGameState.nextSceneID = 0;

    // Create level data - different layout for level 2
    for (int i = 0; i < LEVEL2_WIDTH * LEVEL2_HEIGHT; i++) mLevelData[i] = 0;

    // Gentle double ground (tile ids 9/10), with *small* occasional gaps
    for (int c = 0; c < LEVEL2_WIDTH; ++c) {
        // keep gaps rare and short
        bool gap = (c % 14 == 6); // single-tile gap every ~14 tiles
        if (gap) continue;
        mLevelData[(LEVEL2_HEIGHT - 1) * LEVEL2_WIDTH + c] = 9;
        mLevelData[(LEVEL2_HEIGHT - 2) * LEVEL2_WIDTH + c] = 10;
    }

    // Low mid ledge stripe 
    for (int c = 6; c <= 14; ++c)
        mLevelData[(LEVEL2_HEIGHT - 6) * LEVEL2_WIDTH + c] = 12;

    // Another low stripe later
    for (int c = 20; c <= 28; ++c)
        mLevelData[(LEVEL2_HEIGHT - 7) * LEVEL2_WIDTH + c] = 12;

    // A couple of small islands 
    // A couple of small islands (guard against columns outside the map width)
    for (int c = 34; c <= 36; ++c)
        if (c < LEVEL2_WIDTH)
            mLevelData[(LEVEL2_HEIGHT - 8) * LEVEL2_WIDTH + c] = 14;
    for (int c = 42; c <= 44; ++c)
        if (c < LEVEL2_WIDTH)
            mLevelData[(LEVEL2_HEIGHT - 6) * LEVEL2_WIDTH + c] = 14;
    

    mGameState.bgm = LoadMusicStream("assets/game/looped_background_music.wav");
    SetMusicVolume(mGameState.bgm, 0.5f);
    PlayMusicStream(mGameState.bgm);

    mGameState.jumpSound = LoadSound("assets/game/player_jump_sound.wav");

    /*
      ----------- MAP -----------
    */
    mGameState.map = new Map(
        LEVEL2_WIDTH, LEVEL2_HEIGHT,
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
      ----------- CAT AI (FOLLOWER) -----------
    */
    std::map<Direction, std::vector<int>> catAnimationAtlas = {
        {LEFT,  {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11}},  // 1x12 sprite sheet
        {RIGHT, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11}}   // 1x12 sprite sheet
    };

    mCat = new Entity(
        {mOrigin.x + 100.0f, mOrigin.y - 150.0f},
        {80.0f, 80.0f},
        "assets/game/cat_walk_right.png",
        NPC
    );
    mCat->setLeftRightTextures(
        "assets/game/cat_walk_left.png",
        "assets/game/cat_walk_right.png",
        ATLAS,
        {1.0f, 12.0f}  // 12 columns, 1 row
    );
    mCat->setAnimationAtlas(catAnimationAtlas);
    mCat->setDirection(RIGHT); // Initialize animation indices
    mCat->setFrameSpeed(Entity::DEFAULT_FRAME_SPEED);
    mCat->setAIType(FOLLOWER);
    mCat->setAIState(IDLE);
    mCat->setSpeed(150);
    mCat->setAcceleration({0.0f, ACCELERATION_OF_GRAVITY});
    mCat->setColliderDimensions({70.0f, 70.0f});

    /*
      ----------- CAMERA -----------
    */
    mGameState.camera = { 0 };
    mGameState.camera.target = mGameState.xochitl->getPosition();
    mGameState.camera.offset = mOrigin;
    mGameState.camera.rotation = 0.0f;
    mGameState.camera.zoom = 1.0f;
}

void Level2::update(float deltaTime)
{
    UpdateMusicStream(mGameState.bgm);

    // Update cat AI
    mCat->update(
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
    if (mGameState.xochitl->checkCollisionWithAI(mCat))
    {
        // Player loses a life - handled in main.cpp
        mGameState.nextSceneID = -1; // Signal to lose a life
    }

    // Check if player reached end of level
    if (mGameState.xochitl->getPosition().x > mOrigin.x + 800.0f)
    {
        mGameState.nextSceneID = 3; // Go to Level 3
    }

    // Camera follows player
    Vector2 currentPlayerPosition = { mGameState.xochitl->getPosition().x, mOrigin.y };
    panCamera(&mGameState.camera, &currentPlayerPosition);
}

void Level2::render()
{
    ClearBackground(ColorFromHex(mBGColourHexCode));

    mGameState.map->render();
    mCat->render();
    mGameState.xochitl->render();
}

void Level2::shutdown()
{
    delete mGameState.xochitl;
    delete mGameState.map;
    delete mCat;

    UnloadMusicStream(mGameState.bgm);
    UnloadSound(mGameState.jumpSound);
}

