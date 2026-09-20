#include "Editor.h"

void Editor::GetTypes()
{
	unitTypes = textureManager->UnitTypes();

}

Editor::Editor(float globalSpeed) :
	globalSpeed(globalSpeed),
	mainMenu({ "play", "editor", "exit" }, 25),
	mainMenuEditor({ "towns", "marching screens", "battle_mode", "backgrounds", "battlegrounds", "props", "tiles", "units", "NPCs", "army_captains", "options", "back" }),
	battleModeMenu({"create_army", "create_enemy_army", "exit_battle_mode"})
{
	textureManager = make_shared<TextureManager>();
	battleground = std::make_shared<Battleground>(textureManager);
	intro = textureManager->IntroMedia();

	mainTheme = LoadMusicStream("resources\\audio\\music\\main_theme.mp3");
	marchingTheme = LoadMusicStream("resources\\audio\\music\\albion.mp3");
	SetMusicVolume(marchingTheme, 0.6f);
	marchingLoop = LoadSound("resources\\audio\\sound\\marching.mp3");
	SetSoundVolume(marchingLoop, 0.2f);
	clickButtonSound = LoadSound("resources\\audio\\sound\\clickButton.mp3");

	mainMenu.SetFrameRectColor(RAYWHITE);
	showMainMenu_ = true;
	/*showMenuEditor_ = false;
	showBackgrounds_ = false;
	showProps_ = false;
	showUnits_ = false;
	showNPCs_ = false;*/

	resourceDirectory = fs::path("resources");
	dataDirectory = fs::path("data");
	armyPath = fs::path("data") / "army";
	enemyArmyPath = fs::path("data") / "enemy_army";
	marchingArmyPath = fs::path("data") / "marching_army";
	auto start = std::chrono::high_resolution_clock::now();
	//mainMenuEditor.DrawMenu();
	unitPortraits.reserve(150);
	NPCPortraits.reserve(150);
	GetTypes();
	LoadPortraits("units", unitPortraits);
	LoadPortraits("NPCs", NPCPortraits);
	LoadPortraits("props", propsPortraits);
	LoadPortraits("tiles", tilesPortraits);
	LoadPortraits("backgrounds", backgroundPortraits);
	LoadPortraits("battlegrounds", battlegroundPortraits);
	LoadPortraits("army_captains", armyCaptainPortraits);
	//LoadPortraits("patroling_NPCs", patrolingNPCPortraits);	
	//LoadPortraits("dialogue", dialoguePortraits);

	army = std::make_shared<vector<string>>();
	enemyArmy = std::make_shared<vector<string>>();
	armySpells = std::make_shared<vector<string>>();
	enemyArmySpells = std::make_shared<vector<string>>();
	marchingArmy = std::make_shared<vector<MarchingEntity>>();

	countArmyTypes.reserve(20);
	//LoadArmy(armyPath, countArmyTypes, alliedArmyTypes, army );
	//LoadArmy(enemyArmyPath, enemyCountArmyTypes, enemyArmyTypes);
	LoadTextList(armyPath, "army_list", army);
	LoadTextList(armyPath, "spell_list", armySpells);
	LoadMarchingArmy(marchingArmyPath, marchingCountArmyTypes, marchingTypes, marchingArmy);
	Interface = make_shared<UI>(textureManager, alliedArmyTypes);
	Interface->SetUI(east);

	LoadTowns<Town>(townNames, "towns", townsMenu);
	LoadTowns<MarchingScreen>(marchingScreenNames, "marching_screens", marchingScreensMenu);

	positonButton.SetPosition( townsMenu.GetCellWidth(), 0);
	decreasePosition.SetPosition( positonButton.GetRect().x + positonButton.GetRect().width, positonButton.GetRect().y + 32);
	increasePosition.SetPosition(decreasePosition.GetRect().x + decreasePosition.GetRect().width, decreasePosition.GetRect().y);
	positonButton.SetRectColor(BLUE);
	increasePosition.SetRectColor(BLUE);
	decreasePosition.SetRectColor(BLUE);

	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed = end - start;
	std::cout << "#####Execution time: " << elapsed.count() << " seconds #################\n";
	
}

void Editor::LoadTownNames(vector<string>& names, string townType)
{
	auto directory = dataDirectory / townType;
	names.reserve(Utilities::getFileCount(directory));
	//cout << townType << "NAMES:" << endl;
	for (const auto& entry : fs::directory_iterator(directory))
	{
		auto option = Utilities::split(entry.path().string(), "\\");
		names.push_back(std::move(option.back()));

	}
}

template <typename T>
void Editor::LoadTowns(vector<string>& names, string townType, Menu& menu)
{
	LoadTownNames(names, townType);

	for (auto& name : names)
	{
		Towns[name] = std::make_unique<T>(name, textureManager, Interface, battleground);
	}

	vector<string>menuOptions = names;
	menuOptions.insert(menuOptions.begin(), "back");
	menuOptions.insert(menuOptions.end(), "add_next_town");
	menuOptions.insert(menuOptions.end(), "+");

	menu = Menu(menuOptions);
}

