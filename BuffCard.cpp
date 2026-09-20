#include "BuffCard.h"

BuffCard::BuffCard(shared_ptr<TextureManager> textureManager) :
	SpellCard(textureManager)
{
	cardType = Utilities::CardType::Buff;
	spellAnimation = make_shared<AnimatedAsset>();
}

void BuffCard::PlayCard(BattleEntity& target)
{
	for (Stat s : statsToModify)
		target.AddStat(s, statsModifier);
}