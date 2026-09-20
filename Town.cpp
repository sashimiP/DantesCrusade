#include "Town.h"

Town::Town(std::string townName, shared_ptr<TextureManager> textureManager, shared_ptr<UI> Interface, shared_ptr<Battleground> battleground) :
	townName(std::move(townName)),
	globalSpeed(GLOBAL_SPEED),
	textureManager(textureManager),
	Interface(Interface),
	battleground(battleground)
{
	coordinatesPath = fs::path("data") / "towns" / this->townName / "coordinates";
	danteDataPath = fs::path("data") / "towns" / this->townName / "dante";
	horseDataPath = danteDataPath / "horse";
	unitsDataPath = fs::path("data") / "towns" / this->townName / "units";
	captainsDataPath = fs::path("data") / "towns" / this->townName / "enemy_army_captains";
	backgroundDataPath = fs::path("data") / "towns" / this->townName / "background";
	battlegroundPath = fs::path("data") / "towns" / this->townName / "battleground";
	tilesDataPath = fs::path("data") / "towns" / this->townName / "tiles";
	specialEntitiesPath = fs::path("data") / "towns" / this->townName / "special_units";
	recruiterDataPath = fs::path("data") / "towns" / this->townName / "recruiter";
	//patrolingDataPath = fs::path("data") / "towns" / this->townName / "patrolingNPCs";
	armyDataPath = fs::path("data") / "army";
	army = make_shared<vector<string>>();
	scrollSpeed = EDITING_SPEED;
	Units.reserve(1000);
	LoadCoordinates();
	LoadDante();
	LoadDanteHorse();
	LoadEntity<Entity>(unitsDataPath);
	LoadEntity<EnemyArmyCaptain>(captainsDataPath);
	LoadSpecialEntities();
	LoadRecruiter();
	
	//LoadTiles();
	background = std::make_unique<TownBackground>(textureManager, globalSpeed, backgroundDataPath);
	battlegroundArena = std::make_shared<Background>(textureManager, globalSpeed, battlegroundPath);
	tiles = std::make_unique<Tiles>(0, 658, textureManager, globalSpeed, tilesDataPath);
	dialoguePlatform = std::make_unique<DialoguePlatform>(textureManager);
	//this->battleground = std::make_shared<Battleground>(textureManager);

	stats["blacksmith_master"] = &Dante::armor;
	stats["fencing_master"] = &Dante::attack;
	stats["bishop"] = &Dante::holiness;
	stats["food_merchant"] = &Dante::food;
	stats["gold"] = &Dante::gold;
	//fs::create_directories(captainsDataPath);
}

Town::~Town()
{
	SaveCoordinates();
}

void Town::SaveCoordinates()
{
	//fs::create_directories(coordinatesPath);
	fs::path tempPath = coordinatesPath / "coordinates.txt";
	std::fstream coordinates;

	coordinates.open(tempPath, std::ios::out);
	if (coordinates.is_open())
	{
		coordinates << nextTown << endl;
		coordinates.close();
	}
	else
	{
		cout << "Problem with file:" << endl;
		cout << tempPath << endl;
	}
}

void Town::LoadCoordinates()
{
	fs::path filePath = coordinatesPath / "coordinates.txt";

	if (!fs::exists(filePath)) {
		std::cout << "No coordinates file found at: " << filePath << std::endl;
		return;
	}

	std::ifstream dataFile(filePath);
	if (!dataFile.is_open()) {
		std::cout << "Failed to open file: " << filePath << std::endl;
		return;
	}

	std::string line;
	if (std::getline(dataFile, line)) {
		nextTown = std::move(line);
	}

}

template<typename T>
void Town::LoadEntity(fs::path datapath)
{
	if (!fs::exists(datapath)) return;

	for (const auto& entry : fs::directory_iterator(datapath))
	{
		vector<string> coordinates;
		vector<string> dialogue;

		Utilities::OpenFileAndLoadData(entry, "coordinates", coordinates);
		Utilities::OpenFileAndLoadData(entry, "dialogue", dialogue);

		if (auto parsed = Utilities::ParseEntityData(coordinates, dialogue))
		{
			Units.emplace_back(
				std::make_unique<T>(
					datapath,
					townName,
					textureManager,
					parsed->id,
					parsed->position,
					parsed->type,
					parsed->x,
					parsed->y,
					parsed->direction,
					parsed->drag,
					parsed->stance,
					parsed->dialogue,
					parsed->name,
					parsed->color,
					parsed->dontScale,
					parsed->patrolRoute
				)
			);
			Units.back()->SetEngage(engage);
		}
	}
}

