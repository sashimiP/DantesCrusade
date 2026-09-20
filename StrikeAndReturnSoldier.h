#pragma once
#include "BattleEntity.h"
class StrikeAndReturnSoldier :
    public BattleEntity
{
public:
	StrikeAndReturnSoldier() = default;
	StrikeAndReturnSoldier(Utilities::Relationship relationship,
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

	virtual ~StrikeAndReturnSoldier();

	//void Move(int startIndex, int endIndex);
	//void MoveToCell() override;
	 
	virtual void Return();
	// TrailPath is overriden to ignore obstacles and "fly" over them.
	//vector<int> TrailPath(int startIndex, int endIndex) override;
	virtual void AttackEnemy() override;
	void Attack(shared_ptr<Cell> enemyCell) override;
	//void Display(int, int, float) override;
	void SetStartingPosition(float x, float y);
	void DrawShadow(int animationSpeed, float dt) override;
	
	int GetMoveToIndex() override;

	void EnemyRetaliate(int amount) override;
	//void SetCell(const Rectangle& cell) override;
	//void MouseOver(bool flag) override;
	//void Lured(int moveToIndex) override;

protected:
	int attackFromIndex = -1;
	bool regularAttack = false;
	float postAttackIdleTimer = 0.0f;
	bool waitingAfterAttack = false;
};

