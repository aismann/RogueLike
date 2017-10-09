#include "Room.h"
#include "Field.h"
#include "Common.h"
#include "Types.h"
#include "Player.h"
#include "Enemy.h"

#include <fstream>

Room* current_room = NULL;

Field* Room::getField(int x, int y)
{
	try
	{
		return map_.at(y).at(x);
	}
	catch (std::exception e)
	{
		cout << "Invalid Field!" << endl;
		std::cout << e.what();
	}
	return NULL;
}

Field* Room::getField(Position pos)
{
	try
	{
		return map_.at(pos.y_).at(pos.x_);
	}
	catch (std::exception e)
	{
		cout << "Invalid Field!" << endl;
		std::cout << e.what();
	}
	return NULL;
}


bool Room::stepOn(Position to, Character* who, Position& new_pos)
{
	Position old_pos = who->getPosition();
	Field* new_field = getField(to.x_, to.y_);
	Field* old_field = getField(old_pos.x_, old_pos.y_);
	Room* old_room = current_room;

	/*
	for (auto line : map_)
	{
		for (auto field : line)
			cout << field->status_;
		cout << endl;
	}
	*/

	if (new_field == NULL)
	{
		cout << "new_field == NULL!" << endl;
		return false;
	}

	cout << "Field status: " << new_field->status_ << endl;

	switch(new_field->status_)
	{
		case SOLID:
		return false;

		case FREE:
		old_field->free();
		new_field->occupy(who);
		new_pos = to;
		return true;

		case OCCUPIED:
		who->attack(new_field->getCharacter());
		new_pos = who->getPosition();
		return true;

		case TRIGGER:
			old_field->free();
			new_pos = new_field->trigger(who);
			new_field = current_room->getField(new_pos);
			new_field->occupy(who);
			if (current_room == old_room)
				return true;
			else
				return false; //register as non valid move so enemies dont move as well

		case PICKUP:
		old_field->free();
		new_field->occupy(who);
		new_field->pickUpItem(who);
		new_pos = to;
		return true;
	}
	return false;
}

struct Node
{
	Position pos;
	bool visited = false;
	size_t distance = std::numeric_limits<size_t>::max();
	Node* prev = NULL;
};

std::ostream& operator<<(std::ostream& out, const Node& node)
{
	out << node.pos << "[" << node.visited << "], " << node.distance << endl;
	return out;
};

//--- returns path from "from" to "to" (backwards!!) ---//
vector<Position> Room::getShortestPath(Position from, Position to)
{
	cout << "//-- Path finding start --//" << endl;
	try
	{
		size_t width = getColCount();
		size_t height = getRowCount();
		vector<vector<Node>> nodes(height);

		size_t x;
		size_t y;
		for (y = 0; y < height; y++)
		{
			nodes.at(y).resize(width);
			for (x = 0; x < width; x++)
			{
				Node* node = &nodes.at(y).at(x);
				node->pos = Position(x, y);
			}
		}

		static Position delta[4] = { Position(0, -1), Position(1, 0), Position(0, 1), Position(-1, 0) }; //order to look for neighbours

		//-- init --//
		Node* start = &nodes.at(from.y_).at(from.x_);
		start->distance = 0;

		while (true)
		{
			/*
			for (auto& line : nodes)
				for (auto& node : line)
					cout << node;
			*/


			//-- find node with shortest distance to from--//
			size_t shortest_distance = std::numeric_limits<size_t>::max();
			Node* new_node = NULL;

			for (y = 0; y < nodes.size(); y++)
			{
				for (x = 0; x < nodes.at(y).size(); x++)
				{
					Node* cur = &nodes.at(y).at(x);
					if (cur->visited)
					{
						continue;
					}
					else if (getField(x, y)->status_ == SOLID)
					{
						cur->visited = true;
					}
					else if (cur->distance < shortest_distance)
					{
						shortest_distance = cur->distance;
						new_node = cur;
					}
				} //for x
			} //for y

			if (new_node == NULL)
			{
				cout << "[Error] No path found!" << endl;
				return vector<Position>();
			}

			new_node->visited = true;

			//-- target reached; building path backwards --//
			if (new_node->pos == to)
			{
				cout << "Path found!" << endl;
				vector<Position> path;
				while (new_node->prev != NULL)
				{
					path.push_back(new_node->pos);
					new_node = new_node->prev;
				}
				for (auto pos : path)
					cout << pos;
				cout << endl;
				return path;
			}
			 
			//-- update distance for neighbours --//
			Position tmp;
			for (size_t i = 0; i < 4; i++)
			{
				tmp = new_node->pos + delta[i];
				if (tmp.x_ < 0)
					tmp.x_ = 0;
				if (tmp.y_ < 0)
					tmp.y_ = 0;
				Node* node = &nodes.at(tmp.y_).at(tmp.x_);
				if (!node->visited)
				{
					node->distance = new_node->distance + 1;
					node->prev = new_node;
				}
			}
		} //while

		return vector<Position>();
	}
	catch (std::out_of_range& e)
	{
		cout << "Path finding error" << endl;
		cout << e.what();
	}
}

