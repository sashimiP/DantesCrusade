#include "Recruiter.h"

Recruiter::Recruiter(const vector<string>& typesForHire,
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
	vector<string> dialogue,
	string name,
	int color
) :
	Entity(dataPath,
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
		color),
	typesForHire(typesForHire)
{
	unitsForHirePath = this->unitPath / "units_for_hire" / (unitName + ".txt");
	options.clear();
	options.reserve(15);
	options = {
		"remove",
		"add name",
		"add dialogue",
		"flip",
		"=>",
		"<=",
		"scale",
		"add unit"
	};
	Options.SetAssetOptions(options);
}

Recruiter::~Recruiter()
{
	//std::cout << "DESTRUCTOR CALLED:" << std::endl;
	//std::cout<<"destination: "<< destination << std::endl
	//	<< "id: " << id << std::endl
	//	<< " position: " << position << std::endl
	//	<< " x: " << x << std::endl
	//	<< " y: " << y << std::endl
	//	<< " type: " << type << std::endl
	//	<< " flip: " << direction << std::endl
	//	<< " stance: " << stance << std::endl
	//	<< "name" << name << std::endl
	//	<< std::endl;
	SaveCoordinates();
	save = false;
}

void Recruiter::SaveCoordinates()
{
	string unitName = to_string(this->position) + this->type + to_string(this->id);
	CreateDirectories(unitName);
	std::fstream coordinates;
	//cout << "SAVING UNIT COORDINATES PATH" << coordinatesPath << endl;
	coordinates.open(coordinatesPath, std::ios::out);
	if (coordinates.is_open())
	{
		coordinates << id << std::endl;
		coordinates << position << std::endl;
		coordinates << x << std::endl;
		coordinates << y << std::endl;
		coordinates << type << std::endl;
		coordinates << direction << std::endl;
		coordinates << stance << std::endl;
		coordinates << name << std::endl;
		coordinates << color << std::endl;

		coordinates.close();
	}
	else
	{
		cout << "Problem with file:" << endl;
		cout << coordinatesPath << endl;
	}

	if (!dialogueBoxes.empty())
	{
		std::fstream dialogueFile;
		//cout << "SAVING UNIT DIALOGUES PATH" << coordinatesPath << endl;
		dialogueFile.open(dialoguePath, std::ios::out);
		if (dialogueFile.is_open())
		{
			for (const auto& line : dialogueBoxes)
			{
				dialogueFile << line.GetText() << endl;
			}
			dialogueFile.close();
		}
		else
		{
			cout << "Problem with file:" << endl;
			cout << dialoguePath << endl;
		}
	}

	if (!typesForHire.empty())
	{
		cout << "TYPES FOR HIRE:" << endl;

		for (auto& type : typesForHire)
			cout << type << endl;

		std::fstream typesForHireFile;
		//cout << "SAVING UNIT DIALOGUES PATH" << coordinatesPath << endl;
		typesForHireFile.open(unitsForHirePath, std::ios::out);
		if (typesForHireFile.is_open())
		{
			for (const auto& line : typesForHire)
			{
				typesForHireFile << line << endl;
			}
			typesForHireFile.close();
		}
		else
		{
			cout << "Problem with file:" << endl;
			cout << unitsForHirePath << endl;
		}
	}
}

void Recruiter::CreateGrid()
{
	grid.clear();
	const float width{ static_cast<float>(BASIC_PORTRAIT_WIDTH) };
	const float height{ static_cast<float>(BASIC_PORTRAIT_HEIGHT) };
	float x{ destRect.x - width};
	float y{rect.y};

	grid.reserve(typesForHire.size());

	for (int i{}; i < typesForHire.size(); i++)
	{
		Rectangle rect{ x, y, width, height };
		grid.push_back(rect);
		// Update position for next cell
		x += width;
		//Check if we need to move up a row
		if (x > destRect.x + width) {
			y -= height;
			x = destRect.x-width;
		}
	}
}

void Recruiter::PrepArmyPortraits()
{
	unitsForHire.clear();
	unitsForHire.reserve(typesForHire.size());
	for (int i{}; i<typesForHire.size(); i++)
	{
		auto texture = textureManager->Portrait(typesForHire[i]);
		unitsForHire.emplace_back(typesForHire[i], *texture, grid[i]);
	}
}

