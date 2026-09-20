#include "StrikeAndLure.h"

StrikeAndLure::StrikeAndLure(Utilities::Relationship relationship,
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
	StrikeAndReturnSoldier(relationship, 
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
	//animated_portrait = make_shared<Texture2D>(textureManager->succubus_blue);

	animSpeed = 30;

}

//int StrikeAndLure::Attack(int enemyIndex)
//{
//
//	offence = true;
//	this->enemyIndex = enemyIndex;
//	auto& enemyRect = (*grid)[enemyIndex].rect;
//	enemy = (*grid)[enemyIndex].entity;
//
//
//	rectMoveToIndex = enemyIndex - 1;
//	if (x > enemyRect->x)
//	{
//		rectMoveToIndex = enemyIndex + 1;
//		if (direction == east) Flip();
//
//	}
//
//	auto& rectMoveTo = (*grid)[rectMoveToIndex].rect;
//	// Get enemy center
//	enemyCenter = {
//		enemyRect->x + enemyRect->width / 2.0f,
//		enemyRect->y + enemyRect->height - frameHeight / 2.0f
//	};
//
//	moveToCenter = {
//		rectMoveTo->x + rectMoveTo->width / 2.0f,
//		rectMoveTo->y + rectMoveTo->height - frameHeight / 2.0f
//	};
//
//	//if (x > enemyRect->x && Entity::direction == east ||
//	//	Entity::direction == west && x < enemyRect->x)
//	//{
//	//	Flip();
//	//}
//
//	// Set attack stance
//	SetStance("walk");
//
//	return cellIndex;
//}

//void StrikeAndLure::Lure()
//{
//
//	if (!CheckCollisionPointRec({ enemyCenter.x, enemyCenter.y }, rect))
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
//			float speed = 5.0f;
//			SetX(x + dx * speed);
//			SetY(y + dy * speed);
//			shadowRect.x = x;
//			shadowRect.y = y + rect.height / 2.0f;
//			shadowRect.y -= shadowRect.height / 2.0f;
//		}
//	}
//	else if (!reachedEnemy && CheckCollisionPointRec({ enemyCenter.x, enemyCenter.y }, rect))
//	{
//		reachedEnemy = true;
//		coolOff = true;
//		cout << "Rached Enemy" << endl;
//		if (attackDirection.side == Side::east && direction != west)
//		{
//			cout << "Flip to face enemy's back" << endl;
//			Flip();
//			//UpdateAdjacentCells();
//		}
//		else if (attackDirection.side == Side::west && direction != east)
//		{
//			cout << "Flip to face enemy's front" << endl;
//			Flip();
//			//UpdateAdjacentCells();
//		}
//
//	}
//	else if (reachedEnemy)
//	{
//		timer += GetFrameTime();
//		if (!cellNextToEnemy && coolOff && !attacking && timer >= coolOffDuration)
//		{
//			cout << "Attack" << endl;
//			attacking = true;
//
//			SetStance("attack");
//			timer = 0.0f;
//		}
//		else if (attacking && !enemyHit && timer >= momentOfContact)
//		{
//			enemy->ActivateGetHit();
//			enemyHit = true;
//		}
//		else if (attacking && enemyHit && timer >= attackDuration)
//		{
//			cout << "CoolOff" << endl;
//			SetStance("idle");
//			// Snap to center of cell and stop
//			SetX(moveToCenter.x);
//			SetY(moveToCenter.y);
//			timer = 0.0f;
//			coolOff = true;
//			cellNextToEnemy = true;
//			attacking = false;
//			enemyHit = false;
//
//		}
//		else if (cellNextToEnemy && coolOff && timer >= coolOffDuration)
//		{
//			rectMoveToIndex = cellIndex;
//			auto& rectMoveTo = (*grid)[rectMoveToIndex].rect;
//			moveToCenter = {
//				rectMoveTo->x + rectMoveTo->width / 2.0f,
//				rectMoveTo->y + rectMoveTo->height - frameHeight / 2.0f
//			};
//			Return();
//			int availableCell = FindAvailableCell();
//			enemy->Lured(availableCell, movementPath);
//			(*grid)[availableCell].entity = enemy;
//			(*grid)[enemyIndex].entity = nullptr;
//			cellNextToEnemy = false;
//			offence = false;
//		}
//	}
//}

void StrikeAndLure::Lure()
{

	if (!CheckCollisionPointRec({ enemyCenter.x, enemyCenter.y }, rect))
	{
		float dx = enemyCenter.x - x;
		float dy = enemyCenter.y - y;

		float distance = sqrtf(dx * dx + dy * dy);

		if (distance > 1.0f) // just to avoid division by zero
		{
			dx /= distance;
			dy /= distance;

			float speed = 5.0f;
			SetX(x + dx * speed);
			SetY(y + dy * speed);
			shadowRect.x = x;
			shadowRect.y = y + rect.height / 2.0f;
			shadowRect.y -= shadowRect.height / 2.0f;
		}
	}
	else if (!reachedEnemy && CheckCollisionPointRec({ enemyCenter.x, enemyCenter.y }, rect))
	{
		reachedEnemy = true;
		coolOff = true;
		cout << "Rached Enemy" << endl;
		if (attackDirection.side == Side::west && direction != west)
			{
				cout << "Flip to face enemy's back" << endl;
				Flip();
				//UpdateAdjacentCells();
			}
			else if (attackDirection.side == Side::east && direction != east)
			{
				cout << "Flip to face enemy's front" << endl;
				Flip();
				//UpdateAdjacentCells();
			}

	}
	else if (reachedEnemy)
	{
		
		if (!cellNextToEnemy && coolOff && !attacking && animationFinished)
		{
			cout << "Attack" << endl;
			attacking = true;

			SetStance("attack");
			animationFinished = false;
		}
		else if (attacking && enemyHit)
		{
			enemy->ActivateGetHit();
			enemyHit = false;
		}
		else if (attacking && !enemyHit && animationFinished)
		{
			cout << "CoolOff" << endl;
			SetStance("idle");
			// Snap to center of cell and stop
			SetX(moveToCenter.x);
			SetY(moveToCenter.y);
			animationFinished = false;
			coolOff = true;
			cellNextToEnemy = true;
			attacking = false;
			

		}
		else if (cellNextToEnemy && coolOff && animationFinished)
		{
			rectMoveToIndex = cellIndex;
			auto& rectMoveTo = (*grid)[rectMoveToIndex].rect;
			moveToCenter = {
				rectMoveTo->x + rectMoveTo->width / 2.0f,
				rectMoveTo->y + rectMoveTo->height - frameHeight / 2.0f
			};
			Return();
			int availableCell = FindAvailableCell();
			enemy->Lured(availableCell, movementPath);
			(*grid)[availableCell].entity = enemy;
			(*grid)[enemyIndex].entity = nullptr;
			cellNextToEnemy = false;
			offence = false;
			animationFinished = false;
			coolOff = false;
			reachedEnemy = false;

			finishedAttack = true;
		}
	}
}


void StrikeAndLure::AttackEnemy()
{
	//auto& enemyRect = (*grid)[enemyIndex].rect;
	
	if (regularAttack)
	{
		cout << "adjacent" << endl;
		StrikeAndReturnSoldier::AttackEnemy();
	}
	else
	{
		Lure();
	}

}

int StrikeAndLure::FindAvailableCell()
{
	if (!grid || grid->empty())
		return -1;

	//int gridSize = grid->size();

	//// Expand outward
	//for (int distance = 1; distance < gridSize; distance++)
	//{
	//	// Left
	//	int westIndex = cellIndex - distance;
	//	if (westIndex >= 0 && (*grid)[westIndex].entity == nullptr)
	//		return westIndex;

	//	// Right
	//	int eastIndex = cellIndex + distance;
	//	if (eastIndex < gridSize && (*grid)[eastIndex].entity == nullptr)
	//		return eastIndex;
	//}

	//// No empty cell found
	//return -1;

	int n = grid->size();
	vector<bool>visited(n, false);

	vector<int> res; // create an array to store the traversal
	vector<int> traversal;

	std::queue<int> q;
	visited[attackFromIndex] = true;
	q.push(attackFromIndex);

	while (!q.empty())
	{
		int currentIndex = q.front();
		q.pop();
		if ((*grid)[currentIndex].entity == nullptr) return currentIndex;

		res.push_back(currentIndex);


		for (int x : GetAdjacentCells(currentIndex))
		{
			if (!visited[x])
			{
				visited[x] = true;
				q.push(x);				

			}
		}

	}

	return -1;
}

void StrikeAndLure::Return()
{
	// Find path to target
	std::reverse(movementPath.begin(), movementPath.end());

	if (movementPath.empty()) {
		move = false;
		SetStance("idle");
		cout << Type() << " Attacker-finished animation sequence\n";
		EndTurn();
		return;
	}

	tempIndex = 1; // start moving to the *next* cell (0 is current position)
	rectMoveToIndex = attackFromIndex;
	returning = true;
	move = true;

	auto& tempRect = (*grid)[movementPath[tempIndex]].rect;

	moveToCenter = {
		tempRect->x + tempRect->width / 2.0f,
		tempRect->y + tempRect->height - rect.height / 2.0f
	};
	auto& endRect = (*grid)[attackFromIndex].rect;

	/*UpdateAdjacentCells();*/
	SetStance("lure");

	//if ((direction == east && x > endRect->x) ||
	//	(direction == west && x < endRect->x))
	//{
	//	Flip();
	//	/*UpdateAdjacentCells();*/
	//}
}

//void StrikeAndLure::Display(int animationSpeed, int offsetX, float dt)
//{
//	//DrawRectangleLinesEx(rect, 1, RED);
//
//	animationTimer += dt;
//	if (animationTimer >= (1.0f / animationSpeed)) {
//		animationTimer = 0.0f;
//		currentFrame = (currentFrame + 1) % framesCount;
//		if (coolOff)cout << currentFrame << endl;
//		if (currentFrame == 0)
//		{
//			if (attacking ||
//				getHit ||
//				getHolyHit ||
//				getUp ||
//				transform ||
//				defending ||
//				coolOff ||
//				retaliating)
//			{
//				animationFinished = true;
//
//				//return;
//			}
//
//		}
//		else if ((attacking || retaliating) && currentFrame == momentOfContact)
//		{
//			enemyHit = true;
//
//		}
//
//		sourceRect.x = static_cast<float>(currentFrame * frameWidth);
//
//	}
//
//	DrawTexturePro(
//		*texture,
//		sourceRect,
//		destRect,
//		center,
//		0,
//		RAYWHITE
//	);
//
//	DrawRectangleRec(centerRect, GREEN);
//
//	if (drawDialoguePortrait || mouseOver || selected)
//	{
//		// Portrait Animation Update
//		portraitFrameTimer += dt;
//		if (portraitFrameTimer >= (1.0f / animationSpeed)) {
//			portraitFrameTimer = 0.0f;
//
//			portraitFrameIndex++;
//			if (portraitFrameIndex >= totalPortraitFrames)
//				portraitFrameIndex = 0;
//
//			int col = portraitFrameIndex % animPortCols;
//			int row = portraitFrameIndex / animPortCols;
//
//			portraitSource.x = static_cast<float>(col * animPortraitWidth);
//			portraitSource.y = static_cast<float>(row * animPortraitHeight);
//		}
//
//		// Portrait Display
//		DrawTexturePro(
//			*animatedPortrait,
//			portraitSource,
//			portraitDest,
//			origin,
//			0.0f,
//			RAYWHITE
//		);
//	
//		/*else if (relationship == Relationship::Enemy)
//			DrawTexturePro(*dialoguePortrait, leftPortraitSource, leftPortraitDest, origin, 0.0f, RAYWHITE);*/
//		if (squareFrame)
//			DrawTexturePro(*squareFrame, squareFrameSourceRect, portraitDest, origin, 0.0f, RAYWHITE);
//	}
//
//	for (auto& adjCell : adjacentCells)
//	{
//		//if (adjCell.cell && adjCell.mouseCollide)
//		if (adjCell.cell && adjCell.cell->drawRect)
//			DrawRectangleRec(*adjCell.cell->rect, BATTLE_CELL_MOUSE_OVER_CLR);
//	}
//
//
//}