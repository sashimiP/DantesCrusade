#include "StateAlteringCard.h"

StateAlteringCard::StateAlteringCard(shared_ptr<TextureManager> textureManager) :
	SpellCard(textureManager)
{
	cardType = Utilities::CardType::StateAltering;
	spellAnimation = make_shared<AnimatedAsset>();
}

void StateAlteringCard::SetCard(string type, float x, float y)
{
	this->type = type;
	texture = textureManager->Card(type + "_0");
	manaCost = Utilities::spellStats[type].mana;
	statsModifier = Utilities::spellStats[type].statsModifier;
	statsToModify = Utilities::spellStats[type].statsToModify;
	subType = spellStats[type].stateAltering->subType;
	state = spellStats[type].stateAltering->state;
	stateModifier = spellStats[type].stateAltering->stateModifier;
	description = spellStats[type].description;

	if (!texture)
	{
		cout << "TEXTURE NOT FOUND " << type << endl;
		return;
	}

	float texW = static_cast<float>(texture->width);
	float texH = static_cast<float>(texture->height);

	float scale = std::min(120.f / texW, 180.f / texH);

	baseX = x;
	baseY = y;
	baseWidth = texW * scale;
	baseHeight = texH * scale;

	this->width = baseWidth;        // current size
	this->height = baseHeight;
	this->x = x;
	this->y = y;


	source = {
		0.0f,
		0.0f,
		texW,
		texH
	};


	destination = {
		this->x,
		this->y,
		baseWidth,
		baseHeight
	};

	name = Utilities::spellStats[type].name;
	defaultFontSize = 10;
	UpdateNameBanner();

	hitbox = destination;

	center = { 0.0f, 0.0f };
	mousedOver = false;

	auto spell = textureManager->Asset(type);

	if (spell)
		spellAnimation = make_shared<AnimatedAsset>(type, Utilities::spellStats[type].frames, textureManager);

	UpdateIcons();
	UpdateDescription();
}

void StateAlteringCard::Drawn(string type, float x, float y)
{
	this->type = type;
	texture = textureManager->Card(type + "_0");
	manaCost = Utilities::spellStats[type].mana;
	statsModifier = Utilities::spellStats[type].statsModifier;
	statsToModify = Utilities::spellStats[type].statsToModify;
	subType = spellStats[type].stateAltering->subType;
	state = spellStats[type].stateAltering->state;
	stateModifier = spellStats[type].stateAltering->stateModifier;
	description = spellStats[type].description;

	if (!texture)
	{
		cout << "TEXTURE NOT FOUND " << type << endl;
		return;
	}

	float texW = static_cast<float>(texture->width);
	float texH = static_cast<float>(texture->height);

	float scale = std::min(240.f / texW, 360.f / texH);

	baseX = x;
	baseY = y;
	baseWidth = texW * scale;
	baseHeight = texH * scale;

	this->width = baseWidth;        // current size
	this->height = baseHeight;
	this->x = x;
	this->y = y;


	source = {
		0.0f,
		0.0f,
		texW,
		texH
	};


	destination = {
		this->x,
		this->y,
		baseWidth,
		baseHeight
	};

	name = Utilities::spellStats[type].name;
	defaultFontSize = 10;

	UpdateNameBanner();

	hitbox = destination;

	center = { 0.0f, 0.0f };         // we will compensate for scale ourselves
	mousedOver = false;

	auto spell = textureManager->Asset(type);

	if (spell)
		spellAnimation = make_shared<AnimatedAsset>(type, Utilities::spellStats[type].frames, textureManager);

	UpdateIcons(2.5f);
	UpdateDescription();
}

void StateAlteringCard::PlayCard(BattleEntity& target)
{
	for (Stat s : statsToModify)
	{
		if (subType == CardType::Debuff)
		{
			cout << "DEBUFF" << endl;
			if (s == Stat::Health)
			{
				target.ActivateGetHit(false, statsModifier);
				cout << "ACTIVATE GET HIT: " << statsModifier << endl;
			}
			
			else
			{target.SubStat(s, statsModifier);
			cout << "SUB STAT: " << statsModifier << endl;
			}
		}
		else if(subType == CardType::Buff)
			target.AddStat(s, statsModifier);
	}
	cout << "ALTER STATE: " << type << endl;
	target.AlterState(state, stateModifier);
}