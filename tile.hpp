#include <SFML/Graphics.hpp>

const int BOX_SIZE = 50;
const int PADDING =  500;

class Tile {
public:
  Tile(int x, int y) {
    this->box.setSize(sf::Vector2f(BOX_SIZE, BOX_SIZE));
    this->box.setPosition(sf::Vector2f(PADDING + x*BOX_SIZE, y*BOX_SIZE));
    this->box.setOutlineThickness(1);
    this->box.setOutlineColor(sf::Color::Black);
  }
  ~Tile();
  
  enum states {
    empty,
    // hovered,
    wall,
    start,
    end,
    visited,
    path
  } state = empty;
  
  sf::RectangleShape getBox() { return this->box; }
  
  void setState(states newState) {
    this->state = newState;
    
    switch (this->state)
    {
      case empty:
        this->box.setFillColor(sf::Color(155, 155, 155, 255));
        break;
      // case hovered:
      //   this->box.setFillColor(sf::Color(55, 55, 55, 255));
      //   break;
      case wall:
        this->box.setFillColor(sf::Color(12, 12, 12, 255));
        break;
      case start:
        this->box.setFillColor(sf::Color::Green);
        break;
      case end:
        this->box.setFillColor(sf::Color::Red);
        break;
      case visited:
        this->box.setFillColor(sf::Color::Blue);
        break;
      case path:
        this->box.setFillColor(sf::Color::Yellow);
        break;
    }
  }
  
  bool containsMouse(sf::Vector2f mousePos) { return this->box.getGlobalBounds().contains(mousePos); }

private:
  sf::RectangleShape box;
  // int index;
  // Tile *prevTile;
};
