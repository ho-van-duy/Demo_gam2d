#ifndef TEXTURE_MANAGER_H
#define TEXTURE_MANAGER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>
#include <unordered_map>

class TextureManager {
public:
    static TextureManager& getInstance();

    // Texture management
    bool loadTexture(const std::string& id, const std::string& filepath, SDL_Renderer* renderer);
    void dropTexture(const std::string& id);
    void clearAll();

    SDL_Texture* getTexture(const std::string& id);

    // Drawing functions
    void draw(const std::string& id, int x, int y, int width, int height,
              SDL_Renderer* renderer, SDL_RendererFlip flip = SDL_FLIP_NONE);

    void drawFrame(const std::string& id, int x, int y, int width, int height,
                   int row, int frame, SDL_Renderer* renderer,
                   SDL_RendererFlip flip = SDL_FLIP_NONE);

    void drawTile(const std::string& id, int tileSize, int x, int y,
                  int row, int frame, SDL_Renderer* renderer);

    void drawEx(const std::string& id, int x, int y, int width, int height,
                int row, int frame, double angle, SDL_Point* center,
                SDL_Renderer* renderer, SDL_RendererFlip flip = SDL_FLIP_NONE);

    void drawFrameScaled(const std::string& id, int x, int y, int destWidth, int destHeight,
                         int srcWidth, int srcHeight, int row, int frame,
                         SDL_Renderer* renderer, SDL_RendererFlip flip = SDL_FLIP_NONE);

    // Create colored texture (for placeholder/debug)
    SDL_Texture* createColorTexture(SDL_Renderer* renderer, int w, int h,
                                     Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255);

private:
    TextureManager() = default;
    ~TextureManager();
    TextureManager(const TextureManager&) = delete;
    TextureManager& operator=(const TextureManager&) = delete;

    std::unordered_map<std::string, SDL_Texture*> m_textures;
};

#endif // TEXTURE_MANAGER_H
