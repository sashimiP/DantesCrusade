#include "Projectile.h"

Projectile::Projectile(int rangedEnityCellIndex,
	float rangedEntityWidth,
	float rangedEntityHeight,
	float scalingFactor,
	fs::path dataPath,
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
	int color
):
	Entity(dataPath,
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
		color),
	target(nullptr),
	rangedEnityCellIndex(rangedEnityCellIndex),
	rangedEntityWidth(rangedEntityWidth),
	rangedEntityHeight(rangedEntityHeight),
	scalingFactor(scalingFactor)
{
	startingPosition = { x, y };
	shadowRect = {
	this->x,
	this->y + rangedEntityHeight /2.0f,
	destRect.width * 1.2f,    // stretched
	rect.height * 0.25f       // squashed flat
	};
	//shadowRect.y -= shadowRect.height / 2.0f;
	shadowOrigin = { shadowRect.width / 2,  shadowRect.height / 2 };
	ScaleDestRect(this->scalingFactor);
}

Projectile::~Projectile()
{
	save = false;
}

void Projectile::Update()
{
	if (loose)
	{
		FlyToTarget();
	}
		
}

const float Projectile::SetTarget(const Rectangle& rect, shared_ptr<BattleEntity> target)
{
	enemyRect = rect;
	enemyCenter = {
	enemyRect.x + enemyRect.width / 2.0f,
	enemyRect.y + enemyRect.height / 2.0f
	};

	float dx = enemyCenter.x - x;
	float dy = enemyCenter.y - y;

	// Use atan2f for robust angle calculation (in degrees)
	rotation = atan2f(dy, dx) * (180.0f / PI);

	this->target = target;

	return rotation;

}

void Projectile::Loose()
{
	cout << "LOOSE" << endl;
	loose = true;
}

//void Projectile::FlyToTarget()
//{
//	// If we're NOT inside the destination cell yet keep moving
//	if (!CheckCollisionPointRec({ x, y }, enemyRect))
//	{
//		float dx = enemyCenter.x - x;
//		float dy = enemyCenter.y - y;
//
//		float distance = sqrtf(dx * dx + dy * dy);
//
//		if (distance > 1.0f) // just to avoid division by zero
//		{
//			dx /= distance;
//			dy /= distance;
//
//			SetX(x + dx * speed);
//			SetY(y + dy * speed);
//			shadowRect.x += dx * speed;
//			shadowRect.y += dy * speed;
//		}
//	}
//	else if(!hit && CheckCollisionPointRec({ x, y }, enemyRect))
//	{
//		target->ActivateGetHit();
//		SetStance("hit");
//		auto tempRect = target->GetShadowRect();
//		shadowRect.x = tempRect.x;
//		shadowRect.y = tempRect.y;
//		hit = true;
//	}
//	else if (hit)
//	{
//		timer += GetFrameTime();
//		if(timer >= hitDuration)
//		{
//			cout << "RETURN" << endl;
//			//rotation = 0.0f;
//			SetStance("idle");
//			SetX(startingPosition.x);
//			SetY(startingPosition.y);
//
//			hit = false;
//			timer = 0.0f;
//			loose = false;
//		}
//		
//		
//	}
//
//}


void Projectile::FlyToTarget()
{
	// If we're NOT inside the destination cell yet keep moving
	if (!CheckCollisionPointRec({ x, y }, enemyRect))
	{
		float dx = enemyCenter.x - x;
		float dy = enemyCenter.y - y;

		float distance = sqrtf(dx * dx + dy * dy);

		if (distance > 1.0f) // just to avoid division by zero
		{
			dx /= distance;
			dy /= distance;

			SetX(x + dx * speed);
			SetY(y + dy * speed);
			shadowRect.x += dx * speed;
			shadowRect.y += dy * speed;
		}
	}
	else if (!hit && CheckCollisionPointRec({ x, y }, enemyRect))
	{
		if (resetRotation)
		{
			rotation = 0.0f;
			SetX(target->X());
			SetY(0);
		}
		
		target->ActivateGetHit(true, attack);
		SetStance("hit");
		auto tempRect = target->GetShadowRect();
		shadowRect.x = tempRect.x;
		shadowRect.y = tempRect.y;
		hit = true;
	}
	else if (animationFinished)
	{
			SetStance("idle");
			cout << "RETURN" << endl;
			//rotation = 0.0f;
			
			cout << "onHit valid: " << static_cast<bool>(onHit) << endl;

			if (onHit)
			{
				cout << "CALLING onHit" << endl;
				onHit();
				cout << "onHit finished" << endl;
			}

			SetX(startingPosition.x);
			SetY(startingPosition.y);

			animationFinished = false;
			hit = false;
			loose = false;
			resetRotation = false;


	}

}

