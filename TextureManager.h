#pragma once
#include <string>
#include <vector>
#include<iostream>
#include<fstream>
#include <chrono>
#include<map>
#include<unordered_map>
#include<algorithm>
#include <filesystem>
#include <regex>
#include <ranges>
#include <algorithm>
#include <optional>

namespace fs = std::filesystem;

#include "raylib.h"
#include "raymedia.h"

#include "Utilities.h"

using std::string, std::to_string, std::vector, std::map, std::unordered_map, std::tuple, std::pair, std::cout, std::endl, std::string_view;

struct MediaDeleter
{
	void operator()(MediaStream* m) const
	{
		if (!m) return;
		if (m->ctx) 
		{
			cout << "UNLOAD MEDIA" << endl;
			UnloadMedia(m);
		}
		delete m;
	}
};

using MediaStreamPtr = std::unique_ptr<MediaStream, MediaDeleter>;

struct FrameData
{
	int frames;
	int colors;
	Texture2D texture;
};
using StanceTextures = map<string, FrameData>;
using TextureAtlas = unordered_map<string, StanceTextures>;

struct PortraitData
{
	string type;
	string name;
	Texture2D texture;
};

struct AnimatedData
{
	string type;
	string name;
	MediaStream texture;
};

struct FilePath
{
	string directory;
	string name;
	fs::path filePath;
};

struct NameTexture
{
	string name;
	Texture2D texture;
};

constexpr int DEFAULT_FRAME_COUNT = 0;
const fs::path UNITS_PATH = fs::path("resources") / "units";
const fs::path BACKGROUND_PATH = fs::path("resources") / "background";
const fs::path PORTRAITS_PATH = fs::path("resources") / "portraits";
const fs::path MEDIA_PATH = fs::path("resources") / "media";
const fs::path UI_PATH = fs::path("resources") / "UI";
const fs::path TILES_PATH = fs::path("resources") / "tiles";
const fs::path CARDS_PATH = fs::path("resources") / "cards";

class TextureManager
{
public:
	//TextureManager() = default;
	TextureManager();
	vector<string>* UnitTypes();
	StanceTextures* TextureMap(const string&);
	vector<Texture2D>* BackgroundLayers(const string&);
	std::shared_ptr<const map<string, Texture2D>> GetPortraitData(string_view);
	Texture2D* Portrait(string_view type);
	Texture2D* DialoguePortrait(string_view type);
	Texture2D* AnimatedPortrait(string_view type);

	/*std::shared_ptr<MediaStream> UnitMedia(string_view name_sv);*/
	MediaStreamPtr UnitMedia(std::string_view name);
	//string* UnitMediaPath(string_view type_sv);
	//void UnloadUnitMedia(string_view name_sv);
	std::unique_ptr<MediaStream> IntroMedia();
	void TrailerMedia(vector<std::unique_ptr<MediaStream>>& playlist);

	//Texture2D* LargerPortrait(string_view type);
	Texture2D* Asset(string_view name);
	Texture2D* Tiles(string_view type);

	Texture2D* Card(string_view name_sv);

	//Returns the number of Battle Entities with available Media Portraits
	vector<string> GetAvailableUnitsMediaFiles();

private:
	void LoadTypes(vector<string>&, fs::path);
	void LoadUnitTextures();
	void LoadPortraitTextures();
	void LoadFilePaths(fs::path path, vector<FilePath> filepaths, vector<string>directories);
	void LoadMediaFilePaths();
	vector<Texture2D> LoadBackgroundLayers(const string&);
	void LoadBackgrouds();
	vector<string> LoadStances(const string& type) const;
	string LoadSpriteSheetPath(const string& type, const string& stance) const;
	std::pair<string, FrameData> GetStanceTextures(const string& type, const string& stance);
	//void CreateLargerPortraits(int, int);
	void LoadUIAssets();
	void LoadTiles();
	void LoadCardPaths();


	vector<string> unitTypes;
	vector<string> backgroundTypes;
	vector<string> portraitDirectories;
	vector<string> cardDirectories;
	vector<string> mediaDirectories;
	
	TextureAtlas Textures;
	unordered_map<string, vector<Texture2D>> backgroundLayers;
	vector<PortraitData> portraits;
	vector<PortraitData> dialoguePortraits;
	vector<PortraitData> animatedPortraits;

	vector<string> unitsWithMedia;
	vector<FilePath> mediaFilePaths;
	vector<FilePath> cardFilePaths;
	//unordered_map<string, std::shared_ptr<MediaStream>> loadedUnitsMedia;

	unordered_map<
		std::string,
		fs::path,
		std::hash<std::string>,
		std::equal_to<>
	> animatedPortraitPaths;
	//vector<PortraitData> largerPortraits;
	vector<NameTexture> assets;
	//vector<NameTexture> returnedAssets;
	vector<NameTexture> tileTextures;
	vector<NameTexture> cardTextures;

	

};

