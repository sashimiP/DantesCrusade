#include "MountedSoldier.h"

MountedSoldier::MountedSoldier(Utilities::Relationship relationship,
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
	//ScaleDestRect(scalingFactor);
	SetCell(*(grid->at(cellIndex).rect));
	attackType = "charge";
}

MountedSoldier::~MountedSoldier()
{
	save = false;
}

void MountedSoldier::Update(bool selected)
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

	if (returnFromCharge) ReturnFromCharge();

	if (offence)
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

	if (drawTaunt)
		tauntAnimatedIcon->Update();

	if (skipTurn)
		skipTurnAnimatedIcon->Update();
}

//void MountedSoldier::Move(int rectIndex)
//{
//	move = true;
//	cellIndex = rectIndex;
//	rectMoveToIndex = rectIndex;
//	auto& tempRect = (*grid)[rectMoveToIndex].rect;
//
//	if (x > tempRect->x) Flip();
//	SetStance("walk");
//
//}

void MountedSoldier::MoveToCell()
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

void MountedSoldier::ReturnFromCharge()
{
	auto& tempRect = (*grid)[rectMoveToIndex].rect;
	Vector2 moveToCenter{
		tempRect->x + tempRect->width / 2.0f,
		tempRect->y + tempRect->height - rect.height / 2.0f
	};

	// If we're NOT inside the destination cell yet keep moving
	if (!CheckCollisionPointRec({ centerRect.x, centerRect.y + rect.height / 4.0f }, (*tempRect)))
	{
		float dx = moveToCenter.x - x;
		float dy = moveToCenter.y - y;

		float distance = sqrtf(dx * dx + dy * dy);

		if (distance > 1.0f) // just to avoid division by zero
		{
			dx /= distance;
			dy /= distance;

			float speed = 5.0f;
			SetX(x + dx * speed);
			SetY(y + dy * speed);
			shadowRect.x += dx * speed;
			shadowRect.y += dy * speed;
		}
	}
	else
	{
		// Snap to center of cell and stop
		/*SetX(moveToCenter.x);
		SetY(moveToCenter.y);*/


		if (direction != startingDirection) Flip();
		SetCell((*tempRect));

		SetStance("idle");
		if(taunting) drawTaunt = true;
		returnFromCharge = false;
		EndTurn();

	}
}

void MountedSoldier::AttackEnemy()
{
	auto& enemyRect = (*grid)[enemyIndex].rect;
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
	
	if (CheckCollisionRecs(rect, (*enemyRect)))
	{
		if (offence && !attacking)
		{
			SetStance("attack");
			enemy->ActivateGetHit(false, currentAttack);
			attacking = true;
			
		}
		if (CheckCollisionRecs(centerRect, (*enemyRect)))
		{
			
			enemyCenter = moveToCenter;

		}
	}

	if (x > SCR_WIDTH || x < -200)
	{
		
		returnFromCharge = true;
		attacking = false;
		offence = false;

		SetStance("walk");
	
		auto setX = (flipped) ? SCR_WIDTH : 0.0f;
		
		SetX(setX);
		SetY(enemyRect->y);	

		shadowRect.x = x;
		shadowRect.y = y + rect.height / 2.0f;
		shadowRect.y -= shadowRect.height / 2.0f;
		shadowOrigin = { shadowRect.width / 2,  shadowRect.height / 2 };
	}
	else if (y > SCR_HEIGHT || y < -200)
	{

		returnFromCharge = true;
		attacking = false;
		offence = false;

		SetStance("walk");

		auto setY = (y > SCR_HEIGHT) ? 0.0f : SCR_HEIGHT;
		SetX(enemyRect->x);
		SetY(setY);

		shadowRect.x = x;
		shadowRect.y = y + rect.height / 2.0f;
		shadowRect.y -= shadowRect.height / 2.0f;
		shadowOrigin = { shadowRect.width / 2,  shadowRect.height / 2 };
	}
}

