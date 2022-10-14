#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window/VideoMode.hpp>

#include "tile.hpp"

#include <iostream>
#include <fstream>

class StateEngine {
public:
  // StateEngine();
  // ~StateEngine();

  enum states {
    welcome,
    build,
    dropdownSelection,
    solve
  } state = build;

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
    if (stateEngine.state == StateEngine::states::build)
    {
      this->updateMouse();
      this->updateUI();
      this->updateTiles();
    }
    else if (stateEngine.state == StateEngine::states::solve)
    {
      this->solve(this->algorithm);
    }
  }
  
  void render() {
    this->window->clear();
    
    this->renderBoxes();
    this->renderUI();

    this->window->display();
  }
  
  sf::RenderWindow *getWindow() { return this->window; }
  
private:
  StateEngine stateEngine;
  //variables
  sf::RenderWindow *window;
  sf::VideoMode videoMode;
  sf::Event event;
  Tile *grid[20][20];
  
  //ui
  sf::RectangleShape sliderOutline;
  sf::RectangleShape sliderBox;
  // sf::RectangleShape highlightBox;
  sf::RectangleShape algoDijkstraBox;
  sf::RectangleShape algoAStarBox;
  sf::RectangleShape algoOtherOneBox;
  sf::RectangleShape algoOtherTwoBox;
  sf::RectangleShape solveBox;
  sf::RectangleShape resetBox;
  
  const sf::Color colButton = sf::Color(30, 20, 20, 255);
  const sf::Color colButtonHighlight = sf::Color(40, 30, 30, 255);
  const sf::Color colButtonClick = sf::Color(50, 40, 40, 255);
  const sf::Color colButtonActive = sf::Color(60, 50, 50, 255);
  
  //mouse
  sf::Vector2i mousePosView;
  sf::Vector2f mousePosWindow;
  
  enum algorithms {
    dijkstra,
    aStar,
    otherOne,
    otherTwo
  } algorithm = dijkstra;
  

  void initVariables() {
    this->window = nullptr;
    
    //ui
    this->sliderOutline.setPosition(sf::Vector2f(30, 15));
    this->sliderOutline.setSize(sf::Vector2f(400, 30));
    this->sliderOutline.setFillColor(sf::Color(this->colButton));
    this->sliderOutline.setOutlineColor(sf::Color::White);
    this->sliderOutline.setOutlineThickness(1);
    
    this->sliderBox.setPosition(sf::Vector2f(35, 15));
    this->sliderBox.setSize(sf::Vector2f(20, 20));
    this->sliderBox.setFillColor(sf::Color::White);
    
    // this->highlightBox.setPosition(sf::Vector2f(-100, -100));
    // this->highlightBox.setSize(sf::Vector2f(50, 50));
    // this->highlightBox.setFillColor(sf::Color(100, 100, 100, 100));
    
    this->algoDijkstraBox.setPosition(sf::Vector2f(30, 400));
    this->algoDijkstraBox.setSize(sf::Vector2f(200, 40));
    this->algoDijkstraBox.setFillColor(sf::Color(this->colButton));
    this->algoDijkstraBox.setOutlineColor(sf::Color::White);
    this->algoDijkstraBox.setOutlineThickness(1);

    this->algoAStarBox.setPosition(sf::Vector2f(30, 440));
    this->algoAStarBox.setSize(sf::Vector2f(200, 40));
    this->algoAStarBox.setFillColor(sf::Color(this->colButton));
    this->algoAStarBox.setOutlineColor(sf::Color::White);
    this->algoAStarBox.setOutlineThickness(1);
    
    this->algoOtherOneBox.setPosition(sf::Vector2f(30, 480));
    this->algoOtherOneBox.setSize(sf::Vector2f(200, 40));
    this->algoOtherOneBox.setFillColor(sf::Color(this->colButton));
    this->algoOtherOneBox.setOutlineColor(sf::Color::White);
    this->algoOtherOneBox.setOutlineThickness(1);
    
    this->algoOtherTwoBox.setPosition(sf::Vector2f(30, 560));
    this->algoOtherTwoBox.setSize(sf::Vector2f(200, 40));
    this->algoOtherTwoBox.setFillColor(sf::Color(this->colButton));
    this->algoOtherTwoBox.setOutlineColor(sf::Color::White);
    this->algoOtherTwoBox.setOutlineThickness(1);
    
    this->solveBox.setPosition(sf::Vector2f(30, 800));
    this->solveBox.setSize(sf::Vector2f(200, 30));
    this->solveBox.setFillColor(sf::Color(this->colButton));
    this->solveBox.setOutlineColor(sf::Color::White);
    this->solveBox.setOutlineThickness(1);
    
    this->resetBox.setPosition(sf::Vector2f(30, 900));
    this->resetBox.setSize(sf::Vector2f(200, 30));
    this->resetBox.setFillColor(sf::Color(this->colButton));
    this->resetBox.setOutlineColor(sf::Color::White);
    this->resetBox.setOutlineThickness(1);
    
    for (int i=0; i<20; i++)
    {
      for (int j=0; j<20; j++)
      {
        this->grid[i][j] = new Tile(i, j);
      }
    }
    
    // std::string gridFile = "";
    // std::cout << "File to load grid from (leave blank to load default grid)\n> "; std::cin >> gridFile;
    // if (gridFile == "") this->loadGridFromFile("default_grid.txt");
    // else this->loadGridFromFile(gridFile);
    this->loadGridFromFile("default_grid.txt");
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
          if (this->event.key.code == sf::Keyboard::A)
            this->saveGridToFile("testfromwindow.txt");
          if (this->event.key.code == sf::Keyboard::A)
            this->loadGridFromFile("testgrid.txt");
        default: break;
      }
    }
  }
  
  void updateMouse() {
    this->mousePosView = sf::Mouse::getPosition(*this->window);
    this->mousePosWindow = this->window->mapPixelToCoords(this->mousePosView);
  }
  
  void updateUI() {
    if (this->sliderOutline.getGlobalBounds().contains(this->mousePosWindow) && sf::Mouse::isButtonPressed(sf::Mouse::Left))
    {
      if (this->mousePosWindow.x > 400 || this->mousePosWindow.x < 15) return;
      this->sliderBox.setPosition(sf::Vector2f(this->mousePosWindow.x - 10, 15));
    }

    if (this->solveBox.getGlobalBounds().contains(this->mousePosWindow))
    {
      this->solveBox.setFillColor(sf::Color(this->colButtonHighlight));
      if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
      {
        this->solveBox.setFillColor(this->colButtonClick);
        this->stateEngine.setState(StateEngine::states::solve);
      }
    }
    else
    {
      this->solveBox.setFillColor(sf::Color(this->colButton));
    }

    if (this->resetBox.getGlobalBounds().contains(this->mousePosWindow))
    {
      this->resetBox.setFillColor(sf::Color(this->colButtonHighlight));
      if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
      {
        this->resetBox.setFillColor(this->colButtonClick);
        this->loadGridFromFile("default_grid.txt");
      }
    }
    else
    {
      this->resetBox.setFillColor(sf::Color(this->colButton));
    }

    if (this->algoDijkstraBox.getGlobalBounds().contains(this->mousePosWindow))
    {
      this->algoDijkstraBox.setFillColor(sf::Color(this->colButtonHighlight));
      if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
      {
        this->algorithm = algorithms::dijkstra;
      }
    }
    else
    {
      this->algoDijkstraBox.setFillColor(sf::Color(this->colButton));
    }
    
    if (this->algoAStarBox.getGlobalBounds().contains(this->mousePosWindow))
    {
      this->algoAStarBox.setFillColor(sf::Color(this->colButtonHighlight));
      if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
      {
        this->algorithm = algorithms::aStar;
      }
    }
    else
    {
      this->algoAStarBox.setFillColor(sf::Color(this->colButton));
    }
    
    if (this->algoOtherOneBox.getGlobalBounds().contains(this->mousePosWindow))
    {
      this->algoOtherOneBox.setFillColor(sf::Color(this->colButtonHighlight));
      if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
      {
        this->algorithm = algorithms::otherOne;
      }
    }
    else
    {
      this->algoOtherOneBox.setFillColor(sf::Color(this->colButton));
    }
    
    if (this->algoOtherTwoBox.getGlobalBounds().contains(this->mousePosWindow))
    {
      this->algoOtherTwoBox.setFillColor(sf::Color(this->colButtonHighlight));
      if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
      {
        this->algorithm = algorithms::otherTwo;
      }
    }
    else
    {
      this->algoOtherTwoBox.setFillColor(sf::Color(this->colButton));
    }

    switch (this->algorithm)
    {
      case algorithms::dijkstra:
        this->algoDijkstraBox.setFillColor(sf::Color(this->colButtonActive));
        break;
      case algorithms::aStar:
        this->algoAStarBox.setFillColor(sf::Color(this->colButtonActive));
        break;
      case algorithms::otherOne:
        this->algoOtherOneBox.setFillColor(sf::Color(this->colButtonActive));
        break;
      case algorithms::otherTwo:
        this->algoOtherTwoBox.setFillColor(sf::Color(this->colButtonActive));
        break;
    }
  }
  
  void renderUI() {
    this->window->draw(this->sliderOutline);
    this->window->draw(this->sliderBox);
    // this->window->draw(this->highlightBox);
    this->window->draw(this->algoDijkstraBox);
    this->window->draw(this->algoAStarBox);
    this->window->draw(this->algoOtherOneBox);
    this->window->draw(this->algoOtherTwoBox);
    this->window->draw(this->solveBox);
    this->window->draw(this->resetBox);
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
            // else if (currentState == Tile::start || currentState == Tile::end)
          }
          else if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
          {
            if (currentState == Tile::wall) currentTile->setState(Tile::empty);
          }
          else
          {
            if (currentState == Tile::empty) currentTile->setState(Tile::hovered);
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
  
  void saveGridToFile(std::string fileName) {
    int tileState;
    std::ofstream file;
    file.open(fileName);

    for (int i=0; i<20; i++)
    {
      for (int j=0; j<20; j++)
      {
        file << this->grid[j][i]->state << " ";
      }
      file << std::endl;
    }
    file.close();
  }
  
  void loadGridFromFile(std::string fileName) {
    int fileState;
    std::ifstream file;
    file.open(fileName);

    for (int i=0; i<20; i++)
    {
      for (int j=0; j<20; j++)
      {
        file >> fileState;
        this->grid[j][i]->setState(fileState);
      }
    }
    file.close();
  }
  
  void solve(algorithms algo) {
    switch (algo)
    {
      case dijkstra:
        break;
      case aStar:
        break;
      case otherOne:
        break;
      case otherTwo:
        break;
    }
  }
};
