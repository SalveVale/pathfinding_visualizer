#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window/VideoMode.hpp>

#include "tile.hpp"
#include "algo_dijkstra.cpp"

#include <iostream>
#include <fstream>
#include <vector>

class StateEngine {
public:
  // StateEngine();
  // ~StateEngine();

  enum states {
    welcome,
    build,
    solving,
    solved
  } state = welcome;

  void setState(states newState, sf::RenderWindow *window) {
    if (this->state == welcome && newState == build) this->state = build;
    else if (this->state == build && newState == solving)
    {
      // window->setFramerateLimit(1);
      this->state = solving;
      std::cout << "solving\n";
    }
    else if (this->state == solving && newState == solved)
    {
      std::cout << "solved\n";
      window->setFramerateLimit(this->framerateLimit);
      this->state = solved;
    }
    else if (this->state == solved && newState == build)
    {
      std::cout << "building\n";
      window->setFramerateLimit(60);
      this->state = build;
    }
  }
  
  void setFramerateLimit(int newLimit) { this->framerateLimit = newLimit; }

private:
  int framerateLimit = 50;
};


class Window {
public:
  Window() {
    this->initVariables();
    this->initWindow();
  }
  ~Window() {
    delete this->window;
    for (int i=0; i<50; i++)
    {
      for (int j=0; j<50; j++)
      {
        delete this->grid[i][j];
      }
    }
  }

  const bool isOpen() const {
    return this->window->isOpen();
  }

