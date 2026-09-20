#pragma once
#include "Projectile.h"
class HolyBeam :
    public Projectile
{
public:
    HolyBeam() = default;
	HolyBeam(int rangedEnityCellIndex,
		float rangedEntityWidth,
		float rangedEntityHeight,
		float scalingFactor,
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

	~HolyBeam();

	void FlyToTarget() override;
	void DrawShadow(int animationSpeed, float dt) override;

private:

};