void Editor::LoadTextList(const fs::path& directory, string fileName, std::shared_ptr<std::vector<std::string>> army)
{
	if (!army) {
		std::cerr << "Error: LoadArmy received null shared_ptr\n";
		return;
	}

	const auto filepath = directory / (fileName + ".txt");

	std::ifstream file(filepath);
	if (!file.is_open()) {
		std::cerr << "Failed to open army file: " << filepath << "\n";
		return;
	}

	// Optional: replace previous content (common for "load")
	// army->clear();

	std::string line;
	size_t count = 0;

	while (std::getline(file, line)) {
		// Optional: skip empty lines or lines with only whitespace
		if (line.find_first_not_of(" \t\r\n") != std::string::npos) {
			army->push_back(std::move(line));  // move avoids copy
			++count;
		}
	}

	std::cout << "Loaded " << count << " army entries from " << filepath << "\n";
}

void Editor::LoadMarchingArmy(const fs::path& path, unordered_map<string, float>& countTypes, vector<string> armyTypes, shared_ptr<vector<MarchingEntity>> army)
{
	for (const auto& entry : fs::directory_iterator(path))
	{
		std::vector<std::string> coordinates;
		std::vector<std::string> dialogue;

		Utilities::OpenFileAndLoadData(entry, "coordinates", coordinates);
		Utilities::OpenFileAndLoadData(entry, "dialogue", dialogue);

		if (auto parsed = Utilities::ParseEntityData(coordinates, dialogue)) {
			if(armyTypes.size()>0)
				armyTypes.push_back(parsed->type);  // add only if not already there


			++countTypes[parsed->type];

			
			if(army)//armyTypes.push_back(parsed->type);
				army->emplace_back(
					path,
					"Unknown",
					textureManager,
					parsed->id,
					parsed->position,
					parsed->type,
					parsed->x,
					parsed->y,
					parsed->direction,
					parsed->drag, // drag
					parsed->stance,
					parsed->dialogue, // Move the dialogue vector
					parsed->name,
					parsed->color
				);
		//	std::cout << "Loaded NPC from: " << entry.path() << "\n";
		}
		else {
			std::cerr << "Skipping invalid entity in " << entry.path() << "\n";
		}
	}
	cout << "ARMY SIZE IS :" << army->size() << endl;
}

void Editor::DrawAnimatedPortrait(
	Texture2D* portrait,
	Rectangle& portraitSource,
	Rectangle portraitDest,
	int totalPortraitFrames,
	int animPortCols,
	float dt,
	float& portraitFrameTimer,
	int& portraitFrameIndex
)
{
	// Update animation timer
	portraitFrameTimer += dt;

	float frameDuration = 1.0f / 8.0f;

	while (portraitFrameTimer >= frameDuration)
	{
		portraitFrameTimer -= frameDuration;
		portraitFrameIndex = (portraitFrameIndex + 1) % totalPortraitFrames;

		int col = portraitFrameIndex % animPortCols;
		int row = portraitFrameIndex / animPortCols;

		portraitSource.x = col * portraitSource.width;
		portraitSource.y = row * portraitSource.height;
	}

	DrawTexturePro(
		*portrait,
		portraitSource,
		portraitDest,
		origin,
		0.0f,
		RAYWHITE
	);
}

template <typename T>
void Editor::AddTown(vector<string>& townNames, string name, string townType)
{
	bool townExists = std::find(townNames.begin(), townNames.end(), name) != townNames.end();
	if (townExists) return;
	townNames.push_back(name);

	if(typeid(T) == typeid(Town))
		townsMenu.AddMenuOption(name);
	else if (typeid(T) == typeid(MarchingScreen))
		marchingScreensMenu.AddMenuOption(name);
	
	auto townDirectory = dataDirectory / townType/ name;
	//create_directory(townDirectory);

	for (string dir: Utilities::townDirectories)
	{
		fs::create_directories(townDirectory / dir);
	}

	Towns[name] = std::make_unique<T>(name, textureManager, Interface, battleground);
}

void Editor::AddText(string& textVariable)
{
	// Process typed characters
	int key = GetCharPressed();
	while (key > 0)
	{
		if ((key >= 32) && (key <= 125) && (textVariable.length() < 64))
		{
			textVariable += (char)key;
			textRect.width = 20 + MeasureText(textVariable.c_str(), FONT_SIZE);
			
		}
		key = GetCharPressed();
	}

	// Handle backspace
	if (IsKeyPressed(KEY_BACKSPACE) && !textVariable.empty())
	{
		char deletedKey = textVariable.back();
		textVariable.pop_back();
		textRect.width = 20 + MeasureText(textVariable.c_str(), FONT_SIZE);

	}
	
}

void Editor::Display(float dt)
{
	if (!townName.empty() && Towns[townName]->Engage() || battleMode_)
	{
		DrawBattleground(dt);
		//Towns[townName]->DrawBattleground(dt);
		return;
	}
	DrawTrailer();
	DrawTown(dt);

	DrawMainMenu(dt);
	DrawMenuEditor();
	DrawTownsMenu();
	DrawMarchingScreensMenu();
	DrawTownName();
	DrawGrid();
	DrawUI();
	
}

