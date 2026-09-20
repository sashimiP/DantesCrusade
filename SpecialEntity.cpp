#include "SpecialEntity.h"

SpecialEntity::SpecialEntity(
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
	int color,
	bool dontScale,
	const string& special,
	float growStats,
	int price
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
		color,
		dontScale),
	special(special),
	growStats(growStats),
	price(price)
{
	options = {
		"remove",
		"add name",
		"add dialogue",
		"flip",		
		"=>",
		"<=",
		"scale",
		"add special",
		"set price"
	};
	Options.SetAssetOptions(options);
	yesButton = std::make_unique<Button>(this->special+"("+to_string(price)+" gold)", FONT_SIZE);
	yesButton->SetPosition(rect.x, rect.y - 32.0f);
	specialBackButton = std::make_unique<Button>("back", FONT_SIZE);
	specialBackButton->SetPosition(yesButton->GetRect().x + yesButton->GetRect().width, yesButton->GetRect().y);

	
}

SpecialEntity::~SpecialEntity()
{
	//std::cout << "DESTRUCTOR CALLED:" << std::endl;
	//std::cout << "special: " << special << std::endl
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

void SpecialEntity::Update(float offsetX, float dt)
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

	if (addSpecial)
		AddSpecial();

	if (setPrice)
		SetPrice();

	Patrol(dt, offsetX);
}

string SpecialEntity::ClickAssetOption(float offsetX)
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
		else if (option == "add special")
		{
			addSpecial = true;
			stopMove = true;
		}
		else if (option == "set price")
		{
			cout << "set price" << endl;
			setPrice = true;
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

void SpecialEntity::AddText(string& textVariable)
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
			addSpecial = false;
			setPrice = false;
			textBoxActive = false;
			showCursor = false;

			if (dialogueBoxes.empty())
			{
				Rectangle tempRect = { centerRect.x, destRect.y, (float)MeasureText("!", 20) + 10, 40 };
				dialogueBoxes.emplace_back(TextBox("!", fontSize, tempRect, RAYWHITE));
			}


			if (!text.empty())
				textVariable = text;

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
			addSpecial = false;
			setPrice = false;
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

void SpecialEntity::DrawDialogue(float offsetX)
{
	if (addDialogue || addName || addSpecial || setPrice)
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

void SpecialEntity::AddSpecial()
{
	AddText(special);
	SetButton(special);
}

void SpecialEntity::DrawButton()
{
	if (dialogueCounter>0 && hasCollidedWithDante)
	{
		yesButton->Display();
		specialBackButton->Display();
	}
}

bool& SpecialEntity::HasCollidedWithDante()
{
	ShowDialogue() = hasCollidedWithDante;
	yesButton->SetPosition(rect.x, rect.y - 32.0f);
	specialBackButton->SetPosition(yesButton->GetRect().x + yesButton->GetRect().width, yesButton->GetRect().y);
	return hasCollidedWithDante;
}

string SpecialEntity::YesButton(float& statsToManipulate, float& gold)
{
	if (gold < price) return "not enougth gold!";
	statsToManipulate += growStats;
	gold -= price;
	return yesButton->GetText();
}

string SpecialEntity::BackButton()
{
	return specialBackButton->GetText();
}

void SpecialEntity::SaveCoordinates()
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
		coordinates << dontScale << std::endl;
		coordinates << patrolRoute.x << std::endl;
		coordinates << patrolRoute.y << std::endl;
		coordinates << special << std::endl;
		coordinates << growStats << std::endl;
		coordinates << price << std::endl;

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
}

Rectangle SpecialEntity::YesButtonRect()
{
	return yesButton->GetRect();
}

Rectangle SpecialEntity::BackButtonRect()
{
	return specialBackButton->GetRect();
}

//void SpecialEntity::SetNextTown(const string& nextTownName)
//{
//	destination = nextTownName;
//	SetButton(destination);
//}

int SpecialEntity::GetPrice()
{
	return price;
}

void SpecialEntity::SetButton(const string& special)
{
	yesButton = std::make_unique<Button>(this->special + "(" + to_string(price) + " gold)", FONT_SIZE);
	yesButton->SetPosition(rect.x, rect.y - 32.0f);
}

void SpecialEntity::SetGrowStats(float newGrowStats)
{
	growStats = newGrowStats;
}

void SpecialEntity::SetPrice()
{
	string s = to_string(price);
	AddText(s);
	price = stoi(s);
	SetButton(special);
}