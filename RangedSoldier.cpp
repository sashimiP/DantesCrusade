#include "RangedSoldier.h"

RangedSoldier::RangedSoldier(string projectile_type,
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
	BattleEntity(relationship,
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
	projectile = make_unique<Projectile>(cellIndex, rect.width, rect.height, projectile_scale, "none", "none", textureManager, 0.0f, 3,
		projectile_type, centerRect.x, centerRect.y, east,
		false, "idle");
	projectile->SetAttack(currentAttack);

	projectile->onHit = [this]()
		{
			cout << this->type << " Projectile hit Target. END TURN." << endl;
			SetHadItsTurn(true);
			EndTurn();
		};

	SetCell(*(grid->at(cellIndex).rect));
	 // projectile must always face west, for proper attack calculations!
}

RangedSoldier::~RangedSoldier()
{
	save = false;
}


void RangedSoldier::Update(bool selected)
{
	ShakeEntity();
	auto mousePos = GetMousePosition();

	if (mediaPortrait)
	{
		const bool shouldShow = drawDialoguePortrait || mouseOver|| this -> selected;
		if (shouldShow)
		{
			//cout << type << " shouldShow\n";
			if (GetMediaState(*mediaPortrait) != MEDIA_STATE_PLAYING)
			{
				SetMediaState(*mediaPortrait, MEDIA_STATE_PLAYING);
				//cout << type << " PLAYING\n";
			}

			UpdateMedia(mediaPortrait.get());
		}
		else
		{
			// pause (keeps the current frame) or STOPPED if you prefer a full reset
			/*if (GetMediaState(*mediaPortrait) == MEDIA_STATE_PLAYING)
				SetMediaState(*mediaPortrait, MEDIA_STATE_PAUSED);*/

			if (GetMediaState(*mediaPortrait) != MEDIA_STATE_STOPPED)
			{
				SetMediaPosition(*mediaPortrait, 0.0);
				SetMediaState(*mediaPortrait, MEDIA_STATE_STOPPED);

			}
		}
	}
	if (drag)
	{
		SetX(mousePos.x);
		SetY(mousePos.y);

	}

	if (addDialogue)
		AddDialogue();

	if (addName)
		AddName();

	if (move)
	{
		MoveToCell();
	}

	if (!move && offence)
	{
		AttackEnemy();
	}

	if (retaliate)
	{
		Retaliate();
	}

	if (getHit)
	{
		GetHit();
	}

	if (getHolyHit)
	{
		GetHolyHit();
	}

	if (selected)
	{

		for (int i = 0; i < adjacentCells.size(); i++)
		{
			if (adjacentCells[i].cell &&
				CheckCollisionPointRec(mousePos, *adjacentCells[i].cell->rect))
			{
				adjacentCells[i].mouseCollide = true;

				for (int j = 0; j < adjacentCells.size(); j++)
				{
					if (j == i) continue;
					adjacentCells[j].mouseCollide = false;
				}
			}
		}

	}

	projectile->Update();

	if (ranOutOffHealth && animateDeath)
	{
		Death();
	}

	if (drawTaunt)
		tauntAnimatedIcon->Update();

	if (skipTurn)
		skipTurnAnimatedIcon->Update();
}

//void RangedSoldier::Move(int rectIndex)
//{
//	move = true;
//	rectMoveToIndex = rectIndex;
//	cellIndex = rectIndex;
//	auto& tempRect = (*grid)[rectMoveToIndex].rect;
//	// place projectilen in the center of the cell
//
//	if (x > tempRect->x && !flipped) Flip();
//	SetStance("walk");
//	//ScaleDestRect(scalingFactor);
//
//}

void RangedSoldier::AttackEnemy()
{
	if (nextToEnemy)
	{
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
			animationFinished = false;
			attacking = false;
			offence = false;
			cout << Type() << " Attacker-finished animation sequence\n";
		}

	}

	
}

