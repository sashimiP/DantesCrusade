#include "TextureManager.h"

TextureManager::TextureManager()
{
	auto start = std::chrono::high_resolution_clock::now();	

	LoadUnitTextures();
	LoadBackgrouds();
	LoadPortraitTextures();

	LoadUIAssets();
	LoadTiles();

	LoadMediaFilePaths();

	LoadCardPaths();

	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed = end - start;
	std::cout << "#####TEXTURES LOADED IN: " << elapsed.count() << " seconds #################\n";
}

void TextureManager::LoadTypes(vector<string>& types, fs::path directory)
{
	types.reserve(Utilities::getFileCount(directory));
	cout << "UNITS TYPES:" << endl;
	for (const auto& entry : fs::directory_iterator(directory))
	{
		auto option = Utilities::split(entry.path().string(), "\\");
		cout << option.back() << endl;
		types.push_back(std::move(option.back()));

	}
	
}

vector<string> TextureManager::LoadStances(const std::string& type) const
{
	const auto typePath = UNITS_PATH / type;
	if (!fs::exists(typePath)) std::cout << typePath << " doesn't exist" << std::endl;
	std::vector<std::string> stancess;
	stancess.reserve(Utilities::getFileCount(typePath));
	for (const auto& entry : fs::directory_iterator(typePath))
	{
		auto path_str = entry.path().string();
		auto options = Utilities::split(entry.path().string(), "\\");
		
		if (!options.empty())
		{
			stancess.push_back(std::move(options.back()));
		}
		
	}
	
	return stancess;
}

string TextureManager::LoadSpriteSheetPath(const std::string& type, const std::string& stance) const
{
	auto framePath = UNITS_PATH / type / stance;
	auto frameIter = fs::directory_iterator(framePath);
	auto it = fs::begin(frameIter);

	if (it == fs::end(frameIter)) {
		std::cout << "No valid file found in directory: " << framePath << std::endl;
		return "";
	}

	return it->path().string();
	
}

//std::pair<string, FrameData> TextureManager::GetStanceTextures(const std::string& type, const std::string& stance)
//{
//	auto spriteSheetPath = LoadSpriteSheetPath(type, stance);
//	float scale = 1.0f;
//	Image image = LoadImage(spriteSheetPath.c_str());
//	ImageResize(&image, image.width*scale, image.height*scale);
//	Texture2D texture = LoadTextureFromImage(image);
//
//	// Extract ALL digits from filename
//	int colorsCount = 1; // default colors count
//	size_t numbOfColors = spriteSheetPath.find_first_of("0123456789");
//
//	int framesCount = DEFAULT_FRAME_COUNT;
//
//	size_t firstDigit = spriteSheetPath.find_first_of("0123456789");
//	size_t lastDigit = spriteSheetPath.find_last_of("0123456789");
//
//	if (firstDigit != string::npos && lastDigit != string::npos) {
//		string numberStr = spriteSheetPath.substr(firstDigit, lastDigit - firstDigit + 1);
//		try {
//			framesCount = std::stoi(numberStr);
//		}
//		catch (...) {
//			cout << "Invalid frame count in: " << spriteSheetPath << "\n";
//		}
//	}
//
//	UnloadImage(image);
//
//	return { stance, {framesCount, colorsCount, texture} };
//}

std::pair<std::string, FrameData> TextureManager::GetStanceTextures(const std::string& type, const std::string& stance)
{
	auto spriteSheetPath = LoadSpriteSheetPath(type, stance);
	float scale = 1.0f;
	//Image image = LoadImage(spriteSheetPath.c_str());
	//ImageResize(&image, image.width * scale, image.height * scale);
	/*Texture2D texture = LoadTextureFromImage(image);*/
	Texture2D texture = LoadTexture(spriteSheetPath.c_str());

	int colorsCount = 1;
	int framesCount = DEFAULT_FRAME_COUNT;

	// Extract filename only
	std::string filename = std::filesystem::path(spriteSheetPath).filename().string();

	// Updated pattern for "colors_<colors>_frames_<frames>.png"
	std::regex pattern(R"(colors_(\d+)_frames_(\d+)\.png)");
	std::smatch match;

	if (std::regex_match(filename, match, pattern)) {
		try {
			colorsCount = std::stoi(match[1]);
			framesCount = std::stoi(match[2]);
		}
		catch (...) {
			std::cerr << "Invalid number format in: " << filename << "\n";
		}
	}
	else {
		std::cerr << "Filename format incorrect: " << filename << "\n";
	}

	//UnloadImage(image);

	return { stance, {framesCount, colorsCount, texture} };
}

