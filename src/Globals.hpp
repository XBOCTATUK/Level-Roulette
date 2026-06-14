#pragma once
#include <Geode/Geode.hpp>

using namespace geode::prelude;

struct LevelData {
    std::string name;
    std::string creator;
    std::string diff;
    int levelID = 0;

    void setDefault() {
        this->name = "";
        this->creator = "";
        this->diff = "";
        this->levelID = 0;
    }

    bool operator==(const LevelData& other) const {
        return name == other.name &&
               creator == other.creator &&
               diff == other.diff &&
               levelID == other.levelID;
    }

    bool operator!=(const LevelData& other) const {
        return !(*this == other);
    }
};

class Globals {
public:
    Globals() = delete;

    static const std::vector<int>& getLevels() { return g_levels; }
    static std::vector<int>& getLevelsMutable() { return g_levels; }
    static const std::vector<int>& getUsedLevels() { return g_usedLevels; }
    static std::vector<int>& getUsedLevelsMutable() { return g_usedLevels; }
    static const std::vector<int>& getSelectedLevels() { return g_selectedLevels; }
    static std::vector<int>& getSelectedLevelsMutable() { return g_selectedLevels; }

    static const std::unordered_map<std::string, std::string>& getSpriteNames() { return g_spriteNames; }
    static const std::unordered_map<std::string, std::string>& getAltSpriteNames() { return g_spriteNames1; }
    static const std::unordered_map<std::string, ccColor3B>& getDifficultyColors() { return g_difficultyColors; }
    static std::unordered_map<std::string, LevelData>& getLevelData() { return g_levelData; }

    static LevelData& getCurrentLevel() { return g_currentLvl; }

    static int& getSpinsCount() { return g_spinsCount; }
    static int& getRequirePercent() { return g_requirePercent; }
    static int& getCurrentPercent() { return g_currentPercent; }
    static int& getSkipsCount() { return g_skipsCount; }
    static std::string& getCurrentListName() { return g_currentListName; }

    static void setCurrentLevel(LevelData& level) { g_currentLvl = level; }
    static void setLevelData(std::unordered_map<std::string, LevelData>& levelData) { g_levelData = levelData; }
    static void setSpinsCount(int count) { g_spinsCount = count; }
    static void setSkipsCount(int count) { g_skipsCount = count; }
    static void setRequirePercent(int percent) { g_requirePercent = percent; }
    static void setCurrentPercent(int percent) { g_currentPercent = percent; }
    static void setCurrentListName(std::string name) { g_currentListName = name; }


    static std::string getDiff(GJGameLevel* level) {
		log::info("{} {} {} {}",
			static_cast<int>(level->m_difficulty),
			level->getAverageDifficulty(),
			level->m_stars,
			level->m_demonDifficulty
		);
		if (level->m_stars == 10 && level->m_demonDifficulty == 3) return "EasyDemon";
		else if (level->m_stars == 10 && level->m_demonDifficulty == 4) return "MediumDemon";
		else if (level->m_stars == 10 && level->m_demonDifficulty == 0) return "HardDemon";
		else if (level->m_stars == 10 && level->m_demonDifficulty == 5) return "InsaneDemon";
		else if (level->m_stars == 10 && level->m_demonDifficulty == 6) return "ExtremeDemon";
		else if (level->getAverageDifficulty() == 0) return "N/A";
        else if (level->m_stars == 1) return "Auto";
		else if (level->m_stars != 1 && level->getAverageDifficulty() == 1) return "Easy";
		else if (level->getAverageDifficulty() == 2) return "Normal";
		else if (level->getAverageDifficulty() == 3) return "Hard";
		else if (level->getAverageDifficulty() == 4) return "Harder";
		else if (level->getAverageDifficulty() == 5) return "Insane";
        else return "N/A";
    }


    static geode::Result<std::filesystem::path> getFilePath() {
        auto modFolder = geode::dirs::getModPersistentDir() / Mod::get()->getID();
        auto filePath = modFolder / "lists.json";

        if (!std::filesystem::exists(modFolder)) {
            auto result = file::createDirectoryAll(modFolder);
            if (result.isErr()) {
                return geode::Err("Failed to create mod directory");
            }
        }

        return geode::Ok(filePath);
    }

