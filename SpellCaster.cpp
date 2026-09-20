#include "SpellCaster.h"

SpellCaster::SpellCaster(string projectile_type,
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
	vector<string> dialogue,
	string name,
	int color	
):
	RangedSoldier(projectile_type,
		projectile_scale,
		relationship,
		cellIndex,
		grid,
		dataPath,
		town,
		textureManager,
		id,
		position,
		type,
		x,
		y,
		direction,
		drag,
		stance,
		dialogue,
		name,
		color)
{}

SpellCaster::~SpellCaster()
{
	save = false;
}
