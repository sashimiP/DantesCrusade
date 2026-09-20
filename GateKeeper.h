#pragma once
#include "Entity.h"
class GateKeeper :
    public Entity
{
public:
	GateKeeper() = default;
	GateKeeper(const string& enterTownName,
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
	~GateKeeper();

	void DrawButton();
	bool& HasCollidedWithDante() override;
	Rectangle ButtonRect();
	string ClickButton();
	void SetNextTown(const string& nextTownName);
	void SetButton(const string& nextTownName);

private:
	//void SaveCoordinates() override;
	std::unique_ptr<Button> enterTownButton;
	string enterTownName;
};

