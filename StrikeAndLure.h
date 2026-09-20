#pragma once
#include "StrikeAndReturnSoldier.h"
class StrikeAndLure :
    public StrikeAndReturnSoldier
{
public:
    StrikeAndLure() = default;
	StrikeAndLure(Utilities::Relationship relationship,
		const int cellIndex,
		shared_ptr<vector<Cell>> grid,
		fs::path dataPath,
		string town,
		shared_ptr<TextureManager> textureManager,
		float id,
		int position,
		string type,
		float x,
		float y,
		Utilities::Direction direction,
		bool drag,
		string stance,
		vector<string> dialogue = {},
		string name = "Unknown",
		int color = 0);

	~StrikeAndLure() = default;

	//int Attack(int enemyIndex) override;
	void Lure();
	void AttackEnemy() override;
	int FindAvailableCell();
	void Return() override;
	//void Display(int, int, float) override;

private:
	int numRows;
	int numColumns;
	bool cellNextToEnemy{ false };


	//int animPortraitWidth = 272;
	//int animPortraitHeight = 368;

	//int animPortCurrentFrame = 0;

	//int animPortRow = 0;
	//int animPortCol = 0;

	//// Portrait Animation Data
	//int animPortCols = 10;
	//int animPortRows = 5;
	//int totalPortraitFrames = animPortCols * animPortRows;

	//float portraitFrameTimer = 0.0f;
	//int portraitFrameIndex = 0; // 0 to 49
};