void MountedSoldier::Attack(shared_ptr<Cell> enemyCell)
{
	drawTaunt = false;
	offence = true;
	this->enemyIndex = enemyCell->gridIndex;
	rectMoveToIndex = cellIndex;
	enemy = enemyCell->entity;

	auto& rectMoveTo = (*grid)[rectMoveToIndex].rect;
	auto& enemyRect = (*grid)[enemyIndex].rect;

	// Get enemy center
	enemyCenter = {
		enemyRect->x + enemyRect->width / 2.0f,
		enemyRect->y
	};

	// Compute trajectory from offscreen to enemy
	Vector2 direction = { enemyCenter.x - x, enemyCenter.y - y }; // vector to enemy
	float length = sqrt(direction.x * direction.x + direction.y * direction.y);
	Vector2 dirNorm = { direction.x / length, direction.y / length }; // normalize

	// Place moveToCenter offscreen along the trajectory
	float offscreenDistance = sqrt(SCR_WIDTH * SCR_WIDTH + SCR_HEIGHT * SCR_HEIGHT) + 200.0f;
	moveToCenter = {
		enemyCenter.x + dirNorm.x * offscreenDistance,
		enemyCenter.y + dirNorm.y * offscreenDistance
	};

	// Flip sprite if enemy is to the west
	/*if (x > enemyRect->x)
	{
		Flip();
	}*/

	// Set attack stance
	SetStance(this->attackType);

	// Apply scaling if needed
	//ScaleDestRect(scalingFactor);
}



//void MountedSoldier::Display(int animationSpeed, int offsetX, float dt)
//{
//	FrameData& frameData = (*texturesMap)[stance];
//	Texture2D& texture = frameData.texture;     // sprite sheet
//
//	animationTimer += dt;
//	if (animationTimer >= (1.0f / animationSpeed)) {
//		animationTimer = 0.0f;
//		currentFrame = (currentFrame + 1) % framesCount;
//		sourceRect.x = static_cast<float>(currentFrame * frameWidth);
//	}
//
//	DrawTexturePro(
//		texture,
//		sourceRect,
//		destRect,
//		center,
//		0,
//		RAYWHITE
//	);
//
//	DrawRectangleRec(centerRect, GREEN);
//
//}

//void MountedSoldier::SetCell(const Rectangle& cell)
//{
//	SetX(cell.x + cell.width / 2.0f);
//	SetY(cell.y);
//}

//void MountedSoldier::SetStance(string stance)
//{
//	this->stance = stance;
//	auto& spriteSheet = (*texturesMap)[this->stance];
//
//	framesCount = spriteSheet.frames;
//	colorCount = static_cast<float>(spriteSheet.colors);
//	frameWidth = static_cast<float>(spriteSheet.texture.width) / framesCount;
//	frameHeight = static_cast<float>(spriteSheet.texture.height) / colorCount;
//
//	color = (color > colorCount - 1) ? 0 : color;
//
//	sourceRect = Rectangle{ 0, 0 + frameHeight * color,static_cast<float>(frameWidth),static_cast<float> (frameHeight) };
//	destRect = Rectangle{ x , y, static_cast<float>(frameWidth),static_cast<float> (frameHeight) };
//	destRect = Rectangle{ destRect.x, destRect.y, destRect.width * scalingFactor, destRect.height * scalingFactor };
//	center = Vector2{ static_cast<float>(destRect.width / 2), static_cast<float> (destRect.height / 2) };
//	rect = Rectangle{ this->x - center.x,this->y - center.y,destRect.width,destRect.height };
//
//	if (direction == west) sourceRect.width *= (-1); // Must find a better way
//
//	centerRect = Rectangle{ x, y, 5, 5 };
//}

//void MountedSoldier::MouseOver(bool flag)
//{
//	if (move || offence || attacking)
//		return;
//
//	if (!mouseOver && flag)
//	{
//		mouseOver = flag;
//		SetStance("mouse_over");
//	}
//	else if (mouseOver && !flag)
//	{
//		mouseOver = flag;
//		SetStance("idle");
//	}
//}