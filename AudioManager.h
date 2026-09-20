#pragma once
#include <string>
#include <vector>
#include<iostream>
#include<fstream>
#include<map>
#include<unordered_map>
#include<algorithm>
#include <filesystem>

#include "raylib.h"

namespace fs = std::filesystem;
using std::string, std::cout;

struct SoundData
{
	string title;
	Sound sound;
};

struct MusicData
{
	string title;
	Music music;
};

class AudioManager
{
public:
	AudioManager() = default;
	AudioManager(string);
	~AudioManager();

	Sound* GetSound(string& title);
	Music* GetMusic(string& title);

private:
	fs::path audioPath = fs::path("resources") / "audio";
};

