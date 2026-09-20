#pragma once
#include <functional>

#include "Entity.h"
#include "BattleEntity.h"

class Projectile :
    public Entity
{

public:
    Projectile() = default;
    Projectile(int rangedEnityCellIndex,
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
		vector<string> dialogue = {},
		string name = "Unknown",
		int color = 0
		);

	virtual ~Projectile();

	void Update();
    const float SetTarget(const Rectangle& rect, shared_ptr<BattleEntity> target);
	void Loose();
    virtual void FlyToTarget();
	virtual void DrawShadow(int animationSpeed, float dt);
    void Display(int, int, float) override;
	void SetX(float) override;
	void SetY(float) override;
	void SetCellIndex(int newIndex);
	void SetStartingPosition(float x, float y);
	void SetAttack(int attack);
	void SetStance(string stance);
	bool resetRotation{ false };
	std::function<void()>onHit;

protected:
	int rangedEnityCellIndex;
	float rangedEntityWidth;
	float rangedEntityHeight;
	Vector2 enemyCenter = {0.0f, 0.0f};
	Vector2 startingPosition = { 0.0f, 0.0f };
	Rectangle enemyRect = { 0.0f, 0.0f };
	float rotation = 0.0f; // rotation of the line from Ranged Unit to Enemy center
	float speed = static_cast<float>(PROJECTILE_SPEED);
	bool loose{ false };
	bool hit{ false };

	float scalingFactor;

	float timer = 0.0f;
	float hitDuration = 0.5f;

	bool animationFinished = false;

    shared_ptr<BattleEntity> target;

	float shadowTimer{ 0.0f };
	int shadowFrame = 0;
	Rectangle shadowRect;
	Vector2 shadowOrigin;

	int attack = 0;
};

