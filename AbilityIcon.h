#pragma once
#include <functional>
#include <string>
#include <iostream>
#include "raylib.h"

#include "TextureManager.h"
#include"AnimatedAsset.h"
#include "BattleEntity.h"

using std::string, std::move, std::unique_ptr, std::function;
using Utilities::Stat, Utilities::heroAbilities, Utilities::CardType;
using Utilities::DrawTextOutlined;

class AbilityIcon
{
public:
	AbilityIcon() = default;
	AbilityIcon(string heroType, shared_ptr<TextureManager> textureManager);
	~AbilityIcon();
	void Update();
	void LeftMouseButtonPressed();

	void ClickIcon();
	void SetVisualEffect(string& type, int& frames);
	function<void()> onClick;
	
	
	void Display();
	void DisplayVisualEffect();
	

	Rectangle GetRect();
	bool GetSeekTarget();

	void SetPosition(float& x, float& y);
	void SetIcon(string& type, int& frames);
	void SetDestRect(Rectangle rect);
	void SetTarget(BattleEntity* entity, Rectangle targetRect, std::function<void()> callback=nullptr);

	bool GetAbilityUsed();
	int GetStatModifier();

	void ResetAbilityUsed();
	void Deselect();

private:

	shared_ptr<TextureManager> textureManager;
	string heroType;
	CardType abilityType;
	
	bool abilityUsed{ false };
	Texture2D* abilityUsed_texture= nullptr;
	Rectangle abilityUsedSource;


	unique_ptr<AnimatedAsset> icon;
	Rectangle destRect;

	string ghostType;
	unique_ptr<AnimatedAsset> iconGhost;

	Texture2D* iconStatic = nullptr;
	Rectangle iconStaticSource;

	Texture2D* frame = nullptr;
	Rectangle frameSource;
	Rectangle frameDest;

	int manaCost;
	Vector2 manaCostPos;

	Texture2D* manaCostIcon = nullptr;
	Rectangle manaIconSource;
	Rectangle manaIconDest;
	float manaCostFontSize = 15.f;
	float manaCostScale = 1.5f;

	bool seekTarget{ false };


	Stat statToModify;
	int statModifier;
	unique_ptr<AnimatedAsset> visualEffect;
	BattleEntity* targetEntity = nullptr;

	Color manaCostColor = RAYWHITE;
};