void Room::updateDistanceToPlayer()
{
	cout << "Updating distance to player:" << endl;
	Position player_pos = current_player->getPosition();
	for (auto line : map_)
	{
		for (auto field : line)
		{
			field->distance_to_player_ = abs(field->pos_.x_ - player_pos.x_) + abs(field->pos_.y_ - player_pos.y_);
			//printf_s("%3zd", field->distance_to_player_);
		}
		cout << endl;
	}
}

void Room::freeField(Position pos)
{
	getField(pos.x_, pos.y_)->free();
}

void Room::occupyField(Position pos, Character* who)
{
	getField(pos.x_, pos.y_)->occupy(who);
}

void Room::placeItem(Position pos, Item* item)
{
	getField(pos.x_, pos.y_)->placeItem(item);
}

Room::Room()
{
	
}

Room::Room(const char * filename, Position pos) : pos_(pos)
{
	if(!readRoomFromFile(filename))
	{
		cout << "[Error] Room could not be loaded!" << endl;
		return;
	}

	cout << "Tile Nrs:" << endl;
	for(auto row : map_)
	{
		for(auto field : row)
			cout << field->getTileNr();
		cout << endl;
	}
	tile_map_ = new TileMap();
	tile_map_->load("../images/tileset.png", map_, TILE_SIZE, (int)getColCount(), (int)getRowCount());
}

bool Room::readRoomFromFile(const char * filename)
{
	cout << "Reading Room " << filename << " from file..." << endl;
	name = filename;
	std::fstream file(filename);
	if (!file.is_open())
		return false;

	Row row;
	char c;

	cout << "File opened successfully!" << endl;
	bool reading = true;
	Position pos = { 0,0 };
	while (reading)
	{
		c = file.get();
		Field* field = NULL;

		switch (c)
		{
		case '/':
			map_.push_back(row);
			reading = false;
			continue;
			break;

		case '#':
			field = new Wall(pos);
			break;

		case ' ':
			field = new Floor(pos);
			break;

		case '*':
			field = new Tree(pos);
			break;

		case '%':
			field = new Water(pos);
			break;

		case 'X':
			field = new Lava(pos);
			break;

		case '^':
			field = new Door(pos, UP);
			entries_[UP] = (pos + Position(0,1));
			break;

		case '>':
			field = new Door(pos, RIGHT);
			entries_[RIGHT] = (pos + Position(-1,0));
			break;

		case 'v':
			field = new Door(pos, DOWN);
			entries_[DOWN] = (pos + Position(0,-1));
			break;

		case '<':
			field = new Door(pos, LEFT);
			entries_[LEFT] = (pos + Position(1,0));
			break;

		case '\n':
			map_.push_back(row);
			row.clear();
			field = NULL;
			pos.x_ = 0;
			pos.y_++;
			break;

		default:
			cout << "[Error] Unknown Symbol!" << endl;
			break;
		}

		if (field)
		{
			pos.x_++;
			row.push_back(field);
		}
		cout << c;
		//cout << pos << endl;
	}
	cout << endl;
	return true;
}

void Room::draw(sf::RenderWindow& window)
{
	window.draw(*tile_map_);
	for (auto row : map_)
	{
		for (auto field : row)
		{
			field->draw(window);
		}
	}
	for(auto enemy : enemies_)
	{
		enemy->draw(window);
	}
}

void Room::addField(Field* field)
{
	unsigned int x = field->pos_.x_;
	unsigned int y = field->pos_.y_;

	if (y >= map_.size() || x >= map_.at(y).size())
	{
		cout << "Can't add a field there!" << endl;
		return;
	}

	delete map_.at(y).at(x);
	map_.at(y).at(x) = field;

}

Position Room::getEntryPosition(Direction entry)
{
	return entries_[entry];
}

void Room::addEntryPosition(Direction dir, Position pos)
{
	entries_[dir] = pos;
}

void Room::movePlayerToDoor(Direction entry)
{
	//current_player->pos_ = doors_[entry];
}

void Room::addEnemy(Enemy* enemy)
{
	enemies_.push_back(enemy);
}

void Room::removeEnemy(Enemy* enemy)
{
	enemies_.remove(enemy);
	//dead_enemies_.push_back(enemy);
}

void Room::stepEnemies() //called appr. 63 times a second
{
	vector<Enemy*> dead_enemies;
	cout << "Room::stepEnemies()" << endl;
	for(Enemy* enemy : enemies_)
	{
		if(!enemy->step())
		{
			dead_enemies.push_back(enemy);
		}
	}
	for(auto enemy : dead_enemies)
	{
		enemies_.remove(enemy);
		delete enemy;
	}
	cout << endl;
}