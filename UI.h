#pragma once
#include<iostream>
#include<string>
#include<vector>
#include<map>
#include<unordered_map>

#include "Dante.h"
#include"raylib.h"
#include "TextureManager.h"
#include "MarchingEntity.h"
#include "Portrait.h"

using std::unordered_map, Utilities::Direction;

class UI
{
public:
	UI() = default;
	UI(shared_ptr<TextureManager> textureManager, const vector<string>&armyTypes);
	//UI(vector<string>army, shared_ptr<TextureManager> textureManager, vector<float> stats);
	~UI() = default;
	void CreateGrid(vector<Portrait>& Portraits, Utilities::Direction direction);
	void PrepArmyPortraits();

	void CountTypes();

	void PrepStatsDestRects(Direction direction);
	void PrepStatsPortraits();
	void AddArmyType(string type);

	void SetUI(Direction direction);

	void SetStats(string& stat, float newStat);

	void BeginDrawing(bool draw);
	void Display();
	
	bool engagedInDialogue{ false };
	bool draw{ false };

private:
	shared_ptr<TextureManager> textureManager;

	int armyIconWidth;
	int armyIconHeight;

	int topDecoratedFrameWidth;
	int topDecoratedFrameHeight;
	int bottomDecoratedFrameWidth;
	int bottomDecoratedFrameHeight;
	Rectangle topDecorateFrameRect;
	Rectangle bottomDecorateFrameRect;
	Texture2D* decoratedFrame;
	Rectangle topDecoratedFrameSourceRect;
	Rectangle bottomDecoratedFrameSourceRect;

	Color cellsBackgroundColor;
	
	
	vector<string> armyTypes;
	unordered_map<string, int> countArmyTypes;
	vector<Portrait> armyPortraits;
	vector<Rectangle> armyPortraitCells;
	Texture2D* squareFrame;
	Rectangle squareFrameSourceRect;

	Rectangle armyDestRect;
	Vector2 origin;

	std::map<std::string, float*> stats;
	vector<Portrait> statsPortraits;
	int statsWidth;
	int statsHeight;
	Rectangle statsSourceRect;
	vector<Rectangle> statsDestRect;
};

