#pragma once
#include "BattleEntity.h"
class FootSoldier :
    public BattleEntity
{
public:
    FootSoldier() = default;
	FootSoldier(Utilities::Relationship relationship,
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
		int color = 0
	);

	~FootSoldier();

	//void Update(float offsetX) override;
	//void Move(int rectIndex) override;
	//void MoveToCell() override;
	//void AttackEnemy() override;
	//int Attack(int enemyIndex) override;
	//void Display(int, int, float) override;

	//void SetCell(const Rectangle& cell) override;
	//void MouseOver(bool flag) override;

	//void Lured(int moveToIndex) override;

private:

};

