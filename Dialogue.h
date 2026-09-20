#pragma once
#include<iostream>
#include<string>
#include<vector>
#include<filesystem>
#include"raylib.h"
#include"TextBox.h"
#include"TextureManager.h"

namespace fs = std::filesystem;
using std::string, std::vector, std::cout, std::endl;

struct Cursor {
	int blinkRateOn{ 45 };    // frames cursor stays visible
	int blinkRateOff{ 15 };
	int x;
	int y;
	int width;
	int height;
	bool showCursor = true;
};

class Dialogue
{
public:
	Dialogue() = default;
	Dialogue(const fs::path& path,
			const vector<string>& dialogueStr,
			const Rectangle& rect,
			TextureManager* textureManager,
			const string& unitType);

	~Dialogue();
	void SetPath(fs::path path);
	void SetDialogue(const vector<string>& dialogueStr);
	void SetTextRect(Rectangle rect);
	void SetPortrait(Texture2D* portrait);
	void AddDialogue(Rectangle rect);
	void DrawPortraitDialogue();
	void DrawBasicDialogue();


private:
	fs::path path;
	Rectangle textRect;
	TextureManager* textureManager;
	string unitType;
	Rectangle addButtonRect;
	bool showDialogue{ false };
	bool addDialogue{ false };
	bool textBoxActive{ false };
	string text{ "" };
	int maxChars{ 64 };
	int fontSize{ 20 };
	int dialogueCounter{ 0 };
	Cursor cursor;
	Texture2D* portrait{ nullptr };
	vector<TextBox> dialogue;
	int framesCounter{ 0 };
};

