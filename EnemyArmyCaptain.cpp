#include "EnemyArmyCaptain.h"

EnemyArmyCaptain::EnemyArmyCaptain(fs::path dataPath,
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
	vector<string>dialogue,
	string name,
	int color,
	bool dontScale,
	Vector2 patrolRoute) :
	CampaignEntity(dataPath,
		town,
		textureManager,
		id,
		position,
		type,
		x,
		y,
		direction,
		drag,
		stance,
		dialogue,
		name,
		color,
		dontScale,
		patrolRoute)
{
	armyDataPath = this->unitPath / "army" / (unitName + ".txt");
	options = {
		"remove",
		"add name",
		"add dialogue",
		"flip",
		"=>",
		"<=",
		"scale",
		"dontScale",
		"add units",
		"stop add units",
		"show units"
	};
	Options.SetAssetOptions(options);
	army.reserve(7);
	armyPortraits.reserve(7);
	armyPortraitsGrid.reserve(7);
	keys.reserve(7);
	LoadArmy();
	//Buttons
	closeArmyPanelButton.onClick = [&]()
		{
			showArmyFlag = false;
			showAssetOptions = true;
			cout << "Army panel closed!\n";
		};

	engageButton.onClick = [&]()
		{
			cout << "engageButton";
			*engage = true;
		};

	showArmyButton.onClick = [this]()
		{
			ShowArmy();
		};

	buttons.push_back(engageButton);
	buttons.push_back(showArmyButton);

}

EnemyArmyCaptain::~EnemyArmyCaptain()
{
	// save_flag is very important!!!
	// if flag is not set it will add the coordinates file to the directory!!!
	if (save)
	{
		SaveCoordinates();
		CreateDirectories(unitName);
		std::fstream armyFile;
		armyFile.open(armyDataPath, std::ios::out);
		if (armyFile.is_open())
		{
			for (const auto& unit : army)
			{
				armyFile << unit << endl;
			}
			armyFile.close();
		}
		else
		{
			cout << "Problem with file:" << endl;
			cout << dialoguePath << endl;
		}

	}
	save = false;
}

void EnemyArmyCaptain::Update(float offsetX, float dt)
{
	if (drag)
	{
		SetX(GetMousePosition().x, offsetX);
		SetY(GetMousePosition().y);

	}

	if (addDialogue)
		AddDialogue();

	if (addName)
		AddName();

	Patrol(dt, offsetX);
	closeArmyPanelButton.Update();

	for (auto& button : buttons)
	{
		button.Update();
	}

}

void EnemyArmyCaptain::Drag()
{
	if (CheckCollisionPointRec(GetMousePosition(), (hasRoute) ? rect : centerRect))
	{
		if (drag)
		{
			drag = false;
			showRect = false;
			std::cout << "Deactivate move\n";
			Options.updateGrid(rect);
			UpdateTextRect();
			UpdateShadowRect();
			UpdateArmyPanel();

		}
		else
		{
			drag = true;
			showRect = true;
			beginPatrol = false;
			hasRoute = false;
			if (showAssetOptions) showAssetOptions = false;
			std::cout << "Activate move\n";
		}
	}
}

void EnemyArmyCaptain::CreateDirectories(string& unitName) const
{

	if (!fs::exists(this->unitPath / "coordinates")) {
		fs::create_directories(this->unitPath / "coordinates");
	}
	if (!fs::exists(this->unitPath / "dialogue")) {
		fs::create_directories(this->unitPath / "dialogue");
	}
	if (!fs::exists(this->unitPath / "army")) {
		fs::create_directories(this->unitPath / "army");
	}
}

void EnemyArmyCaptain::LoadArmy()
{
	/*std::ifstream dataFile(armyDataPath);
	int key = 1;
	if (!dataFile.is_open()) {
		std::cout << "Failed to open file: " << armyDataPath << std::endl;
		return;
	}
	string line;
	while (getline(dataFile, line))
	{
		army[key] = line;
		keys.push_back(key++);
		armyPortraits.emplace_back(textureManager->Portrait(line));
	}
	dataFile.close();*/

	army.reserve(7);
	std::ifstream file(armyDataPath);
	if (!file.is_open()) {
		std::cerr << "Failed to open army file: " << armyDataPath << "\n";
		return;
	}

	std::string line;
	size_t count = 0;

	while (std::getline(file, line)) {
		// Optional: skip empty lines or lines with only whitespace
		if (line.find_first_not_of(" \t\r\n") != std::string::npos) {
			armyPortraits.emplace_back(textureManager->Portrait(line));
			army.push_back(std::move(line));  // move avoids copy
			++count;
		}
	}

	std::cout << "Loaded " << count << " army entries from " << armyDataPath << "\n";

	CreateArmyPanel();
}

