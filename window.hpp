#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window/VideoMode.hpp>

#include "tile.hpp"

class StateEngine {
public:
  StateEngine();
  ~StateEngine();

  enum states {
    welcome,
    build,
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
    
    // for (int i=0; i<20; i++)
    // {
    //   for (int j=0; j<20; j++)
    //   {
    //     delete this->grid[i][j];
    //     this->grid[i][j] = 0;
    //   }
    // }
  }

  const bool isOpen() const {
    return this->window->isOpen();
  }

  void update() {
    this->pollEvents();
    this->updateMouse();
    this->updateTiles();
  }
  
  void render() {
    this->window->clear();
    this->renderBoxes();
    this->window->display();
  }
  
private:
  //variables
  sf::RenderWindow *window;
  sf::VideoMode videoMode;
  sf::Event event;
  Tile *grid[20][20];
  
  //mouse
  sf::Vector2i mousePosView;
  sf::Vector2f mousePosWindow;
  

  void initVariables() {
    this->window = nullptr;
    // StateEngine stateEngine;
    
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
            if (currentState == Tile::empty || currentState == Tile::hovered) currentTile->setState(Tile::wall);
          }
          else if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
          {
            if (currentState == Tile::wall) currentTile->setState(Tile::hovered);
          }
          else
          {
            currentTile->setState(Tile::hovered);
          }
        }
        else
        {
          if (currentState == Tile::hovered) currentTile->setState(Tile::empty);
        }
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

