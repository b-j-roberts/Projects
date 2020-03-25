#ifndef NOTECARDS_GUI_H
#define NOTECARDS_GUI_H

#include "components.h"

#include "Harmony/sfml/gui/gui.h"

class Notecards_Gui {

  Gui gui_;
  std::vector<sf::Font> fonts_;

  const size_t rhs_width;
  const size_t button_width;
  const sf::Vector2u start;

  Push_Button flip;
  Push_Button next;
  Push_Button rem;
  Push_Button reset;

  size_t face_idx = 0;
  size_t card_idx;
  size_t font_idx;
  // TO DO : Should I store the current deck in here? Or at least a reference to it?

  sf::RectangleShape card; // TO DO : Change to Text_Display ( after allowing wstring )
  sf::Text card_text;

  // Change the text in card to wstring ( resizing, aligning, and changes font )
  void set_text(const std::wstring&);
  // Grab the next card randomly out of deck ( changes idxs and calls set_text )
  void next_card(const Deck&);

public:

  // params : window, window_size, vector of allowed fonts, deck
  Notecards_Gui(sf::RenderWindow&, const sf::Vector2u&, const std::vector<sf::Font>&, const Deck&);
  // Updates idx, deck & card based on gui_state
  // params: current deck, original deck
  void update(Deck&, const Deck&);
  // Draws gui_, card, and progress bar to the screen
  // params: window, current deck, original deck
  void draw(sf::RenderWindow&, const Deck&, const Deck&) const;
};

#endif
