#pragma once

#include "Entity.h"
#include "Portrait.h"

class Recruiter :
    public Entity
{
public:
	Recruiter() = default;
	Recruiter(const vector<string>& typesForHire,
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
	~Recruiter();

	void SaveCoordinates() override;
	void CreateGrid();
	void PrepArmyPortraits();
	bool& HasCollidedWithDante() override;
	
	void Update(float offsetX, float dt) override;
	string ClickAssetOption(float offsetX) override;	
	void AddText();
	void AddUnitForHire();
	void DrawDialogue(float offsetX) override;
	void DrawUnitsForHire();
	void Display(int, int, float) override;
	vector<Portrait>* GetUnitsForHire();

private:

	//void SaveCoordinates() override;
	bool addUnitForHire{ false };
	fs::path unitsForHirePath;
	vector<string> typesForHire;
	vector<Portrait> unitsForHire;
	vector<Rectangle> grid;
};