void Town::LoadDante()
{
	if (!fs::exists(danteDataPath)) return;
	// Get the first directory entry directly
	fs::directory_iterator dir_iter(danteDataPath);
	if (dir_iter == fs::end(dir_iter)) {
		std::cerr << "No entries found in " << danteDataPath << std::endl;
		return;
	}
	const auto& entry = *dir_iter; // Access the first entry

	vector<string>coordinates;
	Utilities::OpenFileAndLoadData(entry, "coordinates", coordinates);
	vector<string>dialogue;
	Utilities::OpenFileAndLoadData(entry, "coordinates", dialogue);

	if (!textureManager) {
		std::cerr << "ERROR: textureManager is null!\n";
		return;
	}

	if (auto parsed = Utilities::ParseEntityData(coordinates, dialogue)) {
		//std::cout << "Loaded Entity from: " << entry.path() << "\n";
		dante = std::make_unique<Dante>(
			danteDataPath,
			townName,
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
			parsed->color);
	}
	else {
		std::cerr << "Skipping invalid entity in " << entry.path() << "\n";
	}
}

void Town::LoadDanteHorse()
{
	if (!fs::exists(horseDataPath)) return;
	//cout << "Dante Horse: " << horseDataPath << endl;
	// Get the first directory entry directly
	fs::directory_iterator dir_iter(horseDataPath);
	if (dir_iter == fs::end(dir_iter)) {
		std::cerr << "No entries found in " << horseDataPath << std::endl;
		return;
	}
	const auto& entry = *dir_iter; // Access the first entry

	vector<string>coordinates;
	vector<string> dialogue;
	Utilities::OpenFileAndLoadData(entry, "coordinates", coordinates);
	Utilities::OpenFileAndLoadData(entry, "dialogue", dialogue);

	if (!textureManager) {
		std::cerr << "ERROR: textureManager is null!\n";
		return;
	}

	if (auto parsed = Utilities::ParseEntityData(coordinates, dialogue)) {
		danteHorse = make_unique<DanteHorse>(nextTown,
			horseDataPath,
			townName,
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
			parsed->color);

		//std::cout << "Loaded NPC from: " << entry.path() << "\n";
	}
	else {
		std::cerr << "Skipping invalid entity in " << entry.path() << "\n";
	}
}


void Town::LoadSpecialEntities()
{
	if (!fs::exists(specialEntitiesPath)) { cout << specialEntitiesPath << " NOT FOUND" << endl; return; }

	for (const auto& entry : fs::directory_iterator(specialEntitiesPath))
	{
		std::vector<std::string> coordinates;
		std::vector<std::string> dialogue;

		Utilities::OpenFileAndLoadData(entry, "coordinates", coordinates);
		Utilities::OpenFileAndLoadData(entry, "dialogue", dialogue);

		if (!textureManager) {
			std::cerr << "ERROR: textureManager is null!\n";
			return;
		}
		
		if (auto parsed = Utilities::ParseEntityData(coordinates, dialogue)) {
			// Construct directly in the vector using emplace_back
			
			specialEntities.try_emplace(
				parsed->type,  // key
				specialEntitiesPath,
				townName,
				textureManager,
				parsed->id,
				parsed->position,
				parsed->type,
				parsed->x,
				parsed->y,
				parsed->direction,
				parsed->drag,
				parsed->stance,
				std::move(parsed->dialogue),
				parsed->name,
				parsed->color,
				parsed->dontScale,
				parsed->special,
				parsed->growStats,
				parsed->price
			);
			//std::cout << "Loaded NPC from: " << entry.path() << "\n";
		}
		else {
			std::cerr << "Skipping invalid entity in " << entry.path() << "\n";
		}
	}
}

void Town::LoadRecruiter()
{
	if (!fs::exists(recruiterDataPath)) return;
	//cout << "Dante Horse: " << horseDataPath << endl;
	// Get the first directory entry directly
	fs::directory_iterator dir_iter(recruiterDataPath);
	if (dir_iter == fs::end(dir_iter)) {
		std::cerr << "No entries found in " << recruiterDataPath << std::endl;
		return;
	}
	const auto& entry = *dir_iter; // Access the first entry

	vector<string>coordinates;
	vector<string> dialogue;
	vector<string> unitsForHire;
	Utilities::OpenFileAndLoadData(entry, "coordinates", coordinates);
	Utilities::OpenFileAndLoadData(entry, "dialogue", dialogue);
	Utilities::OpenFileAndLoadData(entry, "units_for_hire", unitsForHire);	

	if (!textureManager) {
		std::cerr << "ERROR: textureManager is null!\n";
		return;
	}

	if (auto parsed = Utilities::ParseEntityData(coordinates, dialogue)) {
		recruiter = make_unique<Recruiter>(unitsForHire,
			recruiterDataPath,
			townName,
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
			parsed->color);

		//std::cout << "Loaded NPC from: " << entry.path() << "\n";
	}
	else {
		std::cerr << "Skipping invalid entity in " << entry.path() << "\n";
	}
}

