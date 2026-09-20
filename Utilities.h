// Utilities.h
#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include<fstream>
#include <filesystem>
#include <algorithm>
#include <random>
#include <cctype>
#include <map>
#include<unordered_map>
#include <ranges>


#include "raylib.h"

namespace fs = std::filesystem;

using std::string, std::vector, std::cout, std::endl, std::map, std::unordered_map;

constexpr int SCR_WIDTH = 1280;
constexpr int SCR_HEIGHT = 720;
constexpr int BASIC_PORTRAIT_WIDTH = 64;
constexpr int BASIC_PORTRAIT_HEIGHT = 64;

constexpr int DIALOGUE_PORTRAIT_WIDTH = 512;
constexpr int DIALOGUE_PORTRAIT_HEIGHT = 720;

constexpr int ANIMATION_SPEED = 10;
constexpr int BATTLE_SPEED = 5;
constexpr int FONT_SIZE = 20;
constexpr int EDITING_SPEED = 80;
constexpr int GLOBAL_SPEED = 20;
constexpr int MARCHING_SPEED = 10;
constexpr int PROJECTILE_SPEED = 40;
constexpr float ARMY_SCALING_FACTOR = 0.5;
//constexpr float SCALING_FACTOR = 0.40;
//constexpr float RESCALING_FACTOR = 2.5;

constexpr float SCALING_FACTOR = 0.50;
constexpr float RESCALING_FACTOR = 2.f;

constexpr int BATTLE_CELL_WIDTH = 64;
constexpr int BATTLE_CELL_HEIGHT = 64;

constexpr Color INTERFACE_BG_COLOR = Color(0, 0, 0, 200);
constexpr Color BATTLE_CELL_CLR = Color(0, 0, 0, 90);
constexpr Color BATTLE_CELL_MOUSE_OVER_CLR = Color(253, 249, 0, 55);


namespace Utilities {

    struct UI_Asset
    {
        Texture2D* texture;
        Rectangle source;
        Rectangle dest;
    };

    enum Direction
    {
        east = 0,
        west = 1
    };

    enum class Stance {
        Idle,
        MarchIdle,
        March,
        Walk
    };

    struct EntityData {
        float id{};
        int position{};
        std::string type;
        float x{};
        float y{};
        Utilities::Direction direction{};
        bool drag{ false };
        std::string stance;
        vector<string>dialogue{};
        std::string name{ "Unknown" };
        int color{ 0 };
        bool dontScale{ false };
        Vector2 patrolRoute{ 0,0 };
        string special{ "" };
        float growStats{ 1.0f };
        int price{ 1 };
    };

    // Fix: Use inline to initialize the map in a header file (C++17 and later)
    inline map<string, int> unitForHirePrice = {
        { "knight", 5 },
        { "heavy_knight", 7 },
        { "mounted_knight", 10 },
        { "archer", 5 }
    };

    enum class EntityType {
        Knight,
        HeavyKnight,
        MountedKnight,
        Archer,
        Gorgon,
        SuccubusWhip,
        SuccubusBlue,
        SuccubusGolden,
        SuccubusRed,
        SuccubusWizard,
        VampireLady,
        SpiderQueen,
        BluePixie,
        Angel,
        CentaurBow,
        ElfBow,
        ElfSword,
        ElfSpear,
        Witch,
        DarkSword,
        Griffin,
        Imp,
        BlueWizard,
        GreenGoblin,
        FieldMaid,
        FencingMaiden,
        BattleBride,
        BlueGolem,
        DanteHero,
        EnemyHero,
        Militia,
        Skeleton,
        Spectre,
        ShadowPriest,
        Unknown
    };

    enum class EntitySize {
        Small,
        Medium,
        Big
    };

    EntityType EntityTypeFromString(const std::string& type);

    inline vector<EntityType> rangedUnits = {
        EntityType::Archer,
        EntityType::Gorgon,
        EntityType::ElfBow,
        EntityType::CentaurBow,
        EntityType::BluePixie,
        EntityType::SuccubusWizard,
        EntityType::Angel,
        EntityType::Witch,
        EntityType::BlueWizard,
        EntityType::ShadowPriest,
    };

