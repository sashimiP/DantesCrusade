#pragma once
#include <string>
#include <vector>
#include<iostream>
#include<memory>
#include<map>
#include "raylib.h"
#include<filesystem>
#include <ranges>
#include <algorithm>
#include "Utilities.h"
#include "TextureManager.h"

namespace fs = std::filesystem;

//fs::path BACKGROUND_PATH = fs::path("resources") / "background";


class Background {
public:
    Background() = default;
    Background(std::shared_ptr<TextureManager>, float, fs::path);
    virtual ~Background();

    void LoadLayers(const string&);
    virtual void Update(float, float);  // Handles movement logic
    virtual void Display();    // Renders all layers
    void MoveLeft(float, float);
    void MoveRight(float, float );

    void SetPosition(float x, float y);
    void SetScrollSpeed(float);
    string GetType();

protected:
   
    void SaveCoordinates();
    string LoadCoordinates() const;
    //bool moveLeft;
    //bool moveRight;

    fs::path filepath;
    string type;
    std::shared_ptr<TextureManager> textureManager;
    

    struct Layer {
        Texture2D texture;
        float x;
        float y;
        float speedFactor;
    };

    std::vector< Layer> layers; // Ordered by name
    float scrollSpeed;            // Base scroll speed
    fs::path coordinatesPath;
};

