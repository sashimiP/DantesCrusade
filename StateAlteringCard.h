#pragma once
#include "SpellCard.h"

Utilities::CardType;

class StateAlteringCard :
    public SpellCard
{
public:
    StateAlteringCard() = default;
    StateAlteringCard(shared_ptr<TextureManager> textureManager);

    void SetCard(string type, float x, float y) override;
    void Drawn(string type, float x, float y) override;

    ~StateAlteringCard() = default;

    void PlayCard(BattleEntity& target) override;

private:
    Stat state;
    bool stateModifier;
};

