#include "Harmony/sfml/gui/gui.h"
#include "Harmony/arg_parser/template_version/arg_parser.h"

#include <cstdio>
#include <cstdlib>
#include <ctime>

#include <string>
#include <vector>
#include <array>

#include <iostream>
#include <fstream>

void align_text(const sf::RectangleShape& background_, sf::Text& text_) {
  if(text_.getString().getSize() != 0) {
    sf::Vector2f size = background_.getSize();
    // Select max font size
    unsigned int max_char_width = static_cast<unsigned int>(size.x) / text_.getString().getSize();
    text_.setCharacterSize(max_char_width < size.y ? max_char_width :
                                                     static_cast<unsigned int>(size.y));

    // Helper lambda to get offset vector for first position set
    auto center_pos_TL = [](sf::FloatRect out, sf::FloatRect in){
                                    return sf::Vector2f((out.width - in.width) / 2 + out.left,
                                                        (out.height - in.height) / 2 + out.top); };
    text_.setPosition(center_pos_TL(background_.getGlobalBounds(), text_.getGlobalBounds()));
    // Move based on text bounds
    text_.move(text_.getPosition() -
               sf::Vector2f(text_.getGlobalBounds().left, text_.getGlobalBounds().top));
  }
}

//http://www.cplusplus.com/forum/general/31270/
std::wstring FromUTF8(const char* str)
{
    const unsigned char* s = reinterpret_cast<const unsigned char*>(str);

    static const wchar_t badchar = '?';

    std::wstring ret;

    unsigned i = 0;
    while(s[i])
    {
        try
        {
            if(s[i] < 0x80)         // 00-7F: 1 byte codepoint
            {
                ret += s[i];
                ++i;
            }
            else if(s[i] < 0xC0)    // 80-BF: invalid for midstream
                throw 0;
            else if(s[i] < 0xE0)    // C0-DF: 2 byte codepoint
            {
                if((s[i+1] & 0xC0) != 0x80)		throw 1;

                ret +=  ((s[i  ] & 0x1F) << 6) |
                        ((s[i+1] & 0x3F));
                i += 2;
            }
            else if(s[i] < 0xF0)    // E0-EF: 3 byte codepoint
            {
                if((s[i+1] & 0xC0) != 0x80)		throw 1;
                if((s[i+2] & 0xC0) != 0x80)		throw 2;

                wchar_t ch =
                        ((s[i  ] & 0x0F) << 12) |
                        ((s[i+1] & 0x3F) <<  6) |
                        ((s[i+2] & 0x3F));
                i += 3;

                // make sure it isn't a surrogate pair
                if((ch & 0xF800) == 0xD800)
                    ch = badchar;

                ret += ch;
            }
            else if(s[i] < 0xF8)    // F0-F7: 4 byte codepoint
            {
                if((s[i+1] & 0xC0) != 0x80)		throw 1;
                if((s[i+2] & 0xC0) != 0x80)		throw 2;
                if((s[i+3] & 0xC0) != 0x80)		throw 3;

                unsigned long ch =
                        ((s[i  ] & 0x07) << 18) |
                        ((s[i+1] & 0x3F) << 12) |
                        ((s[i+2] & 0x3F) <<  6) |
                        ((s[i+3] & 0x3F));
                i += 4;

                // make sure it isn't a surrogate pair
                if((ch & 0xFFF800) == 0xD800)
                    ch = badchar;

                if(ch < 0x10000)	// overlong encoding -- but technically possible
                    ret += static_cast<wchar_t>(ch);
                else if(std::numeric_limits<wchar_t>::max() < 0x110000)
                {
                    // wchar_t is too small for 4 byte code point
                    //  encode as UTF-16 surrogate pair

                    ch -= 0x10000;
                    ret += static_cast<wchar_t>( (ch >> 10   ) | 0xD800 );
                    ret += static_cast<wchar_t>( (ch & 0x03FF) | 0xDC00 );
                }
                else
                    ret += static_cast<wchar_t>(ch);
            }
            else                    // F8-FF: invalid
                throw 0;
        }
        catch(int skip)
        {
            if(!skip)
            {
                do
                {
                    ++i;
                }while((s[i] & 0xC0) == 0x80);
            }
            else
                i += skip;
        }
    }

    return ret;
}

class Card {

  std::vector<std::wstring> faces_;

public:

  explicit Card(std::wstring);
  const std::wstring& get_face(size_t n) const { return faces_[n % faces_.size()]; }

};

Card::Card(std::wstring line) {
  size_t pos = 0;
  size_t prev_pos = 0;
  do {
    pos = line.find(',', prev_pos);
    faces_.push_back(line.substr(prev_pos, pos - prev_pos));
    prev_pos = pos + 1;
  } while(pos != std::string::npos);
}

class Deck {

  std::vector<Card> cards_;

public:

  explicit Deck(std::ifstream&);

