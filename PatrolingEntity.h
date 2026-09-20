#pragma once
#include "Entity.h"
class PatrolingEntity :
    public Entity
{
public:
    PatrolingEntity() = default;
	PatrolingEntity(fs::path dataPath,
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
		bool dontScale = false);
	~PatrolingEntity() override;
	string ClickAssetOption(float) override;
	void Display(int, int, float) override;
    void SetRoute();
private:
    float patrolDestination;
	bool hasRoute{ false };
	bool setRoute{ false };
    
};

