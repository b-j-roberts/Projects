#include "notecards_gui.h"

#include <cstdlib> // rand

// Helper : Align sf::Text to middle of sf::RectangleShape, setting appropriate font size for text
// NOTE : This is the same as GUI_Sizing in Harmony/sfml/gui/gui.h, but I couldn't use Text_Display
//        because they cannot take wstrings as of now
void align_text(const sf::RectangleShape& background, sf::Text& text) {
  if(text.getString().getSize() != 0) {
    sf::Vector2f size = background.getSize();
    // Select max font size
    unsigned int max_char_width = static_cast<unsigned int>(size.x) / text.getString().getSize();
    text.setCharacterSize(max_char_width < size.y ? max_char_width :
                                                    static_cast<unsigned int>(size.y));

    // Helper lambda to get offset vector for first position set
    auto center_pos_TL = [](sf::FloatRect out, sf::FloatRect in) {
                               return sf::Vector2f((out.width - in.width) / 2 + out.left,
                                                   (out.height - in.height) / 2 + out.top); };
    text.setPosition(center_pos_TL(background.getGlobalBounds(), text.getGlobalBounds()));
    // Move based on text bounds
    text.move(text.getPosition() -
              sf::Vector2f(text.getGlobalBounds().left, text.getGlobalBounds().top));
  }
}

Notecards_Gui::Notecards_Gui(sf::RenderWindow& window, const sf::Vector2u& window_size,
                             const std::vector<sf::Font>& fonts, const Deck& deck):
  gui_(window, fonts[0]), // TO DO : If no fonts?
  fonts_(fonts),
  rhs_width(window_size.x - window_size.y),
  button_width(static_cast<size_t>(rhs_width * .9)),
  start(window_size.y + (rhs_width - button_width) / 2, window_size.y / 3),
  flip(button_width, 100, start.x, start.y, "FLIP"),
  next(button_width, 100, start.x, start.y + 120, "NEXT"),
  rem(button_width / 2 - 5, 100, start.x, start.y + 240, "REM"),
  reset(button_width / 2 - 5, 100, start.x + button_width / 2 + 5, start.y + 240, "RES"),
  card_idx(static_cast<unsigned long>(rand()) % deck.size()),
  font_idx(static_cast<unsigned long>(rand()) % fonts_.size()),
  card(sf::Vector2f(window_size.x / 2, window_size.y / 2)) {
  // setup card
  card.setPosition(sf::Vector2f(window_size.x / 8, window_size.y / 5));
  card.setFillColor(sf::Color(200, 200, 200));
  card_text.setFillColor(sf::Color(30, 30, 30));
  next_card(deck);
  // Add overall background, rhs background, and card shadow
  gui_.add_background(Background(window_size.y, window_size.y, 0, 0, sf::Color(100, 100, 100)),
                      Background(rhs_width, window_size.y, window_size.y, 0),
                      Background(window_size.x / 2, window_size.y / 2, window_size.x / 8 + 20,
                                 window_size.y / 5 + 20, sf::Color(30, 30, 30, 30)));
  gui_.add_push_button(flip, next, rem, reset);
}

void Notecards_Gui::set_text(const std::wstring& text) {
  card_text.setString(text);
  card_text.setFont(fonts_[font_idx]);
  align_text(card, card_text);
}

void Notecards_Gui::next_card(const Deck& deck) {
  card_idx = static_cast<unsigned long>(rand()) % deck.size();
  face_idx = static_cast<unsigned long>(rand()) % deck.get_card(card_idx).size();
  font_idx = static_cast<unsigned long>(rand()) % fonts_.size();
  set_text(deck.get_card(card_idx).get_face(face_idx));
}

void Notecards_Gui::update(Deck& deck, const Deck& original) {
  const auto& gui_state = gui_.get_state();
  for(const auto& [id, val] : gui_state) {
    if(val) { // GUI - Code interaction if state is true
      if(id == flip.id()) { // flip face
        ++face_idx;
        set_text(deck.get_card(card_idx).get_face(face_idx));
      } else if(id == next.id()) { // next card
        next_card(deck);
      } else if(id == rem.id()) { // remove card
        deck.remove_card(card_idx);
        if(deck.size() != 0) next_card(deck);
        else card_text.setString("");
      } else if(id == reset.id()) { // reset deck
        deck = original;
        next_card(deck);
      }
    }
  }
}

void Notecards_Gui::draw(sf::RenderWindow& window, const Deck& deck, const Deck& original) const {
  // Gui & Card
  gui_.draw(window);
  window.draw(card);
  window.draw(card_text);
  
  // Overall Load Bar
  const sf::Vector2u window_size(window.getSize());
  sf::RectangleShape load_bar(sf::Vector2f(window_size.x / 2, 10));
  load_bar.setPosition(sf::Vector2f(window_size.x / 8, window_size.y * 7 / 8));
  load_bar.setFillColor(sf::Color(255, 0, 0));
  load_bar.setOutlineThickness(2);
  load_bar.setOutlineColor(sf::Color(200, 200, 200));
  window.draw(load_bar);

  // Progress Bar
  float percent = 1.f - static_cast<float>(deck.size()) / static_cast<float>(original.size());
  sf::RectangleShape completed_bar(sf::Vector2f(load_bar.getSize().x * percent, 10));
  completed_bar.setPosition(sf::Vector2f(window_size.x / 8, window_size.y * 7 / 8));
  completed_bar.setFillColor(sf::Color(0, 255, 0));
  window.draw(completed_bar);
}