  size_t size() const { return cards_.size(); }
  const Card& get_card(size_t n) const { return cards_[n % cards_.size()]; }
  void remove_card(size_t n) {
    if(!cards_.empty()) {
      cards_.erase(cards_.begin() + (n % cards_.size()), cards_.begin() + (n % cards_.size()) + 1);
    }
  }
};

Deck::Deck(std::ifstream& stream) {
  std::string line;
  while(std::getline(stream, line)) {
    cards_.emplace_back(FromUTF8(line.c_str()));
  }
}

// TO DO : Loop through all option
// TO DO : Error on removing all
// TO DO : Star option
// TO DO : Writing option
// TO DO : Not possible to land on same card 2x
// TO DO : Start from english side?
int main(int argc, char** argv) {
  XInitThreads();

  srand(time(NULL));

  Parser parser(argc, argv, "Notecards application accepting a csv file for notecards");
  parser.add_argument<std::string>("-f","--file","This is the csv notecard file", true);

  sf::RenderWindow window(sf::VideoMode(1300, 1000), "NOTECARDS");
  window.setFramerateLimit(60);

  const sf::Vector2u window_size(window.getSize());
  const float x_scale(window_size.x * 1.f / window_size.y);

  std::vector<sf::Font> font_vec(2);
  // TO DO : No number
  const std::array<std::string, 2> font_paths({"/usr/share/fonts/umeboshi_.ttf",
                                               "/usr/share/fonts/KosugiMaru-Regular.ttf"});
  
  for(size_t inc = 0; const auto& path : font_paths) {
    if(!font_vec[inc].loadFromFile(font_paths[inc])) {
      throw std::runtime_error("No def file : " + font_paths[inc] + " found!");
    }
    ++inc;
  }

  Gui gui(window, font_vec[0]);
  
  size_t rhs_width = window_size.x - window_size.y;
  Background back(window_size.y, window_size.y, 0, 0, sf::Color(100, 100, 100));
  Background rhs(rhs_width, window_size.y, window_size.y, 0);
  gui.add_background(back, rhs);

  size_t button_width = (rhs_width) * .90;
  Push_Button flip(button_width, 100,
                   window_size.y + (rhs_width - button_width) / 2, window_size.y / 3, "FLIP");
  Push_Button next(button_width, 100,
                   window_size.y + (rhs_width - button_width) / 2, window_size.y / 3 + 120, "NEXT");
  Push_Button rem(button_width / 2 - 5, 100,
                  window_size.y + (rhs_width - button_width) / 2, window_size.y / 3 + 240, "REM");
  Push_Button reset(button_width / 2 - 5, 100, window_size.y + rhs_width / 2 + 5, 
                    window_size.y / 3 + 240, "RES");
  gui.add_push_button(next, flip, rem, reset);

  sf::RectangleShape card(sf::Vector2f(window_size.x / 2, window_size.y / 2));
  card.setPosition(sf::Vector2f(window_size.x / 8, window_size.y / 5));
  card.setFillColor(sf::Color(200, 200, 200));

  std::ifstream deck_file(parser.get<std::string>("file"));
  // TO DO : Error checking on file
  Deck original_deck(deck_file);
  Deck current_deck(original_deck);
  
  sf::Text card_text;
  card_text.setFillColor(sf::Color(30, 30, 30));

  size_t face_idx = 0;
  size_t card_idx = rand() % current_deck.size();
  size_t font_idx = rand() % font_vec.size();

  card_text.setString(current_deck.get_card(card_idx).get_face(face_idx));
  card_text.setFont(font_vec[font_idx]);
  align_text(card, card_text);

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

    auto gui_state = gui.get_state();
    if(gui_state[next()]) {
      face_idx = 0;
      card_idx = rand() % current_deck.size();
      font_idx = rand() % font_vec.size();
      card_text.setString(current_deck.get_card(card_idx).get_face(face_idx));
      card_text.setFont(font_vec[font_idx]);
      align_text(card, card_text);
    }
    if(gui_state[flip()]) {
      ++face_idx;
      card_text.setString(current_deck.get_card(card_idx).get_face(face_idx));
      align_text(card, card_text);
    }
    if(gui_state[rem()]) {
      current_deck.remove_card(card_idx);
      if(current_deck.size() != 0) {
        face_idx = 0;
        card_idx = rand() % current_deck.size();
        font_idx = rand() % font_vec.size();
        card_text.setString(current_deck.get_card(card_idx).get_face(face_idx));
        card_text.setFont(font_vec[font_idx]);
        align_text(card, card_text);
      } else {
        card_text.setString("");
      }
    }
    if(gui_state[reset()]) {
      current_deck = original_deck;
      face_idx = 0;
      card_idx = rand() % current_deck.size();
      font_idx = rand() % font_vec.size();
      card_text.setString(current_deck.get_card(card_idx).get_face(face_idx));
      card_text.setFont(font_vec[font_idx]);
      align_text(card, card_text);
    }

    // Draw
    window.clear();

    gui.draw(window);

    window.draw(card); 
    window.draw(card_text); 

    window.display();

  } // end game loop

  return 0;

}