string EnemyArmyCaptain::ClickAssetOption(float offsetX)
{
	if (!showAssetOptions) return "";
	string option = ClickOption();
	if (showAssetOptions)
	{

		if (option == "remove")
		{
			Remove(false);

		}
		else if (option == "add name")
		{
			addName = true;
			stopMove = true;
		}
		else if (option == "add dialogue")
		{
			cout << "add dialogue" << endl;
			addDialogue = true;
			stopMove = true;
		}
		else if (option == "flip")
		{
			Flip();
		}
		else if (option == "front")
		{
			Front();
		}
		else if (option == "back")
		{
			Back();
		}
		else if (option == "<=")
		{
			color -= 1;
			if (0 > color)
				color = colorCount - 1;
			cout << color << endl;
			sourceRect = Rectangle{ 0, 0 + frameHeight * color,static_cast<float>(frameWidth),static_cast<float> (frameHeight) };
			if (textureManager->AnimatedPortrait(type + "_" + std::to_string(color)) != nullptr)
				animatedPortrait = textureManager->AnimatedPortrait(type + "_" + std::to_string(color));
		}
		else if (option == "=>")
		{
			color += 1;
			if (color > colorCount - 1)
				color = 0;
			cout << color << endl;
			sourceRect = Rectangle{ 0, 0 + frameHeight * color,static_cast<float>(frameWidth),static_cast<float> (frameHeight) };
		}
		else if (option == "scale")
		{
			auto scalingFactor = (scaled) ? RESCALING_FACTOR : SCALING_FACTOR;
			ScaleDestRect(scalingFactor);
			scaled = !scaled;
		}
		else if (option == "dontScale")
		{
			dontScale = !dontScale;
		}
		else if (option == "setRoute")
		{
			SetPatrolRoute();
		}
		else if (option == "patrol")
		{
			BeginPatrol(offsetX);
		}
		else if (option == "add units")
		{
			ActivateAddUnit(true);
		}
		else if (option == "stop add units")
		{
			ActivateAddUnit(false);
		}
		else if (option == "show units")
		{
			ShowArmy();
		}
		else if (option == "")
		{
			showAssetOptions = false;
			showRect = false;
			showArmyFlag = false;
		}
		else
		{
			SetStance(option, offsetX);
		}

	}
	return option;
}

void EnemyArmyCaptain::AddUnit(string type)
{
	if (!addingUnitsFlag) return;

	cout << "ADDING " << type<< endl;
	army.push_back(std::move(type));
	cout << "YOU CAN ADD " << 6 - army.size() << " units" << endl;
	if (army.size() > 6)
	{	
		cout << "EXCEEDED ARMY CAPACITY\nLIMIT IS SIX UNITS" << endl;
		cout << "REMOVED " << army[0] << endl;
		army.erase(army.begin());
	}
}

void EnemyArmyCaptain::ActivateAddUnit(bool flag)
{
	addingUnitsFlag = flag;
}

void EnemyArmyCaptain::ShowArmy()
{
	showAssetOptions = false;
	cout << type << " has army:\n";
	for (auto& unit : army)
		cout << unit << endl;

	if (armyPortraits.size() < army.size())
	{
		CreateArmyPanel();
	}
	showArmyFlag = true;
	closeArmyPanelButton.drawFlag = true;
}

bool EnemyArmyCaptain::GetAddUnitFlag()
{
	return addingUnitsFlag;
}

void EnemyArmyCaptain::Display(int animationSpeed, int offsetX, float dt)
{
	if (dialogueCounter > 0) return;
	// Get screen dimensions dynamically
	const int SCREEN_WIDTH = GetScreenWidth();
	const int SCREEN_HEIGHT = GetScreenHeight();

	// Calculate screen position with offset
	float screenX = x - offsetX;
	float screenY = y;

	// Adjust culling to account for the full texture size, including center offset
	float westEdge = screenX - (frameWidth / 2.0f); // Left edge of the texture
	float eastEdge = screenX + (frameWidth / 2.0f); // Right edge of the texture
	float topEdge = screenY - (frameHeight / 2.0f); // Top edge
	float bottomEdge = screenY + (frameHeight / 2.0f); // Bottom edge


	// Set Patrol Route
	if (setRoute)
	{
		DrawPatrolRoute(offsetX, GetMousePosition());
	}

	// Check if any part of the texture is on-screen
	if (!hasRoute && (eastEdge < 0 || westEdge > SCREEN_WIDTH || bottomEdge < 0 || topEdge > SCREEN_HEIGHT))
	{
		return; // Skip drawing if completely off-screen
	}

	if (showRect) DrawRectangleLines(rect.x, rect.y, rect.width, rect.height, RED);


	FrameData& frameData = (*texturesMap)[stance];
	Texture2D& texture = frameData.texture;     // sprite sheet

	animationTimer += dt;
	if (animationTimer >= (1.0f / animationSpeed)) {
		animationTimer = 0.0f;
		currentFrame = (currentFrame + 1) % framesCount;
		sourceRect.x = static_cast<float>(currentFrame * frameWidth);
		shadowSource.x = static_cast<float>(currentFrame * frameWidth);
	}
	DrawShadow(ANIMATION_SPEED, dt);
	DrawTexturePro(
		texture,
		sourceRect,
		destRect,
		center,
		0.0f,
		RAYWHITE
	);

	// Don't draw the unit's x and y coordinates, when engaged in dialogue
	if (!showDialogue && !marching)
	{
		string temp = "x: " + to_string(static_cast<int>(x));

		DrawText(temp.c_str(), rect.x, rect.y, 20, WHITE);

		temp = "y: " + to_string(static_cast<int>(y));
		DrawText(temp.c_str(), rect.x + 100, rect.y, 20, WHITE);

		if (dontScale)
		{
			temp = "Don't Scale";
			DrawText(temp.c_str(), rect.x + rect.width / 2, rect.y + rect.height, 20, WHITE);
		}

		DrawRectangleRec(centerRect, GREEN);	// draw the center point

	}

	if (showArmyFlag)
		DrawArmyPortraits();
	

}

