#pragma once
#include<memory>
#include <string>
#include <vector>
#include<iostream>
#include<fstream>
#include <chrono>
#include<map>
#include<unordered_map>
#include<algorithm>
namespace fs = std::filesystem;

#include "raylib.h"
#include "raymedia.h" 
#include "Menu.h"
#include "Entity.h"
#include "Town.h"
#include "Portrait.h"
#include "Utilities.h"
#include "TextureManager.h"
#include "Background.h"
#include "MarchingScreen.h"
#include"MarchingEntity.h"
#include "UI.h"
#include "Battleground.h"
//#include "Button.h"


class Editor
{
public:
	Editor() = default;
	Editor(float globalSpeed);

	bool ShowTrailer() const;
	bool ExitGame() const;
	bool ShowMainMenu() const;
	bool ShowMenuEditor() const;
	bool ShowTowns() const;
	bool ShowMarchingScreens() const;
	bool ShowProps() const;
	bool ShowTiles() const;
	bool ShowUnits() const;
	bool ShowNPCs() const;
	bool ShowArmyCaptains() const;
	//bool ShowPatrolingNPCs() const;
	bool ShowBackgrounds() const;
	bool ShowBattlegrounds() const;

	
	void DragUnits();
	void Display(float);
	void DrawTrailer();
	void DrawTown(float);
	void DrawUnits(float);
	//void DrawArmy(float);
	void DrawTiles();
	void DrawBackground(float);
	void DrawTownBattleground();
	void DrawBattleground(float dt);
	void Update(float);

	void Collisions();
	void DrawGrid();
	void ClickPortrait();
	void SelectPortrait(const vector<Portrait>&);
	void AssetOptionsUnits();
	void ClickAssetOption();
	void DrawMainMenu(float dt);
	void DrawMenuEditor();
	void DrawTownsMenu();
	void DrawMarchingScreensMenu();
	void DrawUI();
	template<typename T>
	void HandleAddButtons(Vector2 mouse, vector<string>& names, const string& category);
	void HandlePositionButtons(Vector2 mouse);

	void MainMenuOption();
	void MainMenuOptionEditor();
	void TownsMenuOption(Vector2 mouse);
	void MarchingScreensOption(Vector2 mouse);
	void DrawTownName();
	void KeyReleased();
	void KeyPressed(int);
	void KeyDown(float);

	void MouseButtonReleased();
	void MouseButtonPressed();

	shared_ptr<TextureManager> textureManager;
	
	//Media and Sound Files

	void PlaySoundTracks();

	void StartTrailer();
	void UpdateTrailer();
	
private:
	void UnloadIntroMedia();
	void UnloadMediaFiles();
	vector<unique_ptr<MediaStream>> trailerPlaylist;

	int trailerIndex{ 0 };

	unique_ptr<MediaStream> intro;

	shared_ptr<UI> Interface;	

	float globalSpeed;

	bool exitGame_{ false };

	bool showTrailer_{ false };
	bool showMainMenu_{ false };
	bool showMenuEditor_{ false };
	bool showTowns_{ false };
	bool showMarchingScreens_{ false };
	bool showProps_{ false };
	bool showTiles_{ false };
	bool showUnits_{ false };
	bool showNPCs_{ false };
	bool showArmyCaptains_{ false };
	bool showBackgrounds_{ false };
	bool showBattlegrounds_{ false };
	bool battleMode_{ false };

	bool playlistPlaying;

	// main menu variables
	//Texture2D* mainMenuScreen = nullptr;

	int mainScreenWidth = SCR_WIDTH;
	int mainScreenHeight = SCR_HEIGHT;
	int mainScreenRows = 6;
	int mainScreenCol = 10;
	int mainScreenTotalFrames = mainScreenRows * mainScreenCol;
	float mainScreenFrameTimer = 0.0f;
	int mainScreenFrameIndex = 0; // 0 to 59
	Vector2 origin = { 0.0f,0.0f };
	Rectangle mainScreenSourceRect{ 0.0f,
		0.0f,
		static_cast<float>(mainScreenWidth),
		static_cast<float>(mainScreenHeight)
	};
	Rectangle mainScreenDestRect{ 0.0f,
		0.0f,
		static_cast<float>(mainScreenWidth),
		static_cast<float>(mainScreenHeight)
	};

