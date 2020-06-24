#include "charged_particles_gui.h"

#include <ctime> // time
#include <cstdlib> // srand

int main() {

  XInitThreads();

  srand(time(nullptr));

  // window
  const sf::Vector2u window_size(1300, 1000);
  sf::RenderWindow window(sf::VideoMode(window_size.x, window_size.y), "Charged Particles");
  window.setFramerateLimit(60);

  sf::Font font;
  if(!font.loadFromFile("../rsc/fonts/FreeMonoBold.ttf")) {
    throw std::runtime_error("No font file :  ""../rsc/fonts/FreeMonoBold.ttf"
                             " found!");
  }

  Charged_Particle_Gui gui(window, window_size, font);

  std::vector<Point> points;

  sf::Event event;
  while(window.isOpen()) {

    // INPUT
    while(window.pollEvent(event)) {
      switch(event.type) {
        case sf::Event::Closed:
          window.close();
          break;
        default:
          break;
      }
    } // end event loop

    // UPDATE
    gui.update(window, points);

    // DRAW
    window.clear();
    for(const auto& point : points) point.draw(window);
    gui.draw(window);
    window.display();
    
  } // end game loop

  return 0;
}