void EnemyArmyCaptain::DrawArmyPortraits()
{
	for (auto& rectPortrait : armyPortraitsGrid)
		DrawTexturePro(*rectPortrait.second,
			Rectangle{ 0,0, 64,64 },
			rectPortrait.first,
			Vector2{ 0,0 },
			0.0f,
			RAYWHITE);
	closeArmyPanelButton.Display();
}

void EnemyArmyCaptain::CreateArmyPanel()
{
	armyPortraitsGrid.clear();
	Rectangle firstRect{ rect.x + rect.width, rect.y, 64, 64 };
	

	for (int i{}; i < armyPortraits.size(); i++)
	{
		armyPortraitsGrid.emplace_back(firstRect, armyPortraits[i]);
		firstRect.x += 64;
		if (i > 0 && i % 2 == 0)
		{
			firstRect.x = rect.x + rect.width;
			firstRect.y += 64;
		}
	}
	closeArmyPanelButton.SetPosition(armyPortraitsGrid.back().first.x,
		armyPortraitsGrid.back().first.y + armyPortraitsGrid.back().first.height);
}

void EnemyArmyCaptain::UpdateArmyPanel()
{
	Rectangle firstRect{ rect.x + rect.width, rect.y, 64, 64 };

	for (int i{}; i < armyPortraits.size(); i++)
	{
		armyPortraitsGrid[i].first = firstRect;
		firstRect.x += 64;
		if (i > 0 && i % 2 == 0)
		{
			firstRect.x = rect.x + rect.width;
			firstRect.y += 64;
		}
	}
	closeArmyPanelButton.SetPosition(armyPortraitsGrid.back().first.x,
		armyPortraitsGrid.back().first.y + armyPortraitsGrid.back().first.height);

}

void EnemyArmyCaptain::MoveRight(int scrollSpeed, float dt)
{
	rect.x -= scrollSpeed * GLOBAL_SPEED * dt;
	centerRect.x -= scrollSpeed * GLOBAL_SPEED * dt;
	destRect.x -= scrollSpeed * GLOBAL_SPEED * dt;
	patrolRoute.x -= scrollSpeed * GLOBAL_SPEED * dt;
	startingPosition.x -= scrollSpeed * GLOBAL_SPEED * dt;
	shadowDest.x -= scrollSpeed * GLOBAL_SPEED * dt;
	//startingPosition.x -= scrollSpeed * GLOBAL_SPEED * dt;
	//patrolRoute.x -= scrollSpeed * GLOBAL_SPEED * dt;
	Options.updateGrid(rect);
	UpdateTextRect();
	UpdateArmyPanel();
	//UpdateShadowRect();
}

void EnemyArmyCaptain::MoveLeft(int scrollSpeed, float dt)
{
	rect.x += scrollSpeed * GLOBAL_SPEED * dt;
	centerRect.x += scrollSpeed * GLOBAL_SPEED * dt;
	destRect.x += scrollSpeed * GLOBAL_SPEED * dt;
	patrolRoute.x += scrollSpeed * GLOBAL_SPEED * dt;
	startingPosition.x += scrollSpeed * GLOBAL_SPEED * dt;
	shadowDest.x += scrollSpeed * GLOBAL_SPEED * dt;
	//startingPosition.x += scrollSpeed * GLOBAL_SPEED * dt;
	//patrolRoute.x += scrollSpeed * GLOBAL_SPEED * dt;
	Options.updateGrid(rect);
	UpdateTextRect();
	UpdateArmyPanel();
	//UpdateShadowRect();

}

void EnemyArmyCaptain::UpdateDialogue()
{
	if (!showDialogue || dialogueBoxes.empty()|| dialogueCounter>0) return;

	if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
	{
		cout << dialogueCounter << endl;
		if (dialogueCounter == 0)
		{
			if (CheckCollisionPointRec(GetMousePosition(), rect))
			{
				dialogueCounter++;
			}
		}
		else
		{
			dialogueCounter++;
		}

		if (dialogueCounter >= dialogueBoxes.size())
		{
			dialogueCounter = 0;
		}
	}
}

//unordered_map<int, string>* EnemyArmyCaptain::GetArmy()
//{
//	return &army;
//}

const vector<string>& EnemyArmyCaptain::GetArmy()
{
	return army;
}