void Editor::DrawTrailer()
{
	if (!showTrailer_) return;

	if (!trailerPlaylist.empty())
	{
		MediaProperties props = GetMediaProperties(*trailerPlaylist[trailerIndex]);
		if (props.hasVideo)
			DrawTexture(trailerPlaylist[trailerIndex]->videoTexture, 0, 0, WHITE);
	}
}

void Editor::DrawTown(float dt)
{
	DrawTownBattleground();

	if (!townName.empty() && Towns[townName]->ShowBattlegroundArenaFlag()) return;

	DrawBackground(dt);
	DrawTiles();
	DrawUnits(dt); // Display Dante, each NPC, and the marching units
}

void Editor::DrawBackground(float dt)
{
	if (townName.empty()) return;

	Towns[townName]->DrawBackground(dt);

}

void Editor::DrawTownBattleground()
{
	if (townName.empty()) return;

	Towns[townName]->ShowBattleground();
}

void Editor::DrawUnits(float dt)
{

	if (townName.empty()) return;
	
	
	Towns[townName]->DrawUnits(dt);
	Towns[townName]->DrawDialogue(dt);
	
}

void Editor::DrawTiles()
{
	if (townName.empty()) return;
	Towns[townName]->DrawTiles();

}

void Editor::Update(float dt)
{
	if (!townName.empty())
		Towns[townName]->Update(dt);

	if(textBoxActive)
	{
		AddText(newText);
	}

	if ((!townName.empty() && Towns[townName]->Engage()) || battleMode_)
		battleground->Update();

	if (intro && ShowMainMenu()) UpdateMedia(intro.get());
	UpdateMusicStream(mainTheme);
	
	UpdateTrailer();
}

void Editor::DrawUI()
{
	Interface->Display();
}

void Editor::DrawBattleground(float dt)
{

	battleground->DrawBackground();
	battleground->DrawGrid();
	battleground->DrawShadows(dt);
	battleground->DrawUI();
	battleground->DrawArmies(dt);
	battleground->DisplayCards();
	battleground->DrawMenu();
	
	battleground->DrawAttackCursor();
	
	
}

void Editor::Collisions()
{
	if (townName.empty()) return;

	//auto* town = Towns[townName].get();

	Towns[townName]->Collisions({&showMenuEditor_, &showTowns_, &showTowns_});
	//Towns[townName]->Collisions({});

}

void Editor::DragUnits()
{
	if (townName.empty()) return;
	Towns[townName]->DragUnits();
}

void Editor::AssetOptionsUnits()
{
	if (townName.empty()) return;
	Towns[townName]->AssetOptionsUnits();

}

void Editor::ClickAssetOption()
{
	if (townName.empty()) return;
	auto option = Towns[townName]->ClickAssetOption();
	if (option != std::nullopt)
	{
		if (option == "add units")
		{
			Back();
			showMenuEditor_ = false;
			//showBackgrounds_ = false;
			showUnits_ = true;
			position = 3;
			addingUnitsToCaptain = true;
		}
		else if (option == "stop add units")
		{
			Back();
			addingUnitsToCaptain = false;
		}
	}
	
	
}

void Editor::LoadPortraits(const char* type, vector<Portrait>& Portraits)
{
	auto portraitData = textureManager->GetPortraitData(type);
	if (portraitData && !portraitData->empty())
	{
		CreateGrid(*portraitData, Portraits);
	}
    
}

void Editor::CreateGrid(const map <string, Texture>& nameTextureMap, vector<Portrait>& Portraits)
{
	const float width{ 64.0f };
	const float height{ 64.0f };
	float x { 0.0f };
	float y{ 0.0f };
	vector<Rectangle> Cells;
	Cells.reserve(nameTextureMap.size());

	for (const auto& [name, texture] : nameTextureMap) {
		Rectangle rect{ x, y, width, height };
		Cells.push_back(rect);
		Portraits.emplace_back(name, texture, rect);

		// Update position for next cell
		y += height;

		// Check if we need to move to next column
		if (y + height > SCR_HEIGHT - 128.0f) {  // 128 = 64*2 margin
			y = 0.0f;
			x += width;
		}
	}
}

void Editor::DrawGrid()
{
	if (ShowUnits())
	{
		DrawOptions(unitPortraits);
	}
	else if (ShowNPCs())
	{
		DrawOptions(NPCPortraits);
	}
	else if (ShowArmyCaptains())
	{
		DrawOptions(armyCaptainPortraits);
	}
	else if (ShowBackgrounds())
	{
		DrawOptions(backgroundPortraits);
	}
	else if (ShowBattlegrounds())
	{
		DrawOptions(battlegroundPortraits);
	}
	else if (ShowProps())
	{
		DrawOptions(propsPortraits);
	}
	/*else if (ShowPatrolingNPCs())
	{
		DrawOptions(patrolingNPCPortraits);
	}*/
	else if (ShowTiles())
	{
		DrawOptions(tilesPortraits);
	}
}