void Town::AddUnit(const std::string& type, int position)
{
	if (stopMove) return;
	if (type == "dante") {
		if (dante) dante->Remove(false);
		dante.reset(new Dante(danteDataPath,
			townName,
			textureManager,
			Utilities::randomFloat(0.0, 1.0),
			1, type, 0, 0, east, true, "idle"));
	}
	else if (type == "dante_horse")
	{
		if (danteHorse) danteHorse->Remove(false);
		danteHorse.reset(new DanteHorse(nextTown, horseDataPath,
			townName,
			textureManager,
			Utilities::randomFloat(0.0, 1.0),
			1, type, 0, 0, east, true, "idle"));
	}
	else
	{
		Units.emplace_back(
			std::make_unique<Entity>(
				unitsDataPath,
				townName,
				textureManager,
				Utilities::randomFloat(0.0f, 1.0f),
				position,
				type,
				0.0f,
				0.0f,
				Utilities::Direction::east,
				true,
				"idle"
			)
		);
	}
}

void Town::AddEnemyArmyCaptain(const string& type, int position)
{
	if (stopMove) return;
		Units.emplace_back(
			std::make_unique<EnemyArmyCaptain>(
				captainsDataPath,
				townName,
				textureManager,
				Utilities::randomFloat(0.0f, 1.0f),
				position,
				type,
				0.0f,
				0.0f,
				Utilities::Direction::east,
				true,
				"idle"
			)
		);
}

void Town::AddUnitToArmyCaptain(string type)
{
	if(currentCaptain != nullptr) currentCaptain->AddUnit(type);
}

void Town::DragUnits()
{
	if (stopMove) return;
	if (!Units.empty())
	{
		for (auto& unit : Units)
			unit->Drag();
	}

	if(dante) dante->Drag();
	if (danteHorse) danteHorse->Drag();

	if (!specialEntities.empty())
	{
		for (auto& unit : specialEntities)
			unit.second.Drag();
	}

	if (recruiter) recruiter->Drag();
}

void Town::Update(float dt)
{
	background->Update(dt, scrollSpeed);
	tiles->Update(dt);

	for (auto& unit : Units)
	{
		unit->Update(offsetX, dt);
		if (Entity::stopMove)
			stopMove = true;
		else
			stopMove = false;
	}
	if (danteHorse) danteHorse->Update(offsetX, dt);
	if (dante) dante->Update(offsetX, dt);
	if (recruiter) recruiter->Update(offsetX, dt);
	for (auto& unit : specialEntities)
	{
		unit.second.Update(offsetX, dt);
		if (Entity::stopMove)
			stopMove = true;
		else
			stopMove = false;
	}
	dialoguePlatform->Update();
	
}

void Town::MoveRight(float dt)
{
	if (stopMove) return;
	background->MoveRight(dt, scrollSpeed);
	tiles->MoveRight(dt, scrollSpeed);
	if (!Units.empty())
		for (auto& unit : Units)
			unit->MoveRight(scrollSpeed, dt);



	if(danteHorse) danteHorse->MoveRight(scrollSpeed, dt);
	if (recruiter) recruiter->MoveRight(scrollSpeed, dt);
	if (!specialEntities.empty())
	{
		for (auto& unit : specialEntities)
			unit.second.MoveRight(scrollSpeed, dt);
	}

	offsetX += scrollSpeed * globalSpeed * dt;
}

void Town::MoveLeft(float dt)
{
	if (stopMove) return;
	background->MoveLeft(dt, scrollSpeed);
	tiles->MoveLeft(dt, scrollSpeed);
	if (!Units.empty())
		for (auto& unit : Units)
			unit->MoveLeft(scrollSpeed, dt);
	if(danteHorse) danteHorse->MoveLeft(scrollSpeed, dt);
	if (recruiter) recruiter->MoveLeft(scrollSpeed, dt);
	if (!specialEntities.empty())
	{
		for (auto& unit : specialEntities)
			unit.second.MoveLeft(scrollSpeed, dt);
	}

	offsetX -= scrollSpeed * globalSpeed * dt;
}

