#include "SpiderQueen.h"

SpiderQueen::SpiderQueen(Utilities::Relationship relationship,
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
}

SpiderQueen::~SpiderQueen()
{
	save = false;
}

void SpiderQueen::MoveToCell()
{

	if (!move || movementPath.empty()) return;

	// If we've reached the destination
	if (tempIndex >= movementPath.size()) {
		auto& endOfPathRect = (*grid)[rectMoveToIndex].rect;

		SetCell((*endOfPathRect));
		SetStance("idle");

		if (!offence && (direction != startingDirection)) {
			Flip();

		}

		UpdateAdjacentCells();
		AreEnemiesAdjacent();
		move = false;

		return;
	}

	// Current target cell
	int targetCellIndex = movementPath[tempIndex];
	auto& tempRect = (*grid)[targetCellIndex].rect;

	// If we're not inside the target cell yet, keep moving
	if (!CheckCollisionRecs({ centerRect.x, centerRect.y + rect.height / 4.0f }, (*tempRect))) {
		float dx = moveToCenter.x - x;
		float dy = moveToCenter.y - y;
		float distance = sqrtf(dx * dx + dy * dy);

		if (distance > 1.0f) {
			dx /= distance;
			dy /= distance;

			float speed = 5.0f;
			SetX(x + dx * speed);
			SetY(y + dy * speed);
			shadowRect.x += dx * speed;
			shadowRect.y += dy * speed;
		}
	}
	else {
		// Snap to target cell
	/*	SetCell((*tempRect));*/
		cellIndex = targetCellIndex;

		// Advance to the next step in the path
		tempIndex++;
		if (tempIndex < movementPath.size()) {
			auto& nextRect = (*grid)[movementPath[tempIndex]].rect;
			moveToCenter = {
				nextRect->x + nextRect->width / 2.0f,
				nextRect->y + nextRect->height - rect.height / 2.0f
			};
		}
	}


}

void SpiderQueen::AttackEnemy()
{
	cellIndex = rectMoveToIndex;
	auto& rectMoveTo = (*grid)[rectMoveToIndex].rect;
	auto& enemyRect = (*grid)[enemyIndex].rect;
	auto& enemy = (*grid)[enemyIndex].entity;

	Vector2 moveToCenter{
		rectMoveTo->x + rectMoveTo->width / 2.0f,
		rectMoveTo->y + rectMoveTo->height - rect.height / 2.0f
	};

	timer += GetFrameTime();
	if (jumpUp && timer >= jumpDuration)
	{
		SetStance("fall_down");

		SetCell(*rectMoveTo);
		jumpUp = false;
		fallDown = true;
		timer = 0.0f;
		
	}
	else if (fallDown && timer >= fallDuration)
	{
		SetStance("idle");
		SetCell(*rectMoveTo);
		coolOff = true;
		fallDown = false;
		timer = 0.0f;

	}
	else if (coolOff && timer >= coolOffDuration)
	{
		SetStance("attack");
		attacking = true;
		coolOff = false;
		timer = 0.0f;
		//enemy->ActivateRetaliate(cellIndex);
	}
	else if (attacking && !enemyHit && timer >= momentOfContact)
	{
		enemy->nextToEnemy = true;
		enemy->ActivateRetaliate(currentAttack);
		enemyHit = true;
	}
	//else if (attacking && timer >= attackDuration)
	//{
	//	
	//	SetStance("idle");
	//	//enemy->SetStance("idle");
	//	attacking = false;
	//	enemyHit = false;
	//	offence = false;
	//	timer = 0.0f;
	//}



}

void SpiderQueen::Attack(shared_ptr<Cell> enemyCell)
{
	/*this->enemyCell = enemyCell;
	auto& enemyRect = (*grid)[enemyIndex].rect;
	enemy = (*grid)[enemyIndex].entity;*/

	this->enemyCell = enemyCell;
	auto& enemyRect = this->enemyCell->rect;
	enemy = enemyCell->entity;

	offence = true;
	this->enemyIndex = enemyCell->gridIndex;
	attackDirection = enemy->GetAttackDirection();
	rectMoveToIndex = attackDirection.cell->gridIndex;
	//if (x > enemyRect->x)
	//{
	//	if (direction == east) Flip();

	//}
	//else if (direction == west && x < enemyRect->x)
	//{
	//	Flip();
	//}
	//
	/*if (enemyIndex < cellIndex - 1 || enemyIndex > cellIndex + 1)
	{
		SetStance("jump_up");
		jumpUp = true;
	}*/
	if (cellIndex != rectMoveToIndex)
	{
	
		SetStance("jump_up");
		if (attackDirection.side == Side::east && direction != Utilities::Direction::east)
		{
			cout << "Flip to face enemy's back" << endl;
			Flip();
			//UpdateAdjacentCells();
		}
		else if (attackDirection.side == Side::west && direction != Utilities::Direction::west)
		{
			cout << "Flip to face enemy's front" << endl;
			Flip();
			//UpdateAdjacentCells();
		}
		jumpUp = true;
	}
	else
	{
		if (attackDirection.side == Side::east && direction != Utilities::Direction::east)
		{
			cout << "Flip to face enemy's back" << endl;
			Flip();
			//UpdateAdjacentCells();
		}
		else if (attackDirection.side == Side::west && direction != Utilities::Direction::west)
		{
			cout << "Flip to face enemy's front" << endl;
			Flip();
			//UpdateAdjacentCells();
		}
		attacking = true;
		SetStance("attack");
	}
	SetCell(*(*grid)[cellIndex].rect);

}

void SpiderQueen::DrawShadow(int animationSpeed, float dt)
{
	// Origin at top-center so it pivots from feet
	Vector2 origin = { shadowRect.width / 2,  shadowRect.height / 2 };

	// Display rotated semi-transparent shadow
	DrawTexturePro(
		*texture,
		sourceRect,
		shadowRect,
		origin,
		0.0f,
		Color{ 0, 0, 0, 100 }
	);
}

void SpiderQueen::SetCell(const Rectangle& cell)
{

	auto adjustX = (direction == Utilities::Direction::west) ? cell.x - 20.0f  : cell.x + cell.width+20.0f;
	auto adjustY = cell.y + cell.height - rect.height / 2.0f;

	SetX(adjustX);
	SetY(adjustY);

	shadowRect = {
	this->x,
	this->y + rect.height / 2.0f,
	destRect.width * 1.2f,    // stretched
	rect.height * 0.25f       // squashed flat
	};
	shadowRect.y -= shadowRect.height / 2.0f;
	

}

void SpiderQueen::SetStance(string stance, bool updateRect)
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
	destRect = Rectangle{ destRect.x, destRect.y, destRect.width * scalingFactor, destRect.height * scalingFactor };

	if (updateRect)
	{
		rect = Rectangle{ x - center.x, y - center.y, destRect.width, destRect.height };
		center = Vector2{ static_cast<float>(destRect.width / 2), static_cast<float> (destRect.height / 2) };
	}

	if (direction == west) sourceRect.width *= (-1); // Must find a better way

	centerRect = Rectangle{ x, y, 5, 5 };
}