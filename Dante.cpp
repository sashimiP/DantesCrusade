#include "Dante.h"

float Dante::armor{ 10 };
float Dante::holiness{ 5 };
float Dante::attack{ 10 };
float Dante::food{ 10 };
float Dante::gold{ 10 };


Dante::Dante(fs::path coordinatesPath,
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
	int color):
	Entity(coordinatesPath,
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
		color)
{
	portrait = textureManager->Portrait(this->type);

}

Dante::~Dante()
{
	//cout << "Dante destructor called" << endl;
}


void Dante::Idle()
{
	SetStance("idle");
}

void Dante::MoveRight()
{
	if (drag) return;
	if (direction==west) Flip();
}

void Dante::MoveLeft()
{
	if (drag) return;
	if (direction==east) Flip();
}

void Dante::SetStance(const string& stance)
{ 
	this->stance = stance;

	spriteSheet = &(*texturesMap)[stance];
	texture = &spriteSheet->texture;

	framesCount = spriteSheet->frames;
	auto colorCount = spriteSheet->colors;
	//cout << "NEW FRAMES COUNT: " << framesCount << endl;
	frameWidth = spriteSheet->texture.width / framesCount;
	frameHeight = spriteSheet->texture.height / colorCount;

	sourceRect = Rectangle{ 0,0+(frameHeight*color),static_cast<float>(frameWidth),static_cast<float> (frameHeight) };
	destRect = Rectangle{ x, y,static_cast<float>(frameWidth),static_cast<float> (frameHeight) };
	center = Vector2{ static_cast<float>(frameWidth / 2), static_cast<float> (frameHeight / 2) };
	rect = Rectangle{ x - center.x, y - center.y,sourceRect.width,sourceRect.height };

	if (direction==west) sourceRect.width *= (-1); // Must find a better way

	centerRect = Rectangle{ x, y, 5, 5 };
	Options.updateGrid(rect);
}

void Dante::Display(int animationSpeed, int offsetX, float dt)
{

	if (showRect) DrawRectangleLines(rect.x, rect.y, rect.width, rect.height, RED);


	if (showAssetOptions) Options.drawOptionCells();

	//FrameData& spriteSheet = (*texturesMap)[stance];
	//int frameCount = spriteSheet.frames;         // total number of frames
	//Texture2D& texture = spriteSheet.texture;     // sprite sheet

	animationTimer += dt;
	if (animationTimer >= (1.0f / animationSpeed)) {
		animationTimer = 0.0f;
		currentFrame = (currentFrame + 1) % framesCount;
		sourceRect.x = static_cast<float>(currentFrame * frameWidth);
		shadowSource.x = static_cast<float>(currentFrame * frameWidth);
	}
	DrawShadow(ANIMATION_SPEED, dt);
	DrawTexturePro(
		*texture,
		sourceRect,
		destRect,
		center,
		0,
		RAYWHITE
	);

	if (!hasCollidedWithUnit && !marching)
	{
		string temp = "x: " + std::to_string(static_cast<int>(x));

		DrawText(temp.c_str(), rect.x, rect.y, 20, WHITE);

		temp = "y: " + std::to_string(static_cast<int>(y));
		DrawText(temp.c_str(), rect.x + 100, rect.y, 20, WHITE);
	}
	DrawRectangleRec(centerRect, GREEN);// draw the center point
	
	//if (engagedInDialogue && hasCollidedWithUnit)
	//	DrawPortrait(east);
}

