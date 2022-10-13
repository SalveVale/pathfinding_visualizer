#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window/VideoMode.hpp>

#include "tile.hpp"

#include <iostream>

class StateEngine {
public:
  StateEngine();
  ~StateEngine();

  enum states {
    welcome,
    build,
    dropdownSelection,
    solve
  } state = welcome;

  void setState(states newState) { this->state = newState; }
};


class Window {
public:
  Window() {
    this->initVariables();
    this->initWindow();
  }
  ~Window() {
    delete this->window;
  }

  const bool isOpen() const {
    return this->window->isOpen();
  }

  void update() {
    this->pollEvents();
    this->updateMouse();
    this->updateUI();
    this->updateTiles();
  }
  
  void render() {
    this->window->clear();
    
    this->renderUI();

    //grid
    this->renderBoxes();

    this->window->display();
  }
  
  sf::RenderWindow *getWindow() { return this->window; }
  
private:
  //variables
  sf::RenderWindow *window;
  sf::VideoMode videoMode;
  sf::Event event;
  Tile *grid[20][20];
  
  //ui
  sf::RectangleShape sliderOutline;
  sf::RectangleShape sliderBox;
  sf::RectangleShape highlightBox;
  
  //mouse
  sf::Vector2i mousePosView;
  sf::Vector2f mousePosWindow;
  

  void initVariables() {
    this->window = nullptr;
    // StateEngine stateEngine;
    
    //ui
    this->sliderOutline.setPosition(sf::Vector2f(30, 15));
    this->sliderOutline.setSize(sf::Vector2f(400, 30));
    this->sliderOutline.setFillColor(sf::Color(30, 20, 20, 255));
    this->sliderOutline.setOutlineColor(sf::Color::White);
    this->sliderOutline.setOutlineThickness(1);
    
    this->sliderBox.setPosition(sf::Vector2f(35, 15));
    this->sliderBox.setSize(sf::Vector2f(20, 20));
    this->sliderBox.setFillColor(sf::Color::White);
    
    // this->highlightBox.setPosition(sf::Vector2f(-100, -100));
    this->highlightBox.setSize(sf::Vector2f(50, 50));
    this->highlightBox.setFillColor(sf::Color(0, 0, 0, 100));
    
    int index = 0;
    for (int i=0; i<20; i++)
    {
      for (int j=0; j<20; j++)
      {
        this->grid[i][j] = new Tile(i, j);
      }
    }
  }
  
  void initWindow() {
    this->videoMode.height = 1000;
    this->videoMode.width = 1500;
    
    this->window = new sf::RenderWindow(this->videoMode, "Pathfinding Visualizer");
    this->window->setFramerateLimit(60);
  }
  
  void pollEvents() {
    while (this->window->pollEvent(this->event))
    {
      switch (this->event.type)
      {
        case sf::Event::Closed:
          this->window->close();
          break;
        case sf::Event::KeyPressed:
          if (this->event.key.code == sf::Keyboard::Escape)
            this->window->close();
        default: break;
      }
    }
  }
  
  void updateMouse() {
    this->mousePosView = sf::Mouse::getPosition(*this->window);
    this->mousePosWindow = this->window->mapPixelToCoords(this->mousePosView);
  }
  
  void updateUI() {
    if (this->sliderBox.getGlobalBounds().contains(this->mousePosWindow) && sf::Mouse::isButtonPressed(sf::Mouse::Left))
    {
      this->sliderBox.setPosition(sf::Vector2f(this->mousePosWindow.x, 15));
      std::cout << this->sliderBox.getPosition().x << std::endl;
    }
  }
  
  void renderUI() {
    this->window->draw(this->sliderOutline);
    this->window->draw(this->sliderBox);
  }
  
  void updateTiles() {
    for (int i=0; i<20; i++)
    {
      for (int j=0; j<20; j++)
      {
        Tile *currentTile = this->grid[i][j];
        Tile::states currentState = currentTile->state;
        if (currentTile->containsMouse(this->mousePosWindow))
        {
          if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
          {
            if (currentState == Tile::empty) currentTile->setState(Tile::wall);
            // else if (currentState == Tile::start || currentState == Tile::end)
          }
          else if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
          {
            if (currentState == Tile::wall) currentTile->setState(Tile::empty);
          }
          else
          {
            this->highlightBox.setPosition(sf::Vector2f(currentTile->getBox().getPosition()));
          }
        }
        // else
        // {
        //   if (currentState == Tile::hovered) currentTile->setState(Tile::empty);
        // }
      }
    }
  }

  void renderBoxes() {
    for (int i=0; i<20; i++)
    {
      for (int j=0; j<20; j++)
      {
        this->window->draw(this->grid[i][j]->getBox());
      }
    }
  }
};