    // Fix: Use fully qualified enum value for Archer
    inline unordered_map<EntityType, string> projectileTypes = {
        {EntityType::Archer, "arrow"},
        {EntityType::Gorgon, "arrow"},
        {EntityType::ElfBow, "arrow"},
        {EntityType::CentaurBow, "arrow"},
        {EntityType::Witch, "magic_arrow"},
        {EntityType::BluePixie, "blue_pixie_arrow"},
        {EntityType::SuccubusWizard, "burning_skull"},
        {EntityType::Angel, "holy_fire"},
        {EntityType::BlueWizard, "blue_spell"},
        {EntityType::ShadowPriest, "light_shot"}
    };

    struct EntityTypeHash {
        std::size_t operator()(EntityType type) const noexcept {
            return static_cast<std::size_t>(type);
        }
    };

    struct EntityStats {
        std::string name;
        std::string description;
        int rank = 0;
        float scale = 0.6f;
        int health = 0;
        int damage = 0;
        EntitySize size = EntitySize::Small;
        int momentOfContact = 0;
        bool taunting = false;
        bool evade = false;
    };


    inline std::unordered_map<std::string, EntityStats> entityStats = {
    {"angel", {
        .name = "Holy Saviour",
        .description = "Taunt. Evade.",
        .rank = 8, .scale = 0.65f, .health = 10, .damage = 8,
        .size = EntitySize::Medium, .momentOfContact = 10,
        .taunting = true, .evade = true
    }},
    {"archer", {
        .name = "Longbowmen",
        .description = "Evade.",
        .rank = 2, .scale = 0.6f, .health = 2, .damage = 3,
        .size = EntitySize::Small, .momentOfContact = 4,
        .evade = true
    }},
    {"battle_bride", {
        .name = "Battle Bride",
        .rank = 3, .scale = 0.6f, .health = 3, .damage = 5,
        .size = EntitySize::Small, .momentOfContact = 9
    }},
    {"blue_golem", {
        .name = "Blue Golem",
        .description = "Taunt.",
        .rank = 5, .scale = 0.9f, .health = 6, .damage = 4,
        .size = EntitySize::Big, .momentOfContact = 9,
        .taunting = true
    }},
    {"blue_pixie", {
        .name = "Forest Pixie",
        .description = "Evade.",
        .rank = 5, .scale = 0.45f, .health = 4, .damage = 5,
        .size = EntitySize::Small, .momentOfContact = 7,
        .evade = true
    }},
    {"blue_wizard", {
        .name = "Blue Wizard",
        .description = "Evade.",
        .rank = 3, .scale = 0.65f, .health = 3, .damage = 4,
        .size = EntitySize::Small, .momentOfContact = 4,
        .evade = true
    }},
    {"centaur_bow", {
        .name = "Centaur Archer",
        .description = "Evade.",
        .rank = 6, .scale = 0.56f, .health = 7, .damage = 5,
        .size = EntitySize::Big, .momentOfContact = 9,
        .evade = true
    }},
    {"dante_hero", {
        .name = "Dante",
        .rank = 5, .scale = 0.56f, .health = 30, .damage = 0,
        .size = EntitySize::Big
    }},
    {"dark_rider", {
        .name = "Dark Rider",
        .rank = 5, .scale = 0.56f, .health = 30, .damage = 0,
        .size = EntitySize::Big
    }},
    {"dark_sword", {
        .name = "Dusk Guardian",
        .description = "Taunt.",
        .rank = 5, .scale = 0.6f, .health = 6, .damage = 5,
        .size = EntitySize::Small, .momentOfContact = 9,
        .taunting = true
    }},
    {"elf_bow", {
        .name = "Woodland Ranger",
        .description = "Evade.",
        .rank = 4, .scale = 0.6f, .health = 3, .damage = 5,
        .size = EntitySize::Small, .momentOfContact = 5,
        .evade = true
    }},
    {"elf_spear", {
        .name = "Elven Protector",
        .description = "Taunt.",
        .rank = 6, .scale = 0.6f, .health = 6, .damage = 7,
        .size = EntitySize::Small, .momentOfContact = 4,
        .taunting = true
    }},
    {"elf_sword", {
        .name = "Woodland Duelist",
        .rank = 6, .scale = 0.6f, .health = 4, .damage = 7,
        .size = EntitySize::Small, .momentOfContact = 4,
        .evade = true
    }},
    {"fencing_maiden", {
        .name = "Fencing Maiden",
        .description = "Taunt.",
        .rank = 4, .scale = 0.6f, .health = 5, .damage = 4,
        .size = EntitySize::Small, .momentOfContact = 14,
        .taunting = true
    }},
    {"field_maid", {
        .name = "Field Maid",
        .rank = 2, .scale = 0.6f, .health = 4, .damage = 3,
        .size = EntitySize::Small, .momentOfContact = 4
    }},
    {"gorgon", {
        .name = "Gorgon",
        .description = "Evade.",
        .rank = 5, .scale = 0.65f, .health = 4, .damage = 5,
        .size = EntitySize::Medium, .momentOfContact = 4,
        .evade = true
    }},
    {"green_goblin", {
        .name = "Green Goblin",
        .rank = 2, .scale = 0.56f, .health = 3, .damage = 3,
        .size = EntitySize::Small, .momentOfContact = 4
    }},
    {"griffin", {
        .name = "Griffin",
        .rank = 6, .scale = 0.56f, .health = 6, .damage = 6,
        .size = EntitySize::Big, .momentOfContact = 9
    }},
    {"heavy_knight", {
        .name = "Heavy Knight",
        .description = "Taunt.",
        .rank = 4, .scale = 0.65f, .health = 5, .damage = 4,
        .size = EntitySize::Small, .momentOfContact = 7,
        .taunting = true
    }},
    {"imp", {
        .name = "Lowly Imp",
        .rank = 1, .scale = 0.65f, .health = 2, .damage = 2,
        .size = EntitySize::Small, .momentOfContact = 7
    }},
    {"knight", {
        .name = "Knight",
        .description = "Taunt.",
        .rank = 3, .scale = 0.6f, .health = 4, .damage = 3,
        .size = EntitySize::Small, .momentOfContact = 7,
        .taunting = true
    }},
    {"militia", {
        .name = "Militia",
        .description = "Taunt.",
        .rank = 1, .scale = 0.6f, .health = 3, .damage = 1,
        .size = EntitySize::Small, .momentOfContact = 4,
        .taunting = true
    }},
    { "shadow_priest", {
        .name = "Shadow Priest",
        .description = "Evade.",
        .rank = 5, .scale = 0.65f, .health = 4, .damage = 5,
        .size = EntitySize::Medium, .momentOfContact = 4,
        .evade = true
    } },
    {"skeleton", {
        .name = "Skeleton",
        .rank = 1, .scale = 0.62f, .health = 1, .damage = 3,
        .size = EntitySize::Small, .momentOfContact = 3
    }},
    {"spectre", {
        .name = "Spectre",
        .rank = 2, .scale = 0.65f, .health = 3, .damage = 3,
        .size = EntitySize::Small, .momentOfContact = 3
    }},
    {"spider_queen", {
        .name = "Arachnid Queen",
        .description = "Evade.",
        .rank = 7, .scale = 0.65f, .health = 10, .damage = 7,
        .size = EntitySize::Big, .momentOfContact = 7,
        .evade = true
    }},
    {"succubus_blue", {
        .name = "Violet Temptress",
        .description = "Evade.",
        .rank = 6, .scale = 0.6f, .health = 6, .damage = 4,
        .size = EntitySize::Small, .momentOfContact = 5,
        .evade = true
    }},
    {"succubus_golden", {
        .name = "Golden Mother",
        .rank = 6, .scale = 0.6f, .health = 5, .damage = 6,
        .size = EntitySize::Small, .momentOfContact = 6
    }},
    {"succubus_red", {
        .name = "Crimson Scourge",
        .description = "Taunt.",
        .rank = 7, .scale = 0.6f, .health = 7, .damage = 6,
        .size = EntitySize::Small, .momentOfContact = 11,
        .taunting = true
    }},
    {"succubus_whip", {
        .name = "Dungeon Mistress",
        .rank = 5, .scale = 0.6f, .health = 5, .damage = 6,
        .size = EntitySize::Small, .momentOfContact = 11
    }},
    {"succubus_wizard", {
        .name = "Seductive Wizard",
        .description = "Evade.",
        .rank = 7, .scale = 0.6f, .health = 5, .damage = 8,
        .size = EntitySize::Big, .momentOfContact = 4,
        .evade = true
    }},
    {"vampire_lady", {
        .name = "Countess Sandra",
        .rank = 5, .scale = 0.6f, .health = 4, .damage = 6,
        .size = EntitySize::Small, .momentOfContact = 4
    }},
    {"white_knight", {
        .name = "Mounted Knight",
        .description = "Evade.",
        .rank = 6, .scale = 0.56f, .health = 7, .damage = 5,
        .size = EntitySize::Big,
        .evade = true
    }},
    {"witch", {
        .name = "Bright Witch",
        .description = "Evade.",
        .rank = 5, .scale = 0.6f, .health = 3, .damage = 6,
        .size = EntitySize::Small, .momentOfContact = 5,
        .evade = true
    }}
    };

