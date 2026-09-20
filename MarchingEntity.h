#pragma once
#include "Entity.h"
class MarchingEntity :
    public Entity
{
public:
    MarchingEntity() = default;
	MarchingEntity(fs::path unitPath,
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

	void Display(int, int, float) override;
	void Adjust(Rectangle rect);
	void MoveRight() override;
	void MoveLeft() override;
};

