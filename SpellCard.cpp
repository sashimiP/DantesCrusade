#include "SpellCard.h"

SpellCard::SpellCard(shared_ptr<TextureManager> textureManager) :
	Card(textureManager)
{
	cardType = Utilities::CardType::Spell;
	spellAnimation = make_shared<AnimatedAsset>();
}

void SpellCard::SetCard(string type, float x, float y)
{
	this->type = type;
	texture = textureManager->Card(type + "_0");
	manaCost = Utilities::spellStats[type].mana;
	statsModifier = Utilities::spellStats[type].statsModifier;
	statsToModify = Utilities::spellStats[type].statsToModify;
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

	UpdateNameBanner();

	hitbox = destination;

	center = { 0.0f, 0.0f };
	mousedOver = false;

	auto spell = textureManager->Asset(type);

	if(spell)
		spellAnimation = make_shared<AnimatedAsset> (type, Utilities::spellStats[type].frames, textureManager);
	
	UpdateIcons();
	UpdateDescription();
}

void SpellCard::SetCard(float x, float y)
{

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

	destination = {
		this->x,
		this->y,
		baseWidth,
		baseHeight
	};

	/*nameBannerDest = {
		this->x,
		this->y - nameBannerSouce.height / 2,
		baseWidth,
		nameBannerSouce.height
	};*/

	name = Utilities::spellStats[type].name;

	UpdateNameBanner();
	UpdateIcons();
	UpdateDescription();

	hitbox = destination;

	center = { 0.0f, 0.0f };         // we will compensate for scale ourselves
	mousedOver = false;
}

void SpellCard::Display()
{
	//if (!inHand) return;

	if (selected)
	{
		animatedSquareFrame->Draw();
	}

	DrawTexturePro(
		*texture,
		source,
		destination,
		center,
		0.0f,
		RAYWHITE
	);

	DisplayDescription();

	DrawTexturePro(
		*squareFrame,
		squareFrameSourceRect,
		destination,
		center,
		0.0f,
		RAYWHITE
	);

	DrawTexturePro(
		*nameBanner,
		nameBannerSouce,
		nameBannerDest,
		center,
		0.0f,
		RAYWHITE
	);

	DrawTextOutlined(customFont, name.c_str(), namePos, defaultFontSize, charSpacing, RAYWHITE, BLACK, outlineSize);
	
	if(spellAnimation)
		spellAnimation->DrawOnce();

	DrawTexturePro(
		*manaCostIcon,
		manaIconSource,
		manaIconDest,
		center,
		0.0f,
		RAYWHITE
	);

	DrawTextOutlined(statsFont, to_string(manaCost).c_str(), manaCostPos, defaultFontSize, charSpacing, RAYWHITE, BLACK, outlineSize);
}

void SpellCard::Drawn(string type, float x, float y)
{
	this->type = type;
	texture = textureManager->Card(type + "_0");
	manaCost = Utilities::spellStats[type].mana;
	statsModifier = Utilities::spellStats[type].statsModifier;
	statsToModify = Utilities::spellStats[type].statsToModify;
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

void SpellCard::UpdateIcons(float scale)
{
	statsFontSize = 15 * scale;

	manaIconDest = {
		nameBannerDest.x + nameBannerDest.width * 0.5f,
		nameBannerDest.y,
		static_cast<float>(manaCostIcon->width * scale),
		static_cast<float>(manaCostIcon->height * scale)
	};

	manaIconDest.x -= manaIconDest.width * 0.5f;

	manaCostPos = { manaIconDest.x + manaIconDest.width * 0.5f - MeasureText(to_string(manaCost).c_str(), defaultFontSize) * 0.5f,
		manaIconDest.y + manaIconDest.height * 0.5f - defaultFontSize * 0.5f };

	
}

void SpellCard::SetSpellPosition(float x, float y)
{
	if (!spellAnimation) return;

	spellAnimation->SetPosition(x, y);
}

shared_ptr<AnimatedAsset> SpellCard::GetSpell()
{
	if (!spellAnimation) return nullptr;

	return spellAnimation;
}