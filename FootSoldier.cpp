#include "FootSoldier.h"

FootSoldier::FootSoldier(Utilities::Relationship relationship,
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
		color
	)
{
    SetCell(*(grid->at(cellIndex).rect));
	hitDuration = 0.5f;
}

FootSoldier::~FootSoldier()
{
	save = false;
}

//void FootSoldier::Move(int rectIndex)
//{
//	move = true;
//	rectMoveToIndex = rectIndex;
//	auto& tempRect = (*grid)[rectMoveToIndex].rect;
//  
//	if (startingDirection == east && x > tempRect->x ||
//		startingDirection == west && x < tempRect->x) Flip();
//
//	SetStance("walk");
//
//}

//void FootSoldier::MoveToCell()
//{
//	auto& tempRect = (*grid)[rectMoveToIndex].rect;
//	Vector2 moveToCenter{
//		tempRect->x + tempRect->width / 2.0f,
//		tempRect->y + tempRect->height - rect.height/2.0f
//	};
//
//	// If we're NOT inside the destination cell yet keep moving
//	if (!CheckCollisionRecs(centerRect, (*tempRect)))
//	{
//		float dx = moveToCenter.x - x;
//		float dy = moveToCenter.y - y;
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
//		}
//	}
//	else
//	{
//		// Snap to center of cell and stop
//		/*SetX(moveToCenter.x);
//		SetY(moveToCenter.y);*/
//		SetCell((*tempRect));
//
//		if (direction != startingDirection) Flip();
//
//	
//		SetStance("idle");
//		move = false;
//		
//
//	}
//}

//void FootSoldier::AttackEnemy()
//{
//
//	auto& enemyRect = (*grid)[enemyIndex].rect;
//	auto& enemy = (*grid)[enemyIndex].entity;
//	//auto attackDirection = (*grid)[enemyIndex].entity->GetAttackDirection();
//	
//
//	// If we're NOT inside the destination cell yet keep moving
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
//			shadowRect.x += dx * speed;
//			shadowRect.y += dy * speed;
//		}
//	}
//	else
//	{
//
//
//		if (offence && !attacking)
//		{
//			SetStance("attack");
//			
//			if (attackDirection.side == Side::west && direction != enemy->Direction())
//			{
//				cout << "Flip to face enemy's back" << endl;
//				Flip();
//				UpdateAdjacentCells();
//			}
//			else if (attackDirection.side == Side::east && direction == enemy->Direction())
//			{
//				cout << "Flip to face enemy's front" << endl;
//				Flip();
//				UpdateAdjacentCells();
//			}
//			attacking = true;
//		}
//
//		timer += GetFrameTime();
//		if (!enemyHit && timer >= momentOfContact)
//		{
//			enemy->ActivateRetaliate(rectMoveToIndex);
//			enemyHit = true;
//		}
//		if (timer >= attackDuration)
//		{
//			SetStance("idle");			
//			// Snap to center of cell and stop
//			SetX(moveToCenter.x);
//			SetY(moveToCenter.y);
//			UpdateShadowRect();
//			timer = 0.0f;
//			attacking = false;
//			enemyHit = false;
//			offence = false;
//
//		}
//		
//	}
//}
//
//int FootSoldier::Attack(int enemyIndex)
//{
//	auto& enemyRect = (*grid)[enemyIndex].rect;
//	enemy = (*grid)[enemyIndex].entity;
//    
//    attackDirection = enemy->GetAttackDirection();
//
//	if (attackDirection.side == Side::west)
//	{
//		cout << "Attack enemy's back" << endl;
//	}
//	else if (attackDirection.side == Side::east)
//	{
//		cout << "Attack enemy's front" << endl;
//	}
//
//	this->enemyIndex = enemyIndex;
//	rectMoveToIndex = attackDirection.cell->gridIndex;
//	
//	enemyCenter = {
//		enemyRect->x + enemyRect->width / 2.0f,
//		enemyRect->y + enemyRect->height - rect.height / 2.0f
//	};
//
//	offence = true;
//	if(cellIndex != rectMoveToIndex) Move(rectMoveToIndex);
//
//	
//	return rectMoveToIndex;
//}

//void FootSoldier::Display(int animationSpeed, int offsetX, float dt)
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

//void FootSoldier::SetCell(const Rectangle& cell)
//{
//	auto adjustX = cell.x + cell.width / 2.0f;
//	auto adjustY = cell.y + cell.height - rect.height/2.0f;
//
//	SetX(adjustX);
//	SetY(adjustY);
//
//	//SetX(cell.x + cell.width / 2.0f);
//	//SetY(cell.y + cell.height / 2.0f);
//}

//void FootSoldier::MouseOver(bool flag)
//{
//	if(std::find(options.begin(), options.end(), "mouse_over") == options.end()) return;
//
//	if (move || offence|| attacking)
//		return;
//
//	if (!mouseOver && flag)
//	{
//		mouseOver = flag;
//		SetStance("mouse_over");
//	}
//	else if(mouseOver && !flag)
//	{
//		mouseOver = flag;
//		SetStance("idle");
//	}
//}

//void FootSoldier::Lured(int moveToIndex)
//{
//	cout << "LURED" << endl;
//	lured = true;
//	Move(moveToIndex);
//}