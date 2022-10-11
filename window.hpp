#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window/VideoMode.hpp>

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
  }
  
  void render() {
    this->window->clear(sf::Color::White);
    this->window->display();
  }
  
private:
  sf::RenderWindow *window;
  sf::VideoMode videoMode;
  sf::Event event;
  

  void initVariables() {
    this->window = nullptr;
  }
  
  void initWindow() {
    this->videoMode.height = 500;
    this->videoMode.width = 1000;
    
    this->window = new sf::RenderWindow(this->videoMode, "Pathfinding Visualizer");
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
};

class State {
public:

private:
};

class StateEngine {
public:

private:
};