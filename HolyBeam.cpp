#include "HolyBeam.h"

HolyBeam::HolyBeam(int rangedEnityCellIndex,
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
) :
	Projectile(rangedEnityCellIndex,
		rangedEntityWidth,
		rangedEntityHeight,
		scalingFactor,
		dataPath,
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
HolyBeam::~HolyBeam()
{
	save = false;
}


void HolyBeam::FlyToTarget()
{
	// If we're NOT inside the destination cell yet keep moving
	if (!hit&&!CheckCollisionPointRec({ x, y }, enemyRect))
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
		if (target->Type() == "succubus_whip" ||
			target->Type() == "succubus_whip" ||
			target->Type() == "succubus_blue" || 
			target->Type() == "succubus_golden" ||
			target->Type() == "succubus_red" ||
			target->Type() == "succubus_wizard" )
		{
			target->ActivateHolyHit(attack);
		}
		else
		{
			target->ActivateGetHit(true, attack);
		}
		SetStance("hit");
		rotation = 0.0f;
		SetX(enemyRect.x + enemyRect.width/2.0f);
		SetY((enemyRect.y + enemyRect.height) - rect.height/2.0f);
		cout << "HIT" << endl;
		hit = true;
	}
	else if (animationFinished)
	{
		SetStance("idle");
		cout << "RETURN" << endl;
		//rotation = 0.0f;

		SetX(startingPosition.x);
		SetY(startingPosition.y);

		animationFinished = false;
		hit = false;
		loose = false;
		resetRotation = false;


	}

}

void HolyBeam::DrawShadow(int animationSpeed, float dt)
{
	return;

}
