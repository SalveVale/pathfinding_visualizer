#include "window.hpp"

#include <iostream>

int main() {
  Window window;
  
  while (window.isOpen())
  {
    window.update();
    window.render();
  }
}
