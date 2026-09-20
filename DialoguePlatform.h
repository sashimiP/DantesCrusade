#pragma once
#include <string>
#include <iostream>
#include "Portrait.h"
#include "TextBox.h"
#include "Utilities.h"
#include "TextureManager.h"

using std::string, std::cout, std::endl, std::shared_ptr, std::make_shared;


class DialoguePlatform
{
public:
	DialoguePlatform() = default;
	DialoguePlatform(shared_ptr<TextureManager> textureManager);
	void SetDialoguePlatform(Texture2D* rightPortrait, Texture2D* leftPortrait, vector<Button>* buttons=nullptr);
	//void SetDialoguePlatform(shared_ptr<MediaStream> rightPortrait, shared_ptr<MediaStream> leftPortrait, vector<Button>* buttons = nullptr);
	void SetDialoguePlatform(MediaStream* rightPortrait, MediaStream* leftPortrait, vector<Button>* buttons = nullptr);
	void SetButtons(vector<Button>& buttons);
	void AdjustButtons();
	void DrawAnimatedPortrait(Texture2D* portrait, Rectangle& portraitSource, Rectangle portraitDest, float dt, float& portraitFrameTimer, int& portraitFrameIndex);
	//void GetRightPortrait(Texture*, Utilities::Direction);
	//void GetLeftPortrait(Texture*, Utilities::Direction);
	//void GetName(string& name);
	void DrawPlatform(Texture2D* rightPortrait, Texture2D* leftPortrait, const string& name, const vector<TextBox>& dialogue, int dialogueCounter, float dt, bool animated = false);
	void Update();
	void ResetPortraits();

private:
	shared_ptr<TextureManager> textureManager;

	int rightPosX;
	int rightPosY;
	int leftPosX;
	int leftPosY;
	int portraitWidth;
	int portraitHeight;

	//shared_ptr<MediaStream> rightPortrait;
	//shared_ptr<MediaStream> leftPortrait;

	MediaStream* rightPortrait;
	MediaStream* leftPortrait;


	Texture2D* squareFrame;
	Texture2D* horizontalFrame;

	//Texture2D* rightPortrait;
	//Texture2D* leftPortrait;

	Rectangle rightSquareFrameSourceRect;
	Rectangle rightSquareFrameDestRect;

	Rectangle leftSquareFrameSourceRect;
	Rectangle leftSquareFrameDestRect;

	Rectangle horizontalFrameSourceRect;
	Rectangle horizontalFrameDestRect;
	Vector2 origin;

	Rectangle rightPortraitSource;
	Rectangle rightPortraitDest;

	Rectangle leftPortraitSource;
	Rectangle leftPortraitDest;


	// ***Animated Portrait data***
	int animSpeed = 15;
	shared_ptr<Texture2D> animatedPortrait;

	int animPortraitWidth = 544;
	int animPortraitHeight = 736;

	int rightAnimPortraitWidth = 544;
	int rightAnimPortraitHeight = 736;

	int leftAnimPortraitWidth = 544;
	int leftAnimPortraitHeight = 736;

	int animPortCurrentFrame = 0;

	int animPortRow = 0;
	int animPortCol = 0;

	// Portrait Animation Data
	int animPortCols = 10;
	int animPortRows = 6;
	int totalPortraitFrames = animPortCols * animPortRows;

	float rightPortraitFrameTimer = 0.0f;
	int rightPortraitFrameIndex = 0; // 0 to 59

	float leftPortraitFrameTimer = 0.0f;
	int leftPortraitFrameIndex = 0; // 0 to 59

	Rectangle rightAnimatedPortraitSource = {
	   0.0f,
	   0.0f,
	   static_cast<float>(animPortraitWidth),
	   static_cast<float>(animPortraitHeight)
	};

	Rectangle leftAnimatedPortraitSource = {
	   0.0f,
	   0.0f,
	   static_cast<float>(animPortraitWidth),
	   static_cast<float>(animPortraitHeight)
	};
	

	vector<Button>* buttons;
};