void TextureManager::LoadUnitTextures()
{
	LoadTypes(unitTypes, UNITS_PATH);
	Textures.reserve(unitTypes.size());
	for (const auto& type : unitTypes)
	{
		for (const auto& stance : LoadStances(type))
		{
			Textures[type].insert(GetStanceTextures(type, stance));
		}
	}

}

void TextureManager::LoadPortraitTextures()
{
	cout << "LOADING PORTRAIT TEXTUERS" << endl;
	LoadTypes(portraitDirectories, PORTRAITS_PATH); // load directories
	auto start = std::chrono::high_resolution_clock::now();
	for (const auto& directory : portraitDirectories)
	{
		
		fs::path filepath = PORTRAITS_PATH / directory;
		if (!fs::exists(filepath)) return;

		for (const auto& entry : fs::directory_iterator(filepath))
		{
			const auto& path = entry.path();
			string name{ path.stem().string() };
			auto texture = LoadTexture(path.string().c_str());
			SetTextureFilter(texture, TEXTURE_FILTER_BILINEAR);
			if(directory == "dialogue")
				dialoguePortraits.emplace_back(directory, std::move(name), texture);
			else if(directory == "animated")
				animatedPortraits.emplace_back(directory, std::move(name), texture);
			else 
				portraits.emplace_back(directory, std::move(name), texture);

		}
	}

	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed = end - start;
	std::cout << "#####PORTRAITS LOADED IN: " << elapsed.count() << " seconds #################\n";
}

//void TextureManager::LoadMediaFilePaths()
//{
//	cout << "LOADING MEDIA FILES" << endl;
//	LoadTypes(mediaDirectories, MEDIA_PATH); // load directories
//	auto start = std::chrono::high_resolution_clock::now();
//	for (const auto& directory : mediaDirectories)
//	{
//		
//		fs::path filepath = MEDIA_PATH / directory;
//		if (!fs::exists(filepath)) return;
//		cout << filepath << endl;
//		for (const auto& entry : fs::directory_iterator(filepath))
//		{
//			cout << entry << endl;
//			const auto& path = entry.path();
//			string name{ path.stem().string() };
//			if(directory == "units")
//			{
//				MediaStream media = LoadMediaEx(
//					path.string().c_str(),
//					MEDIA_LOAD_NO_AUDIO | MEDIA_FLAG_LOOP
//				);
//				mediaFiles.emplace_back(directory, std::move(name), media);
//			}
//			else if (directory == "trailer")
//			{
//				MediaStream media = LoadMedia(entry.path().string().c_str());
//				if (IsMediaValid(media))
//					mediaFiles.emplace_back(directory, std::move(name), media);
//			}
//
//
//		}
//	}
//
//	auto end = std::chrono::high_resolution_clock::now();
//	std::chrono::duration<double> elapsed = end - start;
//	std::cout << "#####MEDIA FILES IN: " << elapsed.count() << " seconds #################\n";
//}

void TextureManager::LoadMediaFilePaths()
{
	cout << "LOADING MEDIA FILE PATHS" << endl;
	LoadTypes(mediaDirectories, MEDIA_PATH); // load directories
	auto start = std::chrono::high_resolution_clock::now();
	for (const auto& directory : mediaDirectories)
	{

		fs::path filepath = MEDIA_PATH / directory;
		if (!fs::exists(filepath)) return;
		cout << filepath << endl;
		for (const auto& entry : fs::directory_iterator(filepath))
		{
			cout << entry << endl;
			const auto& path = entry.path();
			string name{ path.stem().string() };
			
			if(directory == "units")
				unitsWithMedia.emplace_back(name);

			mediaFilePaths.emplace_back(directory, std::move(name), entry);
			
		}
	}

	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed = end - start;
	std::cout << "#####MEDIA FILES IN: " << elapsed.count() << " seconds #################\n";
}

