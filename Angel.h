#pragma once
#include "SpellCaster.h"
#include "HolyBeam.h"
class Angel :
    public SpellCaster
{

public:
	Angel() = default;
	Angel(string projectile_type,
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
		int color = 0);

	~Angel();

	virtual void AttackEnemy() override;

protected:
};