void Town::DanteIdle()
{
	if (stopMove) return;
	if (dante) dante->Idle();
}

void Town::DanteMoveRight()
{
	if (stopMove) return;
	if (dante) dante->MoveRight();
}

void Town::DanteMoveLeft()
{
	if (stopMove) return;
	if (dante) dante->MoveLeft();
}
void Town::ArmySetStance(const string& stance, float scalingFactor)
{

}

void Town::ArmyMoveRight()
{

}

void Town::ArmyMoveLeft()
{

}

void Town::DanteSetStance(const string& stance, float scalingFactor)
{
	if (stopMove) return;
	if (dante) dante->SetStance(stance);
	dante->ScaleDestRect(scalingFactor);
}

void Town::CollideRectUnitDante()
{
	if (Units.empty() || dante == nullptr) return;

	for (auto& unit : Units)
	{
		bool isColliding = CheckCollisionRecs(dante->Rect(), unit->Rect());

		if (isColliding)
		{
			if (!unit->HasDialogue()) continue;

			if (!unit->HasCollidedWithDante()) // Collision started
			{
				if (dante->Direction() == unit->Direction())
					unit->Flip();
				cout << "dante collide with " << unit->Type() << endl;
				unit->HasCollidedWithDante() = true;
				dante->HasCollidedWithUnit() = true;
				//dialoguePlatform->SetDialoguePlatform(unit->GetDialoguePortrait(), dante->GetDialoguePortrait(), unit->GetButtons());
				dialoguePlatform->SetDialoguePlatform(unit->GetMediaPortrait(), dante->GetMediaPortrait(), unit->GetButtons());
			}
			unit->ShowDialogue() = true;
			if (unit->DialogueCounter() > 0)
			{
				dante->EngagedInDialogue() = true;
				Interface->engagedInDialogue = true;

			}
			else
			{
				dante->EngagedInDialogue() = false;
				Interface->engagedInDialogue = false;
			}
		}
		else
		{
			unit->ShowDialogue() = false;
			if (unit->HasCollidedWithDante())
			{
				dialoguePlatform->ResetPortraits();
				unit->UnloadMediaPortrait();
				unit->HasCollidedWithDante() = false; // Reset when no longer colliding
				dante->HasCollidedWithUnit() = false;
				unit->DialogueCounter() = 0;
				Interface->engagedInDialogue = false;
				if(unit->Flipped()) unit->Flip();
				
			}
		}
	}
}

void Town::CollideRectSpecialUnitDante()
{
	if (specialEntities.empty() || dante == nullptr) return;
	
	for (auto& unit : specialEntities)
	{
		bool isColliding = CheckCollisionRecs(dante->Rect(), unit.second.Rect());

		if (isColliding)
		{
			if (!unit.second.HasDialogue()) continue;

			if (!unit.second.HasCollidedWithDante()) // Collision started
			{
				if (dante->Direction() == unit.second.Direction())
					unit.second.Flip();
				cout << "dante collide with " << unit.second.Type() << endl;
				cout << "dante collide with " << unit.first << endl;
				specialEntityColliding = unit.first;
				unit.second.HasCollidedWithDante() = true;
				dante->HasCollidedWithUnit() = true;
			}
			unit.second.ShowDialogue() = true;
			if (unit.second.DialogueCounter() > 0)
			{
				dante->EngagedInDialogue() = true;
				Interface->engagedInDialogue = true;
			}
			else
			{
				dante->EngagedInDialogue() = false;
				Interface->engagedInDialogue = false;
				
			}
		}
		else
		{
			unit.second.ShowDialogue() = false;
			if (unit.second.HasCollidedWithDante())
			{
				unit.second.HasCollidedWithDante() = false; // Reset when no longer colliding
				dante->HasCollidedWithUnit() = false;
				unit.second.DialogueCounter() = 0;
				specialEntityColliding.clear();
				Interface->engagedInDialogue = false;
				if (unit.second.Flipped()) unit.second.Flip();

			}
		}
	}

}

bool Town::CollideRectHorseDante()
{
	if (!danteHorse) return 0;

	danteHorse->HasCollidedWithDante() = CheckCollisionRecs(dante->CenterRect(), danteHorse->Rect());
	
	return danteHorse->HasCollidedWithDante();
}

