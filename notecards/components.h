#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <string>
#include <vector>

#include <fstream>

// Stores all the faces of a card as wstrings
class Card {

  std::vector<std::wstring> faces_;

public:
  // Constuct faces_ by splitting wstring on delim
  explicit Card(const std::wstring&, char delim = ',');
  const std::wstring& get_face(size_t n) const { return faces_[n % faces_.size()]; }
};

// Stores deck of Cards ( able to get and remove cards after construction )
class Deck {

  std::vector<Card> cards_;

public:
  // Convert file into a deck of cards_, each line corresponds to a Card ( delimited faces )
  explicit Deck(std::ifstream&);
  size_t size() const { return cards_.size(); }
  const Card& get_card(size_t n) const { return cards_[n % cards_.size()]; }
  // Remove card in cards_ in position n % size
  void remove_card(size_t n);
};

#endif
