#include "Entity.h"
bool Entity::stopMove = false;

Entity::Entity(fs::path dataPath,
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
	Vector2 patrolRoute):
	dataPath(dataPath),
	town(std::move(town)),
	textureManager(textureManager),
	id(id),
	position(std::move(position)),
	type(std::move(type)),
	x(x),
	y(y),
	direction(direction),
	drag(drag),
	stance(std::move(stance)),
	texturesMap(textureManager->TextureMap(this->type)),
	name(name),
	color(color),
	dontScale(dontScale),
	patrolRoute(patrolRoute)
{
	scrollSpeed = 50;
	currentFrame = 0;
	unitName = to_string(this->position) + this->type + to_string(this->id);
	//cout << "UNIT NAME " << unitName << endl;
	this->unitPath = this->dataPath / unitName; //update path to point to the exact unit
	this->coordinatesPath = this->unitPath / "coordinates" / (unitName + ".txt");
	this->dialoguePath = this->unitPath / "dialogue" / (unitName + ".txt");

	showAssetOptions = false;
   /* auto& spriteSheet = (*this->texturesMap)[this->stance];*/

	spriteSheet = &(*texturesMap)[this->stance];
	texture = &spriteSheet->texture;

	framesCount = spriteSheet->frames;
	colorCount = static_cast<float>(spriteSheet->colors);
	frameWidth = static_cast<float> (spriteSheet->texture.width) / framesCount ;
	frameHeight = static_cast<float>(spriteSheet->texture.height) / static_cast<float>(colorCount);

	sourceRect = Rectangle{ 0, frameHeight * color,static_cast<float>(frameWidth),static_cast<float> (frameHeight) };
	destRect = Rectangle{ this->x,this->y,static_cast<float>(frameWidth),static_cast<float> (frameHeight) };
	center = Vector2{ static_cast<float>(destRect.width / 2), static_cast<float> (destRect.height / 2)  };
	rect = Rectangle{ this->x - center.x,this->y - center.y,destRect.width,destRect.height };
	centerRect = Rectangle{ this->x, this->y, 5, 5 };

	Options = AssetOptions(this->type, rect, options);

	flipped = direction;

	//if(direction == west) sourceRect.width *= (-1); // Must find a better way
	sourceRect.width *= (flipped) ? (-1.f) : 1.f;

	textRect = { centerRect.x, rect.y, 20, 40 };

	cursorX = textRect.x + 6;
	cursorY = textRect.y + 5;
	cursorWidth = textRect.x + 6;
	cursorHeight = textRect.y + 35;

	backButton.SetLeftOfRect(textRect);
	addButton.SetLeftOfRect(backButton.GetRect());

	CreateDialogueBoxes(dialogue);

	portrait = textureManager->Portrait(this->type);
	//dialoguePortrait = textureManager->DialoguePortrait(this->type);
	//animatedPortrait = textureManager->AnimatedPortrait(this->type + "_" + to_string(color));
	//mediaPortrait = textureManager->UnitMedia(this->type + "_" + to_string(color));

	startingPosition = { centerRect.x,centerRect.y };
	patrolDestination = this->patrolRoute;

	if (this->patrolRoute.x != 0 && this->patrolRoute.y != 0)
	{
		hasRoute = true;
	}

	beginPatrol = hasRoute;

	UpdateShadowRect();
	//this->name = name;

}

Entity::~Entity() 
{
	//std::cout << "DESTRUCTOR CALLED:" << std::endl;
	//std::cout << "id: " << id
	//	<< " position: " << position
	//	<< " x: " << x
	//	<< " y: " << y
	//	<< " type: " << type
	//	<< " flip: " << direction
	//	<< " stance: " << stance
	//	<< "name" << name
	//	<< std::endl;
	if (hasRoute) {
		direction = east;
		stance = "walk";
	}
	// save_flag is very important!!!
	// if flag is not set it will add the coordinates file to the directory!!!
	if(save) SaveCoordinates();
}

void Entity::CreateDialogueBoxes(vector<string> dialogue)
{
	if (dialogue.empty()) return;

	dialogueBoxes.reserve(dialogue.size());
	for (auto& line : dialogue)
	{
		auto cellWidth = MeasureText(line.c_str(), fontSize) + 10;
		dialogueBoxes.emplace_back(TextBox(line, fontSize, Rectangle( rect.x + (rect.width/2), rect.y, cellWidth, 40), RAYWHITE));
	}
}

