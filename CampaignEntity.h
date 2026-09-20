#pragma once
#include "Entity.h"
class CampaignEntity :
    public Entity
{
public:
    CampaignEntity() = default;
    CampaignEntity(fs::path dataPath,
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
		int color = 0,
		bool dontScale = false,
		Vector2 patrolRoute = { 0,0 }
	);

    virtual ~CampaignEntity();
};

