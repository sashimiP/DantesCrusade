#pragma once
#include <string>
#include <vector>
#include<iostream>
#include<fstream>
#include <chrono>
#include<map>
#include<algorithm>
#include <filesystem>
#include <optional>
namespace fs = std::filesystem;

#include "raylib.h"
#include "Menu.h"
#include "UI.h"
#include "Entity.h"
#include "Portrait.h"
#include "Utilities.h"
#include "TextureManager.h"
#include "Battleground.h"
#include "Background.h"
#include "TownBackground.h"
#include "Dante.h"
#include "Tiles.h"
#include "DialoguePlatform.h"
#include "DanteHorse.h"
#include "SpecialEntity.h"
#include "MarchingEntity.h"
#include "Recruiter.h"
#include "CampaignEntity.h"
#include "EnemyArmyCaptain.h"
//#include "PatrolingEntity.h"


class Town
{
public:
	Town() = default;
	Town(string townName, shared_ptr<TextureManager> textureManager, shared_ptr<UI> Interface, shared_ptr<Battleground> battleground);
	virtual ~Town();
	void SaveCoordinates();
	void LoadCoordinates();
	virtual void AddUnit(const string&, int);
	//void AddPatrolingUnit(const string&, int);
	void AddEnemyArmyCaptain(const string&, int);
	void AddUnitToArmyCaptain(string);
	virtual void SetBackground(const string&);
	void SetBattleground();
	void SetBattleground(const string&);
	virtual void Update(float);
	virtual void DragUnits();
	virtual void AssetOptionsUnits();
	virtual std::optional<std::string> ClickAssetOption();
	virtual void DrawDialogue(float dt);
	virtual void DrawUnits(float);
	//void DrawShadows(float);
	virtual void DrawBackground(float);
	void DrawBattleground(float dt);
	void ShowBattleground();
	void DrawTiles();
	virtual void MoveRight(float);
	virtual void MoveLeft(float);
	void DanteIdle();
	void DanteMoveRight();
	void DanteMoveLeft();
	virtual void ArmySetStance(const string& stance, float scalingFactor = 1.0f);
	virtual void ArmyMoveRight();
	virtual void ArmyMoveLeft();
	void DanteSetStance(const string& stance, float scalingFactor=1.0f);
	virtual void CollideRectUnitDante();
	bool CollideRectHorseDante();
	void CollideRectSpecialUnitDante();
	bool CollideRectRecruiterDante();

	virtual void Collisions(vector<bool*>menuFlags);
	virtual std::optional<std::string> HandleButtonClick(Vector2 mouse, vector<string>validTownNames);
	void RightMouseButtonReleased(Vector2 mouse);
	virtual void LeftMouseButtonReleased(Vector2 mouse, vector<string>validTownNames);
	void LeftMouseButtonDown();


	void SelectTiles(const string& newTiles);
	void SetScrollSpeed(int);
	Rectangle BeginMarchButtonRect();
	string NextTown();
	virtual void SetNextTown(string townName);

	virtual float GetScrollSpeed() const;
	virtual float GetScalingFactor() const;
	virtual std::string GetDanteStance() const;
	virtual void ApplyArmyStance() {};

	bool Engage() const;
	string GetNextTown();
	// quick fix, FIND A BETTER WAY!!!
	//unordered_map <int, string> GetArmy();
	vector<string>& GetArmy();
	bool ShowBattlegroundArenaFlag() const;
	//bool IsWaitingForPatrolClick() const;
	//virtual void SetArmy(const shared_ptr<vector<MarchingEntity>>& army);
	void SetArmy(shared_ptr<vector<string>> army);
	virtual void SetMarchingArmy(const shared_ptr<vector<MarchingEntity>>& marchingArmy);
	void SetShowBattlegroundArena(bool);
	
	
	bool stopMove{ false };
	
protected:

	template<typename T>
	void LoadEntity(fs::path datapath);

	void LoadDante();
	void LoadDanteHorse();
	void LoadRecruiter();

	void LoadSpecialEntities();
	void OpenFileAndLoadData(const fs::directory_entry&, const string&, vector<string>&);

	void SaveTiles();
	void LoadTiles();

	int scrollSpeed;
	std::string townName;
	std::string nextTown = "";
	vector<std::unique_ptr<Entity>> Units;

	//variables used for setting the patrol route of a unit
	Entity* entitySettingPatrol = nullptr;
	bool waitingForPatrolClick = false;
	int clickCount = 0;



	std::unique_ptr<Background> background;

	//Battleground variables
	bool showBattlegroundArena{ false };
	bool engage{ false };
	bool battleGroundSet{ false };
	std::shared_ptr<Background> battlegroundArena; // background that will be passed to the 
	shared_ptr<Battleground> battleground;
	
	std::unique_ptr<Tiles> tiles;
	std::unique_ptr<Dante> dante;
	int globalSpeed;
	shared_ptr<TextureManager> textureManager;
	shared_ptr<UI> Interface;
	std::unique_ptr<DialoguePlatform> dialoguePlatform;

	fs::path coordinatesPath;
	fs::path danteDataPath;
	fs::path horseDataPath;
	fs::path unitsDataPath;
	fs::path specialEntitiesPath;
	fs::path recruiterDataPath;
	fs::path backgroundDataPath;
	fs::path battlegroundPath;
	fs::path tilesDataPath;
	fs::path armyDataPath;
	fs::path captainsDataPath;

	float offsetX{ 0 };
	float offsetY{ 0 };

	std::unique_ptr<DanteHorse> danteHorse;
	std::unique_ptr<Recruiter> recruiter;
	map<string, SpecialEntity> specialEntities;

	map<string, float*> stats;
	//shared_ptr<vector<MarchingEntity>> army;

	shared_ptr<vector<string>> army;
	shared_ptr<vector<MarchingEntity>> marchingArmy;
	string specialEntityColliding = "";

	EnemyArmyCaptain* currentCaptain = nullptr;

	bool activateDialogue{ false };


};

