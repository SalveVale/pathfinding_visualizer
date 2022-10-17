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
    solve
    // solveDijkstra,
    // solveAStar,
    // solveOtherOne,
    // solveOtherTwo
  } state = welcome;

  void setState(states newState, sf::RenderWindow *window, int framerateLimit) {
    if (this->state == welcome && newState == build) this->state = newState;
    if (this->state == build && newState == solve)
    {
      window->setFramerateLimit(framerateLimit);
      this->state = newState;
    }
    if (this->state == solve && newState == build)
    {
      window->setFramerateLimit(60);
      this->state = newState;
    }
  }
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
      case StateEngine::states::solve:
        this->pollEvents();
        switch (this->algorithm)
        {
          case dijkstra:
            this->solveDijkstra();
            break;
          case aStar:
            break;
          case otherOne:
            break;
          case otherTwo:
            break;
        }
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
  int startCoords[2];
  // Tile *startTile;
  // Tile *endTile;
  // std::vector<Tile> unvisitedTiles;
  // std::vector<Tile> visitedTiles;
  
  // bool solved = false;
  
  int solveFrameLimit = 5;
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
    
    this->sliderBox.setPosition(sf::Vector2f(300, 20));
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
            this->stateEngine.setState(StateEngine::states::build, this->window, this->solveFrameLimit);
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
            this->stateEngine.setState(StateEngine::states::solve, this->window, this->solveFrameLimit);
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
          this->solveFrameLimit = 1;
          break;
        case 100:
          this->solveInstant = false;
          this->solveFrameLimit = 5;
          break;
        case 150:
          this->solveInstant = false;
          this->solveFrameLimit = 10;
          break;
        case 200:
          this->solveInstant = false;
          this->solveFrameLimit = 20;
          break;
        case 250:
          this->solveInstant = false;
          this->solveFrameLimit = 30;
          break;
        case 300:
          this->solveInstant = false;
          this->solveFrameLimit = 40;
          break;
        case 350:
          this->solveInstant = false;
          this->solveFrameLimit = 60;
          break;
        case 400:
          this->solveInstant = true;
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
        this->stateEngine.setState(StateEngine::states::solve, this->window, this->solveFrameLimit);
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
    // this->window->draw(this->highlightBox);
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

    for (int i=0; i<50; i++)
    {
      for (int j=0; j<50; j++)
      {
        file >> fileState;
        this->grid[j][i]->setState(fileState);
        if (fileState == 1)
        {
          this->currentTile = grid[j][i];
          this->currentTile->setValue(50);
          this->startCoords[0] = this->currentTile->getCoords(0);
          this->startCoords[1] = this->currentTile->getCoords(1);
          // this->visitedTiles.push_back(*this->startTile);
        }
        // else if (fileState == 2)
        // {
        //   this->endTile = this->grid[j][i];
        // }
      }
    }
    file.close();
  }
  
  // void solve(algorithms algo) {
  //   switch (algo)
  //   {
  //     case dijkstra:
  //       // this->currentTile = dijkstra::solve(this->grid, this->currentTile);
  //       this->solveDijkstra();
  //       break;
  //     case aStar:
  //       break;
  //     case otherOne:
  //       break;
  //     case otherTwo:
  //       break;
  //   }
  // }
  
  void solveDijkstra() {
    int bestValue = 51;
    Tile *bestTile;
    for (int i=0; i<50; i++)
    {
      for (int j=0; j<50; j++)
      {
        Tile *checkedTile = this->grid[i][j];
        //if the tile is visited and it has a lower value
        if ((checkedTile->getState() == Tile::states::visited || checkedTile->getState() == Tile::states::start) && checkedTile->getValue() < bestValue)
        {
          bestTile = checkedTile;
          bestValue = bestTile->getValue();
        }
      }
    }
    this->currentTile = bestTile;
    
    int xCoord = this->currentTile->getCoords(0);
    int yCoord = this->currentTile->getCoords(1);
    if (yCoord > 0)
    {
      Tile *up = this->grid[xCoord][yCoord - 1];
      if (up->getState() == Tile::states::empty)
      {
        int xValue = abs(this->startCoords[0] - xCoord);
        int yValue = abs(this->startCoords[1] - yCoord);
        if (xValue > yValue) up->setValue(xValue);
        else up->setValue(yValue);
        up->setState(Tile::states::visited);
        // this->visitedTiles->push_back(*up);
        return;
      }
    }
    if (xCoord > 0)
    {
      Tile *left = this->grid[xCoord - 1][yCoord];
      if (left->getState() == Tile::states::empty)
      {
        int xValue = abs(this->startCoords[0] - xCoord);
        int yValue = abs(this->startCoords[1] - yCoord);
        if (xValue > yValue) left->setValue(xValue);
        else left->setValue(yValue);
        left->setValue(this->iterations);
        left->setState(Tile::states::visited);
        return;
      }
    }
    if (xCoord < 50)
    {
      Tile *right = this->grid[xCoord + 1][yCoord];
      if (right->getState() == Tile::states::empty)
      {
        int xValue = abs(this->startCoords[0] - xCoord);
        int yValue = abs(this->startCoords[1] - yCoord);
        if (xValue > yValue) right->setValue(xValue);
        else right->setValue(yValue);
        right->setValue(this->iterations);
        right->setState(Tile::states::visited);
        return;
      }
    }
    if (yCoord < 50)
    {
      Tile *down = this->grid[xCoord][yCoord + 1];
      if (down->getState() == Tile::states::empty)
      {
        int xValue = abs(this->startCoords[0] - xCoord);
        int yValue = abs(this->startCoords[1] - yCoord);
        if (xValue > yValue) down->setValue(xValue);
        else down->setValue(yValue);
        down->setValue(this->iterations);
        down->setState(Tile::states::visited);
        return;
      }
    }
  }
  
  // void solveAStar() {
  // }
  
  // void solveOtherOne() {
  // }
  
  // void solveOtherTwo() {
  // }
  
  // void solving() {
  //   std::cout << this->solveFrameLimit << std::endl;
  //   this->window->setFramerateLimit(this->solveFrameLimit);

  //   switch (this->algorithm)
  //   {
  //     case dijkstra:
  //       stateEngine.setState(StateEngine::states::solveDijkstra);
  //       break;
  //     case aStar:
  //       stateEngine.setState(StateEngine::states::solveAStar);
  //       break;
  //     case otherOne:
  //       stateEngine.setState(StateEngine::states::solveOtherOne);
  //       break;
  //     case otherTwo:
  //       stateEngine.setState(StateEngine::states::solveOtherTwo);
  //       break;
  //   }
  // }
};
