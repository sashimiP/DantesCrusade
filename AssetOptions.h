#pragma once  
#include <string>  
#include <vector>  
#include <unordered_map>  
#include <iostream>  
#include <array>
#include<filesystem>
namespace fs = std::filesystem;
#include "Utilities.h"  

#include "raylib.h"  

using std::string, std::vector, std::unordered_map;

class Entity;  

class AssetOptions  
{  
public:  
   AssetOptions() = default;  
   AssetOptions(const string& unitType, const Rectangle& unitRect, const vector<string>& options);
   ~AssetOptions();  

   void updateGrid(const Rectangle& unitRect);  
   void drawOptionCells();  
   std::string getOption() const;
   void SetAssetOptions(const vector<string>& options);
   bool hasOption(const std::string& option) const;

private:  
	int fontSize{ 18 };
	int cellWidth{ 0 };
   Rectangle unitRect;  
   fs::path filepath;  
   vector<Rectangle> cells;  
   vector<std::string> options;
   unordered_map<std::string, Rectangle> optionCells;
	
   void CreateGrid();  
   void FindAvailableOptions(); 
   void CreateAssetOptions();
};