  void update() {
    switch (stateEngine.state)
    {
      case StateEngine::states::welcome:
        this->pollWelcomeEvents();
        break;
      case StateEngine::states::build:
        this->pollEvents();
        this->updateMouse();
        this->updateUI();
        this->updateTiles();
        break;
      case StateEngine::states::solving:
        this->pollEvents();
        this->startTile->setValue(0);
        this->unvisitedTiles.push_back(*this->startTile);
        this->clearGrid();
        switch (this->algorithm)
        {
          case dijkstra:
            this->solveDijkstra();
            break;
          case aStar:
            this->solveAStar();
            break;
          case otherOne:
            this->solveOtherOne();
            break;
          case otherTwo:
            this->solveOtherTwo();
            break;
        }
        break;
      case StateEngine::states::solved:
        this->pollEvents();
        // if (this->solveInstant)
        // {
          this->drawSolving();
        // }
        // else
        // {
        //   this->animateSolving();
        // }
        break;
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
  //solving
  Tile *currentTile;
  int iterations = 1;
  
  StateEngine stateEngine;
  sf::RenderWindow *window;
  sf::VideoMode videoMode;
  sf::Event event;
  Tile *grid[50][50];
  Tile *startTile;
  Tile *endTile;
  std::vector<Tile> unvisitedTiles;
  std::vector<Tile> visitedTilesInOrder;
  
  int animationStep = 0;
  
  // bool solved = false;
  
  bool solveInstant;
  
  //ui
  sf::RectangleShape welcomeShader;
  sf::RectangleShape sliderOutline;
  sf::RectangleShape sliderBox;
  sf::RectangleShape algoDijkstraBox;
  sf::RectangleShape algoAStarBox;
  sf::RectangleShape algoOtherOneBox;
  sf::RectangleShape algoOtherTwoBox;
  sf::RectangleShape solveBox;
  sf::RectangleShape resetBox;
  sf::RectangleShape saveBox;
  
  bool movingStartTile = false;
  bool movingEndTile = false;
  
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
    this->welcomeShader.setSize(sf::Vector2f(1500, 1000));
    this->welcomeShader.setFillColor(sf::Color(0, 0, 0, 185));
    
    this->sliderOutline.setPosition(sf::Vector2f(30, 15));
    this->sliderOutline.setSize(sf::Vector2f(400, 30));
    this->sliderOutline.setFillColor(sf::Color(this->colButton));
    this->sliderOutline.setOutlineColor(sf::Color::White);
    this->sliderOutline.setOutlineThickness(1);
    
    this->sliderBox.setPosition(sf::Vector2f(290, 20));
    this->sliderBox.setSize(sf::Vector2f(20, 20));
    this->sliderBox.setFillColor(sf::Color::White);
    
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
    
    this->algoOtherTwoBox.setPosition(sf::Vector2f(30, 520));
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
    
    this->saveBox.setPosition(sf::Vector2f(280, 900));
    this->saveBox.setSize(sf::Vector2f(200, 30));
    this->saveBox.setFillColor(sf::Color(this->colButton));
    this->saveBox.setOutlineColor(sf::Color::White);
    this->saveBox.setOutlineThickness(1);
    
    for (int i=0; i<50; i++)
    {
      for (int j=0; j<50; j++)
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
  
  void pollWelcomeEvents() {
    while (this->window->pollEvent(this->event))
    {
      switch (this->event.type)
      {
        case sf::Event::Closed:
          this->window->close();
          break;
        case sf::Event::KeyPressed:
          if (this->event.key.code == sf::Keyboard::Escape) this->window->close();
          else
          {
            this->stateEngine.setState(StateEngine::states::build, this->window);
            this->welcomeShader.setFillColor(sf::Color::Transparent);
          }
        default: break;
      }
    }
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
            this->stateEngine.setState(StateEngine::states::solving, this->window);
          break;
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
      int mousePos = this->mousePosWindow.x;
      int remainder = mousePos % 50;
      if (remainder % 50 < 25)
      {
        mousePos -= remainder;
      }
      else
      {
        mousePos += (50 - remainder);
      }
      if (mousePos > 400 || mousePos < 15) return;
      switch (mousePos)
      {
        case 50:
          this->solveInstant = false;
          this->stateEngine.setFramerateLimit(5);
          break;
        case 100:
          this->solveInstant = false;
          this->stateEngine.setFramerateLimit(10);
          break;
        case 150:
          this->solveInstant = false;
          this->stateEngine.setFramerateLimit(20);
          break;
        case 200:
          this->solveInstant = false;
          this->stateEngine.setFramerateLimit(30);
          break;
        case 250:
          this->solveInstant = false;
          this->stateEngine.setFramerateLimit(40);
          break;
        case 300:
          this->solveInstant = false;
          this->stateEngine.setFramerateLimit(50);
          break;
        case 350:
          this->solveInstant = false;
          this->stateEngine.setFramerateLimit(60);
          break;
        case 400:
          this->solveInstant = true;
          this->stateEngine.setFramerateLimit(1);
          break;
      }
      this->sliderBox.setPosition(sf::Vector2f(mousePos - 10, 20));
    }

    if (this->solveBox.getGlobalBounds().contains(this->mousePosWindow))
    {
      this->solveBox.setFillColor(sf::Color(this->colButtonHighlight));
      if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
      {
        this->solveBox.setFillColor(this->colButtonClick);
        this->stateEngine.setState(StateEngine::states::solving, this->window);
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

    if (this->saveBox.getGlobalBounds().contains(this->mousePosWindow))
    {
      this->saveBox.setFillColor(sf::Color(this->colButtonHighlight));
      if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
      {
        this->saveBox.setFillColor(this->colButtonClick);
        this->saveGridToFile("default_grid.txt");
      }
    }
    else
    {
      this->saveBox.setFillColor(sf::Color(this->colButton));
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
    this->window->draw(this->algoDijkstraBox);
    this->window->draw(this->algoAStarBox);
    this->window->draw(this->algoOtherOneBox);
    this->window->draw(this->algoOtherTwoBox);
    this->window->draw(this->solveBox);
    this->window->draw(this->resetBox);
    this->window->draw(this->saveBox);

    this->window->draw(this->welcomeShader);
  }
  
  void updateTiles() {
    for (int i=0; i<50; i++)
    {
      for (int j=0; j<50; j++)
      {
        Tile *currentTile = this->grid[i][j];
        Tile::states currentState = currentTile->state;
        if (currentTile->containsMouse(this->mousePosWindow))
        {
          if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
          {
            if (currentState == Tile::empty || currentState == Tile::hovered)
            {
              if (this->movingStartTile)
              {
                this->startTile = currentTile;
              }
              else if (this->movingEndTile)
              {
                this->endTile = currentTile;
              }
              else 
              {
                currentTile->setState(Tile::wall);
              }
            }
            else if (currentState == Tile::start)
            {
              this->movingStartTile = true;
            }
            else if (currentState == Tile::end)
            {
              this->movingEndTile = true;
            }
          }
          else if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
          {
            if (currentState == Tile::wall) currentTile->setState(Tile::empty);
          }
          else
          {
            this->movingStartTile = false;
            this->movingEndTile = false;
            if (currentState == Tile::empty) currentTile->setState(Tile::hovered);
          }
        }
        else
        {
          // if (currentState == Tile::hovered) currentTile->setState(Tile::empty);
          if (currentState == Tile::hovered || currentState == Tile::start || currentState == Tile::end) currentTile->setState(Tile::empty);
        }
        this->startTile->setState(Tile::start);
        this->endTile->setState(Tile::end);
      }
    }
  }

  void renderBoxes() {
    for (int i=0; i<50; i++)
    {
      for (int j=0; j<50; j++)
      {
        this->window->draw(this->grid[i][j]->getBox());
      }
    }
  }
  
  void saveGridToFile(std::string fileName) {
    int tileState;
    std::ofstream file;
    file.open(fileName);

    for (int i=0; i<50; i++)
    {
      for (int j=0; j<50; j++)
      {
        file << this->grid[j][i]->getState() << " ";
      }
      file << std::endl;
    }
    file.close();
    std::cout << "Successfully saved file\n";
  }
  
  void loadGridFromFile(std::string fileName) {
    this->visitedTilesInOrder.clear();
    int fileState;
    std::ifstream file;
    file.open(fileName);

    for (int i=0; i<50; i++)
    {
      for (int j=0; j<50; j++)
      {
        file >> fileState;
        this->grid[j][i]->setState(fileState);
        if (fileState == 1)
        {
          this->startTile = grid[j][i];
          // this->startTile->setValue(0);
          // this->unvisitedTiles.push_back(*this->startTile);
          // this->visitedTilesInOrder.push_back(*this->startTile);
        }
        else if (fileState == 2)
        {
          this->endTile = grid[j][i];
        }
      }
    }
    file.close();
  }
  
  void clearGrid() {
    // this->startTile->setValue(0);
    // this->visitedTilesInOrder.push_back(*this->startTile);
    for (int i=0; i<50; i++)
    {
      for (int j=0; j<50; j++)
      {
        if (this->grid[i][j]->getState() == Tile::states::hovered) this->grid[i][j]->setState(Tile::states::empty);
      }
    }
  }
  
  void solveDijkstra() {
    std::vector<Tile> neighborTiles;
    for (int i=0; i<this->unvisitedTiles.size(); i++)
    {
      Tile *checkedTile = &this->unvisitedTiles[i];

      int xCoord = checkedTile->getCoords(0);
      int yCoord = checkedTile->getCoords(1);
      
      if (yCoord > 0)
      {
        Tile *up = this->grid[xCoord][yCoord - 1];
        switch (up->getState())
        {
          case Tile::states::end:
            this->endTile->setPrevTile(up);
            this->stateEngine.setState(StateEngine::solved, this->window);
            break;
          case Tile::states::empty:
            up->setState(Tile::states::checked);
            up->setValue(checkedTile->getValue() + 1);
            up->setPrevTile(checkedTile);
            neighborTiles.push_back(*up);
            this->visitedTilesInOrder.push_back(*up);
            break;
          default:
            break;
        }
      }

      if (xCoord < 50)
      {
        Tile *right = this->grid[xCoord + 1][yCoord];
        switch (right->getState())
        {
          case Tile::states::end:
            this->endTile->setPrevTile(right);
            this->stateEngine.setState(StateEngine::solved, this->window);
            break;
          case Tile::states::empty:
            right->setState(Tile::states::checked);
            right->setValue(checkedTile->getValue() + 1);
            right->setPrevTile(checkedTile);
            neighborTiles.push_back(*right);
            this->visitedTilesInOrder.push_back(*right);
            break;
          default:
            break;
        }
      }

      if (yCoord < 50)
      {
        Tile *down = this->grid[xCoord][yCoord + 1];
        switch (down->getState())
        {
          case Tile::states::end:
            this->endTile->setPrevTile(down);
            this->stateEngine.setState(StateEngine::solved, this->window);
            break;
          case Tile::states::empty:
            down->setState(Tile::states::checked);
            down->setValue(checkedTile->getValue() + 1);
            down->setPrevTile(checkedTile);
            neighborTiles.push_back(*down);
            this->visitedTilesInOrder.push_back(*down);
            break;
          default:
            break;
        }
      }

      if (xCoord > 0)
      {
        Tile *left = this->grid[xCoord - 1][yCoord];
        switch (left->getState())
        {
          case Tile::states::end:
            this->endTile->setPrevTile(left);
            this->stateEngine.setState(StateEngine::solved, this->window);
            break;
          case Tile::states::empty:
            left->setState(Tile::states::checked);
            left->setValue(checkedTile->getValue() + 1);
            left->setPrevTile(checkedTile);
            neighborTiles.push_back(*left);
            this->visitedTilesInOrder.push_back(*left);
            break;
          default:
            break;
        }
      }

    }
    
    this->unvisitedTiles.clear();
    this->unvisitedTiles = neighborTiles;
    
    if (this->stateEngine.state != StateEngine::solved) this->solveDijkstra();
    
    this->unvisitedTiles.clear();
  }

  void solveAStar() {  
    this->stateEngine.setState(StateEngine::solved, this->window);
  }
  
  void solveOtherOne() {
    this->stateEngine.setState(StateEngine::solved, this->window);
  }
  
  void solveOtherTwo() {
    this->stateEngine.setState(StateEngine::solved, this->window);
  }

  void animateSolving() {
    this->grid[this->visitedTilesInOrder[this->animationStep].getCoords(0)][this->visitedTilesInOrder[this->animationStep].getCoords(1)]->setState(Tile::states::visited);
    this->animationStep++;
    if (this->animationStep >= this->visitedTilesInOrder.size())
    {
      this->animationStep = 0;
      this->visitedTilesInOrder.clear();
      this->visitedTilesInOrder.push_back(*this->startTile);
      this->stateEngine.setState(StateEngine::build, this->window);
    }
  }
  
  void drawSolving() {
    for (int i=0; i<this->visitedTilesInOrder.size(); i++)
    {
      this->grid[this->visitedTilesInOrder[i].getCoords(0)][this->visitedTilesInOrder[i].getCoords(1)]->setState(Tile::states::visited);
    }

    this->visitedTilesInOrder.clear();
    this->visitedTilesInOrder.push_back(*this->startTile);
    
    std::cout << this->endTile->getCoords(0) << " " << this->endTile->getCoords(1) << std::endl;
    std::cout << this->endTile->getPrevTile()->getState();
    // Tile *currentTile = this->endTile->getPrevTile();
    // std::cout << currentTile->getCoords(0) << " " << currentTile->getCoords(1) << std::endl;
    // currentTile->setState(Tile::states::path);
    // currentTile = currentTile->getPrevTile();
    // std::cout << currentTile->getCoords(0) << " " << currentTile->getCoords(1) << std::endl;

    // while (currentTile->getPrevTile() != 0)
    // {
    //   currentTile->setState(Tile::states::path);
    //   currentTile = currentTile->getPrevTile();
    // }
    
    this->stateEngine.setState(StateEngine::build, this->window);
  }
  
};
