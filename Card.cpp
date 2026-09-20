#include "Card.h"

Card::Card(shared_ptr<TextureManager> textureManager) :
	textureManager(textureManager)
{
	cardType = Utilities::CardType::Entity;
	width = 120.f;
	height = 180.f;
	defaultFontSize = 20;
	statsFontSize = 30;

	squareFrame = this->textureManager->Asset("square_frame");
	squareFrameSourceRect = Rectangle{ 0.0f, 0.0f, static_cast<float>(squareFrame->width), static_cast<float>(squareFrame->height) };

	animatedSquareFrame = make_unique<AnimatedAsset>("animate_square_frame", 10, textureManager);

	nameBanner = textureManager->Asset("name_banner");
	nameBannerSouce = { 0.0f,
		0.0f,
		static_cast<float>(nameBanner->width),
		static_cast<float>(nameBanner->height)
	};

	LoadCustomFont("forgore_regular");
	statsFont = GetFontDefault();

	healthIcon = textureManager->Asset("health_icon");
	healthIconSource = { 
		0.0f,
		0.0f,
		static_cast<float>(healthIcon->width),
		static_cast<float>(healthIcon->height)
	};

	healthIconDest = {
		0.0f,
		0.0f,
		static_cast<float>(healthIcon->width),
		static_cast<float>(healthIcon->height)
	};

	damageIcon = textureManager->Asset("damage_icon");
	damageIconSource = {
		0.0f,
		0.0f,
		static_cast<float>(damageIcon->width),
		static_cast<float>(damageIcon->height)
	};

	damageIconDest = {
		0.0f,
		0.0f,
		static_cast<float>(damageIcon->width),
		static_cast<float>(damageIcon->height)
	};

	manaCostIcon = textureManager->Asset("mana_stone");
	manaIconSource = {
		0.0f,
		0.0f,
		static_cast<float>(manaCostIcon->width),
		static_cast<float>(manaCostIcon->height)
	};

	manaIconDest = {
		0.0f,
		0.0f,
		static_cast<float>(manaCostIcon->width),
		static_cast<float>(manaCostIcon->height)
	};
}

void Card::LoadCustomFont(string font)
{
	fs::path fontPath = fs::path("resources") / "fonts" / (font + ".otf");
	customFont = LoadFont(fontPath.string().c_str());
	if (IsFontValid(customFont))
	{
		TraceLog(LOG_ERROR, "font not valid");
	}
}

void Card::SetCard(string type, float x, float y)
{
	this->type = type;
	texture = textureManager->Card(type + "_0");

	health = Utilities::entityStats[type].health;
	damage = Utilities::entityStats[type].damage;
	manaCost = Utilities::entityStats[type].rank;

	taunt = entityStats[type].taunting;

	evade = entityStats[type].evade;

	description = entityStats[type].description;

	if (taunt)
	{
		tauntIcon = textureManager->Asset("taunt_icon");
		tauntIconSource = {
			0.0f,
			0.0f,
			static_cast<float>(tauntIcon->width),
			static_cast<float>(tauntIcon->height)
		};
	}

	if (evade)
	{
		evadeIcon = textureManager->Asset("evade_icon");
		evadeIconSource = {
			0.0f,
			0.0f,
			static_cast<float>(evadeIcon->width),
			static_cast<float>(evadeIcon->height)
		};
	}

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

	name = Utilities::entityStats[type].name;
	
	UpdateNameBanner();
	
	hitbox = destination;

	center = { 0.0f, 0.0f };         // we will compensate for scale ourselves
	mousedOver = false;

	UpdateIcons();
	UpdateDescription();
}

void Card::SetCard(float x, float y)
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
	
	name = Utilities::entityStats[type].name;

	UpdateNameBanner();
	UpdateIcons();
	UpdateDescription();

	hitbox = destination;

	center = { 0.0f, 0.0f };         // we will compensate for scale ourselves
	mousedOver = false;
}

void Card::Update()
{
	if (selected)
	{
		Vector2 mouse = GetMousePosition();
		SetX(mouse.x - grabOffset.x);
		SetY(mouse.y - grabOffset.y);

		animatedSquareFrame->SetCenterDest(mouse.x, mouse.y, destination.width + 14, destination.height+5);
		animatedSquareFrame->Update();
	}
}