    enum class CardType {
        Entity,
        Spell,
        Buff,
        Debuff,
        StateAltering,
        Unknown
    };

    inline bool IsSpellLike(CardType type)
    {
        constexpr CardType spellTypes[] = {
            CardType::Spell,
            CardType::Buff,
            CardType::Debuff,
            CardType::StateAltering
        };
        return std::ranges::find(spellTypes, type) != std::end(spellTypes);
    }

    enum class Stat { Health, Attack, Taunt, SkipTurn, FinishedTurn };

 /*   struct SpellStats {
        CardType cardType;
        string name;
        int frames;
        int mana;
        int statsModifier;
        vector<Stat> statsToModify;
    };

    struct StateAltering
    {
        CardType subType;
        Stat state;
        bool stateModifier;
    };


    inline unordered_map<string, SpellStats> spellStats = {
    {"black_shield",          {CardType::StateAltering, "Black Shield", 27, 1, 3, {Stat::Health}} },
    {"fire_ball",             {CardType::Debuff, "Fireball", 26, 1, 3,{Stat::Health}}},
    {"golden_sword",             {CardType::Buff, "Golden Sword", 30, 2, 3, {Stat::Attack}}},
    {"health_potion",          {CardType::Buff, "Health Potion", 16, 1, 3, {Stat::Health}}},
    {"personal_squire",          {CardType::StateAltering, "Personal Squire", 27, 1, 2, {Stat::Health, Stat::Attack}}},
    {"shield_breaker",          {CardType::StateAltering, "Shieldbreaker", 23, 3, 3, {Stat::Health}}},
    {"sound_sleeper",          {CardType::StateAltering, "Sound Sleeper", 31, 1, 0, {}}},
    {"sword_thief",          {CardType::Debuff, "Sword Thief", 18, 1, 2, {Stat::Attack}}},
    {"thunderbolt",          {CardType::Debuff, "Thunderbolt", 17, 2, 4, {Stat::Health}}},
    {"violet_poison",          {CardType::StateAltering, "Violet Poison", 26, 1, 2, {Stat::Health}}}
    };

    inline unordered_map<string, StateAltering> stateStats = {
        {"black_shield",          {CardType::Buff, Stat::Taunt, true }},
        {"personal_squire",       {CardType::Buff, Stat::Taunt, true }},
        {"shield_breaker",        {CardType::Debuff, Stat::Taunt, false }},
        {"sound_sleeper",         {CardType::Debuff, Stat::SkipTurn, true}},
        {"violet_poison",         {CardType::Debuff, Stat::SkipTurn, true}}
    };

    inline std::unordered_map<std::string, std::string> cardDescriptions = {
     {"angel", "Taunt. Evade."},
     {"archer", "Evade."},
     {"battle_bride", ""},
     {"black_shield", "Give a unit Taunt and +3 Health."},
     {"blue_golem", "Taunt."},
     {"blue_pixie", "Evade."},
     {"blue_wizard", "Evade."},
     {"centaur_bow", "Evade."},
     {"dark_sword", "Taunt."},
     {"elf_bow", "Evade."},
     {"elf_spear", "Taunt."},
     {"elf_sword", "Evade."},
     {"fencing_maiden", "Taunt."},
     {"field_maid", ""},
     {"fire_ball", "Deal 3 damage to an enemy."},
     {"golden_sword", "Give a unit +3 Attack."},
     {"gorgon", "Evade."},
     {"green_goblin", ""},
     {"griffin", ""},
     {"health_potion", "Give a unit +3 Health."},
     {"heavy_knight", "Taunt."},
     {"imp", ""},
     {"knight", "Taunt."},
     {"militia", "Taunt."},
     {"personal_squire", "Give a unit +2/+2."},
     {"shield_breaker", "Remove Taunt from unit."},
     {"skeleton", ""},
     {"sound_sleeper", "Unit skips next turn."},
     {"spectre", ""},
     {"spider_queen", "Evade."},
     {"succubus_blue", "Evade."},
     {"succubus_golden", ""},
     {"succubus_red", ""},
     {"succubus_whip", ""},
     {"succubus_wizard", "Evade."},
     {"sword_thief", "Give a minion -2 Attack."},
     {"thunderbolt", "Deal 3 damage to an enemy."},
     {"vampire_lady", ""},
     {"violet_poison", "Deal 2 damage to a unit. Unit skips next turn."},
     {"white_knight", "Evade."},
     {"witch", "Evade."}
    };*/

