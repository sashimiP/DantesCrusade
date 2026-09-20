#include "Background.h"


Background::Background(std::shared_ptr<TextureManager> textureManager, float scrollSpeed, fs::path coordinatesPath) :

    textureManager(textureManager),
    scrollSpeed(scrollSpeed),
    coordinatesPath(coordinatesPath/"coordinates.txt")
{
    type = LoadCoordinates();
    LoadLayers(type);
}

Background::~Background() {
    std::cout << "BACKGROUND DESTRUCTOR CALLED" << std::endl;
    cout << type << endl;
    SaveCoordinates();
}

void Background::SaveCoordinates() {
    std::ofstream coordinates(coordinatesPath); // RAII: no need for manual close()
    if (!coordinates) {
        std::cerr << "Failed to save coordinates to " << coordinatesPath << std::endl;
        return;
    }
    coordinates << type << "\n";
    //std::cout << "BACKGROUND SAVED WITH:\ntype: " << type << std::endl;
}

std::string Background::LoadCoordinates() const {
    std::ifstream coordinates(coordinatesPath); // RAII: no need for manual close()
    std::string line;
    if (!coordinates) {
        std::cerr << "Failed to load coordinates from " << coordinatesPath << std::endl;
        return ""; // Default value
    }
    if (!std::getline(coordinates, line)) {
        std::cerr << "File is empty or corrupted: " << coordinatesPath << std::endl;
        return "";
    }
    //std::cout << "LOADING BACKGROUND TYPE: " << line << std::endl;
    return line;
}

void Background::LoadLayers(const string& type) 
{  

   this->type = type;
   cout << coordinatesPath << endl;

   const auto* textures = textureManager->BackgroundLayers(type);  
   if (!textures || textures->empty()) return;  
   layers.clear();
   layers.reserve(textures->size());
   float depth = 0.0f;  

   for (const auto& texture : *textures) {
       if (texture.id == 0) continue;

       layers.emplace_back(Layer{
           .texture = texture,
           .x = 0.0f,
           .y = 0.0f,
           .speedFactor = 1.0f + 0.2f + depth
           });

       depth += 0.3f;  
   }  
}

void Background::Update(float dt, float speed) {

    //for (auto& layer : layers) {
    //    layer.x -= speed * layer.speedFactor * dt;

    //    // Smooth loop without visual snapping
    //    if (layer.x <= -layer.texture.width) {
    //        layer.x += layer.texture.width;
    //    }
    //}

    for (auto& layer : layers) {
        layer.x = std::fmod(layer.x, (float)layer.texture.width);
        if (layer.x > 0) {
            layer.x -= layer.texture.width;
        }
    }

}

void Background::MoveRight(float speed, float dt)
{
    for (auto& layer : layers) {
        layer.x -= speed * layer.speedFactor * dt;

    }
}

void Background::MoveLeft(float speed,float dt)
{

    for (auto& layer : layers) {
        layer.x += speed * layer.speedFactor * dt;
    }

}

void Background::SetPosition(float x, float y)
{
    for (auto& layer : layers) {
        {
            layer.x = x;
            layer.y = y;
        }
    }
}


void Background::Display() {
    for (const auto&  layer : layers) {
        // Use DrawTextureEx for subpixel precision
        Vector2 pos1 = { layer.x, layer.y };
        Vector2 pos2 = { layer.x + (float)layer.texture.width, layer.y };

        DrawTextureEx(layer.texture, pos1, 0.0f, 1.0f, WHITE);
        DrawTextureEx(layer.texture, pos2, 0.0f, 1.0f, WHITE);
    }
}

void Background::SetScrollSpeed(float newSpeed)
{
    scrollSpeed = newSpeed;
}

string Background::GetType()
{
    return type;
}