void Editor::DrawOptions(const vector<Portrait>& Portraits)
{
	for (const auto& portrait : Portraits)
		portrait.Display();

	positonButton.Display();
	increasePosition.Display();
	decreasePosition.Display();

	auto positionRect = Rectangle{positonButton.GetRect().x + positonButton.GetRect().width,  positonButton.GetRect().y, 25,25 };

	/*DrawRectangleRec(positionRect, DARKGRAY);
	DrawRectangleLinesEx(positionRect, 1, BLUE);*/
	DrawText(to_string(position).c_str(), positionRect.x+20, positionRect.y+10, FONT_SIZE, BLACK);

}

void Editor::AddUnit(const std::string& type, int position)
{
	if (townName.empty()) return;
	Towns[townName]->AddUnit(type, position);

}

void Editor::AddUnitToCaptain(const std::string& type)
{
	if (townName.empty()) return;
	Towns[townName]->AddUnitToArmyCaptain(type);
}

void Editor::AddEnemyArmyCaptain(const std::string& type, int position)
{
	if (townName.empty()) return;
	Towns[townName]->AddEnemyArmyCaptain(type, position);

}

void Editor::SwitchTowns(string& nextTown)
{
	//auto nextTown = Towns[townName]->GetNextTown();
	Towns[nextTown]->SetArmy(army);
	Towns[nextTown]->SetMarchingArmy(marchingArmy);
	Towns[nextTown]->SetBattleground();

	if (typeid(*Towns[nextTown]) == typeid(MarchingScreen))
	{
		Interface->BeginDrawing(false);
		Interface->SetUI(west);
	}
	else
	{
		Interface->BeginDrawing(true);
		Interface->SetUI(east);
	}

	townName = std::move(nextTown);
}

void Editor::SelectPortrait(const vector<Portrait>& Portrait)
{
	float portraitColums = std::ceil(Portrait.size() / 9.0f);

	positonButton.SetPosition(64 * portraitColums, 0);
	decreasePosition.SetPosition(positonButton.GetRect().x + positonButton.GetRect().width, positonButton.GetRect().y + 32);
	increasePosition.SetPosition(decreasePosition.GetRect().x + decreasePosition.GetRect().width, decreasePosition.GetRect().y);


	for (const auto& portrait : Portrait)
	{
		if (CheckCollisionPointRec(GetMousePosition(), portrait.GetRect()))
		{
			const auto& type = portrait.GetType();
			//cout << type << endl;
			//if (type == "dante") return;
			if (type == "0back") Back();
			else if(ShowUnits()||ShowNPCs()||ShowProps())
			{
				auto it = std::find(unitTypes->begin(), unitTypes->end(), type);
				if (it != unitTypes->end())
				{
					if (addingUnitsToCaptain)
						AddUnitToCaptain(type);
					else
						AddUnit(type, position);
				};
			}
			else if (ShowArmyCaptains())
			{
				auto it = std::find(unitTypes->begin(), unitTypes->end(), type);
				if (it != unitTypes->end())
				{
					AddEnemyArmyCaptain(type, position);
				};
			}
			else if (ShowTiles())
			{
				if (!townName.empty())
					Towns[townName]->SelectTiles(type);

			}
			else if (ShowBackgrounds())
			{
				if (!townName.empty())
					Towns[townName]->SetBackground(type);

			}
			else if (ShowBattlegrounds())
			{
				if (!townName.empty())
					Towns[townName]->SetBattleground(type);
			}
			/*else if (ShowPatrolingNPCs())
			{
				Towns[townName]->AddPatrolingUnit(type, position);
			}*/

		}
	}
}

void Editor::ClickPortrait()
{
	if (ShowUnits())
	{
		SelectPortrait(unitPortraits);
		
	}
	else if (ShowBackgrounds())
	{
		SelectPortrait(backgroundPortraits);
	}
	else if (ShowProps())
	{
		SelectPortrait(propsPortraits);
		
	}
	else if (ShowTiles())
	{
		SelectPortrait(tilesPortraits);
	}
	else if (ShowNPCs())
	{
		SelectPortrait(NPCPortraits);
		
	}
	else if (ShowArmyCaptains())
	{
		SelectPortrait(armyCaptainPortraits);
	}
	else if (ShowBattlegrounds())
	{
		SelectPortrait(battlegroundPortraits);
	}
	//else if (ShowPatrolingNPCs())
	//{
	//	SelectPortrait(patrolingNPCPortraits);
	//}

}

template <typename T>
void Editor::HandleAddButtons(Vector2 mouse, std::vector<std::string>& names, const std::string& category)
{
	if (addText_ && CheckCollisionPointRec(mouse, addButton.GetRect())) {
		if (addingNewTown)
			AddTown<T>(names, newText, category);
		else if (addingNextTown)
			Towns[townName]->SetNextTown(newText);
		addText_ = false;
		textBoxActive = false;
		newText.clear();
	}
	else if (addText_ && CheckCollisionPointRec(mouse, backButton.GetRect())) {
		addText_ = false;
		addingNewTown = false;
		addingNextTown = false;
		textBoxActive = false;
		newText.clear();
	}
	else if (addText_ && CheckCollisionPointRec(mouse, textRect)) {
		textBoxActive = true;
	}
}

void Editor::HandlePositionButtons(Vector2 mouse)
{
	if (CheckCollisionPointRec(mouse, increasePosition.GetRect())) position++;
	else if (CheckCollisionPointRec(mouse, decreasePosition.GetRect())) position--;
}