void TextureManager::LoadFilePaths(fs::path path, vector<FilePath> filepaths, vector<string>directories)
{
	cout << "LOADING FILE PATHS: " <<path.stem() << endl;
	LoadTypes(directories, path); // load directories
	auto start = std::chrono::high_resolution_clock::now();
	for (const auto& directory : directories)
	{

		fs::path filepath = path / directory;
		if (!fs::exists(filepath)) return;
		cout << filepath << endl;
		for (const auto& entry : fs::directory_iterator(filepath))
		{
			cout << entry << endl;
			const auto& path = entry.path();
			string name{ path.stem().string() };

			filepaths.emplace_back(directory, std::move(name), entry);

		}
	}
}


//void TextureManager::LoadPortraitTextures()
//{
//	cout << "LOADING PORTRAIT TEXTUERS" << endl;
//	LoadTypes(portraitDirectories, PORTRAITS_PATH); // load directories
//	auto start = std::chrono::high_resolution_clock::now();
//	for (const auto& directory : portraitDirectories)
//	{
//
//		fs::path filepath = PORTRAITS_PATH / directory;
//		if (!fs::exists(filepath)) return;
//
//	
//		
//		for (const auto& entry : fs::directory_iterator(filepath))
//		{
//			const auto& path = entry.path();
//			string name{ path.stem().string() };
//			if (directory == "animated")
//			{
//				auto texture = LoadTexture(path.string().c_str());
//
//				if (directory == "dialogue")
//					dialoguePortraits.emplace_back(directory, std::move(name), texture);
//				else
//					portraits.emplace_back(directory, std::move(name), texture);
//			}
//			else
//				animatedPortraitPaths.emplace(std::move(name), path);
//		}
//		
//	}
//	
//
//	auto end = std::chrono::high_resolution_clock::now();
//	std::chrono::duration<double> elapsed = end - start;
//	std::cout << "#####PORTRAITS LOADED IN: " << elapsed.count() << " seconds #################\n";
//}

vector<Texture2D> TextureManager::LoadBackgroundLayers(const string& type)
{
	vector<Texture2D> tempTextures;
	tempTextures.reserve(20);

	fs::path typePath = BACKGROUND_PATH / type;

	if (!fs::exists(typePath)) {
		std::cout << "Directory not found: " << typePath << std::endl;
		return tempTextures;
	}

	for (const auto& entry : fs::directory_iterator(typePath)) {
		if (entry.is_regular_file()) {
			Texture2D texture = LoadTexture(entry.path().string().c_str());
			tempTextures.push_back(texture);
			cout << "Loaded background: " << entry.path().filename().string() << std::endl;
		}
	}

	return tempTextures;
}

void TextureManager::LoadBackgrouds()
{
	LoadTypes(backgroundTypes, BACKGROUND_PATH);
	for (const auto& type : backgroundTypes)
		backgroundLayers[type] = LoadBackgroundLayers(type);
}

std::vector<std::string>* TextureManager::UnitTypes()
{
	return &unitTypes;
}

StanceTextures* TextureManager::TextureMap(const std::string& type)
{
	auto it = Textures.find(type);
	return (it != Textures.end()) ? &it->second : nullptr;
}

vector<Texture2D>* TextureManager::BackgroundLayers(const string& type)
{
	auto it = backgroundLayers.find(type);
	return (it != backgroundLayers.end()) ? &it->second : nullptr;
}

std::shared_ptr<const map<string, Texture2D>> TextureManager::GetPortraitData(string_view type)
{  
	auto result = std::make_shared<std::map<std::string, Texture2D>>();
	for (const auto& portrait : portraits)
	{
		if (type == portrait.type)
		{
			result->emplace(portrait.name, portrait.texture);
		}
	}
	return result;
}

//void TextureManager::CreateLargerPortraits(int newWidth, int newHeight)
//{
//	for (const auto& portrait : portraits)
//	{
//		if ("units" == portrait.type)
//		{
//			Image image = LoadImageFromTexture(portrait.texture);
//			ImageResizeNN(&image, newWidth, newHeight);
//			Texture2D resizedTexture = LoadTextureFromImage(image);
//			
//			largerPortraits.emplace_back(portrait.type, portrait.name, resizedTexture);
//			UnloadImage(image);
//		}
//	}
//}

