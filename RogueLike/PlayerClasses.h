#pragma once

#include "Common.h"
#include "Types.h"
#include "Player.h"

class Warrior : public Player
{
private:
    static Stats base_stats_;
public:
    Warrior(string name, Position pos);
    void attack(Character* target);
};

class Mage : public Player
{
private:
    static Stats base_stats_;
public:
    Mage(string name, Position pos);
    void attack(Character* target);
};

class Thief : public Player
{
private:
    static Stats base_stats_;
public:
    Thief(string name, Position pos);
    void attack(Character* target);
};





