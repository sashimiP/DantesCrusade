#include "CampaignEntity.h"

CampaignEntity::CampaignEntity(fs::path dataPath,
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
	vector<string>dialogue,
	string name,
	int color,
	bool dontScale,
	Vector2 patrolRoute) :
	Entity(dataPath,
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
		color,
		dontScale,
		patrolRoute)
{

}

CampaignEntity::~CampaignEntity()
{
	// save_flag is very important!!!
	// if flag is not set it will add the coordinates file to the directory!!!
	if(save) SaveCoordinates();
	save = false;
}