    struct StateAltering {
        CardType subType{};
        Stat state{};
        bool stateModifier = false;
    };

    struct SpellStats {
        CardType cardType{};
        std::string name;
        std::string description;
        int frames = 0;
        int mana = 0;
        int statsModifier = 0;
        std::vector<Stat> statsToModify{};
        std::optional<StateAltering> stateAltering;
    };

    inline std::unordered_map<std::string, SpellStats> spellStats = {
        {"black_shield", {
            .cardType = CardType::StateAltering,
            .name = "Black Shield",
            .description = "Give a unit Taunt and +3 Health.",
            .frames = 27,
            .mana = 2,
            .statsModifier = 3,
            .statsToModify = {Stat::Health},
            .stateAltering = StateAltering{CardType::Buff, Stat::Taunt, true}
        }},
        {"fire_ball", {
            .cardType = CardType::Debuff,
            .name = "Fireball",
            .description = "Deal 3 damage to an enemy.",
            .frames = 26,
            .mana = 1,
            .statsModifier = 3,
            .statsToModify = {Stat::Health}
        }},
        {"golden_sword", {
            .cardType = CardType::Buff,
            .name = "Golden Sword",
            .description = "Give a unit +3 Attack.",
            .frames = 30,
            .mana = 2,
            .statsModifier = 3,
            .statsToModify = {Stat::Attack}
        }},
        {"health_potion", {
            .cardType = CardType::Buff,
            .name = "Health Potion",
            .description = "Give a unit +3 Health.",
            .frames = 16,
            .mana = 1,
            .statsModifier = 3,
            .statsToModify = {Stat::Health}
        }},
        {"personal_squire", {
            .cardType = CardType::StateAltering,
            .name = "Personal Squire",
            .description = "Give a unit Taunt and +2/+2.",
            .frames = 27,
            .mana = 2,
            .statsModifier = 2,
            .statsToModify = {Stat::Health, Stat::Attack},
            .stateAltering = StateAltering{CardType::Buff, Stat::Taunt, true}
        }},
        {"shield_breaker", {
            .cardType = CardType::StateAltering,
            .name = "Shieldbreaker",
            .description = "Deal 3 damage to a unit. Remove Taunt from target.",
            .frames = 23,
            .mana = 3,
            .statsModifier = 3,
            .statsToModify = {Stat::Health},
            .stateAltering = StateAltering{CardType::Debuff, Stat::Taunt, false}
        }},
        {"sound_sleeper", {
            .cardType = CardType::StateAltering,
            .name = "Sound Sleeper",
            .description = "Unit skips next turn.",
            .frames = 31,
            .mana = 1,
            .stateAltering = StateAltering{CardType::Debuff, Stat::SkipTurn, true}
        }},
        {"sword_thief", {
            .cardType = CardType::Debuff,
            .name = "Sword Thief",
            .description = "Give a unit -2 Attack.",
            .frames = 18,
            .mana = 1,
            .statsModifier = 2,
            .statsToModify = {Stat::Attack}
        }},
        {"thunderbolt", {
            .cardType = CardType::Debuff,
            .name = "Thunderbolt",
            .description = "Deal 4 damage to an enemy.",
            .frames = 17,
            .mana = 2,
            .statsModifier = 4,
            .statsToModify = {Stat::Health}
        }},
        {"violet_poison", {
            .cardType = CardType::StateAltering,
            .name = "Violet Poison",
            .description = "Deal 2 damage to a unit. Target skips next turn.",
            .frames = 26,
            .mana = 1,
            .statsModifier = 2,
            .statsToModify = {Stat::Health},
            .stateAltering = StateAltering{CardType::Debuff, Stat::SkipTurn, true}
        }}
    };




