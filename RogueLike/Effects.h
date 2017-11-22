#pragma once

#include "Common.h"
#include "Types.h"
#include "Animation.h"

class Effect
{
public:
    static void drawEffects(sf::RenderWindow& window);
    static void addEffect(Effect* e);
    static size_t getEffectCount() { return effects_.size(); }
protected:
    static list<Effect*> effects_;

    Effect(string filename);
    virtual void draw(sf::RenderWindow& window) = 0;
    sf::Texture tex_;
    sf::Sprite sprite_;
    bool active_ = true;
    Animation anim_;
};

class MovingEffect : public Effect
{
public:
    MovingEffect(string filename, sf::Vector2f from, sf::Vector2f to, float speed = 1);
private:
    void draw(sf::RenderWindow& window);
    sf::Vector2f step_;
    size_t dur_;
};


