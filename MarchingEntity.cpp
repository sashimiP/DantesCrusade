#include "MarchingEntity.h"

MarchingEntity::MarchingEntity(fs::path unitPath,
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
	int color) :
	Entity(unitPath,
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

}

void MarchingEntity::Display(int animationSpeed, int offsetX, float dt)
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
	}

	DrawTexturePro(
		*texture,
		sourceRect,
		destRect,
		center,
		0,
		RAYWHITE
	);
}

void MarchingEntity::MoveRight()
{
	//if (drag) return;
	if (direction == west) Flip();
}

void MarchingEntity::MoveLeft()
{
	//if (drag) return;
	if (direction == east) Flip();
}

void  MarchingEntity::Adjust(Rectangle rect)
{
	auto adjustX = rect.x;
	auto adjustY = (rect.y + rect.height/2.0f) - this->destRect.height/2.0f;

	SetX(adjustX);
	SetY(adjustY);

	if (direction != Utilities::Direction::east)
		Flip();
}