//void Dante::DrawPortrait(Utilities::Direction facing, bool dialogueBox) const
//{
//	if (portrait == nullptr) return;
//
//	auto square_frame = textureManager->Asset("square_frame", portrait->width, portrait->height);
//	auto horizontal_frame = textureManager->Asset("horizontal_frame", SCR_WIDTH - portrait->width * 2, portrait->height);
//
//	Rectangle squareFrameSourceRect{}
//
//	int posX{ 0 };
//	int posY{ 0 };
//
//	if (facing == east)
//	{
//		posX = SCR_WIDTH - portrait->width;
//	}
//
//	// Background behind the portrait
//	DrawRectangle(posX, posY, portrait->width, portrait->height, Color{ 0, 0, 0, 150 });
//
//	if (dialogueBox)
//	{
//		DrawRectangle(portrait->width, 0, horizontal_frame->width, horizontal_frame->height, Color{ 0, 0, 0, 160 });
//	}
//
//	// Define source rectangle (flipped or normal)
//	Rectangle sourceRec = {
//		(facing == east ? (float)portrait->width : 0.0f),
//		0.0f,
//		(facing == east ? portrait->width : -portrait->width),
//		(float)portrait->height
//	};
//
//	Rectangle destRec = {
//		(float)posX,
//		(float)posY,
//		(float)portrait->width,
//		(float)portrait->height
//	};
//
//	Vector2 origin = { 0.0f, 0.0f };
//
//	// Display flipped (if needed) portrait
//	DrawTexturePro(*portrait, sourceRec, destRec, origin, 0.0f, RAYWHITE);
//
//	// Display portrait frame
//	DrawTexture(*square_frame, posX, posY, RAYWHITE);
//
//	// Display dialogue box frame (if needed)
//	if (dialogueBox)
//	{
//		DrawTexture(*horizontal_frame, portrait->width, 0, RAYWHITE);
//	}
//}

//void Dante::DrawPortrait(Utilities::Direction facing, bool dialogueBox) const
//{
//	if (portrait == nullptr) return;
//
//	auto square_frame = textureManager->Asset("square_frame");
//	auto horizontal_frame = textureManager->Asset("horizontal_frame");
//
//	int posX{ 0 };
//	int posY{ 0 };
//
//	if (facing == east)
//	{
//		posX = SCR_WIDTH - portraitWidth;
//	}
//
//	Rectangle squareFrameSourceRect{ 0, 0, square_frame->width, square_frame->height };
//	Rectangle squareFrameDestRect{ posX, posY, portraitWidth, portraitHeight };
//
//	Rectangle horizontalFrameSourceRect{ 0, 0, horizontal_frame->width, horizontal_frame->height };
//	Rectangle horizontalFrameDestRect{ portraitWidth, 0, SCR_WIDTH - portraitWidth * 2, portraitHeight };
//
//	// Background behind the portrait
//	DrawRectangle(posX, posY, portraitWidth, portraitHeight, Color{ 0, 0, 0, 150 });
//
//	if (dialogueBox)
//	{
//		DrawRectangle(portraitWidth, 0, horizontalFrameDestRect.width, horizontalFrameDestRect.height, Color{ 0, 0, 0, 160 });
//	}
//
//	// Define source rectangle (flipped or normal)
//	Rectangle sourceRec = {
//		(facing == east ? (float)portrait->width : 0.0f),
//		0.0f,
//		(facing == east ? portrait->width : -portrait->width),
//		(float)portrait->height
//	};
//
//	Rectangle destRec = {
//		(float)posX,
//		(float)posY,
//		(float)portraitWidth,
//		(float)portraitHeight
//	};
//
//	Vector2 origin = { 0.0f, 0.0f };
//
//	// Display flipped (if needed) portrait
//	DrawTexturePro(*portrait, sourceRec, destRec, origin, 0.0f, RAYWHITE);
//
//	// Display portrait frame
//	DrawTexturePro(*square_frame, squareFrameSourceRect, squareFrameDestRect, origin, 0.0f, RAYWHITE);
//
//	// Display dialogue box frame (if needed)
//	if (dialogueBox)
//	{
//		DrawTexturePro(*horizontal_frame, horizontalFrameSourceRect, horizontalFrameDestRect, origin, 0.0f, RAYWHITE);
//	}
//}

bool& Dante::HasCollidedWithUnit()
{
	return hasCollidedWithUnit;
}

bool& Dante::EngagedInDialogue()
{
	return engagedInDialogue;
}