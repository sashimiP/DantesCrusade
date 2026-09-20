#pragma once
#include "BattleEntity.h"
class MountedSoldier :
    public BattleEntity
{
public:
	MountedSoldier() = default;
	MountedSoldier(Utilities::Relationship relationship,
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

	~MountedSoldier();

	void Update(bool selected) override;
	//void Move(int rectIndex) override;
	void MoveToCell() override;
	void ReturnFromCharge();
	void AttackEnemy() override;
	void Attack(shared_ptr<Cell> enemyCell) override;
	//void Display(int, int, float) override;
	void SetStartingPosition(float x, float y);

	//void SetCell(const Rectangle& cell) override;
	//void SetStance(string stance) override;
	//void MouseOver(bool flag) override;
	//void Lured(int attackerRectIndex) override;

private:

	float scalingFactor{ 0.8f };
	bool returnFromCharge{ false };
};