void Editor::MainMenuOption()
{
	const string& option = mainMenu.GetOption();
	if (option.size() > 0)
		playClickButtonSound = true;

	if (option == "play")
	{
		StartTrailer();
		UnloadIntroMedia();
	}
	else if (option == "editor")
	{
		showMainMenu_ = false;
		showMenuEditor_ = true;
		UnloadIntroMedia();
	}
	else if (option == "exit")
	{
		UnloadIntroMedia();
		exitGame_ = true;
	}
}

void Editor::MainMenuOptionEditor()
{
	const string& option = mainMenuEditor.GetOption();
	if (option.size() > 0)
		playClickButtonSound = true;

	if (option == "towns")
	{
		showMenuEditor_ = false;
		showTowns_ = true;
	}
	else if (option == "marching screens")
	{
		showMenuEditor_ = false;
		showMarchingScreens_ = true;
	}
	else if (option == "battle_mode")
	{
		showMenuEditor_ = false;
		battleMode_ = true;
		battleground->SetBattleMode(&battleMode_, [this]() {
			showMainMenu_ = true;
			intro = textureManager->IntroMedia();
			});
		LoadTextList(enemyArmyPath, "army_list", enemyArmy);
		LoadTextList(enemyArmyPath, "spell_list", enemyArmySpells);
		battleground->SetBattleground(*army, *armySpells, *enemyArmy, *enemyArmySpells);
	}
	else if (option == "backgrounds")
	{
		showMenuEditor_ = false;
		showBackgrounds_ = true;
	}
	else if (option == "battlegrounds")
	{
		showMenuEditor_ = false;
		showBattlegrounds_ = true;
		if (!townName.empty()) Towns[townName]->SetShowBattlegroundArena(true);
	}
	else if (option == "props")
	{
		showMenuEditor_ = false;
		showProps_ = true;
		position = 1;
	}
	else if (option == "tiles")
	{
		showMenuEditor_ = false;
		showTiles_ = true;
	}
	else if (option == "units")
	{
		showMenuEditor_ = false;
		//showBackgrounds_ = false;
		showUnits_ = true;
		position = 3;
	}
	else if (option == "NPCs")
	{
		showMenuEditor_ = false;
		//showBackgrounds_ = false;
		showNPCs_ = true;
		position = 2;
	}
	else if (option == "army_captains")
	{
		showMenuEditor_ = false;
		//showBackgrounds_ = false;
		showArmyCaptains_ = true;
		position = 3;
	}
	//else if (option == "Patroling NPCs")
	//{
	//	showMenuEditor_ = false;
	//	//showBackgrounds_ = false;
	//	showPatrolingNPCs_ = true;
	//	position = 2;
	//}
	else if (option == "back")
	{
		intro = textureManager->IntroMedia();
		showMenuEditor_ = false;
		showMainMenu_ = true;
		playMarchingTheme = false;

	}
}

void Editor::TownsMenuOption(Vector2 mouse)
{

	string option = townsMenu.GetOption();
	bool townExits = std::find(townNames.begin(), townNames.end(), option) != townNames.end();
	if (option.size() > 0)
		playClickButtonSound = true;

	if (option == "back")
	{
		addText_ = false;
		showMenuEditor_ = true;
		showTowns_ = false;
		addingNextTown = false;
		addingNewTown = false;
	}
	else if (option == "add_next_town")
	{
		addText_ = true;
		addingNextTown = true;
		textRect = {
			townsMenu.GetCell(townNames.size() + 1).x,
			townsMenu.GetCell(townNames.size() + 1).y,
			20,
			townsMenu.GetCellHeight()
		};
		addButton.SetPosition(textRect.x, textRect.y + textRect.height);
		backButton.SetPosition(addButton.GetRect().x + addButton.GetRect().width, addButton.GetRect().y);
		addButton.SetRectColor(BLUE);
		backButton.SetRectColor(BLUE);
	}
	else if (option == "+" && !addingNextTown)
	{
		addText_ = true;
		addingNewTown = true;
		textRect = {
			townsMenu.GetCell(townNames.size() + 2).x,
			townsMenu.GetCell(townNames.size() + 2).y,
			20,
			townsMenu.GetCellHeight()
		};
		addButton.SetPosition(textRect.x, textRect.y + textRect.height);
		backButton.SetPosition(addButton.GetRect().x + addButton.GetRect().width, addButton.GetRect().y);
		addButton.SetRectColor(BLUE);
		backButton.SetRectColor(BLUE);
	}
	else if (townExits)
	{
		//marchingScreenName.clear();
		//townName = option;
		SwitchTowns(option);
	}

	HandlePositionButtons(mouse);
	HandleAddButtons<Town>(mouse, townNames, "towns");
}