bool& Recruiter::HasCollidedWithDante()
{
	if(dialogueCounter>0)
	{ 
		CreateGrid();
		PrepArmyPortraits();
	}

	return hasCollidedWithDante;
}

void Recruiter::Update(float offsetX, float dt)
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

	if (addUnitForHire)
		AddUnitForHire();
}

string Recruiter::ClickAssetOption(float offsetX)
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
		else if (option == "add unit")
		{
			addUnitForHire = true;
			stopMove = true;
		}
		else if (option == "flip")
		{
			Flip();
		}
		else if (option == "<=")
		{
			color -= 1;
			if (0 > color)
				color = colorCount - 1;
			cout << color << endl;
			sourceRect = Rectangle{ 0, 0 + frameHeight * color,static_cast<float>(frameWidth),static_cast<float> (frameHeight) };
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
		else if (option == "")
		{
			if (showAssetOptions) showAssetOptions = false;
			if (showRect) showRect = false;
		}
		else
		{
			SetStance(option, offsetX);
		}

	}
	return option;
}

void Recruiter::AddText()
{
	//cout << "adding text" << endl;

	// Mouse click handling
	if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
	{
		Vector2 mouse = GetMousePosition();
		textBoxActive = CheckCollisionPointRec(mouse, textRect);

		// Add button clicked
		if (CheckCollisionPointRec(mouse, addButton.GetRect()))
		{
			Entity::stopMove = false;
			addName = false;
			addUnitForHire = false;
			textBoxActive = false;
			showCursor = false;

			if (dialogueBoxes.empty())
			{
				Rectangle tempRect = { centerRect.x, destRect.y, (float)MeasureText("!", 20) + 10, 40 };
				dialogueBoxes.emplace_back(TextBox("!", fontSize, tempRect, RAYWHITE));
			}


			if (!text.empty())
			{
				typesForHire.push_back(text);
				cout << "typesForHire size: " << typesForHire.size() << endl;
			}

			if (portrait == nullptr)
				portrait = textureManager->Portrait(type);

			text.clear();
			textRect = Rectangle{ centerRect.x, rect.y, 20, 40 };
			return;
		}
		else if (CheckCollisionPointRec(mouse, backButton.GetRect()))
		{
			Entity::stopMove = false;
			addName = false;
			addUnitForHire = false;
			textBoxActive = false;
			showCursor = false;
		}
	}

	// Handle character input if textbox is active
	if (!textBoxActive)
		return;

	auto updateCursorToLineEnd = [&]() {
		size_t lastNewline = text.rfind('\n');
		string line = (lastNewline == string::npos) ? text : text.substr(lastNewline + 1);
		int lineWidth = MeasureText(line.c_str(), fontSize);
		cursorX = textRect.x + 6 + lineWidth;
		cursorWidth = cursorX;
		};


	// Process typed characters
	int key = GetCharPressed();
	while (key > 0)
	{
		if ((key >= 32) && (key <= 125) && (text.length() < maxChars))
		{
			text += (char)key;
			textRect.width = 20 + MeasureText(text.c_str(), fontSize);
			updateCursorToLineEnd();
		}
		key = GetCharPressed();
	}

	// Handle backspace
	if (IsKeyPressed(KEY_BACKSPACE) && !text.empty())
	{
		char deletedKey = text.back();
		text.pop_back();
		textRect.width = 20 + MeasureText(text.c_str(), fontSize);

		if (deletedKey != '\n')
		{
			updateCursorToLineEnd();
		}
		else // handle newline deletion
		{
			textRect.height -= (fontSize + 4);
			cursorY -= (fontSize + 4);
			cursorHeight = cursorY + 30;
			updateCursorToLineEnd();
		}
	}

	// Cursor blinking logic
	framesCounter++;
	if (showCursor && framesCounter > cursorBlinkRateOn)
	{
		showCursor = false;
		framesCounter = 0;
	}
	else if (!showCursor && framesCounter > cursorBlinkRateOff)
	{
		showCursor = true;
		framesCounter = 0;
	}

	// Handle enter key (new line)
	if (IsKeyPressed(KEY_ENTER))
	{
		text += '\n';
		textRect.width = 20 + MeasureText(text.c_str(), fontSize);
		textRect.height += (fontSize + 4);
		cursorY += (fontSize + 4);
		cursorHeight = cursorY + 30;
		cursorX = textRect.x + 6;
		cursorWidth = cursorX;
	}
}