    struct HeroAbility {
        CardType abilityType;
        string type;
        // Ghost is the animation that plays under the mousePos
        // when the ability is Selected
        string ghostType; 
        int frames;
        int manaCost;
        string visualEffect;
        int visualEffectFrames;
        Stat statToModify;
        int statModifier;
    };

    inline unordered_map<string, HeroAbility> heroAbilities = {
        {"dante_hero", {
            CardType::Buff,
            "dante_heal",
            "dante_heal_ghost",
            10,
            2,
            "dante_heal_effect",
            17,
            Stat::Health,
            2}},
        {"dark_rider", {
            CardType::Debuff,
            "blood_drain",
            "blood_drain_ghost",
            14,
            2,
            "blood_drain_effect",
            19,
            Stat::Health,
            2}}
    };


    //inline unordered_map<string, int> entityPortraitData = {
    //}

    // Is the BattleEntity part of your army or of the enemies army
    enum class Relationship {
        Ally,
        Enemy
    };



    std::string toString(Stance stance);

    // Overload negation operator to use like a bool variable
    Direction operator!(Direction dir);

    // String splitting
    std::vector<std::string> split(const std::string& s, const std::string& delimiter);

    // Natural string comparison (for numbers in strings)
    bool naturalCompare(const std::string& a, const std::string& b);

    // File path operations
    size_t getFileCount(const fs::path& filepath);
    std::string getFilenameFromPath(const fs::path& path);
    std::string getStemFromPath(const fs::path& path);

    // Random number generation
    float randomFloat(float min, float max);

    // String manipulation
    std::string trim(const std::string& str);
    std::string toLower(const std::string& str);

    // Type conversions
    bool stringToBool(const std::string& str);
    int safeStringToInt(const std::string& str, int defaultValue = 0);
    inline std::vector<std::string> townDirectories = { "background", "battleground", "dante", "tiles", "units", "special_units", "enemy_army_captains", "coordinates"};

    // used to load Entities, mostly in Towns
    void OpenFileAndLoadData(const fs::directory_entry& entry, const string& directory, vector<string>& data);
    std::optional<EntityData> ParseEntityData(const vector<string>& coordinates, const vector<string>& dialogue);

    void DrawTextOutlined(Font font, const char* text, Vector2 pos, float fontSize, float spacing, Color textColor, Color outlineColor, int outlineSize);

}