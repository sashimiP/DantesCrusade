#include "Utilities.h"


namespace Utilities {

    EntityType EntityTypeFromString(const std::string& type)
    {
        const unordered_map<string, EntityType> map = {
            {"knight", EntityType::Knight},
            {"heavy_knight", EntityType::HeavyKnight},
            {"white_knight", EntityType::MountedKnight},
            {"archer", EntityType::Archer},
            {"gorgon", EntityType::Gorgon},
            {"witch", EntityType::Witch},
            {"succubus_whip", EntityType::SuccubusWhip},
            {"succubus_blue", EntityType::SuccubusBlue},
            {"succubus_golden", EntityType::SuccubusGolden},
            {"succubus_wizard", EntityType::SuccubusWizard},
            {"succubus_red", EntityType::SuccubusRed},
            {"vampire_lady", EntityType::VampireLady},
            {"spider_queen", EntityType::SpiderQueen},
            {"blue_pixie", EntityType::BluePixie},
            {"angel", EntityType::Angel},
            {"centaur_bow", EntityType::CentaurBow},
            {"elf_bow", EntityType::ElfBow},
            {"elf_sword", EntityType::ElfSword},
            {"elf_spear", EntityType::ElfSpear},
            {"griffin", EntityType::Griffin },
            { "imp", EntityType::Imp},
            {"blue_wizard", EntityType::BlueWizard},
            {"green_goblin", EntityType::GreenGoblin},
            {"field_maid", EntityType::FieldMaid},
            {"fencing_maiden", EntityType::FencingMaiden},
            {"battle_bride", EntityType::BattleBride},
            {"blue_golem", EntityType::BlueGolem},
            {"dante_hero", EntityType::DanteHero},
            {"dark_rider", EntityType::EnemyHero},
            {"militia", EntityType::Militia},
            {"skeleton", EntityType::Skeleton},
            {"spectre", EntityType::Spectre},
            {"shadow_priest", EntityType::ShadowPriest}
        };

        if (auto it = map.find(type); it != map.end())
            return it->second;
        return EntityType::Unknown;
    }

    std::string toString(Stance stance)
    {
        switch (stance) {
        case Stance::Idle: return "idle";
        case Stance::MarchIdle: return "march_idle";
        case Stance::March: return "march";
        case Stance::Walk: return "walk";
        }
        return "idle"; // default fallback
    }

    // Helper function to get the number of files in the directory
    size_t getFileCount(const fs::path& filepath)
    {
        size_t count = 0;
        for (const auto& entry : fs::directory_iterator(filepath)) {
            ++count;
        }

        return count;
    }

    Direction operator!(Direction dir) {
        return (dir == Direction::west)
            ? Utilities::Direction::east
            : Utilities::Direction::east;
    }

    std::vector<std::string> split(const std::string& s, const std::string& delimiter) {
        std::vector<std::string> tokens;
        size_t pos = 0;
        std::string token;
        std::string str = s;

        while ((pos = str.find(delimiter)) != std::string::npos) {
            token = str.substr(0, pos);
            tokens.push_back(token);
            str.erase(0, pos + delimiter.length());
        }
        tokens.push_back(str);
        return tokens;
    }

    bool naturalCompare(const std::string& a, const std::string& b) {
        std::regex numRegex(R"(\d+)");
        auto a_begin = std::sregex_iterator(a.begin(), a.end(), numRegex);
        auto b_begin = std::sregex_iterator(b.begin(), b.end(), numRegex);
        std::string::const_iterator aPos = a.begin(), bPos = b.begin();

        while (a_begin != std::sregex_iterator() && b_begin != std::sregex_iterator()) {
            std::string aStr(aPos, a.begin() + a_begin->position());
            std::string bStr(bPos, b.begin() + b_begin->position());
            if (aStr != bStr) return aStr < bStr;

            int aNum = std::stoi(a_begin->str());
            int bNum = std::stoi(b_begin->str());
            if (aNum != bNum) return aNum < bNum;

            aPos = a.begin() + a_begin->position() + a_begin->length();
            bPos = b.begin() + b_begin->position() + b_begin->length();
            ++a_begin;
            ++b_begin;
        }
        return a < b;
    }

