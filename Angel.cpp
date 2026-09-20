#include "Angel.h"

Angel::Angel(string projectile_type,
	float projectile_scale,
	Utilities::Relationship relationship,
	const int cellIndex,
	shared_ptr<vector<Cell>> grid,
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
	SpellCaster(projectile_type,
		projectile_scale,
		relationship,
		cellIndex,
		grid,
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
	projectile = make_unique<HolyBeam>(cellIndex, rect.width, rect.height, projectile_scale, "none", "none", textureManager, 0.0f, 3,
		projectile_type, centerRect.x, centerRect.y, west,
		false, "idle");
	projectile->SetAttack(currentAttack);
	cout << "CURRENT ATTACK: " << currentAttack << endl;
}

Angel::~Angel()
{
	save = false;
}

void Angel::AttackEnemy()
{
	if (nextToEnemy)
	{
		/*cout << "Angel Melee attack" << endl;
		cout << "Moment of contact " << momentOfContact << endl;*/
		BattleEntity::AttackEnemy();
		return;

		//auto& enemyRect = (*grid)[enemyIndex].rect;
		//auto& enemy = (*grid)[enemyIndex].entity;
		////auto attackDirection = (*grid)[enemyIndex].entity->GetAttackDirection();


		//// If we're NOT inside the destination cell yet keep moving
		//if (!CheckCollisionPointRec({ enemyCenter.x, enemyCenter.y }, rect))
		//{
		//	float dx = enemyCenter.x - x;
		//	float dy = enemyCenter.y - y;

		//	float distance = sqrtf(dx * dx + dy * dy);

		//	if (distance > 1.0f) // just to avoid division by zero
		//	{
		//		dx /= distance;
		//		dy /= distance;

		//		float speed = 5.0f;
		//		SetX(x + dx * speed);
		//		SetY(y + dy * speed);
		//		shadowRect.x += dx * speed;
		//		shadowRect.y += dy * speed;
		//	}
		//}
		//else
		//{
		//	if (offence && !attacking)
		//	{
		//		SetStance("melee_attack");

		//		if (attackDirection.side == Side::west && direction != enemy->Direction())
		//		{
		//			cout << "Flip to face enemy's back" << endl;
		//			Flip();
		//			UpdateAdjacentCells();
		//		}
		//		else if (attackDirection.side == Side::east && direction == enemy->Direction())
		//		{
		//			cout << "Flip to face enemy's front" << endl;
		//			Flip();
		//			UpdateAdjacentCells();
		//		}
		//		attacking = true;
		//	}

		//	timer += GetFrameTime();
		//	if (!enemyHit && timer >= momentOfContact)
		//	{
		//		enemy->ActivateRetaliate(rectMoveToIndex);
		//		enemyHit = true;
		//	}
		//	if (timer >= attackDuration)
		//	{
		//		SetStance("idle");
		//		// Snap to center of cell and stop
		//		SetX(moveToCenter.x);
		//		SetY(moveToCenter.y);
		//		UpdateShadowRect();
		//		timer = 0.0f;
		//		attacking = false;
		//		enemyHit = false;
		//		offence = false;

		//	}

		//}
	}
	else
	{
		//timer += GetFrameTime();
		//auto& enemy = (*grid)[enemyIndex].entity;

		//if (timer >= attackDuration)
		//{
		//	projectile->Loose();
		//	SetStance("idle");
		//	timer = 0.0f;
		//	attacking = false;
		//	offence = false;

		//}

		if (attacking && animationFinished)
		{
			projectile->Loose();
			SetStance("idle");
			hadItsTurn = true;
			animationFinished = false;
			attacking = false;
			offence = false;
			EndTurn();
			cout << "Attacker-finished animation sequence\n";

		}

	}


}