#pragma once
#include "Card.h"
#include "AnimatedAsset.h"
#include "BattleEntity.h"

using Utilities::spellStats;

class SpellCard :
    public Card
{
public:
    SpellCard() = default;
    SpellCard(shared_ptr<TextureManager> textureManager);

    ~SpellCard() = default;

    virtual void SetCard(string type, float x, float y) override;
    void SetCard(float x, float y) override;
    virtual void Drawn(string type, float x, float y) override;

    virtual void PlayCard(BattleEntity& target) = 0;

    void UpdateIcons(float scale = 1.0f) override;

    void Display() override;

    void SetSpellPosition(float x, float y);

    shared_ptr<AnimatedAsset> GetSpell();

protected:

    shared_ptr<AnimatedAsset> spellAnimation;

    int statsModifier = 0;
    vector<Stat> statsToModify;

    bool castSpell{ false };
};