    std::string getFilenameFromPath(const fs::path& path) {
        return path.filename().string();
    }

    std::string getStemFromPath(const fs::path& path) {
        return path.stem().string();
    }

    float randomFloat(float min, float max) {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dist(min, max);
        return dist(gen);
    }

    std::string trim(const std::string& str) {
        auto start = str.begin();
        while (start != str.end() && std::isspace(*start)) start++;

        auto end = str.end();
        do {
            end--;
        } while (std::distance(start, end) > 0 && std::isspace(*end));

        return std::string(start, end + 1);
    }

    void OpenFileAndLoadData(const fs::directory_entry& entry, const string& directory, vector<string>& data)
    {
        //std::fstream dataFile;
        fs::path directoryPath = entry.path() / directory;

        // Check if the directory exists
        if (!fs::exists(directoryPath) || !fs::is_directory(directoryPath)) {
            std::cout << directoryPath << " DIRECTORY NOT FOUND" << std::endl;
            return;
        }

        // Get the only file in the directory
        auto dirIter = fs::directory_iterator(directoryPath);
        auto it = fs::begin(dirIter);

        if (it == fs::end(dirIter) || !fs::is_regular_file(*it)) {
            //std::cout << "No valid file found in directory: " << directoryPath << std::endl;
            return;
        }

        const fs::path& filePath = it->path();
        //std::cout << "Opening file: " << filePath << std::endl;

        // Open and read the file
        std::ifstream dataFile(filePath);

        if (!dataFile.is_open()) {
            std::cout << "Failed to open file: " << filePath << std::endl;
            return;
        }

        string line;
        data.reserve(15);
        while (getline(dataFile, line))
            data.push_back(std::move(line));

        dataFile.close();
    }
    std::optional<EntityData> ParseEntityData(const vector<string>& coordinates, const vector<string>& dialogue) {
        if (coordinates.size() < 7) {
            std::cerr << "Invalid coordinates (expected at least 7, got "
                << coordinates.size() << ")\n";
            return std::nullopt;
        }

        try {
            EntityData data;
            data.id = std::stof(coordinates[0]);
            //cout << "DATA ID: " << data.id << endl;
            data.position = std::stoi(coordinates[1]);
            data.x = std::stof(coordinates[2]);
            data.y = std::stof(coordinates[3]);
            data.type = coordinates[4];
            data.direction = static_cast<Utilities::Direction>(std::stoi(coordinates[5]));
            data.stance = coordinates[6];
            data.name = coordinates[7];
            data.color = std::stoi(coordinates[8]);
            data.dialogue = dialogue;
            

           //if (coordinates.size() > 8) data.name = coordinates[8];
        if (coordinates.size() > 9) data.dontScale = coordinates[9] != "0";
        if (coordinates.size() > 10) data.patrolRoute.x = std::stoi(coordinates[10]);
        if (coordinates.size() > 11)data.patrolRoute.y = std::stoi(coordinates[11]);
        if (coordinates.size() > 12) data.special = coordinates[12];
        if (coordinates.size() > 13) data.growStats = std::stof(coordinates[13]);
        if (coordinates.size() > 14) data.price = std::stoi(coordinates[14]);

            return data;
        }
        catch (const std::exception& e) {
            std::cerr << "Error parsing entity data: " << e.what() << "\n";
            return std::nullopt;
        }
    }

    void DrawTextOutlined(Font font, const char* text, Vector2 pos, float fontSize, float spacing, Color textColor, Color outlineColor, int outlineSize)
    {
        // Draw the outline
        for (int x = -outlineSize; x <= outlineSize; x++)
        {
            for (int y = -outlineSize; y <= outlineSize; y++)
            {
                if (x == 0 && y == 0) continue;
                DrawTextEx(font, text, { pos.x + x, pos.y + y }, fontSize, spacing, outlineColor);
            }
        }

        // Draw the main text on top
        DrawTextEx(font, text, pos, fontSize, spacing, textColor);
    }
}