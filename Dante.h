#pragma once
#include "Entity.h"


class Dante:
	public Entity
{
public:
	Dante() = default;
	Dante(fs::path coordinatesPath,
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
	~Dante();
	void Idle();
	void MoveRight();
	void MoveLeft();
	void SetStance(const string&);
	void Display(int, int, float) override;
	//void DrawPortrait(Utilities::Direction facing=right, bool dialogueBox=false) const override;
	bool& HasCollidedWithUnit();
	bool& EngagedInDialogue();
	
	static float armor;
	static float holiness;
	static float attack;
	static float food;
	static float gold;

private:
	bool hasCollidedWithUnit{ false };
	bool engagedInDialogue{ false };


};