void TextureManager::LoadUIAssets()
{
	assets.reserve(Utilities::getFileCount(UI_PATH));

	for (const auto& entry : fs::directory_iterator(UI_PATH))
	{
		auto texture = LoadTexture(entry.path().string().c_str());
		auto name = entry.path().stem().string();
		//string nameSize = name + "_" + to_string(texture.width) + "x" + to_string(texture.height);
		assets.emplace_back(std::move(name), texture);
		//returnedAssets.emplace_back(std::move(nameSize), texture);
	}

}

void TextureManager::LoadTiles()
{
	tileTextures.reserve(Utilities::getFileCount(TILES_PATH));

	for (const auto& entry : fs::directory_iterator(TILES_PATH))
	{
		
		auto texture = LoadTexture(entry.path().string().c_str());
		auto name = entry.path().stem().string();
		cout << name << endl;
		tileTextures.emplace_back(std::move(name), texture);
	}
}

void TextureManager::LoadCardPaths()
{
	cardTextures.reserve(Utilities::getFileCount(CARDS_PATH));

	for (const auto& entry : fs::directory_iterator(CARDS_PATH))
	{
		cout << entry.path() << endl;
		cardFilePaths.emplace_back("", entry.path().stem().string(), (std::move(entry.path().string())));
	}
}

Texture2D* TextureManager::Portrait(string_view name)
{
	for (auto& portrait : portraits)
	{
		if (portrait.name == name) // std::string == const char* is safe
		{
			return &portrait.texture;
		}
	}
	return nullptr;
}

Texture2D* TextureManager::DialoguePortrait(string_view name)
{
	for (auto& portrait : dialoguePortraits)
	{
		if (portrait.name == name) // std::string == const char* is safe
		{
			return &portrait.texture;
		}
	}
	return nullptr;
}

Texture2D* TextureManager::AnimatedPortrait(string_view name)
{
	for (auto& portrait : animatedPortraits)
	{
		if (portrait.name == name) // std::string == const char* is safe
		{
			return &portrait.texture;
		}
	}
	return nullptr;
}

//std::shared_ptr<MediaStream> TextureManager::UnitMedia(std::string_view name_sv)
//{
//	std::string name{ name_sv };
//	auto it = loadedUnitsMedia.find(name);
//	// Fast path: already loaded
//	if (it != loadedUnitsMedia.end())
//	{
//		TraceLog(LOG_INFO, "[Media] Reusing cached media: %s", name.c_str());
//		return it->second; // cheap shared_ptr copy
//	}
//
//	// Load new media
//	for (const auto& file : mediaFilePaths)
//	{
//		if (file.name == name)
//		{
//			TraceLog(LOG_INFO, "[Media] Loading media: %s", file.filePath.string().c_str());
//			auto mediaPtr = std::shared_ptr<MediaStream>(
//				new MediaStream{
//					LoadMediaEx(
//						file.filePath.string().c_str(),
//						MEDIA_FLAG_LOOP | MEDIA_LOAD_NO_AUDIO
//					)
//				},
//				[](MediaStream* m)
//				{
//					if (m && IsMediaValid(*m))
//					{
//						UnloadMedia(m);
//					}
//					delete m;
//				}
//			);
//
//			// Validate after load
//			if (!IsMediaValid(*mediaPtr))
//			{
//				TraceLog(LOG_ERROR, "Failed to load media: %s", name.c_str());
//				return nullptr;
//			}
//
//			loadedUnitsMedia.emplace(name, mediaPtr);
//			return mediaPtr;
//		}
//	}
//
//	return nullptr; // not found
//}

MediaStreamPtr TextureManager::UnitMedia(std::string_view name_sv)
{
	std::string name{ name_sv };

	for (const auto& file : mediaFilePaths)
	{
		if (file.name != name)
			continue;

		TraceLog(LOG_INFO, "[Media] Loading media: %s", file.filePath.string().c_str());

		auto* raw = new MediaStream{
			LoadMediaEx(
				file.filePath.string().c_str(),
				MEDIA_FLAG_LOOP | MEDIA_LOAD_NO_AUDIO | MEDIA_FLAG_NO_AUTOPLAY
			)
		};

		if (!IsMediaValid(*raw))
		{
			TraceLog(LOG_ERROR, "Failed to load media: %s", name.c_str());
			delete raw;
			return MediaStreamPtr{};   // empty
		}

		return MediaStreamPtr{ raw }; // uses MediaDeleter automatically
	}

	return MediaStreamPtr{};   // not found
}

