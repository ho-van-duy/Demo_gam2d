#include "TextureManager.h"
#include <iostream>

TextureManager& TextureManager::getInstance() {
    static TextureManager instance;
    return instance;
}

TextureManager::~TextureManager() {
    clearAll();
}

bool TextureManager::loadTexture(const std::string& id, const std::string& filepath,
                                  SDL_Renderer* renderer) {
    // Check if already loaded
    if (m_textures.find(id) != m_textures.end()) {
        std::cout << "[TextureManager] Texture '" << id << "' already loaded." << std::endl;
        return true;
    }

    SDL_Surface* surface = IMG_Load(filepath.c_str());
    if (!surface) {
        std::cerr << "[TextureManager] Failed to load image: " << filepath
                  << " - " << IMG_GetError() << std::endl;
        return false;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (!texture) {
        std::cerr << "[TextureManager] Failed to create texture from: " << filepath
                  << " - " << SDL_GetError() << std::endl;
        return false;
    }

    m_textures[id] = texture;
    std::cout << "[TextureManager] Loaded texture '" << id << "' from " << filepath << std::endl;
    return true;
}

void TextureManager::dropTexture(const std::string& id) {
    auto it = m_textures.find(id);
    if (it != m_textures.end()) {
        SDL_DestroyTexture(it->second);
        m_textures.erase(it);
    }
}

void TextureManager::clearAll() {
    for (auto& pair : m_textures) {
        SDL_DestroyTexture(pair.second);
    }
    m_textures.clear();
}

SDL_Texture* TextureManager::getTexture(const std::string& id) {
    auto it = m_textures.find(id);
    if (it != m_textures.end()) {
        return it->second;
    }
    std::cerr << "[TextureManager] Texture '" << id << "' not found!" << std::endl;
    return nullptr;
}

void TextureManager::draw(const std::string& id, int x, int y, int width, int height,
                           SDL_Renderer* renderer, SDL_RendererFlip flip) {
    SDL_Texture* tex = getTexture(id);
    if (!tex) return;

    SDL_Rect srcRect = {0, 0, width, height};
    SDL_Rect destRect = {x, y, width, height};

    // Get actual texture size for source rect
    SDL_QueryTexture(tex, nullptr, nullptr, &srcRect.w, &srcRect.h);

    SDL_RenderCopyEx(renderer, tex, &srcRect, &destRect, 0, nullptr, flip);
}

void TextureManager::drawFrame(const std::string& id, int x, int y, int width, int height,
                                int row, int frame, SDL_Renderer* renderer,
                                SDL_RendererFlip flip) {
    SDL_Texture* tex = getTexture(id);
    if (!tex) return;

    SDL_Rect srcRect = {width * frame, height * row, width, height};
    SDL_Rect destRect = {x, y, width, height};

    SDL_RenderCopyEx(renderer, tex, &srcRect, &destRect, 0, nullptr, flip);
}

void TextureManager::drawTile(const std::string& id, int tileSize, int x, int y,
                               int row, int frame, SDL_Renderer* renderer) {
    SDL_Texture* tex = getTexture(id);
    if (!tex) return;

    SDL_Rect srcRect = {tileSize * frame, tileSize * row, tileSize, tileSize};
    SDL_Rect destRect = {x, y, tileSize, tileSize};

    SDL_RenderCopy(renderer, tex, &srcRect, &destRect);
}

void TextureManager::drawEx(const std::string& id, int x, int y, int width, int height,
                              int row, int frame, double angle, SDL_Point* center,
                              SDL_Renderer* renderer, SDL_RendererFlip flip) {
    SDL_Texture* tex = getTexture(id);
    if (!tex) return;

    SDL_Rect srcRect = {width * frame, height * row, width, height};
    SDL_Rect destRect = {x, y, width, height};

    SDL_RenderCopyEx(renderer, tex, &srcRect, &destRect, angle, center, flip);
}

void TextureManager::drawFrameScaled(const std::string& id, int x, int y, int destWidth, int destHeight,
                                     int srcWidth, int srcHeight, int row, int frame,
                                     SDL_Renderer* renderer, SDL_RendererFlip flip) {
    SDL_Texture* tex = getTexture(id);
    if (!tex) return;

    SDL_Rect srcRect = {srcWidth * frame, srcHeight * row, srcWidth, srcHeight};
    SDL_Rect destRect = {x, y, destWidth, destHeight};

    SDL_RenderCopyEx(renderer, tex, &srcRect, &destRect, 0, nullptr, flip);
}

SDL_Texture* TextureManager::createColorTexture(SDL_Renderer* renderer, int w, int h,
                                                  Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormat(0, w, h, 32, SDL_PIXELFORMAT_RGBA8888);
    if (!surface) return nullptr;

    SDL_FillRect(surface, nullptr, SDL_MapRGBA(surface->format, r, g, b, a));
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (texture) {
        SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    }
    return texture;
}
