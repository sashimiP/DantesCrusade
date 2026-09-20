#include "BattleHero.h"
BattleHero::BattleHero(Utilities::Relationship relationship,
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
	movementPoints = 1;
	SetCell(*(grid->at(cellIndex).rect));
	skipTurn = false;

	abilityType = heroAbilities[type].type;
	abilityFrames = heroAbilities[type].frames;

	abilityManaCost = heroAbilities[type].manaCost;

	specialAbility = make_shared <AbilityIcon> (type, this->textureManager);
	specialAbility->SetIcon(abilityType, abilityFrames);

	float abilityX = portraitDest.x - portraitDest.width - specialAbility->GetRect().width-11;
	float abilityY = portraitDest.y + portraitDest.height - specialAbility->GetRect().height-11;

	specialAbility->SetPosition(abilityX, abilityY);

	string visualEffectType = heroAbilities[type].visualEffect;
	int visualEffectFrames = heroAbilities[type].visualEffectFrames;

	specialAbility->SetVisualEffect(visualEffectType, visualEffectFrames);

	
}

BattleHero::~BattleHero()
{
	save = false;
}

void BattleHero::Update(bool selected)
{
	ShakeEntity();

	Vector2 mousePos = GetMousePosition();

	if (mediaPortrait)
	{
		const bool shouldShow = drawDialoguePortrait || mouseOver || this->selected;
		if (shouldShow)
		{
			//cout << type << " shouldShow\n";
			if (GetMediaState(*mediaPortrait) != MEDIA_STATE_PLAYING)
			{

				SetMediaState(*mediaPortrait, MEDIA_STATE_PLAYING);

				//cout << type << " PLAYING\n";
			}
			//cout << type << " PLAYING\n";
			UpdateMedia(mediaPortrait.get());
		}
		else
		{
			// pause (keeps the current frame) or STOPPED if you prefer a full reset
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

	if (ranOutOffHealth && animateDeath)
	{
		Death();
	}

	if (skipTurn)
		skipTurnAnimatedIcon->Update();

	if (specialAbility && selected)
		specialAbility->Update();

	//if(healthPopUp->GetDrawOnce());
	//	healthPopUp->Update();
}

void BattleHero::Attack(shared_ptr<Cell> enemyCell)
{
	if (currentAttack <= 0)
	{	
		Deselect();
		return;
	}

	drawTaunt = false;
	cout << type << " - Attack" << endl;
	attacker = true;
	enemyIsAdjacent = false;
	this->enemyCell = enemyCell;
	auto& enemyRect = this->enemyCell->rect;
	enemy = enemyCell->entity;
	enemy->drawDialoguePortrait = true;

	this->enemyIndex = enemy->GetCellIndex();
	attackDirection = enemy->GetAttackDirection();
	rectMoveToIndex = attackDirection.cell->gridIndex;
	SetPosition(rectMoveToIndex + 1);
	auto path = TrailPath(cellIndex, rectMoveToIndex);

	if (path.size() > movementPoints + 1)
	{
		int adjust = path.size() - movementPoints - 1;
		path.erase(path.end() - adjust, path.end());
		enemy->finishedAnimationSequence = true;
		Move(path);
		rectMoveToIndex = path[path.size() - 1];
	}

	enemyIsAdjacent = (rectMoveToIndex == cellIndex);


	if (!enemyIsAdjacent)
	{
		attackDirection = *enemy->GetAvailableAdjacentCell();
		rectMoveToIndex = attackDirection.cell->gridIndex;
	}
	else if (enemy->GetAvailableAdjacentCell() == nullptr) return;


	auto& tempRect = (*grid)[rectMoveToIndex].rect;

	moveToCenter = {
		tempRect->x + tempRect->width / 2.0f,
		tempRect->y + tempRect->height - rect.height / 2.0f
	};

	enemyCenter = {
		enemyRect->x + enemyRect->width / 2.0f,
		enemyRect->y + enemyRect->height - rect.height / 2.0f
	};

	offence = true;
	if (!enemyIsAdjacent) { cout << "Move" << endl; Move(rectMoveToIndex); }
}

void BattleHero::Deselect()
{
	selected = false;
	specialAbility->Deselect();

	if (relationship == Relationship::Ally)
	{
		for (auto& i : reachableCells)
			(*grid)[i].color = BATTLE_CELL_CLR;
	}
}

void BattleHero::DrawStatsBar()
{

	DrawTexturePro(
		*healthIcon,
		healthIconSource,
		healthIconDest_portrait,
		{ 0.0f, 0.0f },
		0.0f,
		RAYWHITE
	);

	/*DrawTexturePro(
		*damageIcon,
		damageIconSource,
		damageIconDest_portrait,
		{ 0.0f, 0.0f },
		0.0f,
		RAYWHITE
	);*/

	if (skipTurn)
	{
		DrawTexturePro(
			*skipTurnIcon,
			skipTurnIconSource,
			skipTurnIconDest,
			{ 0.0f, 0.0f },
			0.0f,
			RAYWHITE
		);
	}

	DrawTextOutlined(customFont, to_string(currentHealth).c_str(), healthPos, statsFontSize, charSpacing, healthTextColor, BLACK, outlineSize);

	/*DrawTextOutlined(customFont, to_string(currentAttack).c_str(), damagePos, statsFontSize, charSpacing, damageTextColor, BLACK, outlineSize);*/

	if (specialAbility)
		specialAbility->Display();

}

shared_ptr<AbilityIcon> BattleHero::GetSpecialAbility()
{
	return specialAbility;
}

bool BattleHero::GetSeekTarget()
{
	return specialAbility->GetSeekTarget();
}

void BattleHero::DisplayAbilityEffect()
{
	specialAbility->DisplayVisualEffect();
}

void BattleHero::SpecialAbilityDeselect()
{
	specialAbility->Deselect();
}

bool BattleHero::GetAbilityUsed()
{
	return specialAbility->GetAbilityUsed();
}

int BattleHero::GetAbilityManaCost()
{
	return abilityManaCost;
}

int BattleHero::GetAbilityModifier()
{
	return specialAbility->GetStatModifier();
}

void BattleHero::ResetAbilityUsed()
{
	specialAbility->ResetAbilityUsed();
}

void BattleHero::LeftMouseButtonPressed()
{	
	if (!IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) return;

	if (currentMana && *currentMana >= abilityManaCost)
		specialAbility->ClickIcon();

}

void BattleHero::SetManaPointers(int& availableMana, int& currentMana, int& usedMana)
{
	this->availableMana = &availableMana;
	this->currentMana = &currentMana;
	this->usedMana = &usedMana;
}

void BattleHero::AbilitySetTarget(BattleEntity* entity, Rectangle targetRect, std::function<void()> callback)
{
	if (!specialAbility->GetSeekTarget()) return;
	
	specialAbility->SetTarget(entity, targetRect, callback);

	*usedMana += abilityManaCost;

	*availableMana = *currentMana - *usedMana;
}

void BattleHero::DeselectAbility()
{
	specialAbility->Deselect();
}

void BattleHero::Death()
{
	taunting = false;
	dead = true;
	targeted = false;
	if (ranOutOffHealth && animationFinished)
	{
		animateDeath = false;
		cout << type << " DEATH ANIMATION FINISHED" << endl;
		animationFinished = false;
		stopAnimating = true;
		currentFrame = framesCount - 1;
		sourceRect.x = static_cast<float>(currentFrame * frameWidth);

		if (DeathCallback)
			DeathCallback();

		EndTurn();
	}
}
