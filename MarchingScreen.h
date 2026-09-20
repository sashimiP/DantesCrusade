#pragma once
#include "Town.h"
#include "MarchingEntity.h"
#include "GateKeeper.h"

class MarchingScreen:
	public Town
{
public:
	MarchingScreen() = default;
	MarchingScreen(string townName, shared_ptr<TextureManager> textureManager, shared_ptr<UI> Interface, shared_ptr<Battleground> battleground);
	~MarchingScreen() override;
	void AddUnit(const string&, int) override;
	
	void SetBackground(const string&) override;
	void Update(float) override;
	void DragUnits() override;	
	void MoveRight(float) override;
	void MoveLeft(float) override;
	void DrawBackground(float) override;
	void DrawUnits(float) override;
	void DrawArmy(float);
	//template<typename T>
	//void ScaleUnits(vector<std::unique_ptr<T>>&, float);
	//void ScaleUnits(vector<MarchingEntity>&, float);
	void ScaleUnits(const std::vector<std::unique_ptr<Entity>>& units, float scalingFactor);
	void ScaleUnits(std::vector<MarchingEntity>& units, float scalingFactor);
	void RearrangeArmy(int leadingX, int leadingY, int first=0);
	void SetStance(string);
	void PrepArmyForMarching();
	void PrepEntitiesForMarching();
	void SpacePressed();
	void ArmySetStance(const string& stance, float scalingFactor = 1.0f) override;
	void ArmyMoveRight() override;
	void ArmyMoveLeft() override;
	//void PlaySoundtrack();
	bool moveRight{ true };
	//void StopSoundtrack();
	bool startMarching{ false };

	float GetScrollSpeed() const override;
	float GetScalingFactor() const override;
	std::string GetDanteStance() const override;
	void ApplyArmyStance();
	void Idle();

	void CollideRectUnitDante() override;
	bool CollideRectGateKeeperDante();
	void Collisions(vector<bool*>menuFlags) override;
	Rectangle EnterTownButtonRect();

	std::optional<std::string> HandleButtonClick(Vector2 mouse, vector<string>validTownNames) override;
	void LeftMouseButtonReleased(Vector2 mouse, vector<string>validTownNames) override;


	void AssetOptionsUnits() override;
	std::optional<std::string> ClickAssetOption() override;
	void SetNextTown(string townName) override;
	void SetMarchingArmy(const shared_ptr<vector<MarchingEntity>>& marchingArmy) override;

private:
	//std::unique_ptr<Background> movingBackground;
	void LoadGateKeeper();
	//shared_ptr<vector<MarchingEntity>> marchingArmy;
	fs::path armyDataPath;
	fs::path gateKeeperPath;

	std::unique_ptr<GateKeeper> gateKeeper;
	
	unordered_map<string, float> a;
	unordered_map<string, float> b;
	//Music soundtrack;
};

