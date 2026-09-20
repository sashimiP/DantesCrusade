#pragma once
#include "BattleEntity.h"
#include "Projectile.h"

class RangedSoldier :
    public BattleEntity
{
public:
	RangedSoldier() = default;
	RangedSoldier(string projectile_type,
		float projectile_scale,
		Utilities::Relationship relationship,
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

	virtual ~RangedSoldier();

	void Update(bool selected) override;
//	void Move(int rectIndex) override;
////	void MoveToCell() override;
	virtual void AttackEnemy() override;
	void Attack(shared_ptr<Cell> enemyCell) override;
	virtual void Retaliate() override;
	void Display(int, int, float) override;	
	void ScaleDestRect(float) override;
	

	void SetCell(const Rectangle& cell) override;

	void AddStat(Stat stat, int amount) override;
	void SubStat(Stat stat, int amount) override;

protected:
	unique_ptr<Projectile> projectile;
};