void Entity::SaveCoordinates()
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
		coordinates << patrolDestination.x << std::endl;
		coordinates << patrolDestination.y << std::endl;
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

void Entity::Display(int animationSpeed, int offsetX, float dt)
{
	if (dialogueCounter>0) return;
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
		{ sourceRect.x, sourceRect.y + 1, sourceRect.width, sourceRect.height }, // temp fix
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

}

void Entity::DrawShadow(int animationSpeed, float dt)
{
	DrawTexturePro(
		*texture,
		shadowSource,
		shadowDest,
		shadowOrigin,
		0.0f,
		Color{ 0, 0, 0, 100 }
	);
}

void Entity::DrawPatrolRoute(int offsetX, Vector2 destination)
{
	//DrawLine(startingPosition.x, startingPosition.y, destination.x, destination.y, BLACK);
	DrawLineEx(startingPosition, destination, 10, BLACK);
}

void Entity::DrawAssetOptions(int offsetX)
{
	if (!showAssetOptions) return;
		
	Options.drawOptionCells();
	string temp = to_string(color + 1) + " / " + to_string(colorCount);
	Rectangle tempRect = { centerRect.x - 10, rect.y - 20, MeasureText(temp.c_str(), 20) + 20, 20 };
	DrawRectangleRec(tempRect, DARKGRAY);
	DrawRectangleLines(tempRect.x, tempRect.y, tempRect.width, tempRect.height, BLUE);
	DrawText(temp.c_str(), centerRect.x, rect.y - 20, 20, RAYWHITE);

	auto positionRect = Rectangle{ rect.x,  rect.y + rect.height - 25, 25,25 };
	DrawRectangleRec(positionRect, DARKGRAY);
	DrawRectangleLinesEx(positionRect, 1, BLUE);
	DrawText(to_string(position).c_str(), positionRect.x + 12, positionRect.y, FONT_SIZE, WHITE);

	if (beginPatrol)
	{
		DrawPatrolRoute(offsetX, patrolRoute);
		DrawRectangle(patrolRoute.x, patrolRoute.y, 5, 5, RED);
		Options.updateGrid(rect);
	}
	
}

void Entity::Update(float offsetX, float dt)
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
			
}

// SHOUDL CREATE AN ON_MOUSE_BUTTON_CLICK FUNCTION,
// FOR NOW DRAG ACTS LIKE AN ON_MOUSE_BUTTON_CLICK FUNCTION,
void Entity::Drag()
{
	if (CheckCollisionPointRec(GetMousePosition(), (hasRoute)? rect:centerRect))
	{
		if (drag)
		{
			drag = false;
			showRect = false;
			std::cout << "Deactivate move\n";
			Options.updateGrid(rect);
			UpdateTextRect();
			UpdateShadowRect();

		}
		else
		{
			drag = true;
			showRect = true;
			beginPatrol = false;
			hasRoute = false;
			if(showAssetOptions) showAssetOptions = false;
			std::cout << "Activate move\n";
		}
	}
	//else
	//{
	//	showRect = false;   // MUST FIND A WAY TO HIDE OPTIONS WHEN CLICKING ON A DIFFERENT PART OF SCREEN
	//	drag = false;
	//	showAssetOptions = false;
	//}
}

void Entity::ShowAssetOptions()
{
	if (CheckCollisionPointRec(GetMousePosition(), (hasRoute) ? rect : centerRect))
	{
		if (showAssetOptions|| showRect)
		{
			showAssetOptions = false;
			if (!drag) showRect = false;
			std::cout << " Hide asset options\n";
		}
		else
		{
			showAssetOptions = true;
			showRect = true;
			beginPatrol = false;
			std::cout << " Show asset options\n";
		}
	}
}