void Projectile::DrawShadow(int animationSpeed, float dt)
{
	if (!loose) return;
	// Origin at top-center so it pivots from feet

	// Display rotated semi-transparent shadow
	DrawTexturePro(
		*texture,
		sourceRect,
		shadowRect,
		shadowOrigin,
		rotation,
		Color{ 0, 0, 0, 100 }
	);
}

void Projectile::Display(int animationSpeed, int offsetX, float dt)
{
	if (!loose) return;

	//FrameData& frameData = (*texturesMap)[stance];
	//Texture2D& texture = frameData.texture;     // sprite sheet

	animationTimer += dt;
	if (animationTimer >= (1.0f / animationSpeed)) {
		animationTimer = 0.0f;
		currentFrame = (currentFrame + 1) % framesCount;

		if (hit && currentFrame == 0)
		{
			animationFinished = true;
			return;
		}

		sourceRect.x = static_cast<float>(currentFrame * frameWidth);

		
	}

	DrawTexturePro(
		*texture,
		sourceRect,
		destRect,
		center,
		rotation,
		RAYWHITE
	);

	//DrawRectangleRec(centerRect, GREEN);
}

void Projectile::SetX(float x)
{
	this->x = x;
	rect.x = x - center.x;
	centerRect.x = x;
	destRect.x = x;
	shadowRect.x = x;
}

void Projectile::SetY(float y)
{
	this->y = y;
	rect.y = y - center.y;
	centerRect.y = y;
	destRect.y = y;
	shadowRect.y = y + rangedEntityHeight / 2.0f;
}

void Projectile::SetCellIndex(int newIndex)
{
	rangedEnityCellIndex = newIndex;
}

void Projectile::SetStartingPosition(float x, float y)
{
	startingPosition = { x, y };
}


void Projectile::SetAttack(int attack)
{
	this->attack = attack;
	cout << "HOLY BEAM CURRENT ATTACK: " << attack << endl;
}

void Projectile::SetStance(string stance)
{
	if (!Options.hasOption(stance)) return;

	currentFrame = 0;

	this->stance = std::move(stance);

	spriteSheet = &(*texturesMap)[this->stance];
	texture = &spriteSheet->texture;

	framesCount = spriteSheet->frames;
	colorCount = static_cast<float>(spriteSheet->colors);
	frameWidth = static_cast<float>(spriteSheet->texture.width) / framesCount;
	frameHeight = static_cast<float>(spriteSheet->texture.height) / colorCount;

	color = (color > colorCount - 1) ? 0 : color;

	sourceRect = Rectangle{ 0, 0 + frameHeight * color,static_cast<float>(frameWidth),static_cast<float> (frameHeight) };
	destRect = Rectangle{ x , y, static_cast<float>(frameWidth),static_cast<float> (frameHeight) };
	center = Vector2{ static_cast<float>(destRect.width / 2), static_cast<float> (destRect.height / 2) };
	rect = Rectangle{ x - center.x, y - center.y, destRect.width, destRect.height };

	shadowRect = {
		x,
		y + rangedEntityHeight / 2.0f,
		destRect.width * 1.2f,    // stretched
		rect.height * 0.25f       // squashed flat
	};
	//shadowRect.y -= shadowRect.height / 2.0f;
	shadowOrigin = { shadowRect.width / 2,  shadowRect.height / 2 };

	centerRect = Rectangle{ x, y, 5, 5 };
	
	ScaleDestRect(scalingFactor);
}