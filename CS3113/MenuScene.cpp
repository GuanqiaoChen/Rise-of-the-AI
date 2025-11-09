/**
* Author: Guanqiao Chen
* Assignment: Rise of the AI
* Date due: 2025-11-08, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "MenuScene.h"

constexpr int SCREEN_WIDTH = 1000;
constexpr int SCREEN_HEIGHT = 600;

MenuScene::MenuScene() : Scene { {0.0f}, nullptr } {}
MenuScene::MenuScene(Vector2 origin, const char *bgHexCode) : Scene { origin, bgHexCode } {}

MenuScene::~MenuScene() { shutdown(); }

void MenuScene::initialise()
{
    mGameState.nextSceneID = 0;
}

void MenuScene::update(float deltaTime)
{
    if (IsKeyPressed(KEY_ENTER))
    {
        mGameState.nextSceneID = 1; // Go to first level
    }
}

void MenuScene::render()
{
    ClearBackground(ColorFromHex(mBGColourHexCode));
    
    const char* gameTitle = "Adventure";
    const char* instruction = "Press Enter to Start";
    
    int titleFontSize = 60;
    int instructionFontSize = 30;
    
    int titleWidth = MeasureText(gameTitle, titleFontSize);
    int instructionWidth = MeasureText(instruction, instructionFontSize);
    
    DrawText(gameTitle, 
             SCREEN_WIDTH / 2 - titleWidth / 2, 
             SCREEN_HEIGHT / 2 - 50, 
             titleFontSize, 
             WHITE);
    
    DrawText(instruction, 
             SCREEN_WIDTH / 2 - instructionWidth / 2, 
             SCREEN_HEIGHT / 2 + 30, 
             instructionFontSize, 
             WHITE);
}

void MenuScene::shutdown()
{
    // Nothing to clean up
}

