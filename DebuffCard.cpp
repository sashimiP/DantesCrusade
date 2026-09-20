#include "DebuffCard.h"

DebuffCard::DebuffCard(shared_ptr<TextureManager> textureManager) :
	SpellCard(textureManager)
{
	cardType = Utilities::CardType::Debuff;
	spellAnimation = make_shared<AnimatedAsset>();
}

void DebuffCard::PlayCard(BattleEntity& target)
{
	for (Stat s : statsToModify)
	{
		if (s == Stat::Health)
			target.ActivateGetHit(false, statsModifier);
		else
			target.SubStat(s, statsModifier);
	}
}