bool Town::CollideRectRecruiterDante()
{
	if (!recruiter) return 0;

	if (dante == nullptr) return 0;


		bool isColliding = CheckCollisionRecs(dante->Rect(), recruiter->Rect());

		if (isColliding)
		{
			if (!recruiter->HasDialogue()) return false;
			if (!recruiter->HasCollidedWithDante()) // Collision started
			{
				if (dante->Direction() == recruiter->Direction())
					recruiter->Flip();
				cout << "dante collide with " << recruiter->Type() << endl;
			
				recruiter->HasCollidedWithDante() = true;
				dante->HasCollidedWithUnit() = true;
			}
			recruiter->ShowDialogue() = true;
			if (recruiter->DialogueCounter() > 0)
			{
				dante->EngagedInDialogue() = true;
				Interface->engagedInDialogue = true;
			}
			else
			{
				dante->EngagedInDialogue() = false;
				Interface->engagedInDialogue = false;

			}
		}
		else
		{
			recruiter->ShowDialogue() = false;
			if (recruiter->HasCollidedWithDante())
			{
				recruiter->HasCollidedWithDante() = false; // Reset when no longer colliding
				dante->HasCollidedWithUnit() = false;
				recruiter->DialogueCounter() = 0;
				
				Interface->engagedInDialogue = false;
				if (recruiter->Flipped()) recruiter->Flip();

			}
		}

	return recruiter->HasCollidedWithDante();
}

void Town::Collisions(vector<bool*>menuFlags)
{
	CollideRectUnitDante();
	CollideRectHorseDante();
	CollideRectSpecialUnitDante();
	CollideRectRecruiterDante();


	
	//if(dante->HasCollidedWithUnit())
	//	for (auto flag : menuFlags)
	//		*flag = false;
}

void Town::AssetOptionsUnits()
{
	for (auto& unit : Units)
		unit->ShowAssetOptions();

	if (dante) dante->ShowAssetOptions();

	if (danteHorse) danteHorse->ShowAssetOptions();


	for (auto& unit : specialEntities)
		unit.second.ShowAssetOptions();

	if (recruiter) recruiter->ShowAssetOptions();
	
}

//static void unitSortByPosition(vector<std::unique_ptr<Entity>>& vec) {
//	// Branch prediction will make this very efficient
//
//	std::sort(vec.begin(), vec.end(),
//		[](const Entity& a, const Entity& b) {
//			return a.Position() < b.Position();
//		});
//
//}

static void unitSortByPosition(vector<std::unique_ptr<Entity>>& vec)
{
	std::sort(vec.begin(), vec.end(),
		[](const std::unique_ptr<Entity>& a,
			const std::unique_ptr<Entity>& b)
		{
			return a->Position() < b->Position();
		});
}

std::optional<std::string> Town::ClickAssetOption()
{
	//if (Units.empty()) return;
	
	for (int i{}; i < Units.size(); ++i)
	{
		const auto& option = Units[i]->ClickAssetOption(offsetX);
		if (option == "remove")
		{
			if (entitySettingPatrol == Units[i].get())
				entitySettingPatrol = nullptr;

			Units.erase(Units.begin() + i);
			--i;
		}
		else if (option == "front")
			unitSortByPosition(Units);
		else if (option == "back")
			unitSortByPosition(Units);
		else if (option == "add dialogue")
			stopMove = true;
		else if (option == "setRoute")
		{
			entitySettingPatrol = Units[i].get();
			waitingForPatrolClick = true;
			cout << "CLICK 1" << endl;
			return option; // IMPORTANT: stop further processing this frame
		}
		else if (option == "add units")
		{
			currentCaptain = dynamic_cast<EnemyArmyCaptain*>(Units[i].get());
			return option;
		}
		else if (option == "stop add units")
		{
			currentCaptain = nullptr;
			return option;
		}
	}
	
	if (dante)
	{
		const auto& option = dante->ClickAssetOption(offsetX);
		if (option == "remove")
			dante.reset();
	}

	if (danteHorse)
	{
		const auto& option = danteHorse->ClickAssetOption(offsetX);
		if (option == "remove")
			danteHorse.reset();
	}

	
	for (auto it = specialEntities.begin(); it != specialEntities.end(); )
	{
		auto& entity = it->second; // the SpecialEntity (inherits from Entity)
		const auto& option = entity.ClickAssetOption(offsetX);

		if (option == "remove")
		{
			// erase returns iterator to next element
			it = specialEntities.erase(it);
		}
		else if (option == "add dialogue")
		{
			stopMove = true;
			++it;
		}
		else if (option == "add special")
		{
			stopMove = true;
			++it;
		}
		else if (option == "set price")
		{
			stopMove = true;
			++it;
		}
		else
		{
			++it; // default advance
		}
	}

	if (recruiter)
	{
		const auto& option = recruiter->ClickAssetOption(offsetX);
		if (option == "remove")
			recruiter.reset();
	}

	return std::nullopt;

}

