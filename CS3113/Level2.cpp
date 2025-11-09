/**
* Author: Guanqiao Chen
* Assignment: Rise of the AI
* Date due: 2025-11-08, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "Level2.h"

Level2::Level2() : Scene { {0.0f}, nullptr } {}
Level2::Level2(Vector2 origin, const char *bgHexCode) : Scene { origin, bgHexCode } {}

Level2::~Level2() { shutdown(); }

void Level2::initialise()
{
    mGameState.nextSceneID = 0;

    // Create level data 
    for (int i = 0; i < LEVEL2_WIDTH * LEVEL2_HEIGHT; ++i) mLevelData[i] = 0;

    if (LEVEL2_WIDTH > 0 && LEVEL2_HEIGHT > 1) {
        int r0 = LEVEL2_HEIGHT - 1;     
        int r1 = LEVEL2_HEIGHT - 2;   
        for (int c = 0; c < LEVEL2_WIDTH; ++c) {
            mLevelData[r0 * LEVEL2_WIDTH + c] = 9;   
            mLevelData[r1 * LEVEL2_WIDTH + c] = 10;  
        }
    }

    int skyRow = std::max(0, LEVEL2_HEIGHT - 9);         
    int spanW  = std::max(3, LEVEL2_WIDTH / 6);         
    int c0     = (LEVEL2_WIDTH * 3) / 5;                
    int c1     = std::min(LEVEL2_WIDTH - 1, c0 + spanW);
    c0         = std::min(c0, LEVEL2_WIDTH - 1);        

    for (int c = c0; c <= c1; ++c) {
        mLevelData[skyRow * LEVEL2_WIDTH + c] = 14;     
    }

    mGameState.bgm = LoadMusicStream("assets/game/looped_background_music.wav");
    SetMusicVolume(mGameState.bgm, 0.5f);
    PlayMusicStream(mGameState.bgm);

    mGameState.jumpSound = LoadSound("assets/game/player_jump_sound.wav");

    mGameState.map = new Map(
        LEVEL2_WIDTH, LEVEL2_HEIGHT,
        (unsigned int *) mLevelData,
        "assets/game/tilemap.png",
        TILE_DIMENSION,
        16, 7,  
        mOrigin
    );

    std::map<Direction, std::vector<int>> adventurerAnimationAtlas = {
        {LEFT,  {0, 1, 2, 3, 4, 5, 6, 7}},  
        {RIGHT, {0, 1, 2, 3, 4, 5, 6, 7}}  
    };

    mGameState.xochitl = new Entity(
        {mOrigin.x - 500.0f, mOrigin.y - 150.0f},
        {160.0f, 160.0f},
        "assets/game/adventurer_walk_right.png",
        PLAYER
    );
    mGameState.xochitl->setLeftRightTextures(
        "assets/game/adventurer_walk_left.png",
        "assets/game/adventurer_walk_right.png",
        ATLAS,
        {1.0f, 8.0f}  
    );
    mGameState.xochitl->setAnimationAtlas(adventurerAnimationAtlas);
    mGameState.xochitl->setDirection(RIGHT); 
    mGameState.xochitl->setFrameSpeed(Entity::DEFAULT_FRAME_SPEED);
    mGameState.xochitl->setJumpingPower(550.0f);
    mGameState.xochitl->setColliderDimensions({80.0f, 80.0f});
    mGameState.xochitl->setAcceleration({0.0f, ACCELERATION_OF_GRAVITY});
    mGameState.xochitl->setSpeed(200);

    std::map<Direction, std::vector<int>> catAnimationAtlas = {
        {LEFT,  {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11}},  
        {RIGHT, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11}} 
    };

    mCat = new Entity(
        {mOrigin.x + 100.0f, mOrigin.y - 150.0f},
        {90.0f, 90.0f},
        "assets/game/cat_walk_right.png",
        NPC
    );
    mCat->setLeftRightTextures(
        "assets/game/cat_walk_left.png",
        "assets/game/cat_walk_right.png",
        ATLAS,
        {1.0f, 12.0f}  
    );
    mCat->setAnimationAtlas(catAnimationAtlas);
    mCat->setDirection(RIGHT); 
    mCat->setFrameSpeed(Entity::DEFAULT_FRAME_SPEED);
    mCat->setAIType(FOLLOWER);
    mCat->setAIState(IDLE);
    mCat->setSpeed(85);
    mCat->setAcceleration({0.0f, ACCELERATION_OF_GRAVITY});
    mCat->setColliderDimensions({15.0f, 15.0f});

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

    // Check collision with AI 
    if (mGameState.xochitl->checkCollisionWithAI(mCat))
    {
        // Player loses a life 
        mGameState.nextSceneID = -1; 
    }

    // Check if player reached end of level
    if (mGameState.xochitl->getPosition().x > mOrigin.x + 800.0f)
    {
        mGameState.nextSceneID = 3;
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