    static matjson::Value getListsData() {
        auto filePath = getFilePath();
        if (filePath.isErr()) {
            FLAlertLayer::create(
                "Error",
                "Failed to open the lists file. Please try again later.",
                "Ok"
            )->show();

            return matjson::Value::object();
        }

        if (!std::filesystem::exists(filePath.unwrap())) {
            auto empty = matjson::Value::object();
            auto writeResult = file::writeToJson(filePath.unwrap(), empty);
            if (writeResult.isErr()) {
                FLAlertLayer::create(
                    "Error",
                    "Failed to create the list. Please try again later.",
                    "Ok"
                )->show();
            }
        }

        auto readData = file::readJson(filePath.unwrap());
        if (readData.isErr()) {
            FLAlertLayer::create(
                "Error",
                "Failed to open the lists file. Please try again later.",
                "Ok"
            )->show();
            return matjson::Value::object();
        }
        auto data = readData.unwrap();

        return data;
    }

    static void saveListsData(matjson::Value& data) {
        auto filePath = getFilePath();
        if (filePath.isErr()) {
            FLAlertLayer::create(
                "Error",
                "Failed to save the list. Please try again later.",
                "Ok"
            )->show();
            return;
        }

        auto writeResult = file::writeToJson(filePath.unwrap(), data);
        if (writeResult.isErr()) {
            FLAlertLayer::create(
                "Error",
                "Failed to save the list. Please try again later.",
                "Ok"
            )->show();
        }
    }

private:
    static inline std::vector<int> g_levels;
    static inline std::vector<int> g_usedLevels;
    static inline std::vector<int> g_selectedLevels;
    static inline LevelData g_currentLvl;
    static inline LevelData g_pastLvl;
    static inline int g_spinsCount = 0;
    static inline int g_skipsCount = Mod::get()->getSettingValue<int>("skips-count");
    static inline int g_requirePercent = 1;
    static inline int g_currentPercent = 0;
    static inline std::unordered_map<std::string, LevelData> g_levelData;
    static inline std::string g_currentListName = Mod::get()->getSavedValue<std::string>("current-list-name", "");

    static inline const std::unordered_map<std::string, ccColor3B> g_difficultyColors = {
        {"N/A", {160, 160, 160}},
        {"Auto", {234, 173, 86}},
        {"Easy", {0, 212, 255}},
        {"Normal", {0, 255, 37}},
        {"Hard", {255, 187, 0}},
        {"Harder", {255, 2, 7}},
        {"Insane", {235, 110, 198}},
        {"EasyDemon", {148, 71, 249}},
        {"MediumDemon", {239, 26, 166}},
        {"HardDemon", {255, 57, 80}},
        {"InsaneDemon", {242, 29, 24}},
        {"ExtremeDemon", {176, 0, 0}}
    };
    static inline const std::unordered_map<std::string, std::string> g_spriteNames = {
        {"N/A", "diffIcon_00_btn_001.png"},
        {"Auto", "diffIcon_auto_btn_001.png"},
        {"Easy", "diffIcon_01_btn_001.png"},
        {"Normal", "diffIcon_02_btn_001.png"},
        {"Hard", "diffIcon_03_btn_001.png"},
        {"Harder", "diffIcon_04_btn_001.png"},
        {"Insane", "diffIcon_05_btn_001.png"},
        {"EasyDemon", "diffIcon_07_btn_001.png"},
        {"MediumDemon", "diffIcon_08_btn_001.png"},
        {"HardDemon", "diffIcon_06_btn_001.png"},
        {"InsaneDemon", "diffIcon_09_btn_001.png"},
        {"ExtremeDemon", "diffIcon_10_btn_001.png"}
    };
    static inline const std::unordered_map<std::string, std::string> g_spriteNames1 = {
        {"N/A", "difficulty_00_btn_001.png"},
        {"Auto", "difficulty_auto_btn_001.png"},
        {"Easy", "difficulty_01_btn_001.png"},
        {"Normal", "difficulty_02_btn_001.png"},
        {"Hard", "difficulty_03_btn_001.png"},
        {"Harder", "difficulty_04_btn_001.png"},
        {"Insane", "difficulty_05_btn_001.png"},
        {"EasyDemon", "difficulty_07_btn2_001.png"},
        {"MediumDemon", "difficulty_08_btn2_001.png"},
        {"HardDemon", "difficulty_06_btn2_001.png"},
        {"InsaneDemon", "difficulty_09_btn2_001.png"},
        {"ExtremeDemon", "difficulty_10_btn2_001.png"}
    };
};