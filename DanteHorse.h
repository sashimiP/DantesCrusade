#pragma once
#include "Entity.h"
class DanteHorse :
    public Entity
{
public:
    DanteHorse() = default;
    DanteHorse(const string& destination,
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
	~DanteHorse();

	void DrawButton();
	bool& HasCollidedWithDante() override;
	Rectangle ButtonRect();
	string ClickButton();
	void SetNextTown(const string& nextTownName);
	void SetButton(const string& nextTownName);

private:
	//void SaveCoordinates() override;
    std::unique_ptr<Button> beginMarchButton;
	string destination;
};

