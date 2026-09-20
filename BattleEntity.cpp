#include "BattleEntity.h"

BattleEntity::BattleEntity(Relationship relationship,
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
	relationship(relationship),
	cellIndex(cellIndex),
	grid(grid),
	scalingFactor(entityStats[this->type].scale),
	entitySize(entityStats[this->type].size)
{
	numberOfRows = std::floor((SCR_HEIGHT / BATTLE_CELL_HEIGHT)) - 4;
	numberOfColumns = (SCR_WIDTH / BATTLE_CELL_WIDTH) - 9;


	
	squareFrame = this->textureManager->Asset("square_frame");
	
	mediaPortrait = textureManager->UnitMedia(this->type + "_" + to_string(color));
	SetMediaPortrait();



	squareFrameSourceRect = Rectangle{ 0.0f, 0.0f, static_cast<float>(squareFrame->width), static_cast<float>(squareFrame->height) };

	origin = { 0.0f, 0.0f };

	stats_bar = this->textureManager->Asset("stats_bar");
	health100 = this->textureManager->Asset("health_100");
	health75 = this->textureManager->Asset("health_75");
	health50 = this->textureManager->Asset("health_50");
	health25 = this->textureManager->Asset("health_25");
	health10 = this->textureManager->Asset("health_10");

	HP_bar = health100;

	movementPath.reserve(this->grid->size());
	UpdateAdjacentCells();

	startingDirection = this->direction;
	momentOfContact = entityStats[type].momentOfContact;
	ScaleDestRect(scalingFactor);

	UpdateShadowRect();

	attackType = "attack";

	skipTurn = true;
	hadItsTurn = false;

	rank = entityStats[this->type].rank;
	maxHealth = entityStats[this->type].health;
	currentHealth = maxHealth;
	startingAttack = entityStats[this->type].damage;
	currentAttack = startingAttack;
	movementPoints = 10;
	taunting = entityStats[this->type].taunting;
	evade = entityStats[this->type].evade;
	drawTaunt = taunting;

	float scaleIcons = 2.5f;

	healthIcon = textureManager->Asset("health_icon");
	healthIconSource = {
		0.0f,
		0.0f,
		static_cast<float>(healthIcon->width),
		static_cast<float>(healthIcon->height)
	};

	healthIconDest_portrait = {
		portraitDest.x,
		portraitDest.y + portraitDest.height - healthIconSource.height * scaleIcons - 10.f,
		healthIconSource.width * scaleIcons,
		healthIconSource.height * scaleIcons
	};

	healthPos = {
		healthIconDest_portrait.x + healthIconDest_portrait.width / 2 - MeasureText(to_string(currentHealth).c_str(), statsFontSize) / 2,
		healthIconDest_portrait.y + healthIconDest_portrait.height / 2 - statsFontSize * 0.5f
	};

	damageIcon = textureManager->Asset("damage_icon");
	damageIconSource = {
		0.0f,
		0.0f,
		static_cast<float>(damageIcon->width),
		static_cast<float>(damageIcon->height)
	};

	damageIconDest_portrait = {
		portraitDest.x - portraitDest.width - damageIconSource.width * scaleIcons,
		portraitDest.y + portraitDest.height - healthIconSource.height * scaleIcons - 10.f,
		damageIconSource.width* scaleIcons,
		damageIconSource.height* scaleIcons
	};

	damagePos = {
	damageIconDest_portrait.x + damageIconDest_portrait.width / 2 - MeasureText(to_string(currentAttack).c_str(), statsFontSize) / 2,
	damageIconDest_portrait.y + damageIconDest_portrait.height / 2 - statsFontSize * 0.5f
	};

	manaCostIcon = textureManager->Asset("mana_stone");
	manaIconSource = {
		0.0f,
		0.0f,
		static_cast<float>(manaCostIcon->width),
		static_cast<float>(manaCostIcon->height)
	};

	manaIconDest = {
		portraitDest.x,
		portraitDest.y,
		manaIconSource.width* scaleIcons,
		manaIconSource.height* scaleIcons
	};

	manaCostPos = {
	manaIconDest.x + manaIconDest.width / 2 - MeasureText(to_string(rank).c_str(), statsFontSize) / 2,
	manaIconDest.y + manaIconDest.height / 2 - statsFontSize * 0.5f
	};

	tauntIcon = textureManager->Asset("taunt_icon");
	tauntIconSource = {
		0.0f,
		0.0f,
		static_cast<float>(tauntIcon->width),
		static_cast<float>(tauntIcon->height)
	};
	tauntIconDest = {
		   healthIconDest_portrait.x,
		   healthIconDest_portrait.y - tauntIcon->height * scaleIcons,
		   static_cast<float>(tauntIcon->width * scaleIcons),
		   static_cast<float>(tauntIcon->height * scaleIcons)
		};

	evadeIcon = textureManager->Asset("evade_icon");
	evadeIconSource = {
		0.0f,
		0.0f,
		static_cast<float>(evadeIcon->width),
		static_cast<float>(evadeIcon->height)
	};

	if (!taunting)
	{
		evadeIconDest = tauntIconDest;
	}
	else
	{
		evadeIconDest = {
			   manaIconDest.x,
			   manaIconDest.y,
			   static_cast<float>(evadeIcon->width * scaleIcons),
			   static_cast<float>(evadeIcon->height * scaleIcons)
		};

		evadeIconDest.y += evadeIconDest.height * 1.1f;
	}


	skipTurnIcon = textureManager->Asset("skip_turn_icon");
	skipTurnIconSource = {
		0.0f,
		0.0f,
		static_cast<float>(skipTurnIcon->width),
		static_cast<float>(skipTurnIcon->height)
	};

	skipTurnIconDest = {
		   portraitDest.x - portraitDest.width - skipTurnIcon->width * 0.5f,
			portraitDest.y,
		   static_cast<float>(skipTurnIcon->width),
		   static_cast<float>(skipTurnIcon->height)
	};

	healthPopUp = make_unique<AnimatedAsset>("health_popup", 10, textureManager);

	auto& cell = grid->at(cellIndex);
	auto& cellrect = cell.rect;

	/*healthIconDest = {
		cellrect->x - healthIconSource.width,
		cellrect->y + cellrect->height - healthIconSource.height,
		healthIconSource.width,
		healthIconSource.height
	};

	damageIconDest = {
		cellrect->x + cellrect->width,
		cellrect->y + cellrect->height - damageIconSource.height,
		damageIconSource.width,
		damageIconSource.height
	};*/


	float width = 88.f;
	float height = 88.f;

	healthPopUp->SetCenterDest(
		cellrect->x,
		cellrect->y + cellrect->height - height,
		width,
		height);

	healthPopUpTextPos = {
		cellrect->x,
		cellrect->y + cellrect->height - height
	};

	damagePopUp = make_unique<AnimatedAsset>("damage_popup", 10, textureManager);
	damagePopUp->SetCenterDest(
		cellrect->x + cellrect->width,
		cellrect->y + cellrect->height - height,
		width,
		height);

	damagePopUpTextPos = {
		cellrect->x + cellrect->width,
		cellrect->y + cellrect->height - height,
	};

	tauntAnimatedIcon = make_unique<AnimatedAsset>("taunt_animated", 5, textureManager);
	
	if (relationship == Utilities::Relationship::Ally)
	{
		tauntAnimatedIcon->SetPosition(
			cellrect->x + cellrect->width,
			cellrect->y + cellrect->height * 0.5f - tauntAnimatedIcon->GetDest().height * 0.5f
		);
	}
	else if (relationship == Utilities::Relationship::Enemy)
	{
		tauntAnimatedIcon->SetPosition(
			cellrect->x - tauntAnimatedIcon->GetDest().width,
			cellrect->y + cellrect->height * 0.5f - tauntAnimatedIcon->GetDest().height * 0.5f
		);
	}

	skipTurnAnimatedIcon = make_unique<AnimatedAsset>("skip_turn_animated_icon", 6, textureManager);
	skipTurnAnimatedIcon->SetPosition(
		cellrect->x + cellrect->width * 0.5f,
		cellrect->y - skipTurnAnimatedIcon->GetDest().height
	);

	reachableCells = ReachableCells(this->cellIndex, movementPoints);

}

BattleEntity::~BattleEntity()
{
	save = false;
	//if (mediaPortrait) UnloadMedia(mediaPortrait.get());

	//delete mediaPortrait.get();
}

void BattleEntity::SetMediaPortrait()
{

	// SAFETY CHECK
	if (!mediaPortrait) return;

	float scale = std::min(
		240.0f / (float)this->mediaPortrait->videoTexture.width,
		360.0f / (float)this->mediaPortrait->videoTexture.height
	);

	//cout << "SET DIALOGUE PLATFORM" << endl;
	portraitSource = {
	0.0f,
	0.0f,
	static_cast<float>(this->mediaPortrait->videoTexture.width),
	static_cast<float>(this->mediaPortrait->videoTexture.height)
	};

	portraitPosX = 0;
	portraitPosY = (relationship == Relationship::Ally) ? 0 : mediaPortrait->videoTexture.height * scale;

	portraitDest = {
		static_cast<float>(portraitPosX),
		static_cast<float>(portraitPosY),
		-static_cast<float>(this->mediaPortrait->videoTexture.width * scale),
		static_cast<float>(this->mediaPortrait->videoTexture.height * scale)
	};

	cout << "\nMEDIA PORTRAIT WIDTH: " << portraitDest.width << " MEDIA PORTRAIT HEIGH: " << portraitDest.height << endl;
}

void BattleEntity::Update(bool selected)
{
	Vector2 mousePos = GetMousePosition();
	
	ShakeEntity();

	if (mediaPortrait)
	{
		const bool shouldShow = drawDialoguePortrait || mouseOver||this->selected;
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

	if (drawTaunt)
		tauntAnimatedIcon->Update();

	if (skipTurn)
		skipTurnAnimatedIcon->Update();

	//if(healthPopUp->GetDrawOnce());
	//	healthPopUp->Update();
}


//shared_ptr<BattleEntity> findEnemyWithLeastHealth(const vector<shared_ptr<BattleEntity>>& opposingArmy)
//{
//	auto leastHelth = opposingArmy[0];
//
//	for (auto& unit : opposingArmy)
//	{
//		if (leastHelth->GetHealth() > unit->GetHealth())
//			leastHelth = unit;
//	}
//
//	return leastHelth;
//}

void BattleEntity::Select()
{
	selected = !selected;
	cout << type << " selected" << endl;
	cout << "Moment Of contact: " <<momentOfContact << endl;

	if (relationship == Relationship::Ally)
	{
		for (auto& i : reachableCells)
			(*grid)[i].color = BATTLE_CELL_MOUSE_OVER_CLR;
	}

}

//void BattleEntity::FindTargetAttack()
//{
//	auto it = findEnemyWithLeastHealth(opposingArmy);
//
//	cout << "Enemy with least health: " << it->Type() << ": " << it->GetHealth() << endl;
//
//	Attack(it->GetCellIndex());
//
//}

void BattleEntity::Deselect()
{
	selected = false;
	rectMoveToIndex = -1;
	
	if (relationship == Relationship::Ally)
	{
		for (auto& i : reachableCells)
			(*grid)[i].color = BATTLE_CELL_CLR;
	}
}

void BattleEntity::Move(int rectIndex)
{
	// Find path to target
	movementPath.clear();
	
	if (!CanReachCell(rectIndex)) return;
	movementPath = TrailPath(cellIndex, rectIndex);

	if (movementPath.empty()) {
		move = false;
		SetStance("idle");
		return;
	}

	tempIndex = 1; // start moving to the *next* cell (0 is current position)
	rectMoveToIndex = rectIndex;
	move = true;

	auto& tempRect = (*grid)[movementPath[tempIndex]].rect;

	moveToCenter = {
		tempRect->x + tempRect->width / 2.0f,
		tempRect->y + tempRect->height - rect.height / 2.0f
	};
	auto& endRect = (*grid)[rectIndex].rect;

	/*UpdateAdjacentCells();*/
	SetStance("walk");

	if ((direction == Utilities::Direction::east && x > endRect->x) ||
		(direction == Utilities::Direction::west && x < endRect->x))
	{
		Flip();
		cout << "Flip" << endl;
		/*UpdateAdjacentCells();*/
	}
}

void BattleEntity::Move(vector<int>& path)
{
	// Find path to target
	movementPath.clear();
	movementPath = path;

	if (movementPath.empty()) {
		move = false;
		SetStance("idle");
		return;
	}

	tempIndex = 1; // start moving to the *next* cell (0 is current position)
	rectMoveToIndex = path[path.size()-1];
	move = true;

	auto& tempRect = (*grid)[movementPath[tempIndex]].rect;

	moveToCenter = {
		tempRect->x + tempRect->width / 2.0f,
		tempRect->y + tempRect->height - rect.height / 2.0f
	};
	auto& endRect = (*grid)[rectMoveToIndex].rect;

	/*UpdateAdjacentCells();*/
	SetStance("walk");

	if ((direction == Utilities::Direction::west && x > endRect->x) ||
		(direction == Utilities::Direction::east && x < endRect->x))
	{
		Flip();
		/*UpdateAdjacentCells();*/
		cout << "Flip" << endl;
	}
}

void BattleEntity::MoveToCell()
{
	if (!move || movementPath.empty()) return;

	// If we've reached the destination
	if (tempIndex >= movementPath.size()) {
		auto& endOfPathRect = (*grid)[rectMoveToIndex].rect;

		int& index = rectMoveToIndex;
		Rectangle& deathCell = *grid->at(index).rect;

		const Rectangle& healthRect = healthPopUp->GetDest();

		healthPopUp->SetCenterDest(
			deathCell.x,
			deathCell.y + deathCell.height - healthRect.height,
			healthRect.width,
			healthRect.height);

		healthPopUpTextPos = {
			deathCell.x,
			deathCell.y + deathCell.height - healthRect.height
		};

		SetCell((*endOfPathRect));
		SetStance("idle");
		
		
		if (!offence && (direction != startingDirection)) {
			Flip();
			
		}

		UpdateAdjacentCells();
		AreEnemiesAdjacent();
		move = false;

		if ((!offence && !lured) || returning)
		{
			cout << Type() << " Attacker-finished animation sequence\n";
			if (returning) {
				returning = false;
				drawTaunt = taunting;
				hadItsTurn = true;
				cout << type << " Had its turn" << endl;
			}
			EndTurn();
		}
		else if (lured)
		{
			cout << Type() << " Defender-finished animation sequence\n";
			lured = false;
			EndTurn();
		}
		
		//EndTurn();
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

vector<int> BattleEntity::TrailPath(int startIndex, int endIndex)
{
	int n = grid->size();
	vector<bool>visited(n, false);
    std::vector<int> distances(n);
	std::vector<int> parents(n, -1);
	
	vector<int> res; // create an array to store the traversal
	vector<int> traversal;

	std::queue<int> q;
	visited[startIndex] = true;
	//distances[startIndex] = 0;
	parents[startIndex] = -1;
	q.push(startIndex);

	while (!q.empty())
	{
		int currentIndex = q.front();
		q.pop();
		if (currentIndex == endIndex) break;
		
		res.push_back(currentIndex);
		
		
		for (int x : GetAdjacentCells(currentIndex))
		{
			if (visited[x])
			{
				continue;
			}

			bool blocked = (*grid)[x].entity &&
				(*grid)[x].entity.get() != this &&
				x != endIndex;

			if (blocked)
				continue;
				

			visited[x] = true;
			q.push(x);
			//distances[x] = distances[currentIndex] + 1;
			parents[x] = currentIndex;	
						
		}
		
	}
	vector<int> path;
	if (visited[endIndex] == 0)
	{
		cout << "No path" << endl;
	}
	else
	{
		
		int x = endIndex;
		while (x != -1)
		{
			path.push_back(x);
			x = parents[x];
		}

		std::reverse(path.begin(), path.end());

		
	}
	//cout << "PATH: " << endl;
	//for (auto p : path)
	//	cout << p << endl;

	return path;

}

vector<int> BattleEntity::ReachableCells(int startIndex, int movementPoints)
{
	int n = grid->size();
	vector<bool> visited(n, false);
	vector<int> distance(n, INT_MAX);
	vector<int> reachable;

	queue<int> q;

	visited[startIndex] = true;
	distance[startIndex] = 0;
	q.push(startIndex);

	while (!q.empty())
	{
		int current = q.front();
		q.pop();

		reachable.push_back(current);

		for (int nxt : GetAdjacentCells(current))
		{
			if (!visited[nxt])
			{
				int newCost = distance[current] + 1; // assuming each move = 1 cost
				if (newCost <= movementPoints)
				{
					visited[nxt] = true;
					distance[nxt] = newCost;
					q.push(nxt);
				}
			}
		}
	}

	return reachable;
}

bool BattleEntity::CanReachCell(int cellIndex)
{
	auto it = find(reachableCells.begin(), reachableCells.end(), cellIndex);
	return (it != reachableCells.end());
}


void BattleEntity::UpdateAdjacentCells()
{

	cout << "Update Adjacent Cells" << endl;
	//nextToEnemy = false;
	adjacentCellIndexes.clear();
	int rows = numberOfRows;      // 6
	int cols = numberOfColumns;   // 11
	// total(grid->size()) = rows * cols = 66

	int r = cellIndex / cols;
	int c = cellIndex % cols;

	// Directions: N, S, E, W, NE, NW, SE, SW
	int dRow[] = { -1, 1, 0, 0, -1, -1, 1, 1 };
	int dCol[] = { 0, 0, 1, -1, 1, -1, 1, -1 };
	adjacentCells = {
		{Side::north},
		{Side::south},
		{Side::east},
		{Side::west},
		{Side::north_east},
		{Side::north_west},
		{Side::south_east},
		{Side::south_west} 
	};

	for (int i = 0; i < 8; i++) {
		int nr = r + dRow[i];
		int nc = c + dCol[i];

		if (nr >= 0 && nr < rows &&
			nc >= 0 && nc < cols)
		{
			int newIndex = nr * cols + nc;
			adjacentCellIndexes.push_back(newIndex);
			adjacentCells[i].cell = &(*grid)[newIndex];

		}
		else
		{
			adjacentCells[i].cell = nullptr;
		}
	}

	//for (auto& adjCell : adjacentCells)
	//{
	//	if (adjCell.cell && adjCell.cell->entity)
	//	{
	//		nextToEnemy = true;
	//		break;
	//	}
	//}
}

void BattleEntity::Attack(shared_ptr<Cell> enemyCell)
{
	/*for (auto& adjCell : adjacentCells)
	{
		if (adjCell.cell && (enemyIndex == adjCell.cell->gridIndex))
		{
			nextToEnemy = true;
			break;
		}
		else
		{
			nextToEnemy = false;
		}
	}*/

	drawTaunt = false;
	cout <<type<< " - Attack" << endl;
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

	if (path.size() > movementPoints+1)
	{
		int adjust = path.size() - movementPoints-1;
		path.erase(path.end() - adjust, path.end());
		enemy->finishedAnimationSequence = true;
		Move(path);
		rectMoveToIndex = path[path.size() - 1];
	}

	enemyIsAdjacent = (rectMoveToIndex == cellIndex);

	
	if(!enemyIsAdjacent)
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

void BattleEntity::AttackEnemy()
{

	// If we're NOT inside the destination cell yet keep moving
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
			shadowRect.x += dx * speed;
			shadowRect.y += dy * speed;
		}
	}
	else
	{


		if (offence && !coolOff && !attacking)
		{
			
			coolOff = true;
			SetStance("idle");
		}
		
		else if (coolOff && animationFinished && !attacking)
		{
			SetStance(attackType);
			attacking = true;
			coolOff = false;
			animationFinished = false;
		}
		else if (enemyHit)
		{
			enemy->nextToEnemy = true;
			enemy->ActivateRetaliate(currentAttack);
			enemyHit = false;
		}
		else if (!finishedAttack && animationFinished)
		{
			//timer = 0.0f;
			animationFinished = false;
			SetStance("idle");
			enemy->nextToEnemy = true;
			//enemy->ActivateRetaliate(rectMoveToIndex);
			// Snap to center of cell and stop
			SetX(moveToCenter.x);
			SetY(moveToCenter.y);
			UpdateShadowRect();
			attacking = false;
			//enemyHit = false;
			offence = false;

			finishedAttack = true;
			
			if (enemy->retaliated)
			{
				cout << "Enemy has retaliated" << endl;
				cout << Type()<< " Attacker-finished animation sequence\n";
				EndTurn();
			}
		}

	}
}

void BattleEntity::UpdateShadowRect()
{

	shadowRect = {
		this->x,
		this->y + rect.height / 2.0f,
		destRect.width * 1.2f,    // stretched
		rect.height * 0.25f       // squashed flat
	};
	shadowRect.y -= shadowRect.height / 2.0f;
	shadowOrigin = { shadowRect.width / 2,  shadowRect.height / 2 };
}


void BattleEntity::SetCell(const Rectangle& cell)
{
	auto adjustX = cell.x + cell.width / 2.0f;
	auto adjustY = cell.y + cell.height - rect.height / 2.0f;

	SetX(adjustX);
	SetY(adjustY);

	UpdateShadowRect();


}

void BattleEntity::SetStance(string stance)
{
	if (!Options.hasOption(stance)) {
		return;
	}
	this->stance = std::move(stance);
	spriteSheet = &(*texturesMap)[this->stance];
	texture = &spriteSheet->texture;

	framesCount = spriteSheet->frames;
	colorCount = static_cast<float>(spriteSheet->colors);
	frameWidth = static_cast<float>(spriteSheet->texture.width) / framesCount;
	frameHeight = static_cast<float>(spriteSheet->texture.height) / colorCount;

	currentFrame = 0;

	color = (color > colorCount - 1) ? 0 : color;

	sourceRect = Rectangle{ 0, 0 + frameHeight * color,static_cast<float>(frameWidth),static_cast<float> (frameHeight) };
	sourceRect.width = (flipped) ? -sourceRect.width : sourceRect.width;
	destRect = Rectangle{ x , y, static_cast<float>(frameWidth),static_cast<float> (frameHeight) };
	destRect = Rectangle{ destRect.x, destRect.y, destRect.width * scalingFactor, destRect.height * scalingFactor };
	center = Vector2{ static_cast<float>(destRect.width / 2), static_cast<float> (destRect.height / 2) };
	rect = Rectangle{ this->x - center.x,this->y - center.y,destRect.width,destRect.height };

	centerRect = Rectangle{ x, y, 5, 5 };
}

void BattleEntity::SetAttackType(string attackType)
{
	this->attackType = std::move(attackType);
}

void BattleEntity::DrawShadow(int animationSpeed, float dt)
{
	// Origin at top-center so it pivots from feet

	// Display rotated semi-transparent shadow
	DrawTexturePro(
		*texture,
		sourceRect,
		shadowRect,
		shadowOrigin,
		0.0f,
		Color{ 0, 0, 0, 100 }
	);
}

void BattleEntity::DrawPortrait(int animationSpeed, float dt)
{
	if (dialoguePortrait != nullptr)
		DrawTexturePro(*dialoguePortrait, portraitSource, portraitDest, origin, 0.0f, RAYWHITE);

	if (squareFrame)
		DrawTexturePro(*squareFrame,
			squareFrameSourceRect,
			portraitDest,
			origin,
			0.0f,
			RAYWHITE);
}

void BattleEntity::DrawAnimatedPortrait(float dt)
{
	if (this->mediaPortrait && this->mediaPortrait->videoTexture.id != 0)
	{
		DrawTexturePro(
			this->mediaPortrait->videoTexture,
			portraitSource,
			portraitDest,
			{ 0, 0 },
			0.0f,
			WHITE
		);
		
	}

	if (squareFrame)
		DrawTexturePro(*squareFrame, squareFrameSourceRect, portraitDest, origin, 0.0f, RAYWHITE);

	DrawStatsBar();
}

// Draws HP/mana bar
void BattleEntity::DrawStatsBar()
{

	DrawTexturePro(
		*healthIcon,
		healthIconSource,
		healthIconDest_portrait,
		{0.0f, 0.0f},
		0.0f,
		RAYWHITE
	);

	DrawTexturePro(
		*damageIcon,
		damageIconSource,
		damageIconDest_portrait,
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
		RAYWHITE
	);

	if (taunting)
	{
		DrawTexturePro(
			*tauntIcon,
			tauntIconSource,
			tauntIconDest,
			{ 0.0f, 0.0f },
			0.0f,
			RAYWHITE
		);
	}

	if (evade)
	{
		DrawTexturePro(
			*evadeIcon,
			evadeIconSource,
			evadeIconDest,
			{ 0.0f, 0.0f },
			0.0f,
			RAYWHITE
		);
	}

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

	DrawTextOutlined(customFont, to_string(currentAttack).c_str(), damagePos, statsFontSize, charSpacing, damageTextColor, BLACK, outlineSize);

	DrawTextOutlined(customFont, to_string(rank).c_str(), manaCostPos, statsFontSize, charSpacing, RAYWHITE, BLACK, outlineSize);

}

void BattleEntity::Display(int animationSpeed, int offsetX, float dt)
{
	//DrawRectangleLinesEx(rect, 1, RED);
	if (!stopAnimating) {
		animationTimer += dt;
		if (animationTimer >= (1.0f / animationSpeed)) {
			animationTimer = 0.0f;
			currentFrame = (currentFrame + 1) % framesCount;
			//if(attacking)cout << "currentFrame: "<< currentFrame << endl;
			//if (coolOff)cout << currentFrame << endl;
			if (currentFrame == framesCount-1)
			{
				if (attacking ||
					getHit ||
					getHolyHit ||
					getUp ||
					transform ||
					defending ||
					coolOff ||
					retaliating ||
					animateDeath)
				{
					animationFinished = true;

					//return;
				}

			}
			else if ((attacking || retaliating) && currentFrame == momentOfContact)
			{
				enemyHit = true;
				cout << "ENEMY HIT" << endl;

			}

			sourceRect.x = static_cast<float>(currentFrame * frameWidth);

		}
	}


	DrawTexturePro(
		*texture,
		sourceRect,
		destRect,
		center,
		0,
		RAYWHITE
	);


	if (drawDialoguePortrait || mouseOver || selected)
	{
		//cout<<type << "PLAY MEDIA PORTRAIT" << endl;
		if (mediaPortrait == nullptr)
			DrawPortrait(animationSpeed, dt);
		else
			DrawAnimatedPortrait(dt);

	}

	DrawIcons();


	//for (auto& adjCell : adjacentCells)
	//{
	//	//if (adjCell.cell && adjCell.mouseCollide)
	//	if(adjCell.cell && adjCell.cell->drawRect)
	//	DrawRectangleRec(*adjCell.cell->rect, BATTLE_CELL_MOUSE_OVER_CLR);
	//}

}

void BattleEntity::DisplayTargeted(int animationSpeed)
{
	if (!targeted || dead ) return;

	t += GetFrameTime();
	float pulseSpeed = 3.0f;
	targetRedValue = (std::sin(t * pulseSpeed) + 1.f) * 127.5f;

	targetColor.r = static_cast<unsigned char>(targetRedValue);
	DrawTexturePro(
		*texture,
		sourceRect,
		destRect,
		center,
		0,
		targetColor
	);
}

void BattleEntity::DrawIcons()
{

	/*DrawTexturePro(
	*healthIcon,
	healthIconSource,
	healthIconDest,
	{ 0.0f, 0.0f },
	0,
	RAYWHITE
	);

	DrawTexturePro(
		*damageIcon,
		damageIconSource,
		damageIconDest,
		{0.0f, 0.0f},
		0,
		RAYWHITE
	);*/

	if (drawTaunt && !dead) tauntAnimatedIcon->Draw();
	healthPopUp->DrawOnce();
	damagePopUp->DrawOnce();

	if (healthPopUp->GetDrawOnce())
		DrawTextOutlined(customFont, healthModifier.c_str(), healthPopUpTextPos, statsFontSize, charSpacing, RAYWHITE, BLACK, outlineSize);

	if (damagePopUp->GetDrawOnce())
		DrawTextOutlined(customFont, damageModifier.c_str(), damagePopUpTextPos, statsFontSize, charSpacing, RAYWHITE, BLACK, outlineSize);

	//DrawRectangleRec(centerRect, GREEN);

	if (skipTurn && !dead)
		skipTurnAnimatedIcon->Draw();
}

void BattleEntity::MouseOver(bool flag)
{
	if (!Options.hasOption("mouse_over")) return;

	if (move || offence || attacking || retaliate || defending || getHit ||dead|| ranOutOffHealth)
		return;

	//static Texture2D* currentCursor = nullptr; // persistent between calls

	if (!mouseOver && flag) // just entered
	{
		mouseOver = true;
		SetStance("mouse_over");
		if (selected > -1)
		{
			/*for (auto& adjCell : adjacentCells)
			{
				if (adjCell.cell && adjCell.cell->entity)
				{
					bool isEnemy = (adjCell.cell->entity->GetRelationshio() != relationship);
					adjCell.cell->drawRect = isEnemy;
					
				}
			}*/
			AreEnemiesAdjacent();
		}
	}
	else if (mouseOver && !flag) // just exited
	{
	/*	for (auto& adjCell : adjacentCells)
		{
			if (adjCell.cell)
			{
				adjCell.cell->drawRect = false;

			}
		}*/
		mouseOver = false;
		SetStance("idle");
	}
}

void BattleEntity::AreEnemiesAdjacent()
{
	nextToEnemy = false;
	for (auto& adjCell : adjacentCells)
	{
		if (adjCell.cell && adjCell.cell->entity)
		{
			bool isEnemy = (adjCell.cell->entity->GetRelationship() != relationship);
			if (isEnemy)
			{
				nextToEnemy = true;
				break;
			}
		}
	}
	
}

void BattleEntity::Lured(int moveToIndex, vector<int> path)
{
	cout << "LURED" << endl;
	lured = true;
	movementPath.clear();
	movementPath = std::move(path);
	if (movementPath.size() > 2) {
		movementPath.pop_back();
	}

	auto restOfPath = TrailPath(movementPath.back(), moveToIndex);
	movementPath.insert(movementPath.end(),
		std::make_move_iterator(restOfPath.begin()),
		std::make_move_iterator(restOfPath.end()));
	restOfPath.clear();


	if (movementPath.empty()) {
		move = false;
		SetStance("idle");
		//enemy->EndTurn();
		cout << "Movement path is empty." << endl;

		cout << Type() << "Defender-finished animation sequence\n";
		EndTurn();

		return;
	}

	tempIndex = 0; // start moving to the *next* cell (0 is current position)
	rectMoveToIndex = moveToIndex;
	move = true;

	auto& tempRect = (*grid)[movementPath[tempIndex]].rect;

	moveToCenter = {
		tempRect->x + tempRect->width / 2.0f,
		tempRect->y + tempRect->height - rect.height / 2.0f
	};
	auto& endRect = (*grid)[moveToIndex].rect;

	/*UpdateAdjacentCells();*/
	SetStance("walk");

	if ((direction == Utilities::Direction::west && x > endRect->x) ||
		(direction == Utilities::Direction::east && x < endRect->x))
	{
		Flip();
		/*UpdateAdjacentCells();*/
	}
}

//void BattleEntity::ActivateRetaliate(shared_ptr<Cell> enemyCell)
//{
//	//cout <<"ACTIVATE RETALIATE: "<< endl;
//	//enemyIndex = attackerIndex;
//	//auto& enemy = (*grid)[enemyIndex].entity;
//	defender = true;
//	auto enemyDirection = enemyCell->entity->Direction();
//	if (retaliated)
//	{
//		ActivateGetHit();
//		return;
//	}
//
//	retaliate = true;
//	defending = true;
//	SetStance("hit");
//	if (enemyDirection == direction)
//	{
//		Flip();
//		//UpdateAdjacentCells();
//	}
//	
//}

void BattleEntity::ActivateRetaliate(int dmg)
{
	//cout <<"ACTIVATE RETALIATE: "<< endl;
	//enemyIndex = attackerIndex;
	//auto& enemy = (*grid)[enemyIndex].entity;
	
	currentHealth -= dmg;

	if (currentHealth <= maxHealth * 0.75)
		HP_bar = health75;
	if (currentHealth <= maxHealth * 0.50)
		HP_bar = health50;
	if (currentHealth <= maxHealth * 0.25)
		HP_bar = health25;
	if (currentHealth <= maxHealth * 0.10)
		HP_bar = health10;
	if (currentHealth <= 0)
	{
		HP_bar = nullptr;
		ranOutOffHealth = true;
		animateDeath = true;
		SetStance("death");
		return;
	}

	defender = true;
	auto enemyDirection = enemyCell->entity->Direction();
	if (retaliated)
	{
		ActivateGetHit();
		return;
	}

	retaliate = true;
	defending = true;
	SetStance("hit");
	if (enemyDirection == direction)
	{
		Flip();
		//UpdateAdjacentCells();
	}

}
//void BattleEntity::Retaliate()
//{
//	timer += GetFrameTime();
//
//	auto& enemy = (*grid)[enemyIndex].entity;
//	if (defending && timer >= hitDuration)
//	{
//		timer = 0.0f;
//		cout << "Cool Off" << endl;
//		coolOff = true;
//		SetStance("idle");
//
//		defending = false;
//
//	}
//	else if (coolOff && timer >= coolOffDuration)
//	{
//		cout << "Retaliate" << endl;
//		timer = 0.0f;
//		SetStance(attackType);
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
//		timer = 0.0f;
//		cout << "Idle" << endl;
//		SetStance("idle");
//		retaliate = false;
//		enemyHit = false;
//
//		retaliated = true;
//
//
//
//	}
//}

void BattleEntity::Retaliate()
{
	//auto& enemy = (*grid)[enemyIndex].entity;
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
		animationFinished = false;
		SetStance(attackType);
		retaliating = true;
		coolOff = false;

	}
	else if (enemyHit)
	{	
		
		enemyHit = false;
		enemy->ActivateGetHit();
		enemy->ActivateGetHit();

	}
	else if (retaliating && animationFinished)
	{
		cout << "Idle" << endl;
		SetStance("idle");
		retaliating = false;
		animationFinished = false;

		retaliate = false;
		drawDialoguePortrait = false;
		retaliated = true;
		defender = false;

		
		defender = false;
		cout << Type() << " Defender-finished animation sequence\n";
		EndTurn();
	}
	
}

void BattleEntity::StartShake(float duration, float strength)
{
	shakeEntity = true;
	shakeTime = duration;
	shakeDuration = duration;
	shakeStrength = strength;
}

void BattleEntity::ShakeEntity()
{
	if (!shakeEntity) return;

	shakeTime -= GetFrameTime();
	if (shakeTime <= 0.0f)
	{
		SetCell(*(*grid)[cellIndex].rect);
		
		shakeEntity = false;
		return;
	}

	float t = shakeTime / shakeDuration;      // 1 -> 0
	float mag = shakeStrength * t * t;        // strong at impact, dies out

	float ox = (GetRandomValue(-100, 100) / 100.0f) * mag;
	float oy = (GetRandomValue(-100, 100) / 100.0f) * mag * 0.5f; // less vertical


	SetX(x + ox); 
	SetY(y + oy);
	
}

void BattleEntity::ActivateGetHit(bool dontRetaliate, int dmg)
{
	cout << "ACTIVATE GET HIT" << endl;
	//if (attackerIndex.has_value())
	//{
	//	enemyIndex = *attackerIndex;
	//	cout <<"EnemyIndex: "<< enemyIndex << endl;
	//	enemy = (*grid)[enemyIndex].entity;
	//	defender = true;
	//}
	
	StartShake();
	SubStat(Stat::Health, dmg);
	if (currentHealth <= 0)
	{
		HP_bar = nullptr;
		ranOutOffHealth = true;
		animateDeath = true;
		SetStance("death");
		return;
	}
	getHit = true;
	SetStance("hit");
	this->momentOfContact = momentOfContact;
	this->dontRetaliate = dontRetaliate;
}

void BattleEntity::ActivateHolyHit(int dmg)
{
	SubStat(Stat::Health, dmg);
	if (currentHealth <= 0)
	{
		HP_bar = nullptr;
		ranOutOffHealth = true;
		animateDeath = true;
		SetStance("death");
		return;
	}

	SetStance("holy_hit");
	getHolyHit = true;
	
	this->momentOfContact = momentOfContact;

}

//void BattleEntity::GetHit()
//{
//	this->timer += GetFrameTime();
//	if (getHit && this->timer >= 0.5f)
//	{
//		SetStance("idle");
//		getHit = false;
//		this->timer = 0.0f;
//
//	}
//}

void BattleEntity::GetHit()
{
	//this->timer += GetFrameTime();
	//if (enemy == nullptr) cout << "NULL POINTER" << endl;
	//auto enemyType = Utilities::EntityTypeFromString(enemy->Type());
	//auto it = std::find(Utilities::rangedUnits.begin(), Utilities::rangedUnits.end(), enemyType);
	//bool enemyRanged = (it != Utilities::rangedUnits.end());

	if (!coolOff && animationFinished)
	{
		SetStance("idle");
		coolOff = true;
		animationFinished = false;
		//finishedAnimationSequence = (attacker && finishedAttack);
		//finishedAnimationSequence = (defender && retaliated);

		

	}
	else if (coolOff && animationFinished)
	{
		getHit = false;
		animationFinished = false;
		drawDialoguePortrait = false;
		coolOff = false;
		if (retaliated && defender)
		{
			EndTurn();
			defender = false;
			cout << Type() << " Defender-finished animation sequence\n";
		}
		else if (dontRetaliate)
		{
			EndTurn();
			defender = false;
			cout << Type() << " Defender-finished animation sequence\n";
		}
		else if (attacker) {
			EndTurn();
			attacker = false;
			cout << Type() << " Attacker-finished animation sequence\n";
		}
	}
}

void BattleEntity::GetHolyHit()
{
	if (!getUp && !transform && !coolOff && animationFinished)
	{
		cout << "Get Up" << endl;
		SetStance("get_up");
		//getHolyHit = false;
		animationFinished = false;
		getUp = true;
		

	}
	else if (getUp && animationFinished)
	{
		cout << "Got Up. Set Cleansed Idle." << endl;
		SetStance("cleansed_idle");
		coolOff = true;
		getUp = false;
		animationFinished = false;
	}
	else if (coolOff && animationFinished)
	{
		cout << "Cooled Off. Set Transform." << endl;
		SetStance("transform");
		coolOff = false;
		transform = true;
		animationFinished = false;
	}
	else if (transform && animationFinished)
	{
		cout << "Transformed. Set Idle." << endl;
		SetStance("idle");
		transform = false;
		getHolyHit = false;
		animationFinished = false;
		cout << Type() << " Defender-finished animation sequence\n";
		EndTurn();
	}
}

void BattleEntity::Death()
{
	taunting = false;
	dead = true;
	targeted = false;
	attacking = false;
	offence = false;
	if (ranOutOffHealth && animationFinished)
	{
		animateDeath = false;
		cout << type << " DEATH ANIMATION FINISHED" << endl;
		animationFinished = false;
		stopAnimating = true;
		currentFrame = framesCount - 1;
		sourceRect.x = static_cast<float>(currentFrame * frameWidth);
		EndTurn();
	}
}

int BattleEntity::GetMoveToIndex()
{
	return rectMoveToIndex;
}

Rectangle BattleEntity::GetShadowRect()
{
	return shadowRect;
}

const AdjacentCell BattleEntity::GetAttackDirection()
{
	/*AdjacentCell adjacentCell = frontCell;

	if (attackedFromTop) adjacentCell = topCell;
	else if (attackedFromBottom) adjacentCell = bottomCell;
	else if (attackedFromBack) adjacentCell = backCell;*/
	vector<AdjacentCell>availableCells = adjacentCells;
	AdjacentCell adjacentCell = adjacentCells[2]; // attack from east
	for (auto& adjCell : availableCells)
	{
		if (adjCell.mouseCollide)
		{
			adjacentCell = adjCell;
		}
	}

	return adjacentCell;
}

const AdjacentCell* BattleEntity::GetAvailableAdjacentCell()
{	

	//// PASS 1 — Preferred: has mouse collision + empty
	//for (auto& adjCell : adjacentCells)
	//{
	//	if (adjCell.cell &&
	//		adjCell.mouseCollide)
	//	{
	//		if(enemy && enemy->GetCellIndex() == adjCell.cell->gridIndex)
	//		return &adjCell;
	//	}
	//}

	// PASS 1 — Preferred: has mouse collision + empty
	for (auto& adjCell : adjacentCells)
	{
		if (adjCell.cell &&
			adjCell.mouseCollide &&
			(adjCell.cell->entity == nullptr))
		{
			return &adjCell;
		}
	}

	// PASS 2 — Fallback: just an empty cell
	for (auto& adjCell : adjacentCells)
	{
		if (adjCell.cell &&
			adjCell.cell->entity == nullptr)
		{
			return &adjCell;
		}
	}

	// PASS 3 — No available adjacent cell
	return nullptr;
	
	
}

const bool BattleEntity::GetMouseOver()
{
	return mouseOver;
}


vector<int> BattleEntity::GetAdjacentCells(int cellIndex)
{
	vector<int> adjacentCells;

	int rows = numberOfRows;      // 6
	int cols = numberOfColumns;   // 11
	// total(grid->size()) = rows * cols = 66

	int r = cellIndex / cols;
	int c = cellIndex % cols;

	// Directions: N, S, E, W, NE, NW, SE, SW
	int dRow[] = { -1, 1, 0, 0, -1, -1, 1, 1 };
	int dCol[] = { 0, 0, 1, -1, 1, -1, 1, -1 };

	for (int i = 0; i < 8; i++) {
		int nr = r + dRow[i];
		int nc = c + dCol[i];

		if (nr >= 0 && nr < rows &&
			nc >= 0 && nc < cols)
		{
			int newIndex = nr * cols + nc;
			adjacentCells.push_back(newIndex);
		}
	}

	//cout << "Adjacent Cells for: " << cellIndex << endl;
	//for (auto& i : adjacentCells)
	//	cout << i << " ";
	//cout << endl;

	return adjacentCells;
}

const int BattleEntity::GetCellIndex()
{
	return cellIndex;
}

const Relationship  BattleEntity::GetRelationship()
{
	return relationship;
}

const bool BattleEntity::GetHadItsTurn()
{
	return hadItsTurn;
}

const int BattleEntity::GetRank()
{
	return rank;
}

const int BattleEntity::GetHealth()
{
	return currentHealth;
}

const int BattleEntity::GetAttack()
{
	return currentAttack;
}

MediaStream* BattleEntity::GetMediaPortrait()
{
	return mediaPortrait.get();
}

Utilities::EntitySize BattleEntity::GetSize()
{
	return entitySize;
}

bool BattleEntity::GetSkipTurn()
{
	return skipTurn;
}

string BattleEntity::GetType()
{
	return type;
}

bool BattleEntity::GetTaunting()
{
	return taunting;
}
bool BattleEntity::HasDied()
{
	return dead;
}


//void BattleEntity::SetOpposingArmy(vector<shared_ptr<BattleEntity>> opposingArmy)
//{
//	this->opposingArmy = opposingArmy;
//}

void BattleEntity::SetEnemy(shared_ptr<Cell> enemyCell)
{
	if (enemyCell != nullptr)
	{
		this->enemyCell = enemyCell;
		this->enemy = this->enemyCell->entity;
	}
	else
		cout << "SET ENEMY::NULL POINTER" << endl;
}

void BattleEntity::SetPosition(int newPos)
{
	this->position = newPos;
}

void BattleEntity::SetTargeted(bool targeted)
{
	this->targeted = targeted;
}

void BattleEntity::SetStatsTextColor(const int& startingStats, const int& currentStats, Color& statsColor)
{
	if (currentStats > startingStats)
		statsColor = GREEN;
	else if (startingStats > currentStats)
		statsColor = RED;
	else
		statsColor = RAYWHITE;
}

void BattleEntity::AddStat(Stat stat, int amount)
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
			break;
	}
}

void BattleEntity::SubStat(Stat stat, int amount)
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
			
			break;
	}
}

void BattleEntity::AlterState(Stat stat, bool stateModifier)
{
	targeted = false;
	switch (stat)
	{
	case Stat::Taunt:
		taunting = stateModifier;
		drawTaunt = taunting;
		break;

	case Stat::SkipTurn:
		skipTurn = stateModifier;

		break;

	case Stat::FinishedTurn:
		hadItsTurn = stateModifier;
		break;

	}
}

void BattleEntity::EnemyRetaliate(int amount)
{
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


}

void BattleEntity::SetHadItsTurn(bool hadItsTurn)
{
	this->hadItsTurn = hadItsTurn;
}

void BattleEntity::ResetEnemy()
{
	enemyCell.reset();
	enemy.reset();
}

void BattleEntity::EndTurn()
{
	rectMoveToIndex = -1;
	enemy.reset();
	enemyCell.reset();
	finishedAnimationSequence = true;
	hadItsTurn = true;
	drawDialoguePortrait = false;
	finishedAttack = false;
	SetPosition(cellIndex);
	reachableCells = ReachableCells(cellIndex, movementPoints);
	coolOffDuration = 0.5f;
	Deselect();
}