void Editor::MarchingScreensOption(Vector2 mouse)
{
	string option = marchingScreensMenu.GetOption();
	if (option.size() > 0)
		playClickButtonSound = true;

	bool marchingScreenExists = std::find(marchingScreenNames.begin(), marchingScreenNames.end(), option) != marchingScreenNames.end();

	if (option == "back")
	{
		addText_ = false;
		showMenuEditor_ = true;
		showMarchingScreens_ = false;

		addingNextTown = false;
		addingNewTown = false;
		
	}
	else if (option == "add_next_town")
	{
		addText_ = true;
		addingNextTown = true;
		textRect = {
			marchingScreensMenu.GetCell(marchingScreenNames.size() + 1).x,
			marchingScreensMenu.GetCell(marchingScreenNames.size() + 1).y,
			20,
			marchingScreensMenu.GetCellHeight()
		};
		addButton.SetPosition(textRect.x, textRect.y + textRect.height);
		backButton.SetPosition(addButton.GetRect().x + addButton.GetRect().width, addButton.GetRect().y);
		addButton.SetRectColor(BLUE);
		backButton.SetRectColor(BLUE);
	}
	else if (option == "+" && !addingNextTown)
	{
		addText_ = true;
		addingNewTown = true;
		textRect = {
			marchingScreensMenu.GetCell(marchingScreenNames.size() + 2).x,
			marchingScreensMenu.GetCell(marchingScreenNames.size() + 2).y,
			20,
			marchingScreensMenu.GetCellHeight()
		};
		addButton.SetPosition(textRect.x, textRect.y + textRect.height);
		backButton.SetPosition(addButton.GetRect().x + addButton.GetRect().width, addButton.GetRect().y);
		addButton.SetRectColor(BLUE);
		backButton.SetRectColor(BLUE);
	}
	else if (marchingScreenExists)
	{
		/*townName.clear();
		marchingScreenName = option;*/
		//townName = option;
		SwitchTowns(option);
	}

	HandlePositionButtons(mouse);
	HandleAddButtons<MarchingScreen>(mouse, marchingScreenNames, "marching_screens");
}

void Editor::DrawMainMenu(float dt)
{
	if (ShowMainMenu())
	{
		//DrawAnimatedPortrait(mainMenuScreen,
		//	mainScreenSourceRect,
		//	mainScreenDestRect,
		//	mainScreenTotalFrames,
		//	mainScreenCol,
		//	dt,
		//	mainScreenFrameTimer,
		//	mainScreenFrameIndex
		//);
		if (intro && intro->videoTexture.id != 0)
		{
			// Simple full-screen draw (adjust as needed)
			DrawTexturePro(intro->videoTexture,
				{ 0, 0, (float)intro->videoTexture.width, (float)intro->videoTexture.height },
				{ 0, 0, (float)GetScreenWidth(), (float)GetScreenHeight() },
				{ 0, 0 }, 0.0f, WHITE);
		}
		else
		{
			cout << "ERROR" << endl;
		}
		mainMenu.DrawMenu();
	}
}

void Editor::DrawMenuEditor()
{
	
	if (ShowMenuEditor())
	{
		mainMenuEditor.DrawMenu();

	}
	
}

void Editor::DrawTownsMenu()
{
	if (ShowTowns())
	{
		
		positonButton.Display();
		increasePosition.Display();
		decreasePosition.Display();
		auto positionRect = Rectangle{ positonButton.GetRect().x + positonButton.GetRect().width,  positonButton.GetRect().y, 25,25 };
		DrawText(to_string(position).c_str(), positionRect.x + 20, positionRect.y + 10, FONT_SIZE, BLACK);

		if (addText_)
		{
			if(addingNewTown)
				townsMenu.DrawMenu(townNames.size() + 2);
			else if(addingNextTown)
				townsMenu.DrawMenu(townNames.size() + 1);

			DrawRectangleRec(textRect, textBoxActive? LIGHTGRAY:GRAY);
			addButton.Display();
			backButton.Display();
			if(textBoxActive)
				DrawText(newText.c_str(), textRect.x + 5, textRect.y + 10, 20, BLACK);
		}
		else
		{
			townsMenu.DrawMenu();
		}

	}
}

void Editor::DrawMarchingScreensMenu()
{
	if (ShowMarchingScreens())
	{
		if (addText_)
		{
			
			if (addingNewTown)
				marchingScreensMenu.DrawMenu(marchingScreenNames.size() + 2);
			else if (addingNextTown)
				marchingScreensMenu.DrawMenu(marchingScreenNames.size() + 1);

			DrawRectangleRec(textRect, textBoxActive ? LIGHTGRAY : GRAY);
			addButton.Display();
			backButton.Display();
			if (textBoxActive)
				DrawText(newText.c_str(), textRect.x + 5, textRect.y + 10, 20, BLACK);
		}
		else
		{
			marchingScreensMenu.DrawMenu();
		}


	}
}

void Editor::DrawTownName()
{
	if (townName.empty()) return;

	if(!townName.empty())
		DrawText(townName.c_str(), SCR_WIDTH / 2, 5, 30, BLACK);
	//else if (!marchingScreenName.empty())
	//	DrawText(marchingScreenName.c_str(), SCR_WIDTH / 2, 5, 30, BLACK);
}