void RangedSoldier::Attack(shared_ptr<Cell> enemyCell)
{
	cout << type << " - Attack" << endl;
	this->enemyCell = enemyCell;
	//auto it = std::find(adjacentCellIndexes.begin(), adjacentCellIndexes.end(), this->enemyCell->gridIndex);

	for (auto& adjCell : adjacentCells)
		if ((adjCell.cell != nullptr) && adjCell.cell->entity != nullptr && adjCell.cell->entity->GetRelationship() != relationship )
			nextToEnemy = true;
	
	//nextToEnemy = (it != adjacentCellIndexes.end());
	attackType = (nextToEnemy) ? "melee_attack" : "attack";
	
	if (nextToEnemy)
	{
		BattleEntity::Attack(this->enemyCell);
		rectMoveToIndex = attackDirection.cell->gridIndex;
	}
	else
	{
		offence = true;
		this->enemyIndex = enemyCell->gridIndex;
		rectMoveToIndex = cellIndex;
		auto& enemy = (*grid)[enemyIndex].entity;

		auto& rectMoveTo = (*grid)[rectMoveToIndex].rect;
		auto& enemyRect = (*grid)[enemyIndex].rect;

		SetStance(attackType);

		float rotation = abs(projectile->SetTarget(*enemyRect, enemy));
		if ((rotation >= 0.0f && rotation < 90.0f) && direction == Utilities::Direction::west)
		{
			Flip();
		}
		else if ((rotation > 90.0f && rotation <= 180.0f) && direction == Utilities::Direction::east)
		{
			Flip();
		}
		attacking = true;

		//return rectMoveToIndex;
	}
}

//void RangedSoldier::Retaliate()
//{
//	timer += GetFrameTime();
//
//	auto& enemy = (*grid)[enemyIndex].entity;
//	if (defending && timer >= hitDuration)
//	{
//		cout << "Cool Off" << endl;
//		coolOff = true;
//		SetStance("idle");
//		timer = 0.0f;
//		defending = false;
//
//	}
//	else if (coolOff && timer >= coolOffDuration)
//	{
//		cout << "Retaliate" << endl;
//		attackType = (nextToEnemy) ? "melee_attack" : "attack";
//		SetStance(attackType);
//		timer = 0.0f;
//		coolOff = false;
//
//	}
//	else if (!defending && !coolOff && !enemyHit && timer >= momentOfContact)
//	{
//		enemy->ActivateGetHit();
//		enemyHit = true;
//	}
//	else if (!defending && !coolOff && enemyHit && timer >= attackDuration)
//	{
//		cout << "Idle" << endl;
//		cout << timer << endl;
//		SetStance("idle");
//		retaliate = false;
//		enemyHit = false;
//
//		retaliated = true;
//
//		timer = 0.0f;
//
//	}
//}

void RangedSoldier::Retaliate()
{
	
	/*auto& enemy = (*grid)[enemyIndex].entity;*/
	if (defending && animationFinished)
	{
		animationFinished = false;
		cout << "Cool Off" << endl;
		coolOff = true;
		SetStance("idle");

		defending = false;

	}
	else if (coolOff && animationFinished)
	{
		cout << "Retaliate" << endl;
		attackType = (nextToEnemy) ? "melee_attack" : "attack";
		animationFinished = false;
		SetStance(attackType);
		retaliating = true;
		coolOff = false;

	}
	else if (enemyHit)
	{
		enemyHit = false;
		enemy->ActivateGetHit();
		EndTurn();
	}
	else if (retaliating && animationFinished)
	{
		cout << "Idle" << endl;
		SetStance("idle");
		retaliating = false;
		animationFinished = false;

		retaliate = false;
		retaliated = true;
		EndTurn();
		cout << "Attacker-finished animation sequence\n";
	}
}

