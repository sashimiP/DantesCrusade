#pragma once
#include "BattleEntity.h"
#include "AbilityIcon.h"

using Utilities::heroAbilities;

class BattleHero :
    public BattleEntity
{
public:
	BattleHero() = default;
	BattleHero(Utilities::Relationship relationship,
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
		vector<string> dialogue = {},
		string name = "Unknown",
		int color = 0
	);

	virtual ~BattleHero();

	void Update(bool selected) override;
	void Attack(shared_ptr<Cell> enemyCell) override;

	void DrawStatsBar() override;
	void Deselect() override;
	void SpecialAbilityDeselect();

	void DisplayAbilityEffect();

	//Button* GetButton();

	shared_ptr<AbilityIcon> GetSpecialAbility();
	bool GetSeekTarget();
	bool GetAbilityUsed();
	int GetAbilityManaCost();
	int GetAbilityModifier();

	void SetManaPointers(int& availableMana, int& currentMana, int& usedMana);

	void LeftMouseButtonPressed();

	void ResetAbilityUsed();
	void AbilitySetTarget(BattleEntity* entity, Rectangle targetRect, std::function<void()> callback = nullptr);
	void DeselectAbility();

	void Death() override;

	function <void()> DeathCallback;

private:
	string abilityType;
	int abilityFrames;

	int abilityManaCost = 0;
	int* availableMana = nullptr;
	int* currentMana = nullptr;
	int* usedMana = nullptr;
	//Rectangle abilityDest;
	shared_ptr<AbilityIcon> specialAbility;

	string visualEffectType;
	int visualEffectFrames;
};