string Entity::ClickAssetOption(float offsetX)
{
	if (!showAssetOptions) return "";
	string option = ClickOption();
		
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
			color = colorCount-1;
		cout << color << endl;
		sourceRect = Rectangle{ 0, 0 + frameHeight * color,static_cast<float>(frameWidth),static_cast<float> (frameHeight) };
		if (textureManager->AnimatedPortrait(type + "_" + std::to_string(color)) != nullptr)
			animatedPortrait = textureManager->AnimatedPortrait(type + "_" + std::to_string(color));
	}
	else if (option == "=>")
	{
		color += 1;
		if (color > colorCount-1)
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
	else if (option == "")
	{
		if (showAssetOptions) showAssetOptions = false;
		if (showRect) showRect = false;
	}
	else
	{
		SetStance(option, offsetX);
	}

	
	return option;
}

void Entity::AddText(string& textVariable)
{

	// Mouse click handling
	if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
	{
		Vector2 mouse = GetMousePosition();
		textBoxActive = CheckCollisionPointRec(mouse, textRect);

		// Add button clicked
		if (CheckCollisionPointRec(mouse, addButton.GetRect()))
		{
			stopMove = false;
			addName = false;
			textBoxActive = false;
			showCursor = false;

			if (dialogueBoxes.empty())
			{
				Rectangle tempRect = { centerRect.x, destRect.y - 60, (float)MeasureText("!", 20)+10, 40 };
				dialogueBoxes.emplace_back(TextBox("!", fontSize, tempRect, RAYWHITE));
			}


			if(!text.empty())
				textVariable = text;

			if (portrait == nullptr)
				portrait = textureManager->Portrait(type);

			text.clear();
			textRect = Rectangle{ centerRect.x, rect.y, 20, 40 };
			return;
		}
		else if (CheckCollisionPointRec(mouse, backButton.GetRect()))
		{
			stopMove = false;
			addName = false;
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

void Entity::AddText(vector<TextBox>& textVariable)
{
	
	// Mouse click handling
	if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
	{
		Vector2 mouse = GetMousePosition();
		textBoxActive = CheckCollisionPointRec(mouse, textRect);

		// Add button clicked
		if (CheckCollisionPointRec(mouse, addButton.GetRect()))
		{
			stopMove = false;
			addDialogue = false;
			textBoxActive = false;
			showCursor = false;

			if (dialogueBoxes.empty())
			{
				Rectangle tempRect = { centerRect.x, rect.y, (float)MeasureText("!", 20)+10, 40 };
				dialogueBoxes.emplace_back(TextBox("!", fontSize, tempRect, RAYWHITE));
			}

			if(!text.empty())

				textVariable.emplace_back(TextBox(text, fontSize, textRect, RAYWHITE));

			if (portrait == nullptr)
				portrait = textureManager->Portrait(type);

			text.clear();
			textRect = Rectangle{ centerRect.x, rect.y, 20, 40 };
			return;
		}
		else if (CheckCollisionPointRec(mouse, backButton.GetRect()))
		{
			stopMove = false;
			addDialogue = false;
			textBoxActive = false;
			showCursor = false;
			return;
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

void Entity::AddDialogue()
{
	AddText(dialogueBoxes);
}

void Entity::UpdateDialogue()
{
	if (!showDialogue || dialogueBoxes.empty()) return;

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

void Entity::AddName()
{
	AddText(name);
}

void Entity::DrawDialogue(float offsetX)
{
	if (addDialogue||addName)
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
		if(dialogueCounter==0)
			dialogueBoxes[dialogueCounter].Display();
	}
	
}

// WORKS FOR NOW FIX LATER
void Entity::Flip()
{	
	//cout << "Before flip ";
	//switch (direction) {
	//case east: cout << "East." << endl; break;
	//case west: cout << "West." << endl; break;
	//}
	direction = (direction == west)? east : west;
	sourceRect.width = -sourceRect.width;
	UpdateShadowRect();
	flipped = !flipped;
	
	
	//cout << "After flip ";
	//switch (direction) {
	//case east: cout << "East." << endl; break;
	//case west: cout << "West." << endl; break;
	//}
	
}

void Entity::SetStance(string newStance, float offsetX)
{
	if (!Options.hasOption(newStance)) return;
	stance = newStance;
	spriteSheet = &(*texturesMap)[stance];
	texture = &spriteSheet->texture;

	framesCount = spriteSheet->frames;
	colorCount = static_cast<float>(spriteSheet->colors);
	frameWidth = static_cast<float>(spriteSheet->texture.width) / framesCount;
	frameHeight = static_cast<float>(spriteSheet->texture.height) / colorCount;

	color = (color > colorCount-1) ? 0 : color;

	sourceRect = Rectangle{ 0, 0 + frameHeight * color,static_cast<float>(frameWidth),static_cast<float> (frameHeight) };
	destRect = Rectangle{ x - offsetX, y, static_cast<float>(frameWidth),static_cast<float> (frameHeight) };
	center = Vector2{ static_cast<float>(destRect.width / 2), static_cast<float> (destRect.height / 2) };
	rect = Rectangle{ x - center.x - offsetX, y - center.y, destRect.width, destRect.height };

	if (flipped) Flip();

	centerRect = Rectangle{ x- offsetX, y, 5, 5 };
	Options.updateGrid(rect);
}

void Entity::SetPatrolRoute()
{
	setRoute = true;
	showAssetOptions = false;
}

void Entity::DeactivateSetPatrolRoute()
{
	setRoute = false;
}

void Entity::BeginPatrol(float offsetX)
{
	if (!hasRoute) return;

	string temp = (beginPatrol) ? "idle" : "walk";

	SetStance(temp, offsetX);
	beginPatrol = !beginPatrol;
}

void Entity::Patrol(float dt, float offsetX)
{
	if (!beginPatrol)
		return;

	float targetX = patrolRoute.x;
	float dx = targetX - rect.x;
	float distance = fabsf(dx);

	const float arrivalThreshold = 2.0f;

	if (distance <= arrivalThreshold)
	{
		// Snap to target
		rect.x = targetX;
		centerRect.x = targetX;
		destRect.x = targetX;

		// Swap patrol points
		std::swap(patrolRoute, startingPosition);

		// Flip once when changing direction
		Flip();

		return;
	}

	// Move direction: -1 or +1
	float dir = (dx > 0.0f) ? 1.0f : -1.0f;

	const float speed = 200.0f; // units per second

	rect.x += dir * speed * dt;
	centerRect.x += dir * speed * dt;
	destRect.x += dir * speed * dt;
	shadowDest.x += dir * speed * dt;
}

string Entity::ClickOption()
{
	return Options.getOption();
}

//void Entity::Remove(bool save)
//{
//	this->save = save;
//	try {
//		bool removed = fs::remove_all(unitPath);
//		if (removed) {
//			std::cout << unitPath <<" deleted successfully" << std::endl;
//		}
//		else {
//			// File didn't exist
//		}
//	}
//	catch (const fs::filesystem_error& err) {
//		// Handle error
//	}
//}

void Entity::Remove(bool save)
{
	this->save = save;

	std::cout << "[REMOVE] Trying to delete: " << unitPath << std::endl;
	std::cout << "[EXISTS] " << fs::exists(unitPath) << std::endl;

	try {
		auto count = fs::remove_all(unitPath);
		std::cout << "[REMOVED FILES] " << count << std::endl;
	}
	catch (const fs::filesystem_error& err) {
		std::cerr << err.what() << std::endl;
	}
}

void Entity::UpdateShadowRect()
{
	shadowSource = sourceRect;
	shadowSource.height = -sourceRect.height;
	/*if (flipped) shadowSource.width = sourceRect.width * (-1.f);*/

	shadowDest = {
		destRect.x,
		rect.y + destRect.height - 5.0f,
		destRect.width,
		destRect.height * 0.2f       // squashed flat
	};
	//shadowRect.x += shadowRect.width / 2.0f;
	shadowDest.y += shadowDest.height / 2.0f;
	shadowOrigin = { shadowDest.width / 2,  shadowDest.height / 2 };
}

//void Entity::UpdateShadowRect()
//{
//	// ---- CONFIG ----
//	Vector2 lightDir = { 1.f, 0.f };   // light from right + behind
//	const float shadowLength = 18.0f;
//	const float shadowStretch = 1.2f;
//	const float shadowSquash = 0.25f;
//
//	// Normalize light direction
//	float len = sqrtf(lightDir.x * lightDir.x + lightDir.y * lightDir.y);
//	if (len != 0.0f)
//	{
//		lightDir.x /= len;
//		lightDir.y /= len;
//	}
//
//	// Shadow goes opposite direction
//	Vector2 shadowOffset = {
//		-lightDir.x * shadowLength,
//		-lightDir.y * shadowLength
//	};
//
//	shadowRect = {
//		destRect.x + shadowOffset.x,
//		destRect.y + rect.height / 2.0f + shadowOffset.y,
//		destRect.width * shadowStretch,
//		rect.height * shadowSquash
//	};
//
//	shadowRect.y -= shadowRect.height / 2.0f;
//
//	shadowOrigin = {
//		shadowRect.width / 2,
//		shadowRect.height / 2
//	};
//}

//void Entity::UpdateShadowRect()
//{
//	// Shadow is orthogonal to character (90°)
//	// Character = vertical, shadow = horizontal (ground)
//
//	const float shadowOffsetX = -20.0f;   // LEFT only
//	const float shadowHeight = rect.height * 0.25f;
//	const float shadowWidth = destRect.width * 1.2f;
//
//	// Anchor shadow exactly at the feet
//	float groundY = rect.y + rect.height;
//
//	shadowRect = {
//		destRect.x + shadowOffsetX,
//		groundY - shadowHeight / 2.0f,
//		shadowWidth,
//		shadowHeight
//	};
//
//	shadowOrigin = {
//		shadowRect.width / 2,
//		shadowRect.height / 2
//	};
//}

void Entity::Front()
{
	Remove(true);
	position++;

	unitName = to_string(position) + type + to_string(id);
	unitPath = dataPath / unitName; //update path to point to the exact unit
	coordinatesPath = unitPath / "coordinates" / (unitName + ".txt");
	dialoguePath = unitPath / "dialogue" / (unitName + ".txt");
}

void Entity::Back()
{
	Remove(true);
	position--;
	unitName = to_string(position) + type + to_string(id);
	unitPath = dataPath / unitName; //update path to point to the exact unit
	coordinatesPath = unitPath / "coordinates" / (unitName + ".txt");
	dialoguePath = unitPath / "dialogue" / (unitName + ".txt");
}

void Entity::MoveRight()
{

}

void Entity::MoveLeft()
{

}

void Entity::MoveRight(int scrollSpeed, float dt)
{
	rect.x -= scrollSpeed * GLOBAL_SPEED *dt;
	centerRect.x -= scrollSpeed * GLOBAL_SPEED * dt;
	destRect.x -= scrollSpeed * GLOBAL_SPEED * dt;
	patrolRoute.x -= scrollSpeed * GLOBAL_SPEED * dt;
	startingPosition.x -= scrollSpeed * GLOBAL_SPEED * dt;
	shadowDest.x -= scrollSpeed * GLOBAL_SPEED * dt;
	//startingPosition.x -= scrollSpeed * GLOBAL_SPEED * dt;
	//patrolRoute.x -= scrollSpeed * GLOBAL_SPEED * dt;
	Options.updateGrid(rect);
	UpdateTextRect();
	//UpdateShadowRect();
}

void Entity::MoveLeft(int scrollSpeed, float dt)
{
	rect.x += scrollSpeed * GLOBAL_SPEED * dt;
	centerRect.x += scrollSpeed * GLOBAL_SPEED * dt;
	destRect.x += scrollSpeed * GLOBAL_SPEED * dt;
	patrolRoute.x += scrollSpeed * GLOBAL_SPEED * dt;
	startingPosition.x += scrollSpeed * GLOBAL_SPEED * dt;
	shadowDest.x += scrollSpeed* GLOBAL_SPEED* dt;
	//startingPosition.x += scrollSpeed * GLOBAL_SPEED * dt;
	//patrolRoute.x += scrollSpeed * GLOBAL_SPEED * dt;
	Options.updateGrid(rect);
	UpdateTextRect();
	//UpdateShadowRect();

}

void Entity::UpdateTextRect()
{
	textRect = Rectangle{ centerRect.x, rect.y, 20, 40 };

	backButton.SetLeftOfRect(textRect);
	addButton.SetLeftOfRect(backButton.GetRect());
	if (!dialogueBoxes.empty())
	{
		for (auto& dialogue : dialogueBoxes)
			dialogue.UpdateRect(centerRect);
	}
	cursorX = textRect.x + 6;
	cursorWidth = textRect.x + 6;
}

void Entity::CreateDirectories(string& unitName) const
{

	if (!fs::exists(this->unitPath / "coordinates")) {
		fs::create_directories(this->unitPath / "coordinates");
	}
	if (!fs::exists(this->unitPath / "dialogue")) {
		fs::create_directories(this->unitPath / "dialogue");
	}
}

void Entity::UnloadMediaPortrait()
{
	mediaPortrait.reset();

}

//Getters

float Entity::ID() const
{
	return id;
}

float Entity::X() const
{
	return x;
}

float Entity::Y() const
{
	return y;
}

int Entity::Position() const
{
	return position;
}

Texture2D* Entity::GetPortrait() const
{
	return portrait;
}

MediaStream* Entity::GetMediaPortrait()
{
	if(!mediaPortrait) mediaPortrait = textureManager->UnitMedia(this->type + "_" + to_string(color));
	return mediaPortrait.get();
}

Texture2D* Entity::GetDialoguePortrait() const
{
	if(animatedPortrait == nullptr)
		return dialoguePortrait;

	return animatedPortrait;
}

string Entity::Name() const
{
	return name;
}

vector<TextBox> Entity::Dialogue() const
{
	return dialogueBoxes;
}

Rectangle Entity::Rect() const
{
	return rect;
}

Rectangle Entity::DestRect() const
{
	return destRect;
}

Rectangle Entity::CenterRect() const
{
	return centerRect;
}

Vector2 Entity::Center() const
{
	return center;
}

string Entity::Type() const
{
	return type;
}

Utilities::Direction Entity::Direction() const
{
	return direction;
}

bool Entity::Flipped() const
{
	return flipped;
}

vector<Button>* Entity::GetButtons()
{
	cout<<"RETURN BUTTONS\n";
	return &buttons;
}

bool& Entity::HasCollidedWithDante() //Setter masked as getter
{
	
	return hasCollidedWithDante;
}

bool& Entity::ShowDialogue() //Setter masked as getter
{
	return showDialogue;
}

int& Entity::DialogueCounter() //Setter masked as getter
{
	return dialogueCounter;
}

bool Entity::HasDialogue() const
{
	return !dialogueBoxes.empty();
}

bool Entity::HasAnimatedPortrait() const
{
	return (mediaPortrait != nullptr);
}

//Setters
void Entity::SetScrollSpeed(int scrollSpeed)
{
	this->scrollSpeed = scrollSpeed;
}

void Entity::SetX(float x)
{
	this->x = x;
	rect.x = x - center.x;
	centerRect.x = x;
	destRect.x = x;
}

void Entity::SetX(float x, float offsetX)
{
	this->x = x + offsetX;
	rect.x = x - center.x;
	centerRect.x = x;
	destRect.x = x;
	startingPosition.x = x;

	//center.x = x;
}

void Entity::SetY(float y)
{
	this->y = y;
	rect.y = y - center.y;
	centerRect.y = y;
	destRect.y = y;
	startingPosition.y = y;

	//center.y = y;
}

void Entity::ScaleDestRect(float scalingFactor)
{
	destRect = Rectangle{ destRect.x, destRect.y, destRect.width * scalingFactor, destRect.height * scalingFactor };
	center = Vector2{ static_cast<float>(destRect.width / 2), static_cast<float> (destRect.height / 2) };
	rect = Rectangle{ this->x - center.x,this->y - center.y,destRect.width,destRect.height };
}

void Entity::SetPatrolDestination(Vector2 patrolRoute, int offsetX)
{
	this->patrolRoute = { patrolRoute.x, patrolRoute.y };
	this->patrolDestination = { this->patrolRoute.x + offsetX,  this->patrolRoute.y };
	hasRoute = true;
	cout<<"Starting position: X: " << this->startingPosition.x << " Y: " << this->startingPosition.y << endl;
	cout << "Patrol Route: X: " << this->patrolDestination.x << " Y: " << this->patrolDestination.y << endl;
	DeactivateSetPatrolRoute();
}

void Entity::SetEngage(bool& engage)
{
	this->engage = &engage;
}