void Card::Drawn(string type, float x, float y)
{
	this->type = type;
	texture = textureManager->Card(type + "_0");
	health = Utilities::entityStats[type].health;
	damage = Utilities::entityStats[type].damage;
	manaCost = Utilities::entityStats[type].rank;

	taunt = entityStats[type].taunting;

	evade = entityStats[type].evade;

	description = entityStats[type].description;

	if (taunt)
	{
		tauntIcon = textureManager->Asset("taunt_icon");
		tauntIconSource = {
			0.0f,
			0.0f,
			static_cast<float>(tauntIcon->width),
			static_cast<float>(tauntIcon->height)
		};
	}

	if (evade)
	{
		evadeIcon = textureManager->Asset("evade_icon");
		evadeIconSource = {
			0.0f,
			0.0f,
			static_cast<float>(evadeIcon->width),
			static_cast<float>(evadeIcon->height)
		};
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

	name = Utilities::entityStats[type].name;

	UpdateNameBanner();
	UpdateIcons(2.5f);
	UpdateDescription();

	hitbox = destination;

	center = { 0.0f, 0.0f };         // we will compensate for scale ourselves
	mousedOver = false;

}

void Card::MousedOver()
{
	if (mousedOver) return;

	const float scale = 2.f;        // how much bigger
	float newW = baseWidth * scale;
	float newH = baseHeight * scale;

	// Keep the card centered horizontally and raise it
	destination.width = newW;
	destination.height = newH;
	destination.x = x - (newW - baseWidth) * 0.5f;
	destination.y = y - (newH - baseHeight) - 110.f;   // rise up

	hitbox = destination;
	hitbox.height += 70.f;
	mousedOver = true;

	/*nameBannerDest = {
	destination.x,
	destination.y,
	destination.width,
	nameBannerSouce.height * (destination.width/ baseWidth)
	};

	nameBannerDest.y -= nameBannerDest.height/2;*/

	UpdateNameBanner();
	UpdateIcons(2.5f);
	UpdateDescription();
}

void Card::PlaceInHand()
{
	if (!mousedOver) return;

	destination = { x, y, baseWidth, baseHeight };
	hitbox = destination;

	/*nameBannerDest = {
		this->x,
		this->y - nameBannerSouce.height/2,
		baseWidth,
		nameBannerSouce.height
	};*/

	UpdateNameBanner();
	UpdateIcons();
	UpdateDescription();

	mousedOver = false;
}

void Card::Select()
{
	if (selected) return;

	cout << "CARD SELECTED: " << type << endl;
	destination = { x, y, baseWidth, baseHeight };
	hitbox = destination;

	/*nameBannerDest = {
		this->x,
		this->y - nameBannerSouce.height / 2,
		baseWidth,
		nameBannerSouce.height
	};*/

	UpdateNameBanner();
	UpdateIcons();
	UpdateDescription();

	mousedOver = false;

	grabOffset.x = destination.width * 0.5f;
	grabOffset.y = destination.height * 0.5f;

	selected = true;
}

void Card::Deselect()
{
	if (!selected) return;

	x = baseX;
	y = baseY;
	destination = { x, y, baseWidth, baseHeight };

	/*nameBannerDest = {
		this->x,
		this->y - nameBannerSouce.height / 2,
		baseWidth,
		nameBannerSouce.height
	};*/

	UpdateNameBanner();
	UpdateIcons();
	UpdateDescription();

	hitbox = destination;
	selected = false;
}

//void Card::DrawTextCurved(const char* text, Vector2 center, float radius, float maxAngleDeg,
//	int fontSize, Color color)
//{
//	int length = TextLength(text);
//	if (length == 0) return;
//
//	float totalWidth = (float)MeasureText(text, fontSize);
//	float maxAngle = maxAngleDeg * DEG2RAD;
//	float startAngle = -maxAngle * 0.5f;
//
//	float currentX = 0.0f;
//
//	for (int i = 0; i < length; i++)
//	{
//		char c[2] = { text[i], '\0' };
//		float charWidth = (float)MeasureText(c, fontSize);
//
//		float t = (currentX + charWidth * 0.5f) / totalWidth; // 0 -> 1
//		float angle = startAngle + t * maxAngle;
//
//		float nx = (t - 0.5f) * 2.0f;                       // -1 -> +1
//
//		// Small arch so the text stays in the middle of the banner
//		float archAmount = nameBannerDest.height * 0.12f;
//		float archY = -archAmount * (1.0f - nx * nx);
//
//		float x = center.x + nx * (nameBannerDest.width * 0.40f);
//		float y = center.y + archY;
//
//		DrawTextPro(GetFontDefault(), c,
//			{ x, y },
//			{ charWidth * 0.5f, fontSize * 0.5f },
//			angle * RAD2DEG * 0.6f,                 // less tilt looks better
//			(float)fontSize,
//			1.0f,
//			color);
//
//		currentX += charWidth;
//	}
//}

void Card::UpdateBannerCurve()
{
	bannerCenter = {
		nameBannerDest.x + nameBannerDest.width * 0.5f,
		nameBannerDest.y + nameBannerDest.height * 0.55f
	};

}

void Card::UpdateNameBanner()
{
	// Banner size relative to the current card size
	float bannerHeight = destination.height * 0.28f;
	float bannerWidth = destination.width * 1.05f;

	nameBannerDest = {
		destination.x + (destination.width - bannerWidth) * 0.5f,  // centered
		destination.y - bannerHeight * 0.65f,                      // sits on top
		bannerWidth,
		bannerHeight
	};

	// Font size scales with the banner
	defaultFontSize = static_cast<int>(bannerHeight * 0.30f);  // tweak 0.45–0.65
	//cout << "FONT SIZE: " << defaultFontSize << endl;
	
	
	//// Optional safety limits
	//if (defaultFontSize < 30)  defaultFontSize = 20;
	//if (defaultFontSize > 30) defaultFontSize = 30;

	Vector2 textSize = MeasureTextEx(customFont, name.c_str(), defaultFontSize, charSpacing);

	float textWidth  = textSize.x;
	float textHeight = textSize.y;

	namePos = {
		nameBannerDest.x + nameBannerDest.width * 0.5f - textWidth * 0.5f,
		nameBannerDest.y + nameBannerDest.height * 0.5f - defaultFontSize * 0.25f
	};

	UpdateBannerCurve();
}

void Card::UpdateIcons(float scale)
{
	statsFontSize = 15 * scale;

	healthIconDest = {
		destination.x,
		destination.y + destination.height - healthIcon->height * scale,
		static_cast<float>(healthIcon->width * scale),
		static_cast<float>(healthIcon->height * scale)
	};

	healthPos = {
		healthIconDest.x + healthIconDest.width/2 - MeasureText(to_string(health).c_str(), statsFontSize) * 0.5f,
		healthIconDest.y + healthIconDest.height / 2 - statsFontSize * 0.5f
	};

	damageIconDest = {
		destination.x + destination.width - damageIcon->width * scale,
		destination.y + destination.height - damageIcon->height * scale,
		static_cast<float>(damageIcon->width * scale),
		static_cast<float>(damageIcon->height * scale)
	};

	damagePos = {
	damageIconDest.x + damageIconDest.width * 0.5f - MeasureText(to_string(damage).c_str(), statsFontSize) * 0.5f,
	damageIconDest.y + damageIconDest.height * 0.5f - statsFontSize * 0.5f
	};

	manaIconDest = {
		nameBannerDest.x + nameBannerDest.width * 0.5f,
		nameBannerDest.y,
		static_cast<float>(manaCostIcon->width * scale),
		static_cast<float>(manaCostIcon->height * scale)
	};

	manaIconDest.x -= manaIconDest.width * 0.5f;

	manaCostPos = { manaIconDest.x + manaIconDest.width/2 - MeasureText(to_string(manaCost).c_str(), defaultFontSize) * 0.5f,
		manaIconDest.y + manaIconDest.height/2 - defaultFontSize/2};

	if (taunt)
	{
		/*tauntIconDest = {
		   healthIconDest.x,
		   healthIconDest.y - tauntIcon->height * scale,
		   static_cast<float>(tauntIcon->width * scale),
		   static_cast<float>(tauntIcon->height * scale)
		};*/

		tauntIconDest = {
		   healthIconDest.x,
		   nameBannerDest.y + nameBannerDest.height,
		   static_cast<float>(tauntIcon->width * scale),
		   static_cast<float>(tauntIcon->height * scale)
		};
	}

	if (evade)
	{
		evadeIconDest = {
		   tauntIconDest.x,
		   tauntIconDest.y + tauntIconDest.height,
		   static_cast<float>(evadeIcon->width * scale),
		   static_cast<float>(evadeIcon->height * scale)
		};

		if (!taunt)
		{
			evadeIconDest = {
			   healthIconDest.x,
			   nameBannerDest.y + nameBannerDest.height,
			   static_cast<float>(evadeIcon->width * scale),
			   static_cast<float>(evadeIcon->height * scale)
			};
		}
	}

}

void Card::Display()
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
	//DrawTextCurved(name.c_str(),
	//	bannerCenter,
	//	bannerRadius,
	//	bannerArcDeg,
	//	defaultFontSize,
	//	BLACK);


	DrawTexturePro(
		*healthIcon,
		healthIconSource,
		healthIconDest,
		center,
		0.0f,
		RAYWHITE
	);

	DrawTexturePro(
		*damageIcon,
		damageIconSource,
		damageIconDest,
		center,
		0.0f,
		RAYWHITE
	);

	DrawTexturePro(
		*manaCostIcon,
		manaIconSource,
		manaIconDest,
		center,
		0.0f,
		RAYWHITE
	);

	if (taunt && tauntIcon)
	{
		DrawTexturePro(
			*tauntIcon,
			tauntIconSource,
			tauntIconDest,
			center,
			0.0f,
			RAYWHITE
		);
	}

	if (evade && evadeIcon)
	{
		DrawTexturePro(
			*evadeIcon,
			evadeIconSource,
			evadeIconDest,
			center,
			0.0f,
			RAYWHITE
		);
	}

	DrawTextOutlined(statsFont, to_string(health).c_str(), healthPos, statsFontSize, charSpacing, RAYWHITE, BLACK, outlineSize);
	DrawTextOutlined(statsFont, to_string(damage).c_str(), damagePos, statsFontSize, charSpacing, RAYWHITE, BLACK, outlineSize);
	DrawTextOutlined(statsFont, to_string(manaCost).c_str(), manaCostPos, defaultFontSize, charSpacing, RAYWHITE, BLACK, outlineSize);


	
}

