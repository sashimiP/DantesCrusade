#pragma once
#include "Entity.h"
class SpecialEntity :
    public Entity
{
public:
    SpecialEntity() = default;
    SpecialEntity(
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
        int color = 0,
        bool dontScale = false,
        const string& special = "None",
        float growStats = 1.0f,
        int price = 1
    );
    ~SpecialEntity() override;

    // Add move constructor and move assignment operator to allow storing in std::map
    SpecialEntity(SpecialEntity&&) noexcept = default;
    SpecialEntity& operator=(SpecialEntity&&) noexcept = default;

    void Update(float, float) override;
    void DrawButton();
    bool& HasCollidedWithDante() override;
    Rectangle YesButtonRect();
    Rectangle BackButtonRect();
    string YesButton(float& statsToManipulate, float& gold);
    string BackButton();
    void SetButton(const string& special);
    void SaveCoordinates() override;
    string ClickAssetOption(float) override;
    void DrawDialogue(float) override;
    void AddSpecial();
    void AddText(string& textVariable) override;

    int GetPrice();
    
    void SetGrowStats(float newGrowStats);
    void SetPrice();
   
    
    
private:
    std::unique_ptr<Button> yesButton;
    std::unique_ptr<Button> specialBackButton;
    string special;
    bool addSpecial{ false };
    bool setPrice{ false };
    float growStats{1.0f};
    int price{ 1 };
};