void Town::SetBackground(const string& newBackground)
{
	background->LoadLayers(newBackground);
}

void Town::SetBattleground()
{
	battlegroundArena->LoadLayers(battlegroundArena->GetType());
}

void Town::SetBattleground(const string& newBattleground)
{
	battlegroundArena->LoadLayers(newBattleground);
}

void Town::DrawDialogue(float dt)
{
	for (auto& unit : Units)
	{
		if (unit->ShowDialogue() && unit->DialogueCounter()>0)
		{
			activateDialogue = true;
			bool hasAnimatedPortrait = unit->HasAnimatedPortrait();
			// if dialogue portrait isn't ready yet
			auto unitPortrait = (unit->GetDialoguePortrait()) ? unit->GetDialoguePortrait() : unit->GetPortrait();
			dialoguePlatform->DrawPlatform(unitPortrait, dante->GetDialoguePortrait(), unit->Name(), unit->Dialogue(), unit->DialogueCounter(), dt, hasAnimatedPortrait);
		}
		else if (activateDialogue && unit->DialogueCounter() <= 0)
		{
			activateDialogue = false;
		}
	}

	if (recruiter)
	{
		if (recruiter->ShowDialogue() && recruiter->DialogueCounter() > 0)
		{
			activateDialogue = true;
			// if dialogue portrait isn't ready yet
			auto unitPortrait = (recruiter->GetDialoguePortrait()) ? recruiter->GetDialoguePortrait() : recruiter->GetPortrait(); 
			dialoguePlatform->DrawPlatform(unitPortrait, dante->GetDialoguePortrait(), recruiter->Name(), recruiter->Dialogue(), recruiter->DialogueCounter(), dt);
		}
		else if (activateDialogue && recruiter->DialogueCounter() <= 0)
		{
			activateDialogue = false;
		}
	}

	for (auto& unit : specialEntities)
	{
		if (unit.second.ShowDialogue() && unit.second.DialogueCounter() > 0)
		{
			activateDialogue = true;
			// if dialogue portrait isn't ready yet
			auto unitPortrait = (unit.second.GetDialoguePortrait()) ? unit.second.GetDialoguePortrait() : unit.second.GetPortrait();

			dialoguePlatform->DrawPlatform(unitPortrait, dante->GetDialoguePortrait(), unit.second.Name(), unit.second.Dialogue(), unit.second.DialogueCounter(), dt);
		}
		else if (activateDialogue && unit.second.DialogueCounter() <= 0)
		{
			activateDialogue = false;
		}

	}
}

void Town::DrawUnits(float dt) {

	if (activateDialogue) return;
	for (auto& unit : Units)
	{
		unit->Display(ANIMATION_SPEED, offsetX, dt);
		unit->DrawDialogue(offsetX);
	}
	for (auto& unit : Units)
		unit->DrawAssetOptions(offsetX);

	if (danteHorse)
	{
		danteHorse->Display(ANIMATION_SPEED, offsetX, dt);
		danteHorse->DrawButton();
	}

	if (recruiter)
	{
		recruiter->Display(ANIMATION_SPEED, offsetX, dt);

		recruiter->DrawDialogue(offsetX);
		recruiter->DrawAssetOptions();
	}

	for (auto& unit : specialEntities)
	{
		unit.second.Display(ANIMATION_SPEED, offsetX, dt);
		unit.second.DrawButton();
		unit.second.DrawDialogue(offsetX);

	}
	for (auto& unit : specialEntities)
		unit.second.DrawAssetOptions();
	if (dante)dante->Display(ANIMATION_SPEED, offsetX, dt);

	//dialoguePlatform->DrawPlatform(Units[6].Portrait(), dante->Portrait(), Units[6].Name(), Units[6].Dialogue(), Units[6].DialogueCounter());
}

