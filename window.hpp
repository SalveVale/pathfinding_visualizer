#include <SFML/Graphics/Texture.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window/VideoMode.hpp>

#include "tile.hpp"
#include "algo_dijkstra.cpp"

#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>

class StateEngine {
public:
  enum states {
    welcome,
    build,
    buttonClicked,
    solving,
    solved,
    buildSolved
  } state = welcome;

  void setState(states newState, sf::RenderWindow *window) {
    if (this->state == welcome && newState == build)
    {
      this->state = build;
    }
    else if (this->state == build && newState == buttonClicked)
    {
      this->state = buttonClicked;
    }
    else if (this->state == buttonClicked && newState == build)
    {
      this->state = build;
    }
    else if (this->state == build && newState == solving)
    {
      this->state = solving;
    }
    else if (this->state == solving && newState == solved)
    {
      window->setFramerateLimit(this->framerateLimit);
      this->state = solved;
    }
    else if (this->state == solved && newState == buildSolved)
    {
      window->setFramerateLimit(60);
      this->state = buildSolved;
    }
    else if (this->state == buildSolved && newState == solved)
    {
      this->state = solved;
    }
    else if (this->state == buildSolved && newState == build)
    {
      this->state = build;
    }
  }
  
  void setFramerateLimit(int newLimit) { this->framerateLimit = newLimit; }

private:
  int framerateLimit = 120;
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
      case StateEngine::states::buttonClicked:
        this->pollEvents();
        if (!sf::Mouse::isButtonPressed(sf::Mouse::Left))
          this->stateEngine.setState(StateEngine::build, this->window);
        break;
      case StateEngine::states::solving:
        this->pollEvents();
        this->clearGrid();
        this->solve();
        break;
      case StateEngine::states::solved:
        this->pollEvents();
        if (this->solveInstant)
        {
          this->drawSolving();
        }
        else if (this->drawnVisited)
        {
          this->animatePath();
        }
        else
        {
          this->animateVisited();
        }
      case StateEngine::states::buildSolved:
        this->pollEvents();
        this->updateMouse();
        this->updateUISolved();
        this->updateTilesSolved();
        break;
    }
  }
  
  
  void render() {
    this->window->clear(sf::Color(17, 20, 26, 255));
    
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
  
  int solvingStep;
  int animationStep = 0;
  
  bool solveInstant;
  bool drawnVisited = false;
  
  int prevStartCoords[2];
  int prevEndCoords[2];
  
  //ui
  int welcomeSlideNum = 0;
  sf::RectangleShape welcomeShader;
  sf::Texture txPlaceWalls;
  sf::Texture txEraseWalls;
  sf::Texture txMoveStartEnd;
  sf::Texture txSolveSpeed;
  sf::Texture txAlgo;
  sf::Texture txSolve;
  sf::Texture txMoveAfterSolve;
  sf::Sprite spPlaceWalls;
  sf::Sprite spEraseWalls;
  sf::Sprite spMoveStartEnd;
  sf::Sprite spSolveSpeed;
  sf::Sprite spAlgo;
  sf::Sprite spSolve;
  sf::Sprite spMoveAfterSolve;
  
  sf::RectangleShape sliderOutline;
  sf::RectangleShape sliderBox;
  sf::RectangleShape algoDijkstraBox;
  sf::RectangleShape algoAStarBox;
  sf::RectangleShape algoDumbyBox;
  sf::RectangleShape solveBox;
  sf::RectangleShape resetBox;
  sf::RectangleShape saveBox;
  
  sf::Font font;
  
  sf::Text welcomeTitle;
  sf::Text welcomeText;
  sf::Text welcomeContinueText;
  
  sf::Text sliderText;
  sf::Text sliderNums;
  sf::Text algorithmsText;
  sf::Text algoDijkstraText;
  sf::Text algoAStarText;
  sf::Text algoDumbyText;
  sf::Text algoSummaryText;
  sf::Text solveBoxText;
  sf::Text resetBoxText;
  sf::Text saveBoxText;
  
  //logic
  bool movingStartTile = false;
  bool movingEndTile = false;
  
  int animationCoords[2];
  
  const sf::Color colButton = sf::Color(29, 22, 22, 255);
  const sf::Color colButtonHighlight = sf::Color(40, 30, 30, 255);
  const sf::Color colButtonClick = sf::Color(50, 40, 40, 255);
  const sf::Color colButtonActive = sf::Color(70, 50, 48, 255);
  
  //mouse
  sf::Vector2i mousePosView;
  sf::Vector2f mousePosWindow;
  
  enum algorithms {
    dijkstra,
    aStar,
    dumby
  } algorithm = dijkstra;
  

  void initVariables() {
    this->window = nullptr;
    
    //ui
    this->welcomeShader.setSize(sf::Vector2f(1600, 1000));
    this->welcomeShader.setFillColor(sf::Color(35, 35, 35, 245));
    
    this->sliderOutline.setPosition(sf::Vector2f(30, 15));
    this->sliderOutline.setSize(sf::Vector2f(400, 30));
    this->sliderOutline.setFillColor(sf::Color(this->colButton));
    this->sliderOutline.setOutlineColor(sf::Color::White);
    this->sliderOutline.setOutlineThickness(1);
    
    this->sliderBox.setPosition(sf::Vector2f(290, 20));
    this->sliderBox.setSize(sf::Vector2f(20, 20));
    this->sliderBox.setFillColor(sf::Color::White);
    
    if (!this->txPlaceWalls.loadFromFile("resources/tutorial_pictures/placing_walls.png")) std::cout << "Failed to load 'placing_walls.png'";
    this->spPlaceWalls.setTexture(this->txPlaceWalls);
    this->spPlaceWalls.setPosition(9000, 9000);
    if (!this->txEraseWalls.loadFromFile("resources/tutorial_pictures/erasing_walls.png")) std::cout << "Failed to load 'erasing_walls.png'";
    this->spEraseWalls.setTexture(this->txEraseWalls);
    this->spEraseWalls.setPosition(9000, 9000);
    if (!this->txMoveStartEnd.loadFromFile("resources/tutorial_pictures/moving_startandend.png")) std::cout << "Failed to load 'moving_startandend.png'";
    this->spMoveStartEnd.setTexture(this->txMoveStartEnd);
    this->spMoveStartEnd.setPosition(9000, 9000);
    if (!this->txSolveSpeed.loadFromFile("resources/tutorial_pictures/animation_speed.png")) std::cout << "Failed to load 'animation_speed.png'";
    this->spSolveSpeed.setTexture(this->txSolveSpeed);
    this->spSolveSpeed.setPosition(9000, 9000);
    if (!this->txAlgo.loadFromFile("resources/tutorial_pictures/choose_algo.png")) std::cout << "Failed to load 'choose_algo.png'";
    this->spAlgo.setTexture(this->txAlgo);
    this->spAlgo.setPosition(9000, 9000);
    if (!this->txSolve.loadFromFile("resources/tutorial_pictures/solve.png")) std::cout << "Failed to load 'solve.png'";
    this->spSolve.setTexture(this->txSolve);
    this->spSolve.setPosition(9000, 9000);
    if (!this->txMoveAfterSolve.loadFromFile("resources/tutorial_pictures/moving.png")) std::cout << "Failed to load 'moving.png'";
    this->spMoveAfterSolve.setTexture(this->txMoveAfterSolve);
    this->spMoveAfterSolve.setPosition(9000, 9000);
    
    if (!this->font.loadFromFile("resources/Roboto/Roboto-Regular.ttf")) std::cout << "Failed to load font from file";

    this->welcomeTitle.setFont(this->font);
    this->welcomeTitle.setCharacterSize(30);
    this->welcomeTitle.setPosition(sf::Vector2f(550, 10));
    this->welcomeTitle.setString("Pathfinding Algorithm Visualizer");
    
    this->welcomeText.setFont(this->font);
    this->welcomeText.setCharacterSize(18);
    this->welcomeText.setPosition(sf::Vector2f(60, 200));
    this->welcomeText.setString("This simple program allows you to visualize how different pathfinding algoriths work. You can draw and erase obsticles on a grid and move the stating and ending tiles");
    
    this->welcomeContinueText.setFont(this->font);
    this->welcomeContinueText.setCharacterSize(17);
    this->welcomeContinueText.setPosition(sf::Vector2f(500, 850));
    this->welcomeContinueText.setString("Press Space to continue or press Escape to skip the tutorial");
    
    this->sliderText.setFont(this->font);
    this->sliderText.setCharacterSize(20);
    this->sliderText.setPosition(sf::Vector2f(30, 50));
    this->sliderText.setString("Solve animation speed:");
    
    this->sliderNums.setFont(this->font);
    this->sliderNums.setCharacterSize(20);
    this->sliderNums.setPosition(sf::Vector2f(300, 50));
    this->sliderNums.setString("120");

    this->algorithmsText.setFont(this->font);
    this->algorithmsText.setCharacterSize(20);
    this->algorithmsText.setPosition(sf::Vector2f(30, 350));
    this->algorithmsText.setString("Algorithms:");
    
    this->algoDijkstraText.setFont(this->font);
    this->algoDijkstraText.setCharacterSize(20);
    this->algoDijkstraText.setPosition(sf::Vector2f(40, 410));
    this->algoDijkstraText.setString("Dijkstra");
    
    this->algoAStarText.setFont(this->font);
    this->algoAStarText.setCharacterSize(20);
    this->algoAStarText.setPosition(sf::Vector2f(40, 450));
    this->algoAStarText.setString("A*");
    
    this->algoDumbyText.setFont(this->font);
    this->algoDumbyText.setCharacterSize(20);
    this->algoDumbyText.setPosition(sf::Vector2f(40, 490));
    this->algoDumbyText.setString("Dumby");
    
    this->algoSummaryText.setFont(this->font);
    this->algoSummaryText.setCharacterSize(15);
    this->algoSummaryText.setPosition(sf::Vector2f(160, 350));
    
    this->solveBoxText.setFont(this->font);
    this->solveBoxText.setCharacterSize(20);
    this->solveBoxText.setPosition(sf::Vector2f(40, 800));
    this->solveBoxText.setString("Solve");
    
    this->resetBoxText.setFont(this->font);
    this->resetBoxText.setCharacterSize(20);
    this->resetBoxText.setPosition(sf::Vector2f(40, 900));
    this->resetBoxText.setString("Reset");
    
    this->saveBoxText.setFont(this->font);
    this->saveBoxText.setCharacterSize(20);
    this->saveBoxText.setPosition(sf::Vector2f(290, 900));
    this->saveBoxText.setString("Save");
    
    this->algoDijkstraBox.setPosition(sf::Vector2f(30, 400));
    this->algoDijkstraBox.setSize(sf::Vector2f(100, 40));
    this->algoDijkstraBox.setFillColor(sf::Color(this->colButton));
    this->algoDijkstraBox.setOutlineColor(sf::Color::White);
    this->algoDijkstraBox.setOutlineThickness(1);

    this->algoAStarBox.setPosition(sf::Vector2f(30, 440));
    this->algoAStarBox.setSize(sf::Vector2f(100, 40));
    this->algoAStarBox.setFillColor(sf::Color(this->colButton));
    this->algoAStarBox.setOutlineColor(sf::Color::White);
    this->algoAStarBox.setOutlineThickness(1);
    
    this->algoDumbyBox.setPosition(sf::Vector2f(30, 480));
    this->algoDumbyBox.setSize(sf::Vector2f(100, 40));
    this->algoDumbyBox.setFillColor(sf::Color(this->colButton));
    this->algoDumbyBox.setOutlineColor(sf::Color::White);
    this->algoDumbyBox.setOutlineThickness(1);
    
    this->solveBox.setPosition(sf::Vector2f(30, 800));
    this->solveBox.setSize(sf::Vector2f(100, 30));
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
    
    std::string gridFile = "";
    std::cout << "File to load grid from (enter 1 to load default grid)\n> "; std::cin >> gridFile;
    if (gridFile == "1") this->loadGridFromFile("default_grid.txt");
    else this->loadGridFromFile(gridFile);
  }
  
  void initWindow() {
    this->videoMode.height = 1000;
    this->videoMode.width = 1600;
    
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
          if (this->event.key.code == sf::Keyboard::Escape) this->stateEngine.setState(StateEngine::build, this->window);
          if (this->event.key.code == sf::Keyboard::Space) this->nextWelcomeSlide();
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
        default: break;
      }
    }
  }
  
  void updateMouse() {
    this->mousePosView = sf::Mouse::getPosition(*this->window);
    this->mousePosWindow = this->window->mapPixelToCoords(this->mousePosView);
  }
  
  void nextWelcomeSlide() {
    this->welcomeSlideNum++;
    switch (this->welcomeSlideNum)
    {
      case 1:
        this->welcomeText.setString("Left click to place obsticles\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\nRight click to remove obsticles");
        this->welcomeText.setPosition(sf::Vector2f(25, 200));
        this->spPlaceWalls.setPosition(sf::Vector2f(100, 100));
        this->spEraseWalls.setPosition(sf::Vector2f(100, 700));
        break;
      case 2:
        this->spPlaceWalls.setPosition(sf::Vector2f(9000, 9000));
        this->spEraseWalls.setPosition(sf::Vector2f(9000, 9000));
        this->welcomeText.setString("Left click drag on the start (green) and end (red) tiles to move them");
        this->welcomeText.setPosition(sf::Vector2f(50, 200));
        this->spMoveStartEnd.setPosition(sf::Vector2f(100, 100));
        break;
      case 3:
        this->spMoveStartEnd.setPosition(sf::Vector2f(9000, 9000));
        this->welcomeText.setString("Select speed to visualize the algorithm");
        this->spSolveSpeed.setPosition(sf::Vector2f(100, 100));
        break;
      case 4:
        this->spSolveSpeed.setPosition(sf::Vector2f(9000, 9000));
        this->welcomeText.setString("Choose which algorithm you want to see");
        this->spAlgo.setPosition(sf::Vector2f(100, 100));
        break;
      case 5:
        this->spAlgo.setPosition(sf::Vector2f(9000, 9000));
        this->welcomeText.setString("Solve");
        this->spSolve.setPosition(sf::Vector2f(100, 100));
        break;
      case 6:
        this->spSolve.setPosition(sf::Vector2f(9000, 9000));
        this->welcomeText.setString("Moving the start or end tiles after the solve will update the shortest path in real time");
        this->spMoveAfterSolve.setPosition(sf::Vector2f(100, 100));
        break;
    }
    if (this->welcomeSlideNum >= 7)
    {
      this->stateEngine.setState(StateEngine::build, this->window);
    }
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
          this->sliderNums.setString("5");
          this->solveInstant = false;
          this->stateEngine.setFramerateLimit(5);
          break;
        case 100:
          this->sliderNums.setString("10");
          this->solveInstant = false;
          this->stateEngine.setFramerateLimit(10);
          break;
        case 150:
          this->sliderNums.setString("20");
          this->solveInstant = false;
          this->stateEngine.setFramerateLimit(20);
          break;
        case 200:
          this->sliderNums.setString("30");
          this->solveInstant = false;
          this->stateEngine.setFramerateLimit(30);
          break;
        case 250:
          this->sliderNums.setString("60");
          this->solveInstant = false;
          this->stateEngine.setFramerateLimit(60);
          break;
        case 300:
          this->sliderNums.setString("120");
          this->solveInstant = false;
          this->stateEngine.setFramerateLimit(120);
          break;
        case 350:
          this->sliderNums.setString("200");
          this->solveInstant = false;
          this->stateEngine.setFramerateLimit(200);
          break;
        case 400:
          this->sliderNums.setString("Instant");
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
        
        this->stateEngine.setState(StateEngine::build, this->window);
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
        this->saveGridToFile("test_grid.txt");
        
        this->stateEngine.setState(StateEngine::buttonClicked, this->window);
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
        
        this->stateEngine.setState(StateEngine::buttonClicked, this->window);
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
        
        this->stateEngine.setState(StateEngine::buttonClicked, this->window);
      }
    }
    else
    {
      this->algoAStarBox.setFillColor(sf::Color(this->colButton));
    }
    
    if (this->algoDumbyBox.getGlobalBounds().contains(this->mousePosWindow))
    {
      this->algoDumbyBox.setFillColor(sf::Color(this->colButtonHighlight));
      if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
      {
        this->algorithm = algorithms::dumby;
        
        this->stateEngine.setState(StateEngine::buttonClicked, this->window);
      }
    }
    else
    {
      this->algoDumbyBox.setFillColor(sf::Color(this->colButton));
    }
    
    switch (this->algorithm)
    {
      case algorithms::dijkstra:
        this->algoSummaryText.setString("Dijkstra'a algorithm searches each neighboring tile blind\nto where the end tile is located. The tiles farther away\nfrom the start cost more than closer tiles. Once it finds\nthe end tile, it follows the lowest cost tiles back to the\nstart tile. Dijkstra's is useful for when you don't know\nthe location of the end tile.");
        this->algoDijkstraBox.setFillColor(sf::Color(this->colButtonActive));
        break;
      case algorithms::aStar:
        this->algoSummaryText.setString("The Alpha Star algorithm searches within a group of\ntiles that are reachable from the start tile. Each tile in\nthe group has a cost associated with it. The cost is\nthe sum of the distance from the start tile and the\ndistance from the end tile. Each iteration it picks the\ntile in the group with the lowest cost and adds its\nneighbors to the group, calculating their cost, then\nremoves the current tile from the group and marking it\nas closed. When the end tile is found, it follows the\nclosed tiles with the lowest cost back to the start tile.\nAlpha Star is very efficient, but can only be used when\nthe location of the end tile is known");
        this->algoAStarBox.setFillColor(sf::Color(this->colButtonActive));
        break;
      case algorithms::dumby:
        this->algoSummaryText.setString("The Dumby algorithm simply searches the tile above\nthe current tile. If it is blocked, it searches to the right.\nThen down, then to the left. Once the end tile is found,\nit traces the path it took back to the start tile.\nThe Dumby algorithm is very inefficient and it not\nespecially useful, but it can be used when the location\nof the end tile is unknown.");
        this->algoDumbyBox.setFillColor(sf::Color(this->colButtonActive));
        break;
    }
  }

  void updateUISolved() {
    this->solveBox.setFillColor(sf::Color(this->colButton));

    if (this->resetBox.getGlobalBounds().contains(this->mousePosWindow))
    {
      this->resetBox.setFillColor(sf::Color(this->colButtonHighlight));
      if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
      {
        this->resetBox.setFillColor(this->colButtonClick);
        this->loadGridFromFile("default_grid.txt");
        
        if (this->stateEngine.state == StateEngine::buildSolved)
        {
          this->stateEngine.setState(StateEngine::build, this->window);
        }
      }
    }
    else
    {
      this->resetBox.setFillColor(sf::Color(this->colButton));
    }
  }  

  void renderUI() {
    this->window->draw(this->sliderOutline);
    this->window->draw(this->sliderBox);
    this->window->draw(this->algoDijkstraBox);
    this->window->draw(this->algoAStarBox);
    this->window->draw(this->algoDumbyBox);
    this->window->draw(this->solveBox);
    this->window->draw(this->resetBox);
    this->window->draw(this->saveBox);
    
    this->window->draw(this->sliderText);
    this->window->draw(this->sliderNums);
    this->window->draw(this->algorithmsText);
    this->window->draw(this->algoDijkstraText);
    this->window->draw(this->algoAStarText);
    this->window->draw(this->algoDumbyText);
    this->window->draw(this->algoSummaryText);
    this->window->draw(this->solveBoxText);
    this->window->draw(this->resetBoxText);
    this->window->draw(this->saveBoxText);

    if (this->stateEngine.state == StateEngine::welcome)
    {
      this->window->draw(this->welcomeShader);
      this->window->draw(this->welcomeTitle);
      this->window->draw(this->welcomeText);
      this->window->draw(this->welcomeContinueText);
      this->window->draw(this->spPlaceWalls);
      this->window->draw(this->spEraseWalls);
      this->window->draw(this->spMoveStartEnd);
      this->window->draw(this->spSolveSpeed);
      this->window->draw(this->spAlgo);
      this->window->draw(this->spSolve);
      this->window->draw(this->spMoveAfterSolve);
    }

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
          if (currentState == Tile::hovered || currentState == Tile::start || currentState == Tile::end) currentTile->setState(Tile::empty);
        }
      }
      this->startTile->setState(Tile::start);
      this->endTile->setState(Tile::end);
    }
  }

  void updateTilesSolved() {
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
            if (currentState == Tile::empty || currentState == Tile::visited)
            {
              if (this->movingStartTile)
              {
                this->prevStartCoords[0] = this->startTile->getCoords(0);
                this->prevStartCoords[1] = this->startTile->getCoords(1);
                this->startTile = currentTile;
              }
              else if (this->movingEndTile)
              {
                this->prevEndCoords[0] = this->endTile->getCoords(0);
                this->prevEndCoords[1] = this->endTile->getCoords(1);
                this->endTile = currentTile;
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
          else
          {
            this->movingStartTile = false;
            this->movingEndTile = false;
          }
        }
        else
        {
          if (currentState == Tile::start || currentState == Tile::end) currentTile->setState(Tile::empty);
        }
      }
    }

    this->startTile->setState(Tile::start);
    this->endTile->setState(Tile::end);

    if (this->movingStartTile || this->movingEndTile)
    {
      if ((this->startTile->getCoords(0) != this->prevStartCoords[0] && this->startTile->getCoords(1) != this->prevStartCoords[1]) || (this->endTile->getCoords(0) != this->prevEndCoords[0] && this->endTile->getCoords(1) != this->prevEndCoords[1]))
      {
        this->clearGrid();

        this->solve();
        this->resetTileColors();
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
        file << this->grid[j][i]->getStateInt() << " ";
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
    for (int i=0; i<50; i++)
    {
      for (int j=0; j<50; j++)
      {
        Tile *checkedTile = this->grid[i][j];
        checkedTile->setValue(0);
        if (checkedTile->getState() != Tile::states::wall && checkedTile->getState() != Tile::states::start && checkedTile->getState() != Tile::states::end) checkedTile->setStateNoColor(Tile::states::empty);
      }
    }
  }
  
  void solveDijkstra() {
    if (this->stateEngine.state != StateEngine::solved)
    {
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
              this->endTile->setPrevCoords(xCoord, yCoord);
              this->animationCoords[0] = xCoord;
              this->animationCoords[1] = yCoord;
              this->stateEngine.setState(StateEngine::solved, this->window);
              break;
            case Tile::states::empty:
              up->setState(Tile::states::checked);
              up->setPrevCoords(checkedTile->getCoords(0), checkedTile->getCoords(1)); 
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
              this->endTile->setPrevCoords(xCoord, yCoord);
              this->animationCoords[0] = xCoord;
              this->animationCoords[1] = yCoord;
              this->stateEngine.setState(StateEngine::solved, this->window);
              break;
            case Tile::states::empty:
              right->setState(Tile::states::checked);
              right->setPrevCoords(checkedTile->getCoords(0), checkedTile->getCoords(1)); 
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
              this->endTile->setPrevCoords(xCoord, yCoord);
              this->animationCoords[0] = xCoord;
              this->animationCoords[1] = yCoord;
              this->stateEngine.setState(StateEngine::solved, this->window);
              break;
            case Tile::states::empty:
              down->setState(Tile::states::checked);
              down->setPrevCoords(checkedTile->getCoords(0), checkedTile->getCoords(1)); 
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
              this->endTile->setPrevCoords(xCoord, yCoord);
              this->animationCoords[0] = xCoord;
              this->animationCoords[1] = yCoord;
              this->stateEngine.setState(StateEngine::solved, this->window);
              break;
            case Tile::states::empty:
              left->setState(Tile::states::checked);
              left->setPrevCoords(checkedTile->getCoords(0), checkedTile->getCoords(1)); 
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
      
      this->solveDijkstra();
    }
    else
    {
      this->unvisitedTiles.clear();
    }
  }

  void solveAStar() {
    if (this->stateEngine.state != StateEngine::solved)
    {
      int xCoord = this->currentTile->getCoords(0);
      int yCoord = this->currentTile->getCoords(1);
    
      if (yCoord > 0)
      {
        Tile *up = this->grid[xCoord][yCoord - 1];
        switch (up->getState())
        {
          case Tile::states::end:
            this->endTile->setPrevCoords(xCoord, yCoord);
            this->animationCoords[0] = xCoord;
            this->animationCoords[1] = yCoord;
            this->stateEngine.setState(StateEngine::solved, this->window);
            break;
          case Tile::states::empty:
            up->setState(Tile::states::checked);
            up->setValue(this->solvingStep*10, this->calcHValue(xCoord, (yCoord - 1)));
            up->setPrevCoords(xCoord, yCoord); 
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
            this->endTile->setPrevCoords(xCoord, yCoord);
            this->animationCoords[0] = xCoord;
            this->animationCoords[1] = yCoord;
            this->stateEngine.setState(StateEngine::solved, this->window);
            break;
          case Tile::states::empty:
            right->setState(Tile::states::checked);
            right->setValue(this->solvingStep*10, this->calcHValue((xCoord + 1), yCoord));
            right->setPrevCoords(xCoord, yCoord); 
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
            this->endTile->setPrevCoords(xCoord, yCoord);
            this->animationCoords[0] = xCoord;
            this->animationCoords[1] = yCoord;
            this->stateEngine.setState(StateEngine::solved, this->window);
            break;
          case Tile::states::empty:
            down->setState(Tile::states::checked);
            down->setValue(this->solvingStep*10, this->calcHValue(xCoord, (yCoord + 1)));
            down->setPrevCoords(xCoord, yCoord); 
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
            this->endTile->setPrevCoords(xCoord, yCoord);
            this->animationCoords[0] = xCoord;
            this->animationCoords[1] = yCoord;
            this->stateEngine.setState(StateEngine::solved, this->window);
            break;
          case Tile::states::empty:
            left->setState(Tile::states::checked);
            left->setValue(this->solvingStep*10, this->calcHValue((xCoord - 1), yCoord));
            left->setPrevCoords(xCoord, yCoord); 
            this->visitedTilesInOrder.push_back(*left);
            break;
          default:
            break;
        }
      }
    
      int bestFCost = 99999;
      Tile *bestTile;
      for (int i=0; i<50; i++)
      {
        for (int j=0; j<50; j++)
        {
          Tile *checkedTile = this->grid[i][j];
          if (checkedTile->getState() == Tile::states::checked)
          {
            int checkedValue = checkedTile->getValue();
            if (checkedValue < bestFCost)
            {
              bestFCost = checkedValue;
              bestTile = checkedTile;
            }
          }
        }
      }
      bestTile->setState(Tile::states::closed);
      this->currentTile = bestTile;

      this->solveAStar();
    }
  }
  
  void solveDumby() {
    if (this->stateEngine.state != StateEngine::solved)
    {
      std::vector<Tile> neighborTiles;
      int xCoord = this->currentTile->getCoords(0);
      int yCoord = this->currentTile->getCoords(1);
    
      if (xCoord > 0)
      {
        Tile *left = this->grid[xCoord - 1][yCoord];
        switch (left->getState())
        {
          case Tile::states::end:
            this->endTile->setPrevCoords(xCoord, yCoord);
            this->animationCoords[0] = xCoord;
            this->animationCoords[1] = yCoord;
            this->stateEngine.setState(StateEngine::solved, this->window);
            break;
          case Tile::states::empty:
            left->setState(Tile::states::checked);
            left->setPrevCoords(xCoord, yCoord); 
            neighborTiles.push_back(*left);
            this->visitedTilesInOrder.push_back(*left);
            this->currentTile = left;
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
            this->endTile->setPrevCoords(xCoord, yCoord);
            this->animationCoords[0] = xCoord;
            this->animationCoords[1] = yCoord;
            this->stateEngine.setState(StateEngine::solved, this->window);
            break;
          case Tile::states::empty:
            down->setState(Tile::states::checked);
            down->setPrevCoords(xCoord, yCoord); 
            neighborTiles.push_back(*down);
            this->visitedTilesInOrder.push_back(*down);
            this->currentTile = down;
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
            this->endTile->setPrevCoords(xCoord, yCoord);
            this->animationCoords[0] = xCoord;
            this->animationCoords[1] = yCoord;
            this->stateEngine.setState(StateEngine::solved, this->window);
            break;
          case Tile::states::empty:
            right->setState(Tile::states::checked);
            right->setPrevCoords(xCoord, yCoord); 
            neighborTiles.push_back(*right);
            this->visitedTilesInOrder.push_back(*right);
            this->currentTile = right;
            break;
          default:
            break;
        }
      }

      if (yCoord > 0)
      {
        Tile *up = this->grid[xCoord][yCoord - 1];
        switch (up->getState())
        {
          case Tile::states::end:
            this->endTile->setPrevCoords(xCoord, yCoord);
            this->animationCoords[0] = xCoord;
            this->animationCoords[1] = yCoord;
            this->stateEngine.setState(StateEngine::solved, this->window);
            break;
          case Tile::states::empty:
            up->setState(Tile::states::checked);
            up->setPrevCoords(xCoord, yCoord); 
            neighborTiles.push_back(*up);
            this->visitedTilesInOrder.push_back(*up);
            this->currentTile = up;
            break;
          default:
            break;
        }
      }
      this->solveDumby();
    }
  }
  
  int calcHValue(int startx, int starty) {
    int x = abs(startx - this->endTile->getCoords(0));
    int y = abs(starty - this->endTile->getCoords(1));
    return static_cast<int>(sqrt((x * x) + (y * y)));
  }

  void animateVisited() {
    Tile *setTile = this->grid[this->visitedTilesInOrder[this->animationStep].getCoords(0)][this->visitedTilesInOrder[this->animationStep].getCoords(1)];
    if (setTile->getState() == Tile::states::closed)
    {
      setTile->setState(Tile::states::closedColor);
    }
    else
    {
      setTile->setState(Tile::states::visited);
    } 
    this->animationStep++;
    if (this->animationStep >= this->visitedTilesInOrder.size())
    {
      this->animationStep = 0;
      this->visitedTilesInOrder.clear();
      // this->visitedTilesInOrder.push_back(*this->startTile);
      this->drawnVisited = true;
    }
  }
  
  void animatePath() {
    if (this->animationCoords[0] == this->startTile->getCoords(0) && this->animationCoords[1] == this->startTile->getCoords(1))
    {
      this->drawnVisited = false;
      this->stateEngine.setState(StateEngine::buildSolved, this->window);
    }
    
    Tile *currentTile = this->grid[this->animationCoords[0]][this->animationCoords[1]];
    currentTile->setState(Tile::states::path);
    this->animationCoords[0] = currentTile->getPrevCoords(0);
    this->animationCoords[1] = currentTile->getPrevCoords(1);
  }
  
  void drawSolving() {
    for (int i=0; i<this->visitedTilesInOrder.size(); i++)
    {
      Tile *setTile = this->grid[this->visitedTilesInOrder[i].getCoords(0)][this->visitedTilesInOrder[i].getCoords(1)];
      if (setTile->getState() == Tile::states::closed)
      {
        setTile->setState(Tile::states::closedColor);
      }
      else
      {
        setTile->setState(Tile::states::visited);
      }
    }

    this->visitedTilesInOrder.clear();
    this->visitedTilesInOrder.push_back(*this->startTile);
    
    while (this->animationCoords[0] != this->startTile->getCoords(0) || this->animationCoords[1] != this->startTile->getCoords(1))
    {
      Tile *currentTile = this->grid[this->animationCoords[0]][this->animationCoords[1]];
      currentTile->setState(Tile::states::path);
      this->animationCoords[0] = currentTile->getPrevCoords(0);
      this->animationCoords[1] = currentTile->getPrevCoords(1);
    }
    
    this->stateEngine.setState(StateEngine::buildSolved, this->window);
  }
  
  void resetTileColors() {
    for (int i=0; i<50; i++)
    {
      for (int j=0; j<50; j++)
      {
        this->grid[i][j]->resetColor();
      }
    }
  }
  
  void solve() {
    this->clearGrid();
    switch (this->algorithm)
    {
      case dijkstra:
        this->startTile->setValue(0);
        this->unvisitedTiles.push_back(*this->startTile);
        this->solveDijkstra();
        break;
      case aStar:
        this->solvingStep = 0;
        this->currentTile = this->startTile;
        this->solveAStar();
        break;
      case dumby:
        this->currentTile = this->startTile;
        this->solveDumby();
        break;
    }
  }
};
