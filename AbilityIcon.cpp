#include "AbilityIcon.h"


AbilityIcon::AbilityIcon(string heroType, shared_ptr<TextureManager> textureManager):
	heroType(heroType),
	textureManager(textureManager)
{
	icon = make_unique<AnimatedAsset>();
	iconGhost = make_unique<AnimatedAsset>();
	frame = textureManager->Asset("square_frame");
	frameSource = { 0.0f, 0.0f, frame->width * 1.f, frame->height * 1.f };
	abilityType = heroAbilities[heroType].abilityType;

	manaCost = heroAbilities[heroType].manaCost;
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
		manaCostIcon->width * manaCostScale,
		manaCostIcon->height * manaCostScale
	};
	
	abilityUsed_texture = textureManager->Asset("hero_ability_used");
	abilityUsedSource = {
		0.0f,
		0.0f,
		static_cast<float>(abilityUsed_texture->width),
		static_cast<float>(abilityUsed_texture->height)
	};

}

AbilityIcon::~AbilityIcon(){}

void AbilityIcon::Update()
{

	if (icon)
		if (CheckCollisionPointRec(GetMousePosition(), destRect))
			icon->Update();

	if (seekTarget)
	{
		Vector2 mousePos = GetMousePosition();
		Vector2 offSet = { iconGhost->GetDest().width * 0.5f, iconGhost->GetDest().height * 0.5f };
		iconGhost->SetPosition(mousePos.x - offSet.x, mousePos.y - offSet.y);
		iconGhost->Update();
	}
}

void AbilityIcon::LeftMouseButtonPressed()
{
	ClickIcon();
}

void AbilityIcon::ClickIcon()
{
	
	if (!IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) return;
	if (abilityUsed) return;
	
	auto mouse = GetMousePosition();
	if (CheckCollisionPointRec(mouse, destRect))
	{
		seekTarget = true;
		std::cout << "Icon clicked!\n";
		if (onClick) {
			onClick();           // This is the magic line
			
		}
	}
	
}

void AbilityIcon::SetPosition(float& x, float& y)
{
	destRect.x = x;
	destRect.y = y;

	icon->SetPosition(x, y);

	manaIconDest.x = destRect.x + destRect.width * 0.5f - manaIconDest.width * 0.5f;
	manaIconDest.y = destRect.y - manaIconDest.height * 0.5f;

	manaCostPos = { manaIconDest.x + manaIconDest.width * 0.5f - MeasureText(to_string(manaCost).c_str(), manaCostFontSize) * 0.5f,
		manaIconDest.y + manaIconDest.height * 0.5f - manaCostFontSize * 0.5f };
}

bool AbilityIcon::GetSeekTarget()
{
	return seekTarget;
}

Rectangle AbilityIcon::GetRect()
{
	return destRect;
}

void AbilityIcon::Display()
{
	if (icon)
	{
		if (CheckCollisionPointRec(GetMousePosition(), destRect))
		{
			icon->Draw();
			icon->DrawOnce();
		}
		else
		{
			DrawTexturePro(
				*iconStatic,
				iconStaticSource,
				destRect,
				{ 0.0f, 0.0f },
				0.0f,
				RAYWHITE
			);
		}

		if (abilityUsed)
			DrawTexturePro(
				*abilityUsed_texture,
				abilityUsedSource,
				destRect,
				{ 0.0f, 0.0f },
				0.0f,
				RAYWHITE
		);

		DrawTexturePro(
			*frame,
			frameSource,
			destRect,
			{ 0.0f, 0.0f },
			0.0f,
			RAYWHITE
		);

		DrawTexturePro(
			*manaCostIcon,
			manaIconSource,
			manaIconDest,
			{ 0.0f, 0.0f },
			0.0f,
			manaCostColor
		);

		if (!abilityUsed)
			DrawTextOutlined(GetFontDefault(), to_string(manaCost).c_str(), manaCostPos, manaCostFontSize, 2.f, RAYWHITE, BLACK, 2);

			
	}
}

void AbilityIcon::DisplayVisualEffect()
{
	if (seekTarget)
	{
		iconGhost->Draw(SKYBLUE);
	}
	visualEffect->DrawOnce();

}


void AbilityIcon::SetIcon(string& type, int& frames)
{
	this->icon = make_unique<AnimatedAsset> (type, frames, textureManager);

	ghostType = heroAbilities[heroType].ghostType;

	this->iconGhost = make_unique<AnimatedAsset>(ghostType, frames, textureManager);
	destRect = icon->GetDest();
	frameDest = icon->GetDest();

	iconStatic = icon->GetTexture();
	float iconFrameWidth = iconStatic->width / frames;
	iconStaticSource = { 0.0f, 0.0f, iconFrameWidth, iconStatic->height * 1.0f};

	statToModify = heroAbilities[heroType].statToModify;
	statModifier = heroAbilities[heroType].statModifier;

}

void AbilityIcon::SetVisualEffect(string& type, int& frames)
{
	visualEffect = make_unique<AnimatedAsset>(type, frames, textureManager);

}

void AbilityIcon::SetTarget(BattleEntity* entity, Rectangle targetRect, std::function<void()> callback)
{
	if (abilityUsed) return;

	int x = targetRect.x + targetRect.width * 0.5f - visualEffect->GetDest().width * 0.5f;
	int y = targetRect.y + targetRect.height * 0.5f - visualEffect->GetDest().height * 0.5f;

	visualEffect->SetPosition(x, y);
	if (entity)
		targetEntity = entity;
	else
		cout << heroType << " VISUAL EFFECT SET TARGET: ENTITY IS NULL" << endl;

	visualEffect->SetDrawOnce(true, 0.5f, [this, callback]()
		{
		
			if (abilityType == CardType::Buff)
			{
				targetEntity->AddStat(statToModify, statModifier);
			}
			else if (abilityType == CardType::Debuff)
			{
				int s = statModifier;
				if (statToModify == Stat::Health)
					targetEntity->ActivateGetHit(false, s);
				else
					targetEntity->SubStat(statToModify, s);
			}
		seekTarget = false;
		if (callback)
			callback();
		});

	abilityUsed = true;
	manaCostColor = DARKGRAY;

}

bool AbilityIcon::GetAbilityUsed()
{
	return abilityUsed;
}

int AbilityIcon::GetStatModifier()
{
	return statModifier;
}

void AbilityIcon::ResetAbilityUsed()
{
	manaCostColor = RAYWHITE;
	abilityUsed = false;
}

void AbilityIcon::Deselect()
{
	seekTarget = false;
}