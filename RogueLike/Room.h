#pragma once

#include "Field.h"
#include "Types.h"
#include "TileMap.h"

#include "Common.h"

class Enemy;
class TileMap;
class Item;
class Room
{
	friend class Dungeon;

	private:
		TileMap* tile_map_;
		Map map_; //Fields
		Field* getField(int x, int y);
		string name;
		Position pos_; //Position in Dungeon

		Position entries_[4];

		list<Enemy*> enemies_;


	public:
		std::string getName() { return name; };
		size_t getColCount() { return map_.front().size(); };
		size_t getRowCount() { return map_.size(); };

		vector<Position> getShortestPath(Position from, Position to);
		Position stepOn(Position new_pos, Character* who); //returns new Position of Character
		void freeField(Position pos);
		void occupyField(Position pos, Character* who);
		void placeItem(Position pos, Item* item, size_t count);

		Room();
		Room(const char* filename, Position pos = {0,0});

		bool readRoomFromFile(const char* filename);
		void draw(sf::RenderWindow& window);
		void addField(Field * field);

		Position getEntryPosition(Direction entry);
		void     addEntryPosition(Direction dir, Position pos);

		void movePlayerToDoor(Direction entry);

		void addEnemy(Enemy* enemy);
		void removeEnemy(Enemy* enemy);
		void stepEnemies();
};

extern Room* current_room;