	Music* currentMusic = nullptr;

	Music mainTheme;

	// marching variables
	bool playMarchingTheme{ false };
	Music marchingTheme;
	Sound marchingLoop;

	Sound clickButtonSound;
	bool playClickButtonSound{ false };

	string townName;
	fs::path resourceDirectory;
	fs::path dataDirectory;
	fs::path armyPath;
	fs::path enemyArmyPath;
	fs::path marchingArmyPath;

	vector<Portrait> unitPortraits;
	vector<Portrait> NPCPortraits;
	vector<Portrait> armyCaptainPortraits;
	vector<Portrait> propsPortraits;
	vector<Portrait> tilesPortraits;
	vector<Portrait> backgroundPortraits;
	vector<Portrait> battlegroundPortraits;
	//vector<Portrait> patrolingNPCPortraits;
	//vector<Portrait> dialoguePortraits;
	vector<string>* unitTypes;

	Menu mainMenuEditor;
	Menu townsMenu;
	Menu marchingScreensMenu;
	Menu battleModeMenu;

	Menu mainMenu;
	
	vector<string> townNames;
	vector<string> marchingScreenNames;
	vector<string> alliedArmyTypes;
	vector<string> enemyArmyTypes;
	vector<string> marchingTypes;

	unordered_map<string, float> countArmyTypes;
	unordered_map<string, float> enemyCountArmyTypes;
	unordered_map<string, float> marchingCountArmyTypes;

	//shared_ptr<vector<MarchingEntity>> army;
	shared_ptr<vector<string>>army;
	shared_ptr<vector<string>>enemyArmy;
	shared_ptr<vector<string>>armySpells;
	shared_ptr<vector<string>>enemyArmySpells;
	shared_ptr<vector<MarchingEntity>> marchingArmy; // its the army that will be marching behind Dante on the marching screens

	map<string, std::unique_ptr<Town>> Towns;
	//map<string, MarchingScreen> MarchingScreens;
	int position{ 0 };
	Button positonButton = { "position: ", 18 };
	Button increasePosition = { " => ", 20 };
	Button decreasePosition = { " <= ", 20 };


	// Adding a new Town name writing variable
	bool textBoxActive{ false };
	bool addText_{ false };
	bool addingNewTown{ false };
	bool addingNextTown{ false };

	// Adding units to army captain variables
	bool addingUnitsToCaptain{ false };

	Rectangle textRect;
	Button addButton = { "Add", 20 };
	Button backButton = { "Back", 20 };
	string newText = "";

	std::shared_ptr<Battleground> battleground;

   void LoadTownNames(vector<string>&, string);

   template <typename T>
   void LoadTowns(vector<string>&, string, Menu&); // Also takes care of towns menues
   void LoadTextList(const fs::path& path, string fileName, shared_ptr<vector<string>> container);
   void LoadMarchingArmy(const fs::path&,
	   unordered_map<string,
	   float>&,
	   vector<string> armyTypes = {},
	   shared_ptr<vector<MarchingEntity>> = nullptr
	  );
   void DrawAnimatedPortrait(
	   Texture2D* portrait,
	   Rectangle& portraitSource,
	   Rectangle portraitDest,
	   int totalPortraitFrames,
	   int animPortCols,
	   float dt,
	   float& portraitFrameTimer,
	   int& portraitFrameIndex
   );
   template <typename T>
   void AddTown(vector<string>&, string, string);
   void AddText(string& textVariable);
   void GetTypes();  
   void LoadPortraits(const char*, vector<Portrait>&);
   void CreateGrid(const map <string, Texture2D>&, vector<Portrait>&);
   void DrawOptions(const vector<Portrait>&);
   void AddUnit(const string& type, int position);
   void AddUnitToCaptain(const string& type);
   void AddEnemyArmyCaptain(const string& type, int position);



   void SwitchTowns(string& nextTown);

   void Back();
};