//string* TextureManager::UnitMediaPath(string_view type_sv)
//{
//	for (const auto& file : mediaFilePaths)
//	{
//		if (file.name != type_sv)
//			continue;
//
//		return &file.filePath.string();
//	}
//
//	return nullptr;   // not found
//}

//void TextureManager::UnloadUnitMedia(string_view name_sv)
//{
//	std::string name{ name_sv };
//	
//	if (auto it = loadedUnitsMedia.find(name); it != loadedUnitsMedia.end())
//	{
//		loadedUnitsMedia.erase(it);
//	}
//}

std::unique_ptr<MediaStream> TextureManager::IntroMedia()
{
	auto it = std::ranges::find(mediaFilePaths, "intro", &FilePath::directory);
	if (it == mediaFilePaths.end()) {
		TraceLog(LOG_WARNING, "No 'intro' media found");
		return nullptr;
	}

	const auto& path = it->filePath;

	// Optional but strongly recommended
	if (!std::filesystem::exists(path)) {
		TraceLog(LOG_ERROR, "Intro path does not exist: %s", path.string().c_str());
		return nullptr;
	}

	// Use UTF-8 representation
	auto raw = LoadMediaEx(path.string().c_str(), MEDIA_FLAG_LOOP);

	if (!IsMediaValid(raw)) {
		TraceLog(LOG_ERROR, "Failed to load intro: %s", path.string().c_str());
		UnloadMedia(&raw);  // safe even if invalid, but good habit
		return nullptr;
	}

	return std::make_unique<MediaStream>(raw);
}

void TextureManager::TrailerMedia(vector<std::unique_ptr<MediaStream>>& playlist)
{
	playlist.reserve(5);
	for (auto& mediaFile : mediaFilePaths)
	{
		if (mediaFile.directory == "trailer")
		{
			auto m = LoadMedia(mediaFile.filePath.string().c_str());
			playlist.emplace_back(std::make_unique<MediaStream>(m));
		}
	}
}

//std::shared_ptr<Texture2D> TextureManager::AnimatedPortrait(string_view name)
//{
//	auto it = animatedPortraitPaths.find(std::string{ name });
//	if (it == animatedPortraitPaths.end())
//	{
//		return nullptr;
//	}
//	auto texture = LoadTexture(it->second.string().c_str());
//
//	return std::make_shared<Texture2D>(texture);
//	
//
//	return nullptr;
//}

Texture2D* TextureManager::Asset(string_view name)
{

	for (auto& asset : assets)
	{
		if (asset.name == name)
		{
			return &asset.texture;
		}
	}
	cout << name << " Not found" << endl;

	return nullptr;
}

Texture2D* TextureManager::Tiles(string_view name)
{
	for (auto& tile : tileTextures)
	{
		if (tile.name == name) // std::string == const char* is safe
		{
			return &tile.texture;
		}
	}
	cout << "TILE "<<name<< " NOT FOUND" << endl;
	cout << "Available tiles:\n";

	for (auto& tile : tileTextures)
	{
		cout << tile.name << endl;
	}
	return nullptr;
}

Texture2D* TextureManager::Card(string_view name_sv)
{
	std::string name{ name_sv };

	for (auto& card : cardTextures)
	{
		if (card.name != name)
			continue;

		return &card.texture;
	}

	for (const auto& file : cardFilePaths)
	{
		if (file.name != name)
			continue;



		auto texture = LoadTexture(file.filePath.string().c_str());

		if (!IsTextureValid(texture))
		{
			TraceLog(LOG_ERROR, "Failed to load texture: %s", name.c_str());
			
			return nullptr;   // empty
		}
		
		cardTextures.emplace_back(name, texture);

		return &cardTextures.back().texture;
	}

	TraceLog(LOG_ERROR, "Failed to locate texture: %s", name.c_str());
	return nullptr;   // not found
}


vector<string> TextureManager::GetAvailableUnitsMediaFiles()
{
	return unitsWithMedia;
}