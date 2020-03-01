#include "Harmony/sfml/gui/gui.h"

#include <array>
#include <map>

#include <iostream> // TO DO

enum Pieces { pawn = 0, rook, bishop, knight, queen, king, none };

struct Game_Position {
  Pieces piece = none;
  size_t player_number = 0; // 0 - none, 1 - top, 2 - bot
};

template<size_t I>
class Game_Board {
protected:
  std::array<std::array<Game_Position, I>, I> board_;
public:
  const Game_Position& get_position(size_t x, size_t y) const {
    return board_.at(y).at(x); // TO DO : Index checking
  }
};

class Chess_Board : public Game_Board<8> { // TO DO : Template?
public:
  Chess_Board() {
    board_[0][0] = board_[0][7] = Game_Position{rook, 1};
    board_[0][1] = board_[0][6] = Game_Position{knight, 1};
    board_[0][2] = board_[0][5] = Game_Position{bishop, 1};
    board_[0][3] = Game_Position{queen, 1};
    board_[0][4] = Game_Position{king, 1};
    for(auto& pos : board_[1]) { pos.piece = pawn; pos.player_number = 1; } 

    board_[7][0] = board_[7][7] = Game_Position{rook, 2};
    board_[7][1] = board_[7][6] = Game_Position{knight, 2};
    board_[7][2] = board_[7][5] = Game_Position{bishop, 2};
    board_[7][3] = Game_Position{queen, 2};
    board_[7][4] = Game_Position{king, 2};
    for(auto& pos : board_[6]) { pos.piece = pawn; pos.player_number = 2; } 
  } 
};

int main() {
  
  XInitThreads();

  //const sf::Vector2u window_size(1300, 1000);
  const sf::Vector2u window_size(1400, 1000);
  sf::RenderWindow window(sf::VideoMode(window_size.x, window_size.y), "Chess");
  window.setFramerateLimit(60);

  sf::Font font;
  if(!font.loadFromFile("/usr/share/fonts/truetype/freefont/FreeMonoBold.ttf")) {
    throw std::runtime_error("No font file :""/usr/share/fonts/truetype/freefont/FreeMonoBold.ttf"
                             " found!");
  }

  Gui gui(window, font);

  //sf::Color board_white = sf::Color(180, 180, 180);
  //sf::Color board_black = sf::Color(20, 20, 20);
  sf::Color board_white = sf::Color(145, 80, 60);
  sf::Color board_black = sf::Color(80, 30, 41);

  //Board
  const size_t board_size = 8;
  const size_t section_dim = window_size.y / board_size;
  std::vector<std::vector<Push_Button>> button_board(board_size);
  for(size_t row_num = 0; auto& row : button_board) {
    for(size_t j = 0; j < board_size; ++j) {
      sf::Color color = ((j + row_num) % 2) == 0 ? board_white : board_black;
      row.emplace_back(section_dim, section_dim, j * section_dim, row_num * section_dim,
                       "", color, color);
      gui.add_push_button(*(row.end() - 1));
    }
    ++row_num;
  }

  Chess_Board board;

  // RHS
  const size_t rhs_border_size = 10;
  Background rhs_border(window_size.x - window_size.y, window_size.y, 
                        window_size.y, 0, board_black * board_white);
  Background rhs_back((window_size.x - window_size.y) - 2 * rhs_border_size, 
                      window_size.y - 2 * rhs_border_size, window_size.y + rhs_border_size, 
                      rhs_border_size, board_black);
  gui.add_background(rhs_border, rhs_back);

  sf::Event event;
  while(window.isOpen()) {

    // INPUT
    while(window.pollEvent(event)) {
      switch(event.type) {
        case sf::Event::KeyPressed:
          switch(event.key.code) {
            default: break;
          }
          break;
        
        case sf::Event::Closed:
          window.close();
          break;

        default: break;
      }
    } // end event loop

    auto gui_state = gui.get_state();

    for(size_t y = 0; const auto& row : button_board) {
      for(size_t x = 0; const auto& pos : row) {
        if(gui_state[pos()]) std::cout << board.get_position(x, y).piece << '\n';
        ++x;
      }
      ++y;
    }

    window.clear();

    gui.draw(window);

    window.display();

  } // end game loop

}
