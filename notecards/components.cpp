#include "components.h"
#include <iostream>
#include <algorithm>

// Convert const char* to std::wstring ( using UTF8 characters )
// from : http://www.cplusplus.com/forum/general/31270/
std::wstring to_Wstring(const char* str) {
  const unsigned char* s = reinterpret_cast<const unsigned char*>(str);
  static const wchar_t badchar = '?';
  std::wstring ret;

  unsigned i = 0;
  while(s[i]) {
    try {
      if(s[i] < 0x80) { // 00-7F: 1 byte codepoint
        ret += s[i];
        ++i;
      } else if(s[i] < 0xC0) { // 80-BF: invalid for midstream
        throw 0;
      } else if(s[i] < 0xE0) { // C0-DF: 2 byte codepoint
        if((s[i+1] & 0xC0) != 0x80) throw 1;
        ret +=  ((s[i] & 0x1F) << 6) | ((s[i+1] & 0x3F));
        i += 2;
      } else if(s[i] < 0xF0) { // E0-EF: 3 byte codepoint
        if((s[i+1] & 0xC0) != 0x80) throw 1;
        if((s[i+2] & 0xC0) != 0x80) throw 2;
        wchar_t ch = ((s[i] & 0x0F) << 12) | ((s[i+1] & 0x3F) <<  6) | (s[i+2] & 0x3F);
        i += 3;
        // make sure it isn't a surrogate pair
        if((ch & 0xF800) == 0xD800) ch = badchar;
        ret += ch;
      } else if(s[i] < 0xF8) { // F0-F7: 4 byte codepoint
        if((s[i+1] & 0xC0) != 0x80)   throw 1;
        if((s[i+2] & 0xC0) != 0x80)   throw 2;
        if((s[i+3] & 0xC0) != 0x80)   throw 3;
        unsigned long ch = static_cast<unsigned long>(
                               ((s[i] & 0x07) << 18) | ((s[i+1] & 0x3F) << 12) |
                               ((s[i+2] & 0x3F) <<  6) | ((s[i+3] & 0x3F)));
        i += 4;
        // make sure it isn't a surrogate pair
        if((ch & 0xFFF800) == 0xD800) ch = badchar;

        if(ch < 0x10000) ret += static_cast<wchar_t>(ch); // overlong encoding
        else if(std::numeric_limits<wchar_t>::max() < 0x110000) {
          // wchar_t is too small for 4 byte code point
          //  encode as UTF-16 surrogate pair

          ch -= 0x10000;
          ret += static_cast<wchar_t>((ch >> 10   ) | 0xD800 );
          ret += static_cast<wchar_t>((ch & 0x03FF) | 0xDC00 );
        } else ret += static_cast<wchar_t>(ch);
      } else throw 0; // F8-FF: invalid
    } catch(unsigned skip) {
      if(!skip) {
        do { ++i; } while((s[i] & 0xC0) == 0x80);
      }
      else i += skip;
    }
  }
  return ret;
}

Card::Card(const std::wstring& line, char delim) {
  size_t pos = 0;
  size_t prev_pos = 0;
  do { // split on delim
    pos = line.find(delim, prev_pos);
    faces_.push_back(line.substr(prev_pos, pos - prev_pos));
    prev_pos = pos + 1;
  } while(pos != std::string::npos);
}

void Deck::remove_card(size_t n) {
  if(!cards_.empty()) {
    long pos = static_cast<long>(n % cards_.size());
    cards_.erase(cards_.begin() + pos, cards_.begin() + pos + 1);
  }
}

Deck::Deck(std::ifstream& stream, int numberOfCards) {
  std::string line;
  std::vector<Card> newCards;
  while(std::getline(stream, line)) {
    newCards.emplace_back(to_Wstring(line.c_str()));
  }

  int m = newCards.size() > numberOfCards ? numberOfCards : newCards.size();
  // Find m unique random numbers
  std::vector<int> random_positions;
  for(int i = 0; i < m; ++i) {
     int r;
     do {
        r = rand() % newCards.size();
     } while(std::count(random_positions.begin(), random_positions.end(), r));
     random_positions.push_back(r); 
  }

  for(int i = 0; i < random_positions.size(); ++i) {
    cards_.push_back(newCards[random_positions[i]]);
  }

}
