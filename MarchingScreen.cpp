#include "MarchingScreen.h"

MarchingScreen::MarchingScreen(string townName, shared_ptr<TextureManager> textureManager, shared_ptr<UI> Interface, shared_ptr<Battleground> battleground)
{
	this->townName = (std::move(townName));
	this->globalSpeed = (GLOBAL_SPEED);
	this->textureManager = (textureManager);
	this->Interface = Interface;
	this->battleground = battleground;

	army = make_shared<vector<string>>();

	coordinatesPath = fs::path("data") / "marching_screens" / this->townName / "coordinates";
	danteDataPath = fs::path("data") / "dante_marching_screen";
	unitsDataPath = fs::path("data") / "marching_screens" / this->townName / "units";
	captainsDataPath = fs::path("data") / "marching_screens" / this->townName / "enemy_army_captains";
	backgroundDataPath = fs::path("data") / "marching_screens" / this->townName / "background";
	battlegroundPath = fs::path("data") / "marching_screens" / this->townName / "battleground";
	tilesDataPath = fs::path("data") / "marching_screens" / this->townName / "tiles";
	gateKeeperPath = fs::path("data") / "marching_screens" / this->townName / "special_units"/"gate_keeper";

	scrollSpeed = MARCHING_SPEED;
	Units.reserve(1000);
	LoadCoordinates();
	LoadDante();
	//LoadEntity(unitsDataPath, Units);
	LoadEntity<Entity>(unitsDataPath);
	LoadEntity<EnemyArmyCaptain>(captainsDataPath);
	//LoadEntity<PatrolingEntity>(patrolingDataPath);
	
	
	LoadGateKeeper();
	
	tiles = std::make_unique<Tiles>(0, 658, textureManager, globalSpeed, tilesDataPath);
	dialoguePlatform = std::make_unique<DialoguePlatform>(textureManager);

	background = std::make_unique<Background>(textureManager, globalSpeed, backgroundDataPath);
	battlegroundArena = std::make_shared<Background>(textureManager, globalSpeed, battlegroundPath);
	//marchingArmyDataPath = fs::path("data") / "marching_screen_marchingArmy";
	//LoadEntity(marchingArmyDataPath, marchingArmy);

	//PrepArmyForMarching();
	ScaleUnits(Units, SCALING_FACTOR);
	//ScaleUnits(*marchingArmy, ARMY_SCALING_FACTOR);
	if(gateKeeper) gateKeeper->ScaleDestRect(SCALING_FACTOR);

	/*if (dante)
	{
		RearrangeArmy(dante->X() - 80, dante->Y() + 40);
	}*/
	dante->SetY(tiles->GetY() - 50);

	// Load the background soundtrack
	//soundtrack = LoadMusicStream("E:\\Projects\\raylib\\youtube\\data\\rome-paris\\sound\\dream_of_albion.mp3"); // Replace with your music file path
	//SetMusicVolume(soundtrack, 0.5f); // Optional: Set volume (0.0f to 1.0f)
	
	
	//fs::create_directories(captainsDataPath);
	

	//a.insert(pair<string, float>("witch", 1.f));
	//b.insert(pair<string, float>("witch", 1.f));

	//this->battleground->SetBattleground(a, b);
}

MarchingScreen::~MarchingScreen()
{
	
}