void Editor::Back()
{
	showMenuEditor_ = true;
	if (ShowUnits()) showUnits_ = false;
	else if (ShowNPCs()) showNPCs_ = false;
	else if (ShowArmyCaptains()) showArmyCaptains_ = false;
	else if (ShowBackgrounds()) showBackgrounds_ = false;
	else if (ShowBattlegrounds())
	{
		showBattlegrounds_ = false;
		if (!townName.empty()) Towns[townName]->SetShowBattlegroundArena(false);
	}
	else if (ShowProps()) showProps_ = false;
	else if (ShowTiles()) showTiles_ = false;
	//else if (ShowPatrolingNPCs()) showPatrolingNPCs_ = false;
}

void Editor::KeyPressed(int key)
{
	if (townName.empty()) return;

	if (key == KEY_D)
	{
		auto danteStance = "walk";
		auto newSpeed = GLOBAL_SPEED;
		auto newScalingFactor = 1.0f;

		if (typeid(*Towns[townName]) == typeid(MarchingScreen)) {
			danteStance = "march";
			newSpeed = EDITING_SPEED;
			newScalingFactor = ARMY_SCALING_FACTOR;
		}

			Towns[townName]->SetScrollSpeed(EDITING_SPEED);
			Towns[townName]->DanteSetStance(danteStance, newScalingFactor);
			Towns[townName]->DanteMoveRight();
			Towns[townName]->ArmySetStance("walk", ARMY_SCALING_FACTOR);
			if (typeid(*Towns[townName]) == typeid(Town)) Towns[townName]->ArmyMoveRight();
	}
	if (key == KEY_A)
	{
		auto danteStance = "walk";
		auto newSpeed = GLOBAL_SPEED;
		auto newScalingFactor = 1.0f;
		
		if (typeid(*Towns[townName]) == typeid(MarchingScreen)) {
			danteStance = "march";
			newSpeed = EDITING_SPEED;
			newScalingFactor = ARMY_SCALING_FACTOR;
		}

		Towns[townName]->SetScrollSpeed(EDITING_SPEED);
		Towns[townName]->DanteSetStance(danteStance, newScalingFactor);
		if(ShowTowns()) Towns[townName]->DanteMoveLeft();
		Towns[townName]->ArmySetStance("walk", ARMY_SCALING_FACTOR);
	}
	if (key == KEY_ENTER)
	{
		//if(Towns[townName].stopMove = true)
		//	Towns[townName].stopMove = false;
	}
	if (key == KEY_SPACE)
	{
		cout << "KEY_SPACE pressed" << endl;
		if (auto ms = dynamic_cast<MarchingScreen*>(Towns[townName].get())) {
			ms->SpacePressed();
			playMarchingTheme = !playMarchingTheme;
			
		}
	}
		
}

void Editor::KeyReleased()
{
	if (townName.empty()) return;
	//if (ShowMarchingScreens()) return;

	if (IsKeyReleased(KEY_D) || IsKeyReleased(KEY_A))
	{
		/*auto newSpeed = GLOBAL_SPEED;
		auto danteStance = "idle";
		auto newScalingFactor = 1.0f;

		if (auto ms = dynamic_cast<MarchingScreen*>(Towns[townName].get())) 
		{
			newSpeed = MARCHING_SPEED;
			danteStance = "march_idle";
			newScalingFactor = DANTE_SCALING_FACTOR;
			if (ms->startMarching)
			{
				danteStance = "march";
				
				ms->ArmySetStance("walk", DANTE_SCALING_FACTOR);
				
			}
			else
			{
				danteStance = "march_idle";
				
				ms->ArmySetStance("idle", DANTE_SCALING_FACTOR);
				
			}

		}

		Towns[townName]->DanteSetStance(danteStance, newScalingFactor);
		Towns[townName]->SetScrollSpeed(newSpeed);*/

		auto* town = Towns[townName].get();

		town->ApplyArmyStance();
		town->DanteSetStance(town->GetDanteStance(), town->GetScalingFactor());
		town->SetScrollSpeed(town->GetScrollSpeed());
		
	}

	if (IsKeyReleased(KEY_SPACE))
	{
		
		
	}
}

void Editor::KeyDown(float dt)
{
	if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
	{
		if(battleMode_)
			battleground->LeftMouseButtonDown();
	}

	if (townName.empty())return;

	if (IsKeyDown(KEY_A))
	{
		if (townName.empty()) return;
		
		Towns[townName]->MoveLeft(dt);

	}
	else if (IsKeyDown(KEY_D))
	{
		if (townName.empty()) return;
		
		Towns[townName]->MoveRight(dt);

	}

	if (IsKeyDown(MOUSE_BUTTON_LEFT))
	{
		Towns[townName]->LeftMouseButtonDown();
	}
}