std::vector<std::string> Card::WrapText(Font font, const std::string& text,
	float fontSize, float spacing, float maxWidth)
{
	std::vector<std::string> lines;
	std::string line, word;

	auto flushWord = [&]()
		{
			std::string trial = line.empty() ? word : line + " " + word;
			float w = MeasureTextEx(font, trial.c_str(), fontSize, spacing).x;
			if (w <= maxWidth || line.empty())
				line = std::move(trial);
			else
			{
				lines.push_back(line);
				line = word;
			}
			word.clear();
		};

	for (char c : text)
	{
		if (c == ' ' || c == '\n')
		{
			if (!word.empty()) flushWord();
			if (c == '\n' && !line.empty())
			{
				lines.push_back(line);
				line.clear();
			}
		}
		else
			word += c;
	}
	if (!word.empty()) flushWord();
	if (!line.empty()) lines.push_back(line);
	return lines;
}

void Card::UpdateDescription()
{
	descriptionFontSize = static_cast<int>(destination.height * 0.055f);
	if (descriptionFontSize < 14) descriptionFontSize = 14;

	float pad = destination.width * 0.10f;
	float maxW = destination.width - pad * 2.0f;

	descriptionLines = WrapText(GetFontDefault(), description,
		(float)descriptionFontSize, charSpacing, maxW);
}

