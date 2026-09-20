#include "PatrolingEntity.h"

PatrolingEntity::PatrolingEntity(fs::path dataPath,
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
	bool dontScale) :
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
		dontScale)
{
	options = {
		"remove",
		"add name",
		"add dialogue",
		"flip",
		"=>",
		"<=",
		"scale",
		"dontScale",
		"setRoute"
	};

	Options.SetAssetOptions(options);
}

PatrolingEntity::~PatrolingEntity()
{	
	if (save) SaveCoordinates();
	save = false;
}

string PatrolingEntity::ClickAssetOption(float offsetX)
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
			cout << "Set Route 2" << endl;
			SetRoute();
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

void PatrolingEntity::Display(int animationSpeed, int offsetX, float dt)
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

	// Check if any part of the texture is on-screen
	if (eastEdge < 0 || westEdge > SCREEN_WIDTH || bottomEdge < 0 || topEdge > SCREEN_HEIGHT)
	{
		return; // Skip drawing if completely off-screen
	}
	//cout << "DRAW" << endl;

	if (showRect) DrawRectangleLines(rect.x, rect.y, rect.width, rect.height, RED);





	FrameData& frameData = (*texturesMap)[stance];
	Texture2D& texture = frameData.texture;     // sprite sheet

	animationTimer += dt;
	if (animationTimer >= (1.0f / animationSpeed)) {
		animationTimer = 0.0f;
		currentFrame = (currentFrame + 1) % framesCount;
		sourceRect.x = static_cast<float>(currentFrame * frameWidth);
	}

	DrawTexturePro(
		texture,
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

		if (dontScale)
		{
			temp = "Don't Scale";
			DrawText(temp.c_str(), rect.x + rect.width / 2, rect.y + rect.height, 20, WHITE);
		}

		DrawRectangleRec(centerRect, GREEN);	// draw the center point

		if (setRoute)
		{
			Vector2 mousePos = GetMousePosition();
			DrawText("->", mousePos.x - 20, mousePos.y - 20, 20, BLACK);

			const int fontSize = 20;
			const char* dash = "-";

			int dashWidth = MeasureText(dash, fontSize);

			float dx = mousePos.x - x;
			float dy = mousePos.y - y;
			float distance = sqrtf(dx * dx + dy * dy);

			int dashCount = static_cast<int>(distance / dashWidth);

			for (int i = 0; i < dashCount; i++)
			{
				DrawText(
					dash,
					static_cast<int>(x + dx / dashCount * i),
					static_cast<int>(y + dy / dashCount * i),
					fontSize,
					BLACK
				);
			}
		}

	}
	//if (dialogueCounter > 0)
	//{
	//	DrawPortrait(west, true);
	//}

}

void PatrolingEntity::SetRoute()
{
	setRoute = true;
}