void MarchingScreen::AddUnit(const std::string& type, int position)
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
	else if (type == "gate_keeper")
	{
		if (gateKeeper) gateKeeper->Remove(false);
		gateKeeper.reset(new GateKeeper(nextTown, gateKeeperPath,
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

void MarchingScreen::LoadGateKeeper()
{
	if (!fs::exists(gateKeeperPath)) return;
	//cout << "Gate Keeper: " << gateKeeperPath << endl;
	// Get the first directory entry directly
	fs::directory_iterator dir_iter(gateKeeperPath);
	if (dir_iter == fs::end(dir_iter)) {
		std::cerr << "No entries found in " << gateKeeperPath << std::endl;
		return;
	}
	//cout << dir_iter->path() << endl;
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
		gateKeeper = std::make_unique<GateKeeper>(
			nextTown,
			gateKeeperPath,
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
//
//void MarchingScreen::LoadArmy()
//{
//	if (!fs::exists(marchingArmyDataPath)) return;
//	for (const auto& entry : fs::directory_iterator(marchingArmyDataPath))
//	{
//		vector<string>coordinates;
//		cout <<"ENTRY PATH" << entry.path() << endl;
//		
//		OpenFileAndLoadData(entry, "coordinates", coordinates);
//		
//
//		if (!textureManager) {
//			std::cerr << "ERROR: textureManager is null!\n";
//			return;
//		}
//
//		std::cout << "Loaded data for NPC from: " << entry.path() << std::endl;
//		for (size_t i = 0; i < coordinates.size(); ++i) {
//			std::cout << "  data[" << i << "]: " << coordinates[i] << std::endl;
//		}
//
//		string& id = coordinates[0];
//		string& position = coordinates[1];
//		string& type = coordinates[4];
//		string& x = coordinates[2];
//		string& y = coordinates[3];
//		string& direction = coordinates[5];
//		string& stance = coordinates[6];
//
//		marchingArmy.emplace_back(marchingArmyDataPath,
//			townName,
//			textureManager,
//			stof(id), // id
//			stoi(position), // position
//			type, // type  = data[2]
//			static_cast<float>(std::stol(x)),// x = data[0]
//			static_cast<float>(std::stol(y)),// y = data[1]
//			static_cast<Utilities::Direction>(std::stoi(direction)), // flip = data[3]
//			false, // drag
//			stance // stance
//
//		);
//	}
//}

void MarchingScreen::SetBackground(const string& newBackground)
{
	background->LoadLayers(newBackground);
}

void MarchingScreen::Update(float dt)
{
	//cout <<"SCROLL SPEED: " << scrollSpeed << endl;
	dialoguePlatform->Update();
	if (!Units.empty())
		for (auto& unit : Units)
		{
			unit->Update(offsetX, dt);
			stopMove = Entity::stopMove;
			
		}
	//std::cout << "Updating MarchingScreen for " << townName << " with dt: " << dt << std::endl;
	if (gateKeeper) gateKeeper->Update(offsetX, dt);
	if (background) {
		background->Update(dt, scrollSpeed);
	}
	if (!startMarching) return;
	if (moveRight)
	{
		tiles->MoveRight(dt, scrollSpeed);
		if(dante) dante->MoveRight();
		background->MoveRight(dt, scrollSpeed);

		if (dante) dante->Update(offsetX, dt);

		offsetX += scrollSpeed * globalSpeed * dt;

		for (auto& unit : Units)
			unit->MoveRight(scrollSpeed, dt);

		if (gateKeeper) gateKeeper->MoveRight(scrollSpeed, dt);
	}
	else if(!moveRight)
	{
		tiles->MoveLeft(dt, scrollSpeed);
		if (dante) dante->MoveLeft();
		background->MoveLeft(dt, scrollSpeed);

		if (dante) dante->Update(offsetX, dt);
		
		offsetX -= scrollSpeed * globalSpeed * dt;

		for (auto& unit : Units)
			unit->MoveLeft(scrollSpeed, dt);

		if (gateKeeper) gateKeeper->MoveLeft(scrollSpeed, dt);
	}
	//if (engage)
	//	battleground->Update();
	// Update the music stream to keep it playing
	//UpdateMusicStream(soundtrack);
}

void MarchingScreen::DragUnits()
{
	if (stopMove) {
		return;
	}

	for (auto& unit : Units) {
		unit->Drag();
	}

	if (dante) {
		dante->Drag();
	}

	if (gateKeeper) gateKeeper->Drag();
}

void MarchingScreen::MoveRight(float dt)
{
	if (stopMove) return;
	moveRight =  true;
	background->MoveRight(dt, scrollSpeed);
	tiles->MoveRight(dt, scrollSpeed);

	if (!Units.empty())
		for (auto& unit : Units)
			unit->MoveRight(scrollSpeed, dt);
	if (gateKeeper) gateKeeper->MoveRight(scrollSpeed, dt);
	offsetX += scrollSpeed * globalSpeed * dt;
}

void MarchingScreen::MoveLeft(float dt)
{
	if (stopMove) return;
	moveRight =  false ;
	
	background->MoveLeft(dt, scrollSpeed);
	tiles->MoveLeft(dt, scrollSpeed);

	if (!Units.empty())
		for (auto& unit : Units)
			unit->MoveLeft(scrollSpeed, dt);
	if (gateKeeper) gateKeeper->MoveLeft(scrollSpeed, dt);
	offsetX -= scrollSpeed * globalSpeed * dt;
}

void MarchingScreen::DrawBackground(float)
{
	background->Display();
	string text = "x: " + std::to_string(offsetX);
	DrawText(text.c_str(), (SCR_WIDTH / 2), 50, 30, BLACK);
}


void MarchingScreen::DrawArmy(float dt)
{
	/*if (!startMarching) return;*/

	//return;

	if (!marchingArmy->empty())
	{
		for (auto& unit : *marchingArmy)
		{
			unit.Display(ANIMATION_SPEED, offsetX, dt);
			unit.DrawDialogue(offsetX);
		}
	}
	if (dante)dante->Display(ANIMATION_SPEED, offsetX, dt);
}

void MarchingScreen::DrawUnits(float dt) {

	
	if (!Units.empty())
	{
		for (auto& unit : Units)
		{
			unit->Display(ANIMATION_SPEED, offsetX, dt);
			unit->DrawDialogue(offsetX);
		}
		for(auto& unit : Units)
			unit->DrawAssetOptions();
	}
	if (gateKeeper)
	{
		gateKeeper->Display(ANIMATION_SPEED, offsetX, dt);
		if (gateKeeper->HasCollidedWithDante())
			gateKeeper->DrawButton();
		gateKeeper->DrawAssetOptions();
	}
	DrawArmy(dt);
	
};

//void MarchingScreen::ScaleUnits(vector<MarchingEntity>& units, float scalingFactor)
//{
//	if (units.empty()) return;
//	for (auto& unit : units)
//	{
//		if (unit.dontScale == false)
//		{
//			unit.ScaleDestRect(scalingFactor);
//			unit.scaled = true;
//		}
//	}
//}

void MarchingScreen::ScaleUnits(
	const std::vector<std::unique_ptr<Entity>>& units,
	float scalingFactor)
{
	for (const auto& unit : units)
	{
		if (!unit->dontScale)
		{
			unit->ScaleDestRect(scalingFactor);
		}
	}
}

void MarchingScreen::ScaleUnits(
	std::vector<MarchingEntity>& units,
	float scalingFactor)
{
	for (auto& unit : units)
	{
		if (!unit.dontScale)
		{
			unit.ScaleDestRect(scalingFactor);
		}
	}
}

//void MarchingScreen::RearrangeArmy(int leadingX, int leadingY, int first)
//{
//	if (marchingArmy->empty() || first >= marchingArmy->size()) return; // Safer base case
//	int adjust = 0;
//	if ((*marchingArmy)[first].Type() == "white_knight") adjust += 40; // Scale adjustment
//	if ((*marchingArmy)[first].Type() == "archer") adjust += 10;
//	Rectangle destRect = (*marchingArmy)[first].DestRect();
//	(*marchingArmy)[first].SetX(leadingX); // Align top-left with leadingX
//	(*marchingArmy)[first].SetY(leadingY - adjust);
//	// Log for debugging
//	//cout << (*marchingArmy)[first].Type()<<" ";
//	//TraceLog(LOG_INFO, "Unit %d at (%f, %f), width: %f", first, (*marchingArmy)[first].X(), (*marchingArmy)[first].Y(), destRect.width);
//	// Space the next unit (e.g., to the right)
//	int nextX = leadingX - destRect.width/3;
//	int nextY = leadingY;
//	RearrangeArmy(nextX, nextY, first + 1);
//}

void MarchingScreen::RearrangeArmy(int leadingX, int leadingY, int first)
{
	if (marchingArmy->empty() || first >= marchingArmy->size()) return; // Safer base case
	Rectangle destRect = (*marchingArmy)[first].DestRect();

	Rectangle tempRect = dante->DestRect();

	tempRect.x = leadingX;

	(*marchingArmy)[first].Adjust(tempRect);
	// Log for debugging
	//cout << (*marchingArmy)[first].Type()<<" ";
	//TraceLog(LOG_INFO, "Unit %d at (%f, %f), width: %f", first, (*marchingArmy)[first].X(), (*marchingArmy)[first].Y(), destRect.width);
	// Space the next unit (e.g., to the right)
	int nextX = leadingX - destRect.width / 3;
	int nextY = leadingY;
	RearrangeArmy(nextX, nextY, first + 1);
}

void MarchingScreen::SetStance(string stance)
{
	if (marchingArmy->empty()) return;
	for (auto& unit : *marchingArmy)
	{
		unit.SetStance(stance, offsetX);
		unit.ScaleDestRect(ARMY_SCALING_FACTOR);
	}
}


void MarchingScreen::PrepArmyForMarching()
{
	if (!dante) return;
	dante->marching = true;
	dante->SetStance("march_idle");
	dante->ScaleDestRect(ARMY_SCALING_FACTOR);

	if (marchingArmy->empty())
		cout << "MARCHING ARMY IS EMPTY" << endl;
	for (auto& unit : *marchingArmy)
	{
		unit.marching = true;
		unit.SetStance("idle", offsetX);
		unit.SetScrollSpeed(scrollSpeed);
	}
}

void MarchingScreen::PrepEntitiesForMarching()
{
	if (Units.empty()) return;
	for (auto& unit : Units)
	{
		unit->SetScrollSpeed(scrollSpeed);
		unit->SetStance("idle", offsetX);
	}
}

void MarchingScreen::SpacePressed()
{
	if (stopMove) return;
	startMarching = !startMarching;
	Idle();
	
}

void MarchingScreen::ArmySetStance(const string& stance, float scalingFactor)
{
	for (auto& unit : *marchingArmy)
	{
		unit.SetStance(stance, scalingFactor);
		unit.ScaleDestRect(scalingFactor);
	}
}

void MarchingScreen::ArmyMoveRight()
{
	for (auto& unit : *marchingArmy)
		unit.MoveRight();
}

void MarchingScreen::ArmyMoveLeft()
{
	for (auto& unit : *marchingArmy)
		unit.MoveRight();
}

float MarchingScreen::GetScrollSpeed() const  { return MARCHING_SPEED; }
float MarchingScreen::GetScalingFactor() const  { return ARMY_SCALING_FACTOR; }

std::string MarchingScreen::GetDanteStance() const  {
	return startMarching ? "march" : "march_idle";
}

void MarchingScreen::ApplyArmyStance()  {
	if (startMarching) {
		ArmySetStance("walk", ARMY_SCALING_FACTOR);
	}
	else {
		ArmySetStance("idle", ARMY_SCALING_FACTOR);
	}
}

void MarchingScreen::Idle()
{
	auto stance = (startMarching) ? "walk" : "idle";
	auto danteStance = (startMarching) ? "march" : "march_idle";
	SetStance(stance);
	dante->SetStance(danteStance);
	dante->ScaleDestRect(ARMY_SCALING_FACTOR);
	//ScaleUnits(marchingArmy, DANTE_SCALING_FACTOR);
	RearrangeArmy(dante->X() - 80, dante->Y() + dante->Rect().height);
}

void MarchingScreen::CollideRectUnitDante()
{
	if (Units.empty() || dante == nullptr) return;

	for (auto& unit : Units)
	{
		bool isColliding = CheckCollisionRecs(dante->Rect(), unit->Rect());

		if (isColliding)
		{
			if (!unit->HasDialogue()) continue;
			if (startMarching)
			{
				startMarching = false;
				Idle();
			}
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
				if (engage && battleGroundSet == false)
				{	
					if (auto* captain = dynamic_cast<EnemyArmyCaptain*>(unit.get())) {

						/*unordered_map<int, string> a;
						unordered_map<int, string> b;
						a.insert(pair<int, string>(1, "witch"));
						b.insert(pair<int, string>(1, "witch"));*/
						battleground->SetBackground(battlegroundArena->GetType());
						battleground->SetBattleground(GetArmy(), captain->GetArmy());
						battleGroundSet = true;
						cout << "Set Battleground" << endl;
					}
				}
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
				unit->HasCollidedWithDante() = false; // Reset when no longer colliding
				dante->HasCollidedWithUnit() = false;
				unit->DialogueCounter() = 0;
				Interface->engagedInDialogue = false;
				dialoguePlatform->ResetPortraits();
				unit->UnloadMediaPortrait();
				if (unit->Flipped()) unit->Flip();

			}
		}
	}
}

bool MarchingScreen::CollideRectGateKeeperDante()
{
	if (!gateKeeper) return 0;

	bool isColliding = CheckCollisionRecs(dante->Rect(), gateKeeper->Rect());
	if (isColliding)
	{ 
		if (startMarching)
		{
			startMarching = false;
			Idle();
		} 
		
		gateKeeper->HasCollidedWithDante() = true;
	}
	else
	{
		gateKeeper->HasCollidedWithDante() = false;
	}

	return gateKeeper->HasCollidedWithDante();
}

void MarchingScreen::Collisions(vector<bool*>menuFlags)
{
	CollideRectUnitDante();
	CollideRectGateKeeperDante();
}

Rectangle MarchingScreen::EnterTownButtonRect()
{
	return gateKeeper->ButtonRect();
}

void MarchingScreen::AssetOptionsUnits()
{
	if (!Units.empty())
		for (auto& unit : Units)
			unit->ShowAssetOptions();

	if (dante) dante->ShowAssetOptions();

	if (gateKeeper) gateKeeper->ShowAssetOptions();
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

std::optional<std::string> MarchingScreen::ClickAssetOption()
{
	// Call base logic first
	if (auto baseResult = Town::ClickAssetOption())
	{
		return baseResult; // forward Town result
	}

	if (gateKeeper)
	{
		const auto& option = gateKeeper->ClickAssetOption(offsetX);
		if (option == "remove")
			gateKeeper.reset();

		return option;
	}

	return std::nullopt;
}

std::optional<std::string> MarchingScreen::HandleButtonClick(Vector2 mouse, vector<string>validTownNames)
{

	Town::HandleButtonClick(mouse, validTownNames);

	if (CollideRectGateKeeperDante())
	{
		bool buttonClicked = CheckCollisionPointRec(mouse, EnterTownButtonRect());

		if (buttonClicked)
		{
			cout << "BUTTON CLICKED" << endl;

			bool townReady = std::find(validTownNames.begin(), validTownNames.end(), nextTown) != validTownNames.end();

			if (townReady) {
				return nextTown;
			}
			
		}
	}
	return std::nullopt;
}

void MarchingScreen::LeftMouseButtonReleased(Vector2 mouse, vector<string>validTownNames)
{
	for (auto& unit : Units)
		unit->UpdateDialogue();

	HandleButtonClick(mouse, validTownNames);
}

void MarchingScreen::SetNextTown(string townName)
{
	nextTown = std::move(townName);
	gateKeeper->SetNextTown(nextTown);
}

void MarchingScreen::SetMarchingArmy(const shared_ptr<vector<MarchingEntity>>& marchingArmy)
{
	/*if(marchingArmy->empty())
		cout << "MARCHING ARMY IS EMPTY" << endl;*/
	this->marchingArmy = marchingArmy;
	PrepArmyForMarching();
	ScaleUnits(*this->marchingArmy, ARMY_SCALING_FACTOR);

	if (dante)
	{
		RearrangeArmy(dante->X() - 80, dante->Y() + dante->Rect().height);
	}
}

//void MarchingScreen::StopSoundtrack() {
//	StopMusicStream(soundtrack);
//}
//
//void MarchingScreen::PlaySoundtrack()
//{
//	if (!IsMusicStreamPlaying(soundtrack)) {
//		PlayMusicStream(soundtrack); // Start or resume the soundtrack
//	}
//}