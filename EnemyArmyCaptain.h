#pragma once
#include "CampaignEntity.h"
#include "Button.h"

//ADD AN UPDATE OPTIMISATION

class EnemyArmyCaptain :
    public CampaignEntity
{
public:
    EnemyArmyCaptain() = default;
    EnemyArmyCaptain(fs::path dataPath,
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
		Vector2 patrolRoute = { 0,0 });

    ~EnemyArmyCaptain();
	void Update(float offsetX, float dt) override;
	void Drag() override;
	string ClickAssetOption(float) override;
	void AddUnit(string);
	void ActivateAddUnit(bool);
	void ShowArmy();
	void Display(int, int, float) override;
	void DrawArmyPortraits();
	void CreateArmyPanel();
	void UpdateArmyPanel();
	void MoveRight(int scrollSpeed, float dt) override;
	void MoveLeft(int scrollSpeed, float dt) override;
	void UpdateDialogue() override;

	bool GetAddUnitFlag();
	//const unordered_map<int, string> GetArmy();
	const vector<string>& GetArmy();


private:
	void CreateDirectories(string&) const override;
	void LoadArmy();
	
	fs::path armyDataPath;
	vector<int> keys;
	//unordered_map<int, string> army;
	vector<string> army;
	vector<Texture2D*> armyPortraits;
	vector<std::pair<Rectangle, Texture2D*>> armyPortraitsGrid;
	bool addingUnitsFlag{ false };
	bool showArmyFlag{ false };
	Button closeArmyPanelButton{ "back", 20 };
	Button engageButton{ "engage", 20 };
	Button showArmyButton{ "show army", 20 };
};