void Editor::MouseButtonReleased()
{
	// Left mousebutton
	// Drag each Entity
	if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
		Vector2 mouse = GetMousePosition();

		// Menu has top priority
		if (ShowMainMenu())
		{
			MainMenuOption();
		}
		else if (ShowMenuEditor())
		{
			MainMenuOptionEditor();

		}
		// Portraits come next
		else if (ShowUnits() || ShowNPCs() || ShowArmyCaptains() || ShowBackgrounds() || ShowBattlegrounds() || ShowProps() || ShowTiles()) {
			ClickPortrait();
			HandlePositionButtons(mouse);

		}
		else if (ShowTowns())
		{
			TownsMenuOption(mouse);

		}
		else if (ShowMarchingScreens()) {
			MarchingScreensOption(mouse);

		}
		DragUnits();
		ClickAssetOption();

		if (!townName.empty())
		{
			auto* town = Towns[townName].get();
			auto validNames = townNames;
			validNames.insert(validNames.end(), marchingScreenNames.begin(), marchingScreenNames.end());
			
			//// Travel to next town Button mechanics
			if (auto newName = town->HandleButtonClick(mouse, validNames)) {
				SwitchTowns(*newName); 
			}
				
					
		}

		if(battleMode_)
			battleground->LeftMouseButtonReleased();

		//if (!townName.empty())
		//{
		//	auto* town = Towns[townName].get();

		//	// IMPORTANT: consume click if we just entered patrol mode
		//	if (town->IsWaitingForPatrolClick())
		//		return;

		//	auto validNames = townNames;
		//	validNames.insert(validNames.end(),
		//		marchingScreenNames.begin(),
		//		marchingScreenNames.end());

		//	if (auto newName = town->HandleButtonClick(mouse, validNames))
		//	{
		//		SwitchTowns(*newName);
		//	}
		//}

	}

	// right mousebutton
	if (IsMouseButtonReleased(MOUSE_RIGHT_BUTTON))
	{
		AssetOptionsUnits();
	}
}

void Editor::MouseButtonPressed()
{
	if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
		battleground->LeftMouseButtonPressed();
}

bool Editor::ShowTrailer() const
{
	return showTrailer_;
}

bool Editor::ExitGame() const
{
	return exitGame_;
}

bool Editor::ShowMainMenu() const
{
	return showMainMenu_;
}

bool Editor::ShowMenuEditor() const
{
	return showMenuEditor_;
}

bool Editor::ShowMarchingScreens() const
{
	return showMarchingScreens_;
}

bool Editor::ShowTowns() const
{
	return showTowns_;
}

bool Editor::ShowProps() const
{
	return showProps_;
}

bool Editor::ShowTiles() const
{
	return showTiles_;
}

bool Editor::ShowUnits() const
{
	return showUnits_;
}

bool Editor::ShowNPCs() const
{
	return showNPCs_;
}

bool Editor::ShowArmyCaptains() const
{
	return showArmyCaptains_;
}

//bool Editor::ShowPatrolingNPCs() const
//{
//	return showPatrolingNPCs_;
//}

bool Editor::ShowBackgrounds() const
{
	return showBackgrounds_;
}

bool Editor::ShowBattlegrounds() const
{
	return showBattlegrounds_;
}

void Editor::PlaySoundTracks()
{
	//return;
	Music* nextMusic = nullptr;

	if (ShowMainMenu())
		nextMusic = &mainTheme;
	else if (playMarchingTheme)
	{
		nextMusic = &marchingTheme;
		// Ambient marching loop
		if (!IsSoundPlaying(marchingLoop))
			PlaySound(marchingLoop);
	}
	else if (!playMarchingTheme)
		StopSound(marchingLoop);

	// If we need to switch music
	if (nextMusic != currentMusic)
	{
		if (currentMusic)
			StopMusicStream(*currentMusic);   // stop old music
		if (nextMusic)
			PlayMusicStream(*nextMusic);      // start new music

		currentMusic = nextMusic;             // update state
	}

	// always update current music stream
	if (currentMusic)
		UpdateMusicStream(*currentMusic);

	if (playMarchingTheme)
	{
		// Ambient marching loop
		if (!IsSoundPlaying(marchingLoop))
			PlaySound(marchingLoop);
	}
	else
		StopSound(marchingLoop);

	if (playClickButtonSound)
	{
		PlaySound(clickButtonSound);
		playClickButtonSound = false;
	}
}

void Editor::StartTrailer()
{
	textureManager->TrailerMedia(trailerPlaylist);
	if (trailerPlaylist.empty()) return;
	trailerIndex = 0;
	SetMediaState(*trailerPlaylist[trailerIndex], MEDIA_STATE_PLAYING);
	showTrailer_ = true;
	showMainMenu_ = false;
}

void Editor::UpdateTrailer()
{
	if (!showTrailer_ || trailerPlaylist.empty()) return;

	MediaStream& media = *trailerPlaylist[trailerIndex];

	UpdateMedia(&media);

	MediaProperties props = GetMediaProperties(media);
	double position = GetMediaPosition(media);

	if (GetMediaState(media) == MEDIA_STATE_STOPPED)
	{

		trailerIndex++;

		if (trailerIndex < trailerPlaylist.size())
		{
			SetMediaState(*trailerPlaylist[trailerIndex], MEDIA_STATE_PLAYING);
		}
		else
		{
			for (int i{ 0 }; i < trailerPlaylist.size(); i++)
				UnloadMedia(trailerPlaylist[i].get());

			trailerPlaylist.clear();
			showTrailer_ = false;

			intro = textureManager->IntroMedia();

			showMainMenu_ = true;
			trailerIndex = 0;
		}
	}
}


void Editor::UnloadIntroMedia()
{
	if (intro) {
		UnloadMedia(intro.get());
		intro.reset();
	}
}