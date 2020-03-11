#include "k_near_gui.h"

#include <map>

#include <memory> // shared_ptr
#include <stdexcept> // runtime_error

// TO DO : Labels & type detection
// TO DO : weight algorithms, ...

int main() {

  XInitThreads();

  // window
  const sf::Vector2u window_size(1300, 1024);
  sf::RenderWindow window(sf::VideoMode(window_size.x, window_size.y), "K-Nearest");
  window.setFramerateLimit(60);

  sf::Font font;
  if(!font.loadFromFile("/usr/share/fonts/truetype/freefont/FreeMonoBold.ttf")) {
    throw std::runtime_error("No font file :""/usr/share/fonts/truetype/freefont/FreeMonoBold.ttf"
                             " found!");
  }
  // Initializations
  size_t neighbors = 0;
  std::map<Mode, std::shared_ptr<Mode_Base>> state;
  state.emplace(int_, std::make_shared<Mode_State<int>>());
  state.emplace(float_, std::make_shared<Mode_State<float>>());

  K_Near_Gui gui(window, font, neighbors);

  sf::Event event;
  while(window.isOpen()) {

    // INPUT
    while(window.pollEvent(event)) {
      switch(event.type) {
        case sf::Event::Closed:
          window.close();
          break;
        default: break;
      }
    } // end event loop

    // UPDATE
    gui.update(window, state, neighbors);

    // DRAW
    window.clear();
    gui.draw(window, state, neighbors);
    window.display();

  } // end game loop

  return 0;
}
