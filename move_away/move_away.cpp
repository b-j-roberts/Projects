#include "move_away_gui.h"

#include <cstdio> // NULL
#include <ctime> // time

int main() {

  XInitThreads();

  srand(time(NULL));

  // window
  const sf::Vector2u window_size(1300, 1000);
  sf::RenderWindow window(sf::VideoMode(window_size.x, window_size.y), "Move Away!");
  window.setFramerateLimit(60);

  sf::Font font;
  if(!font.loadFromFile("/usr/share/fonts/truetype/freefont/FreeMonoBold.ttf")) {
    throw std::runtime_error("No font file :""/usr/share/fonts/truetype/freefont/FreeMonoBold.ttf"
                             " found!");
  }

  Move_Away_Gui gui(window, window_size, font);

  //const size_t amount = 1500; // MAX
  const size_t amount = 400;
  std::vector<Point> points;
  for(size_t i = 0; i < amount; ++i) {
    points.emplace_back(static_cast<int>(window_size.y), static_cast<int>(window_size.y));
  }
  /*std::vector<Source> sources;
  for(size_t i = 0; i < 10; ++i) {
    sources.emplace_back(rand() % window_size.y, rand() % window_size.y, rand() % 11 - 5);
  }*/

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
    gui.update(window_size, points);

    // DRAW
    window.clear();
    for(const auto& point : points) point.draw(window);
    //for(const auto& source : sources) source.draw(window);
    gui.draw(window);
    window.display();
    
  } // end game loop

  return 0;

}
