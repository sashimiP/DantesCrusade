#include "AnimatedAsset.h"

AnimatedAsset::AnimatedAsset(string type, int frames, shared_ptr<TextureManager> textureManager) :
	textureManager(textureManager),
	type(std::move(type)),
	frames(frames)
{
	texture = textureManager->Asset(this->type);

	frameWidth = static_cast<float> (texture->width) / frames;

	sourceRect = Rectangle{ 0.0f, 0.0f, frameWidth, static_cast<float> (texture->height) };
	destRect = sourceRect;
}

void AnimatedAsset::SetPosition(float x, float y)
{
	destRect = { x, y, frameWidth,  sourceRect.height };
}

void AnimatedAsset::SetDest(Rectangle dest)
{
	destRect = dest;
}

void AnimatedAsset::SetCenterDest(float x, float y, float width, float height)
{
	destRect = {
		x - width * 0.5f,
		y - height * 0.5f,
		width,
		height
	};
}

void AnimatedAsset::Update()
{
	if (texture == nullptr) return;

	float dt = GetFrameTime();
	animationTimer += dt;
	if (animationTimer >= (1.0f / animationSpeed)) {
		animationTimer = 0.0f;
		currentFrame = (currentFrame + 1) % frames;
		sourceRect.x = static_cast<float>(currentFrame * frameWidth);
	}

}

void AnimatedAsset::Draw(Color color)
{
	if (texture == nullptr) return;

	DrawTexturePro(
		*texture,
		{ sourceRect.x, sourceRect.y + 1, sourceRect.width, sourceRect.height }, // temp fix
		destRect,
		center,
		0.0f,
		color
	);
}

void AnimatedAsset::DrawOnce()
{
	if (!drawOnce) return;

	delayTimer -= GetFrameTime();

	if (delayTimer > 0.0f)
		return;

	float dt = GetFrameTime();
	animationTimer += dt;
	if (animationTimer >= (1.0f / animationSpeed)) {
		animationTimer = 0.0f;
		currentFrame = (currentFrame + 1) % frames;
		sourceRect.x = static_cast<float>(currentFrame * frameWidth);


		if (currentFrame == frames-1)
		{
			if (onFinished)
				onFinished();
			drawOnce = false;
			return;
		}
	}

	DrawTexturePro(
		*texture,
		{ sourceRect.x, sourceRect.y + 1, sourceRect.width, sourceRect.height }, // temp fix
		destRect,
		center,
		0.0f,
		RAYWHITE
	);
}

Rectangle AnimatedAsset::GetDest()
{
	return destRect;
}

bool AnimatedAsset::GetDrawOnce()
{
	return drawOnce;
}

Texture2D* AnimatedAsset:: GetTexture()
{
	return texture;
}

void AnimatedAsset::SetDrawOnce(bool drawOnce, float delayTimer, std::function<void()> callback)
{
	this->delayTimer = delayTimer;
	this->drawOnce = drawOnce;
	/*currentFrame = 0;
	animationTimer = 0.0f;*/
	onFinished = std::move(callback);

}