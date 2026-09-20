#pragma once

#include <string>
#include <vector>
#include<iostream>
#include <functional>

#include"TextureManager.h"
#include "Utilities.h"

using std::string, std::vector, std::map, std::unordered_map, std::tuple, std::pair, std::cout, std::endl,
std::to_string, std::shared_ptr, std::make_shared, std::unique_ptr, std::make_unique;

class AnimatedAsset
{
public:
	AnimatedAsset() = default;
	AnimatedAsset(string type, int frames, shared_ptr<TextureManager> textureManager);

	~AnimatedAsset() = default;

	void SetPosition(float x, float y);
	void SetDest(Rectangle dest);
	void SetCenterDest(float x, float y, float width, float height);
	void Update();
	void Draw(Color color = RAYWHITE);
	void DrawOnce();

	Rectangle GetDest();
	bool GetDrawOnce();
	Texture2D* GetTexture();

	void SetDrawOnce(bool drawOnce, float delayTimer = 0.0f, std::function<void()> callback = nullptr);
	std::function<void()> onFinished;
private:

	shared_ptr<TextureManager> textureManager;
	string type;
	int frames;
	Texture2D* texture = nullptr;
	float animationTimer{ 0.0f };
	int currentFrame{ 0 };
	float frameWidth;
	Rectangle sourceRect;
	Rectangle destRect;
	Vector2 center{ 0.0f, 0.0f };
	int animationSpeed = 10.0f;

	

	bool drawOnce = false;
	float delayTimer = 0.0f;
};

