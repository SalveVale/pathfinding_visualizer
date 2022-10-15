#pragma once

#include <SFML/Graphics.hpp>

const int BOX_SIZE = 20;

class Tile {
public:
  Tile(int x, int y) {
    this->coords[0] = x;
    this->coords[1] = y;
    this->index = (10 * x) + y;
    this->box.setSize(sf::Vector2f(BOX_SIZE, BOX_SIZE));
    this->box.setPosition(sf::Vector2f(500 + x*BOX_SIZE, y*BOX_SIZE));
    this->box.setFillColor(sf::Color(80, 80, 80, 255));
    this->box.setOutlineThickness(1);
    this->box.setOutlineColor(sf::Color::Black);
  }
  ~Tile();
  
  enum states {
    empty,
    hovered,
    wall,
    start,
    end,
    visited,
    path
  } state = empty;
  
  sf::RectangleShape getBox() { return this->box; }
  
  int getCoords(int key) { return this->coords[key]; }
  
  void setState(states newState) {
    this->state = newState;
    
    switch (this->state)
    {
      case empty:
        this->box.setFillColor(sf::Color(80, 80, 80, 255));
        break;
      case hovered:
        this->box.setFillColor(sf::Color(55, 55, 55, 255));
        break;
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
  void setState(int newState) {
    switch (newState)
    {
      case 0:
        this->state = empty;
        this->box.setFillColor(sf::Color(80, 80, 80, 255));
        break;
      case 1:
        this->state = start;
        this->box.setFillColor(sf::Color::Green);
        break;
      case 2:
        this->state = end;
        this->box.setFillColor(sf::Color::Red);
        break;
      case 3:
        this->state = wall;
        this->box.setFillColor(sf::Color(12, 12, 12, 255));
        break;
    }
  }
  
  states getState() { return this->state; }
  
  int getIndex() { return this->index; }
  
  bool containsMouse(sf::Vector2f mousePos) { return this->box.getGlobalBounds().contains(mousePos); }

private:
  sf::RectangleShape box;
  int value;
  int coords[2];
  int index;
  // Tile *prevTile;
};
