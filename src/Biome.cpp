//
// Created by oliver on 03/03/18.
//

#include "Biome.h"

namespace Biomes
{
    Biome::Biome(const string const& name, int temp, int hum, EFloorType floor) : name_(name), temp_(temp), hum_(hum), floor_(floor)
    {

    }

    Biome::Biome(Biome* orig) : name_(orig->name_), temp_(orig->temp_), hum_(orig->hum_), floor_(orig->floor_)
    {

    }

    //------------------------------------------------------------

    template <typename T>
    Attribute<T>::Attribute(T value) : value_(value)
    {

    }

    template <typename T>
    bool Attribute<T>::compare(EComparison comp, Attribute<T> other) const
    {
        switch (comp)
        {
            case SMALLER: return value_ < other.value_;
            case EQUAL: return value_ == other.value_;
            case BIGGER: return value_ > other.value_;
        }
    }

    //------------------------------------------------------------

    Temperature::Temperature(int value) : Attribute(value)
    {

    }

    Humidity::Humidity(int value) : Attribute(value)
    {

    }

    FloorType::FloorType(EFloorType value) : Attribute(value)
    {

    }

    //------------------------------------------------------------

    bool Biome::satisfies(Condition<Temperature> cond)
    {
        return temp_.compare(cond.eq_, cond.attribute_);
    }

    bool Biome::satisfies(Condition<Humidity> cond)
    {
        return hum_.compare(cond.eq_, cond.attribute_);
    }

    bool Biome::satisfies(Condition<FloorType> cond)
    {
        return floor_.compare(cond.eq_, cond.attribute_);
    }
}

template <>
Factory<Biomes::Biome>::Factory()
{
    const char* filename = "data/biomes.xml";
    std::ifstream file(filename);
    if(!file.is_open())
    {
        cout << "Factory<Biomes::Biome> failed to open file " << filename << endl;
        return;
    }
    //-----------------------------------------------------------------------------------
    cout << "Loading in biomes..." << endl;
    xml_document doc;
    xml_parse_result res = doc.load(file);

    if(!res)
    {
        cout << "Parsing of " << filename << " failed:" << endl;
        cout << res << endl;
    }
    //-----------------------------------------------------------------------------------
    auto enemies = doc.child("biomes");

    for(auto biome : enemies.children())
    {
        string name = biome.attribute("name").value();

        if (name == "Template")
            continue;

        entity_names_.push_back(name);

        cout << separator << endl;
        cout << "Biome: " << name << endl;
        string texture = biome.attribute("texture").value();
        //-----------------------------------------------------------------------------------
        auto attributes_node = biome.child("attributes");
        int temperature = std::stoi(attributes_node.child("temperature").child_value());
        cout << "Temperature: " << temperature << endl;
        int humidity    = std::stoi(attributes_node.child("humidity").child_value());
        cout << "Humidity: " << humidity << endl;
        auto floor = static_cast<Biomes::EFloorType>(std::stoi(attributes_node.child("floor").child_value()));
        cout << "Floor type: " << floor << endl;
        //-----------------------------------------------------------------------------------

        entity_templates_[name] = std::make_unique<Biomes::Biome>(name, temperature, humidity, floor);
        cout << separator << endl << endl;
    }
}
