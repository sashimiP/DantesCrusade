#pragma once

#include <string>
#include <vector>
#include<iostream>

#include"TextureManager.h"
#include "Utilities.h"
#include "AnimatedAsset.h"

using std::string, std::vector, std::map, std::unordered_map, std::tuple, std::pair, std::cout, std::endl,
std::to_string, std::shared_ptr, std::make_shared, std::unique_ptr, std::make_unique, Utilities::CardType, Utilities::Stat, Utilities::DrawTextOutlined;
using Utilities::entityStats;

class Card
{
public:
	Card() = default;
	Card(shared_ptr<TextureManager> textureManager);
	void LoadCustomFont(string font);

	virtual ~Card() = default;

	virtual void SetCard(string type, float x, float y);
	virtual void SetCard(float x, float y);
	void Update();

	virtual void Drawn(string type, float x, float y);
	void MousedOver();
	void PlaceInHand();
	void Select();
	void Deselect();

	/*void DrawTextCurved(const char* text, Vector2 center, float radius, float maxAngleDeg,
		int fontSize, Color color);*/
	
	void UpdateBannerCurve();
	void UpdateNameBanner();
	virtual void UpdateIcons(float scale = 1.0f);

	virtual void Display();

	vector<std::string> WrapText(Font font, const std::string& text,
		float fontSize, float spacing, float maxWidth);

	void UpdateDescription();
	void DisplayDescription();

	void SetX(float x);
	void SetY(float y);
	void SetBaseX(float x);
	void SetBaseY(float y);
	void SetDest(Rectangle dest);

	void PrepForDisplay(float x, float y);

	float GetX();
	float GetBaseX();
	float GetBaseY();
	float GetWidth();
	Rectangle GetRectDest();
	bool GetMouseOver();
	string GetType() const;
	Utilities::CardType GetCardType() const;
	virtual Utilities::CardType GetSubType() const { return subType; }
	int GetManaCost() const;

protected:

	Utilities::CardType cardType;
	CardType subType = CardType::Unknown;

	shared_ptr<TextureManager> textureManager;

	string type;
	Texture2D* texture;
	float x;
	float y;
	Rectangle source;
	Rectangle destination;
	Vector2 center;
	Vector2 grabOffset = { 0.0f, 0.0f };

	//Description variables
	std::string description;
	std::vector<std::string> descriptionLines;
	int descriptionFontSize = 14;

	float width;
	float height;

	Texture2D* squareFrame;
	Rectangle squareFrameSourceRect;

	unique_ptr<AnimatedAsset> animatedSquareFrame;

	Rectangle hitbox;

	float baseX = 0.0f;
	float baseY = 0.0f;
	float baseWidth = 120.f;
	float baseHeight = 180.f;

	bool mousedOver{ false };
	bool chosen{ false };
	bool selected{ false };
	bool inDeck{false};
	bool inHand{ false };

	Texture2D* nameBanner;
	Rectangle nameBannerSouce;
	Rectangle nameBannerDest;

	Font customFont;
	string name;
	Vector2 namePos;
	int defaultFontSize;
	float charSpacing = 2.0f;
	int outlineSize = 2;
	Vector2 bannerCenter{ 0.0f, 0.0f };
	float bannerRadius = 0.0f;
	float bannerArcDeg = 55.0f;

	Texture2D* healthIcon = nullptr;
	Rectangle healthIconSource;
	Rectangle healthIconDest;

	Texture2D* damageIcon = nullptr;
	Rectangle damageIconSource;
	Rectangle damageIconDest;

	Texture2D* manaCostIcon = nullptr;
	Rectangle manaIconSource;
	Rectangle manaIconDest;

	bool taunt{ false };
	Texture2D* tauntIcon = nullptr;
	Rectangle tauntIconSource;
	Rectangle tauntIconDest;

	bool evade{ false };
	Texture2D* evadeIcon = nullptr;
	Rectangle evadeIconSource;
	Rectangle evadeIconDest;

	// Stats
	int statsFontSize;
	Font statsFont;

	int health;
	Vector2 healthPos;

	int damage;
	Vector2 damagePos;

	int manaCost;
	Vector2 manaCostPos;

};