//void Town::DrawShadows(float dt)
//{
//	for (auto& unit : Units)
//	{
//		unit->DrawShadow(ANIMATION_SPEED, offsetX, dt);
//	}
//	for (auto& unit : Units)
//		unit->DrawAssetOptions(offsetX);
//
//	if (danteHorse)
//	{
//		danteHorse->Display(ANIMATION_SPEED, offsetX, dt);
//		danteHorse->DrawButton();
//	}
//
//	if (recruiter)
//	{
//		recruiter->Display(ANIMATION_SPEED, offsetX, dt);
//
//		recruiter->DrawDialogue(offsetX);
//		recruiter->DrawAssetOptions();
//	}
//
//	for (auto& unit : specialEntities)
//	{
//		unit.second.Display(ANIMATION_SPEED, offsetX, dt);
//		unit.second.DrawButton();
//		unit.second.DrawDialogue(offsetX);
//
//	}
//	for (auto& unit : specialEntities)
//		unit.second.DrawAssetOptions();
//	if (dante)dante->Display(ANIMATION_SPEED, offsetX, dt);
//}

//void Town::DrawUnits(float dt) {
//
//	for (auto& unit : Units)
//	{
//		unit.Display(ANIMATION_SPEED, offsetX, dt);
//		unit.DrawDialogue(offsetX);
//		if (unit.ShowDialogue())
//		{
//			auto unitPortrait = (unit.GetDialoguePortrait()) ? unit.GetDialoguePortrait() : unit.GetPortrait();
//			dialoguePlatform->DrawPlatform(unitPortrait, dante->GetDialoguePortrait(), unit.Name(), unit.Dialogue(), unit.DialogueCounter());
//		}
//	}
//	for (auto& unit : Units)
//		unit.DrawAssetOptions();
//	
//	if (danteHorse)
//	{
//		danteHorse->Display(ANIMATION_SPEED, offsetX, dt);
//		danteHorse->DrawButton();
//	}
//
//	if (recruiter)
//	{
//		recruiter->Display(ANIMATION_SPEED, offsetX, dt);
//		
//		recruiter->DrawDialogue(offsetX);
//		if (recruiter->ShowDialogue())
//		{
//			auto unitPortrait = (recruiter->GetDialoguePortrait()) ? recruiter->GetDialoguePortrait() : recruiter->GetPortrait(); // if dialogue portrat
//			dialoguePlatform->DrawPlatform(unitPortrait, dante->GetDialoguePortrait(), recruiter->Name(), recruiter->Dialogue(), recruiter->DialogueCounter());
//		}
//		recruiter->DrawAssetOptions();
//	}
//
//	for (auto& unit : specialEntities)
//	{
//		unit.second.Display(ANIMATION_SPEED, offsetX, dt);
//		unit.second.DrawButton();
//		unit.second.DrawDialogue(offsetX);
//		if (unit.second.ShowDialogue())
//		{
//			auto unitPortrait = (unit.second.GetDialoguePortrait()) ? unit.second.GetDialoguePortrait() : unit.second.GetPortrait();
//
//			dialoguePlatform->DrawPlatform(unitPortrait, dante->GetDialoguePortrait(), unit.second.Name(), unit.second.Dialogue(), unit.second.DialogueCounter());
//		}
//
//	}
//	for (auto& unit : specialEntities)
//		unit.second.DrawAssetOptions();
//	if (dante)dante->Display(ANIMATION_SPEED, offsetX, dt);
//	
//	//dialoguePlatform->DrawPlatform(Units[6].Portrait(), dante->Portrait(), Units[6].Name(), Units[6].Dialogue(), Units[6].DialogueCounter());
//};

void Town::DrawBackground(float dt)
{
	background->Display();
	string text = "x: " + std::to_string(offsetX);
	DrawText(text.c_str(), (SCR_WIDTH / 2), 50, 30, BLACK);
}

void Town::ShowBattleground()
{
	if (!showBattlegroundArena) return;
	
	battlegroundArena->Display();
}