void Recruiter::AddUnitForHire()
{
	AddText();
}

void Recruiter::DrawDialogue(float offsetX)
{
	if (addDialogue || addName || addUnitForHire)
	{
		DrawRectangleRec(textRect, textBoxActive ? LIGHTGRAY : GRAY);
		DrawText(text.c_str(), textRect.x + 5, textRect.y + 10, 20, BLACK);
		DrawRectangleLinesEx(textRect, 1, BLACK);

		addButton.Display();
		backButton.Display();


	}
	// Display blinking cursor if active
	if (textBoxActive && showCursor)
	{

		int textWidth = MeasureText(text.c_str(), 20);


		DrawLine(cursorX, cursorY, cursorWidth, cursorHeight, BLACK);
	}

	if (showDialogue && !dialogueBoxes.empty())
	{
		//cout << "DRAW DIALOGUE" << endl;
		if (dialogueCounter == 0)
			dialogueBoxes[dialogueCounter].Display();
	}

}

void Recruiter::DrawUnitsForHire()
{
	for (const auto& unit : unitsForHire)
	{
		DrawRectangleRec(unit.GetRect(), INTERFACE_BG_COLOR);
		unit.Display();
		std::string price = std::to_string(Utilities::unitForHirePrice[unit.GetType()]);
		std::string text = ("price:\n" + price + " gold");
		auto width = MeasureText(text.c_str(), 20);
		DrawRectangle(unit.GetRect().x, unit.GetRect().y - 40, width, 40, INTERFACE_BG_COLOR);
		DrawText(text.c_str(), unit.GetRect().x, unit.GetRect().y - 40, 20, YELLOW);
	}
}

void Recruiter::Display(int animationSpeed, int offsetX, float dt)
{
	// Get screen dimensions dynamically
	const int SCREEN_WIDTH = GetScreenWidth();
	const int SCREEN_HEIGHT = GetScreenHeight();

	// Calculate screen position with offset
	float screenX = x - offsetX;
	float screenY = y;

	// Adjust culling to account for the full texture size, including center offset
	float leftEdge = screenX - (frameWidth / 2.0f); // Left edge of the texture
	float rightEdge = screenX + (frameWidth / 2.0f); // Right edge of the texture
	float topEdge = screenY - (frameHeight / 2.0f); // Top edge
	float bottomEdge = screenY + (frameHeight / 2.0f); // Bottom edge

	// Check if any part of the texture is on-screen
	if (rightEdge < 0 || leftEdge > SCREEN_WIDTH || bottomEdge < 0 || topEdge > SCREEN_HEIGHT)
	{
		return; // Skip drawing if completely off-screen
	}
	//cout << "DRAW" << endl;

	if (showRect) DrawRectangleLines(rect.x, rect.y, rect.width, rect.height, RED);

	//FrameData& frameData = (*texturesMap)[stance];
	//Texture2D& texture = frameData.texture;     // sprite sheet

	animationTimer += dt;
	if (animationTimer >= (1.0f / animationSpeed)) {
		animationTimer = 0.0f;
		if (framesCount > 0) {
    currentFrame = (currentFrame + 1) % framesCount;
    sourceRect.x = static_cast<float>(currentFrame * frameWidth);
  }
	}

	DrawTexturePro(
		*texture,
		sourceRect,
		destRect,
		center,
		0,
		RAYWHITE
	);

	// Don't draw the unit's x and y coordinates, when engaged in dialogue
	if (!showDialogue && !marching)
	{
		string temp = "x: " + to_string(static_cast<int>(x));

		DrawText(temp.c_str(), rect.x, rect.y, 20, WHITE);

		temp = "y: " + to_string(static_cast<int>(y));
		DrawText(temp.c_str(), rect.x + 100, rect.y, 20, WHITE);

		DrawRectangleRec(centerRect, GREEN);	// draw the center point

	}

	if (dialogueCounter > 0) DrawUnitsForHire();

}

vector<Portrait>* Recruiter::GetUnitsForHire()
{
	return &unitsForHire;
}
