#include "StrikeAndReturnSoldier.h"

StrikeAndReturnSoldier::StrikeAndReturnSoldier(Utilities::Relationship relationship,
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
	SetCell(*(grid->at(cellIndex).rect));
	coolOffDuration = 0.5f;
}

StrikeAndReturnSoldier::~StrikeAndReturnSoldier()
{
	save = false;
}

void StrikeAndReturnSoldier::Return()
{
	// Find path home

	
	movementPath = TrailPath(rectMoveToIndex, attackFromIndex);
	cout << "RECT MOVE TO INDEX " << rectMoveToIndex << endl;
	cout << "ATTACK FROM INDEX: " << attackFromIndex << endl;

	returning = true;
	offence = false;
	SetStance("walk");
	rectMoveToIndex = attackFromIndex;
	if (movementPath.size() <= 1)
	{
		auto& home = *(*grid)[attackFromIndex].rect;
		SetStance("idle");
		SetCell(home);
		cellIndex = attackFromIndex;
		move = false;
		returning = false;
		hadItsTurn = true;
		EndTurn();
		return;
	}

	tempIndex = 1; // start moving to the *next* cell (0 is current position)
	move = true;


	auto& next = (*grid)[movementPath[tempIndex]].rect;

	moveToCenter = {
		next->x + next->width / 2.0f,
		next->y + next->height - rect.height / 2.0f
	};

	auto& endRect = (*grid)[attackFromIndex].rect;

	/*UpdateAdjacentCells();*/

	if ((direction == Utilities::Direction::west && x > endRect->x) ||
		(direction == Utilities::Direction::east && x < endRect->x))
	{
		Flip();
		/*UpdateAdjacentCells();*/
	}
}

//void StrikeAndReturnSoldier::Return()
//{
//	// Find path to target
//	std::reverse(movementPath.begin(), movementPath.end());
//
//	/*if (movementPath.empty()) {
//		move = false;
//		SetStance("idle");
//		cout << Type() << " Attacker-finished animation sequence\n";
//		EndTurn();
//		hadItsTurn = true;
//		cout << type << "Had its turn" << endl;
//		return;
//	}*/
//
//	tempIndex = 1; // start moving to the *next* cell (0 is current position)
//	rectMoveToIndex = attackFromIndex;
//	move = true;
//	returning = true;
//
//	auto& tempRect = (*grid)[movementPath[tempIndex]].rect;
//
//	moveToCenter = {
//		tempRect->x + tempRect->width / 2.0f,
//		tempRect->y + tempRect->height - rect.height / 2.0f
//	};
//	auto& endRect = (*grid)[attackFromIndex].rect;
//
//	/*UpdateAdjacentCells();*/
//	SetStance("walk");
//
//	if ((direction == Utilities::Direction::west && x > endRect->x) ||
//		(direction == Utilities::Direction::east && x < endRect->x))
//	{
//		Flip();
//		/*UpdateAdjacentCells();*/
//	}
//}