void RangedSoldier::Display(int animationSpeed, int offsetX, float dt)
{
	if (!stopAnimating) {
		animationTimer += dt;
		if (animationTimer >= (1.0f / animationSpeed)) {
			animationTimer = 0.0f;
			currentFrame = (currentFrame + 1) % framesCount;
			/*if (attacking)
			{
				cout << "current frame " << currentFrame << endl;
			}*/
			if (currentFrame == 0)
			{

				if (attacking ||
					getHit ||
					getHolyHit ||
					getUp ||
					transform ||
					defending ||
					coolOff ||
					retaliating||
					ranOutOffHealth)
				{
					cout << "moment of contact " << momentOfContact << endl;
					animationFinished = true;
					return;
				}


			}
			else if ((attacking || retaliating) && currentFrame == momentOfContact)
			{
				cout << "enemy hit" << endl;
				enemyHit = true;

			}
			sourceRect.x = static_cast<float>(currentFrame * frameWidth);




		}
	}

	if (drawDialoguePortrait || mouseOver || selected)
	{
		if (mediaPortrait == nullptr)
			DrawPortrait(animationSpeed, dt);
		else
			DrawAnimatedPortrait(dt);

	}

	DrawTexturePro(
		*texture,
		sourceRect,
		destRect,
		center,
		0,
		RAYWHITE
	);

	DrawIcons();

	projectile->DrawShadow(animationSpeed, dt);
	projectile->Display(animationSpeed, offsetX, dt);

}

void RangedSoldier::ScaleDestRect(float scalingFactor)
{
	destRect = Rectangle{ destRect.x, destRect.y, destRect.width * scalingFactor, destRect.height * scalingFactor };
	center = Vector2{ static_cast<float>(destRect.width / 2), static_cast<float> (destRect.height / 2) };
	rect = Rectangle{ this->x - center.x,this->y - center.y,destRect.width,destRect.height };

	projectile->ScaleDestRect(scalingFactor);
}

void RangedSoldier::SetCell(const Rectangle& cell)
{
	auto adjustX = cell.x + cell.width / 2.0f;
	auto adjustY = cell.y + cell.height - rect.height / 2.0f;

	SetX(adjustX);
	SetY(adjustY);

	UpdateShadowRect();
	UpdateAdjacentCells();

	projectile->SetCellIndex(cellIndex);
	projectile->SetX(centerRect.x);
	projectile->SetY(centerRect.y);
	projectile->SetStartingPosition(centerRect.x, centerRect.y);
}

void RangedSoldier::AddStat(Stat stat, int amount)
{
	targeted = false;
	switch (stat)
	{
	case Stat::Health:
		currentHealth += amount;
		healthModifier = "+" + to_string(amount);
		healthPopUp->SetDrawOnce(true);
		SetStatsTextColor(maxHealth, currentHealth, healthTextColor);
		break;

	case Stat::Attack:
		currentAttack += amount;
		damageModifier = "+" + to_string(amount);
		damagePopUp->SetDrawOnce(true);
		SetStatsTextColor(startingAttack, currentAttack, damageTextColor);
		projectile->SetAttack(currentAttack);

		break;
	}
}

void RangedSoldier::SubStat(Stat stat, int amount)
{
	targeted = false;
	switch (stat)
	{
	case Stat::Health:
		currentHealth -= amount;
		healthModifier = "-" + to_string(amount);
		healthPopUp->SetDrawOnce(true);
		SetStatsTextColor(maxHealth, currentHealth, healthTextColor);
		if (currentHealth <= 0)
		{

			HP_bar = nullptr;
			ranOutOffHealth = true;
			animateDeath = true;
			SetStance("death");
			return;
		}
		break;

	case Stat::Attack:
		currentAttack -= amount;
		damageModifier = "-" + to_string(amount);
		damagePopUp->SetDrawOnce(true);
		SetStatsTextColor(startingAttack, currentAttack, damageTextColor);
		projectile->SetAttack(currentAttack);

		break;
	}
}