void Card::DisplayDescription()
{
	if (descriptionLines.empty()) return;

	if (mousedOver || chosen)
	{

		float padX = destination.width * 0.10f;
		float lineH = descriptionFontSize * 1.15f;
		float blockH = descriptionLines.size() * lineH + 8.0f;

		Rectangle box{
			destination.x + padX * 0.5f,
			destination.y + destination.height * 0.62f,
			destination.width - padX,
			destination.height * 0.38f
		};

		DrawRectangleRec(box, Color{ 0, 0, 0, 130 });

		float y = box.y + 4.0f;
		for (const auto& line : descriptionLines)
		{
			float w = MeasureTextEx(GetFontDefault(), line.c_str(),
				(float)descriptionFontSize, charSpacing).x;
			Vector2 pos{
				box.x + (box.width - w) * 0.5f,
				y
			};
			DrawTextOutlined(GetFontDefault(), line.c_str(), pos,
				descriptionFontSize, charSpacing,
				RAYWHITE, BLACK, 1);
			y += lineH;
		}
	}
}

void Card::SetX(float x)
{
	this->x = x;
	destination.x = x;
	hitbox.x = x;
	//nameBannerDest.x = x;
	UpdateNameBanner();
	UpdateIcons();
}

void Card::SetY(float y)
{
	this->y = y;
	destination.y = y;
	hitbox.y = y;
	//nameBannerDest.y = destination.y - nameBannerDest.height/2;
	UpdateNameBanner();
	UpdateIcons();
}

void Card::SetBaseX(float x)
{
	baseX = x;
}

void Card::SetBaseY(float y)
{
	baseY = y;
}

void Card::SetDest(Rectangle dest)
{
	destination = dest;
	UpdateNameBanner();
	UpdateIcons();
	UpdateDescription();
}

void Card::PrepForDisplay(float x, float y)
{
	float texW = static_cast<float>(texture->width);
	float texH = static_cast<float>(texture->height);

	float scale = std::min(240.f / texW, 360.f / texH);

	destination = {
		x,
		y,
		texW* scale,
		texH* scale
	};

	UpdateNameBanner();
	UpdateIcons(2.5f);
	UpdateDescription();
	chosen = true;
}

float Card::GetX()
{
	return x;
}

float Card::GetBaseX()
{
	return baseX;
}

float Card::GetBaseY()
{
	return baseY;
}

float Card::GetWidth()
{
	return width;
}

Rectangle Card::GetRectDest()
{
	// IMPORTANT: return the hitbox for collision tests
	return hitbox;
}

bool Card::GetMouseOver()
{
	return mousedOver;
}

string Card::GetType() const
{
	return type;
}

Utilities::CardType Card::GetCardType() const
{
	return cardType;
}

int Card::GetManaCost() const
{
	return manaCost;
}