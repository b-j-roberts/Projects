#include "Harmony/arg_parser/template_version/arg_parser.h"

#include "notecards_gui.h"

#include <cstdlib> // srand
#include <ctime> // time

#include <array>

// TO DO : Loop through all option
// TO DO : Star option
// TO DO : Writing option
// TO DO : Not possible to land on same card 2x
// TO DO : Start from different side
int main(int argc, char** argv) {
  XInitThreads();
  srand(time(nullptr));

  // Setup Parser
  Parser parser(argc, argv, "Notecards application accepting a csv file for notecards");
  parser.add_argument<std::string>("-f","--file","This is the csv notecard file", true);

  // Setup window
  sf::RenderWindow window(sf::VideoMode(1300, 1000), "NOTECARDS");
  window.setFramerateLimit(60);
  const sf::Vector2u window_size(window.getSize());
  //const float x_scale(window_size.x * 1.f / window_size.y);

  // Setup fonts
  constexpr size_t num_of_fonts = 2; // TO DO
  std::vector<sf::Font> font_vec(num_of_fonts);
  const std::array<std::string, num_of_fonts> font_paths({"./fonts/umeboshi_.ttf",
                                                          "./fonts/KosugiMaru-Regular.ttf"});
  for(size_t inc = 0; const auto& path : font_paths) {
    if(!font_vec[inc].loadFromFile(path)) {
      throw std::runtime_error("No def file : " + path + " found!");
    }
    ++inc;
  }

  // Load Deck
  std::ifstream deck_file(parser.get<std::string>("file"));
  if(!deck_file.is_open()) { throw std::runtime_error("Error opening file : " + parser.get<std::string>("file")); }
  Deck original_deck(deck_file);
  Deck current_deck(original_deck);
  deck_file.close();

  // GUI
  Notecards_Gui gui(window, window_size, font_vec, current_deck);
  
  // Game loop
  sf::Event event;
  while(window.isOpen()) {

    // Input
    while(window.pollEvent(event)) {
      switch(event.type) {

        case sf::Event::KeyPressed:
          case sf::Event::Closed:
            window.close();
            break;

        default: break;
      }
    } // end event loop

    // Update
    gui.update(current_deck, original_deck);

    // Draw
    window.clear();
    gui.draw(window, current_deck, original_deck);
    window.display();

  } // end game loop

  return 0;
}