std::optional<std::string> Town::HandleButtonClick(Vector2 mouse, vector<string>validTownNames)
{
	for (auto& unit : Units)
		unit->UpdateDialogue();
	
	if (entitySettingPatrol)
	{
		clickCount++;
		if (clickCount > 1)
		{
			cout << "Setting Patrol: " << entitySettingPatrol->Type() << "X: " << mouse.x << "Y: " << mouse.y << endl;
			entitySettingPatrol->SetPatrolDestination(mouse, offsetX);
			entitySettingPatrol = nullptr;
			waitingForPatrolClick = false;
			clickCount = 0;
			//return std::nullopt; // consume click
		}
	}
	
	if (CollideRectHorseDante())
	{
		bool buttonClicked = CheckCollisionPointRec(mouse, BeginMarchButtonRect());

		if (buttonClicked)
		{
			cout << "BUTTON CLICKED" << endl;
			string nextTownMarchScreen = townName + "-" + nextTown;


			bool marchScreenReady = std::find(validTownNames.begin(), validTownNames.end(), nextTownMarchScreen) != validTownNames.end();

			if (marchScreenReady) {
				return nextTownMarchScreen;
			}
		}
	}

	if (!specialEntityColliding.empty())
	{
		string specialEntityType = specialEntityColliding;
		SpecialEntity& special = specialEntities[specialEntityType];
		bool yesButtonClicked = CheckCollisionPointRec(mouse, special.YesButtonRect());
		bool backButtonClicked = CheckCollisionPointRec(mouse, special.BackButtonRect());
		if (yesButtonClicked) {
			std::string goldStr = "gold";
			cout << special.YesButton(*stats[specialEntityType], *stats[goldStr]) << endl;
			//cout << *stats[specialEntityType] << endl;
			//cout << *stats[goldStr] << endl;
			//Interface->SetStats(specialEntityType, *stats[specialEntityType]);
            
            //Interface->SetStats(goldStr, *stats[goldStr]);
		}
		else if(backButtonClicked) cout << special.BackButton() << endl;

		special.UpdateDialogue();
	}

	if (recruiter)
	{
		auto unitsForHire = recruiter->GetUnitsForHire();

		for (auto& unit : *unitsForHire)
		{
			bool clicked = CheckCollisionPointRec(mouse, unit.GetRect());
			if (clicked)
			{
				int& price = Utilities::unitForHirePrice[unit.GetType()];
				if (Dante::gold >= price)
				{
					Interface->AddArmyType(unit.GetType());
					Dante::gold -= price;
					army->emplace_back(unit.GetType());
				}
			}
		}
		recruiter->UpdateDialogue();
	}
	

	return std::nullopt;
}

void Town::LeftMouseButtonReleased(Vector2 mouse, vector<string>validTownNames)
{
	HandleButtonClick(mouse, validTownNames);

}

void Town::LeftMouseButtonDown()
{
	battleground->LeftMouseButtonDown();
}

void Town::RightMouseButtonReleased(Vector2 mouse)
{

}

void Town::SelectTiles(const string& newTiles)
{
	tiles->SetTexture(newTiles);
}

void Town::DrawTiles()
{
	tiles->Display();
}

void Town::DrawBattleground(float dt)
{
	if (!engage) return;
	battleground->DrawBackground();
	battleground->DrawGrid();
	battleground->DrawShadows(dt);
	battleground->DrawUI();
	battleground->DrawArmies(dt);
	battleground->DrawAttackCursor();


}

void Town::SetScrollSpeed(int speed)
{
	scrollSpeed = speed;
	cout << "SCROLL SPEED IS NOW: " << scrollSpeed<<endl;
}

Rectangle Town::BeginMarchButtonRect()
{
	return danteHorse->ButtonRect();
}

string Town::NextTown()
{
	return nextTown;
}

float Town::GetScrollSpeed() const
{
	return GLOBAL_SPEED;
}

float Town::GetScalingFactor() const
{
	return 1.0f;
}

std::string Town::GetDanteStance() const 
{ 
	return "idle";
}

bool Town::Engage() const
{
	return engage;
}

string Town::GetNextTown()
{
	return nextTown;
}

//unordered_map <int, string> Town::GetArmy()
//{
//	unordered_map <int, string> temp;
//	temp.reserve(army->size());
//	cout << "ARMY SIZE IF: " << army->size() << endl;
//	int i{};
//	for (auto& unit : *army)
//	{
//		temp.insert({ i++, unit.Type()});
//	}
//	cout << "TEMP SIZE IS: " << temp.size() << endl;
//	return temp;
//}

vector<string>& Town::GetArmy()
{
	return *army;
}

bool Town::ShowBattlegroundArenaFlag() const
{
	return showBattlegroundArena;
}

//bool Town::IsWaitingForPatrolClick() const
//{
//	return waitingForPatrolClick;
//}

void Town::SetNextTown(string townName)
{
	nextTown = std::move(townName);
	danteHorse->SetNextTown(nextTown);
}

void Town::SetArmy(shared_ptr<vector<string>> army)
{
	this->army = army;
}

void Town::SetMarchingArmy(const shared_ptr<vector<MarchingEntity>>& marchingArmy)
{
	this->marchingArmy = marchingArmy;
}

void Town::SetShowBattlegroundArena(bool flag)
{
	showBattlegroundArena = flag;
}