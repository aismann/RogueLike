#include <sstream>
#include "UI.h"
#include "Types.h"
#include "Common.h"
#include "Room.h"
#include "Player.h"
#include "Textbox.h"
#include "Minimap.h"
#include "Ressource.h"
#include "Abilities.h"

Textbox* UI::textbox_ = NULL;
sf::Font UI::Text::default_font_;

//-------------------------------------------------------------------------------------------------
UI::Text::Text(float x, float y, string content, unsigned int size, sf::Color color)
{
    text_.setPosition(x,y);
    text_.setCharacterSize(size);
    text_.setColor(color);
    text_.setString(content);
}

void UI::Text::setFont(sf::Font &font)
{
    text_.setFont(font);
    font_loaded_ = true;
}

void UI::Text::draw(sf::RenderWindow& window)
{
    if(!font_loaded_)
    {
        setFont(default_font_);
    }

    window.draw(text_);
}
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
UI::Bar::Bar(sf::Vector2f pos, sf::Vector2f size, Ressource const& ress, sf::Color bar_color) :
        size_(size), ressource_(ress), background_(size), bar_(size)
{
    background_.setPosition(pos);
    background_.setFillColor(sf::Color(150, 150, 150));
    bar_.setPosition(pos);
    bar_.setFillColor(bar_color);

    //--- Text ---//
    text_cur_.setPosition(pos);
    text_cur_.setCharacterSize(15);
    text_max_.setPosition(pos + sf::Vector2f(size.x - 30, 0));
    text_max_.setCharacterSize(15);
}

void UI::Bar::draw(sf::RenderWindow& window)
{
    bar_.setSize(sf::Vector2f(ressource_.relative() * size_.x, size_.y));
    window.draw(background_);
    window.draw(bar_);
    text_cur_.setString(std::to_string(ressource_.cur()));
    text_max_.setString(std::to_string(ressource_.max()));
    window.draw(text_cur_);
    window.draw(text_max_);
}
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
UI::CharacterInfo::~CharacterInfo()
{
    delete bar1_;
    delete bar2_;
    delete bar3_;
}

void UI::CharacterInfo::draw(sf::RenderWindow & window)
{
    title_.setString(source_->getName() + ", Lv " + std::to_string(source_->getLevel()) + " " + source_->getClassName());
    window.draw(title_);

    if(bar1_ != nullptr)
    {bar1_->draw(window);}
    if (bar2_ != nullptr)
    {bar2_->draw(window);}
    if (bar3_ != nullptr)
    {bar3_->draw(window);}
}

UI::CharacterInfo::CharacterInfo(sf::Vector2f pos, Character * source) : source_(source), pos_(pos)
{
    title_.setPosition(pos_);
    title_.setCharacterSize(16);
    bar1_ = new Bar(pos_ + sf::Vector2f(0, 20), { 200, 20 }, source_->getHp());
    bar2_ = new Bar(pos_ + sf::Vector2f(0, 40), { 200, 20 }, source_->getMana(), sf::Color::Blue);
    bar3_ = new Bar(pos_ + sf::Vector2f(0, 60), { 200, 20 }, source_->getExp(), sf::Color(200,0,200));

}
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
UI::UI() : player_info_({500,300}, current_player),
           abilities_(15, 450, "Abilities:", 20)
{
	//-- load default font --//
	if (!font_.loadFromFile("../fonts/8bitOperatorPlus-Regular.ttf"))
		cout << "Failed to load UI font!" << endl;

    Text::setDefaultFont(font_);

	//-- set font --//
	stat_names_.setFont(font_);
	stat_values_.setFont(font_);
	player_info_.title_.setFont(font_);
	player_info_.bar1_->text_cur_.setFont(font_);
	player_info_.bar2_->text_cur_.setFont(font_);
	player_info_.bar3_->text_cur_.setFont(font_);
	player_info_.bar1_->text_max_.setFont(font_);
	player_info_.bar2_->text_max_.setFont(font_);
	player_info_.bar3_->text_max_.setFont(font_);

	//-- set size --//
	stat_names_.setCharacterSize(font_size_);
	stat_values_.setCharacterSize(font_size_);
	
	
	//-- set Positions --//
	pos_inv_   =       { 23 * TILE_SIZE + 10, 0                  };
	pos_stats_ =       { 23 * TILE_SIZE + 14, 200                };
	pos_bottom_text_ = { 10                 , TILE_SIZE * 9 + 10 };

	stat_names_.setPosition(pos_stats_.x_, pos_stats_.y_);
	stat_values_.setPosition(pos_stats_.x_ + 150, pos_stats_.y_);


	//-- set fixed Text --//
	stat_names_.setString("Strength\n"
						  "Endurance\n"
						  "Dexterity\n"
						  "Intelligence\n"
						  "Willpower\n");

	textbox_ = new Textbox(5, 20, font_, 
		    pos_bottom_text_, 420, 120,
			5, 5, sf::Color(100, 100, 100), sf::Color(50,50,50));
			
}

UI::~UI()
{
	delete textbox_;
}

void UI::draw(sf::RenderWindow& window)
{
	//-- draw inventory --//
	current_player->getInventory()->draw(window, pos_inv_);

	//-- draw stats --//
	window.draw(stat_names_);

	Stats stats = current_player->getStats();
	string buffer =
					std::to_string(stats.str_)  + '\n' +
					std::to_string(stats.end_)  + '\n' +
					std::to_string(stats.dex_)  + '\n' +
					std::to_string(stats.int_)  + '\n' +
					std::to_string(stats.will_) + '\n';
	stat_values_.setString(buffer.c_str());
	window.draw(stat_values_);
	textbox_->draw(window);
	player_info_.draw(window);
	Minimap::draw(window);

    //-- Draw Abilites --//
    std::stringstream s;
    s << "Abilities:\n";
    vector<Ability*> player_abilities = current_player->getAbilities();

    size_t ab_nr = 1;
    for(Ability* ability : player_abilities)
    {
        if(ability == nullptr)
            continue;

        s << ab_nr++ << " - " << ability->getName() << '\n';
    }
    abilities_.setString(s.str());
    abilities_.draw(window);
}

void UI::displayText(string text)
{
	textbox_->displayText(text);
}
//-------------------------------------------------------------------------------------------------