void StrikeAndReturnSoldier::AttackEnemy()
{
	auto& enemy = (*grid)[enemyIndex].entity;

	// Still running in
	if (!CheckCollisionPointRec({ enemyCenter.x, enemyCenter.y }, rect))
	{
		float dx = enemyCenter.x - x;
		float dy = enemyCenter.y - y;
		float distance = sqrtf(dx * dx + dy * dy);
		if (distance > 1.0f)
		{
			dx /= distance;
			dy /= distance;
			const float speed = 5.0f;
			SetX(x + dx * speed);
			SetY(y + dy * speed);
			shadowRect.x += dx * speed;
			shadowRect.y += dy * speed;
		}
		return;
	}

	// Pause after the attack, then go home
	if (waitingAfterAttack)
	{
		attacking = false;         
		postAttackIdleTimer -= GetFrameTime();
		if (postAttackIdleTimer > 0.0f)
			return;

		EnemyRetaliate(enemy->GetAttack());
		attacking = false;
		animationFinished = false;

		if (ranOutOffHealth) {
			grid->at(attackFromIndex).entity = nullptr;
			grid->at(attackFromIndex).entityHealth = 0;
			grid->at(attackFromIndex).entityType = Utilities::EntityType::Unknown;
			cout << "DEAD" << endl;
			waitingAfterAttack = false;
			offence = false;
			return;
		}

		waitingAfterAttack = false;
		offence = false;

		/*if (!enemyIsAdjacent)*/
		//if(!dead)
		//	Return();

		/*else
		{
			SetX(moveToCenter.x);
			SetY(moveToCenter.y);
			UpdateShadowRect();
			EndTurn();
		}*/
		finishedAttack = true;
		return;
	}

	// Start the attack clip once
	if (!attacking && !animationFinished)
	{
		SetStance(attackType);
		attacking = true;           // Display() can now detect last frame / hit frame
		animationFinished = false;
		enemyHit = false;
		return;
	}

	// Contact frame
	if (attacking && enemyHit)
	{
		if (enemy)
		{
			enemy->ActivateGetHit(true, currentAttack);
		}
		enemyHit = false;
	}

	// Attack clip finished -> idle for a moment
	if (attacking && animationFinished)
	{
		SetStance("idle");
		waitingAfterAttack = true;
		postAttackIdleTimer = 0.6f;
	}
}


void StrikeAndReturnSoldier::Attack(shared_ptr<Cell> enemyCell)
{	
	drawTaunt = false;
	enemyIsAdjacent = false;
	this->enemyCell = enemyCell;
	auto& enemyRect = this->enemyCell->rect;
	enemy = enemyCell->entity;
	attackFromIndex = cellIndex;

	attackDirection = enemy->GetAttackDirection();

	this->enemyIndex = this->enemyCell->gridIndex;
	rectMoveToIndex = attackDirection.cell->gridIndex;

	auto path = TrailPath(cellIndex, rectMoveToIndex);

	enemyIsAdjacent = (rectMoveToIndex == cellIndex);

	auto it = find(adjacentCellIndexes.begin(), adjacentCellIndexes.end(), enemyIndex);
	regularAttack = (it != adjacentCellIndexes.end());

	//if (!enemyIsAdjacent)
	//{
	//	attackDirection = *enemy->GetAvailableAdjacentCell();
	//	rectMoveToIndex = attackDirection.cell->gridIndex;
	//}
	//else if (enemy->GetAvailableAdjacentCell() == nullptr) return -1;

	//enemyIsAdjacent = (rectMoveToIndex == cellIndex);

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
	if (!enemyIsAdjacent) Move(rectMoveToIndex);
}

void StrikeAndReturnSoldier::DrawShadow(int animationSpeed, float dt)
{
	// Display rotated semi-transparent shadow
	DrawTexturePro(
		*texture,
		sourceRect,
		shadowRect,
		shadowOrigin,
		0.0f,                 // tilt angle (e.g. -30 for west, 30 for east)
		Color{ 0, 0, 0, 100 }
	);
}

int StrikeAndReturnSoldier::GetMoveToIndex()
{
	return attackFromIndex;
}

void StrikeAndReturnSoldier::EnemyRetaliate(int amount)
{
	currentHealth -= amount;
	healthModifier = "-" + to_string(amount);
	std::function<void()> returnCallback = healthPopUp->onFinished = [this]()
		{
			waitingAfterAttack = false;
			offence = false;
			finishedAttack = true;
			Return();
		};
	SetStatsTextColor(maxHealth, currentHealth, healthTextColor);
	if (currentHealth <= 0)
	{

		HP_bar = nullptr;
		ranOutOffHealth = true;
		animateDeath = true;
		SetStance("death");
		healthPopUp->SetDrawOnce(true);
	}
	else
	{
		healthPopUp->SetDrawOnce(true, 0.0f, returnCallback);
	}
	
}