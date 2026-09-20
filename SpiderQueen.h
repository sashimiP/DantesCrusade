#pragma once
#include "BattleEntity.h"
class SpiderQueen :
    public BattleEntity
{
public:
	SpiderQueen() = default;
	SpiderQueen(Utilities::Relationship relationship,
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

	~SpiderQueen();

	//void Update(float offsetX) override;
	//void Move(int rectIndex) override;
	void MoveToCell() override;
	void AttackEnemy() override;
	void Attack(shared_ptr<Cell> enemyCell) override;
	void DrawShadow(int animationSpeed, float dt) override;

	void SetCell(const Rectangle& cell) override;
	void SetStance(string stance, bool updateRect = true);
	//void MouseOver(bool flag) override;

	//void Lured(int moveToIndex) override;

private:
	
	float fallDuration = 1.2f;
	float jumpDuration = 1.14f;
	

	bool jumpUp = false;
	bool fallDown = false;
	
};

