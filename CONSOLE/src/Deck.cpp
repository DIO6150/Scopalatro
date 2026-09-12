#include "Deck.hpp"

void Deck::addCard(std::unique_ptr<Card> card) {
    cards.push_back(std::move(card));
}

void Deck::shuffle() {
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::shuffle(cards.begin(), cards.end(), std::default_random_engine(seed));
}

std::unique_ptr<Card> Deck::draw() {
    if (cards.empty()) {
        return nullptr;
    }
    std::unique_ptr<Card> drawnCard = std::move(cards.back());
    cards.pop_back();
    return drawnCard;
}

bool Deck::isEmpty() const {
    return cards.empty();
}

size_t Deck::size() const {
    return cards.size();
}

void Deck::addCards(std::vector<std::unique_ptr<Card>>& pile) {
    for (auto& card : pile) {
        cards.push_back(std::move(card));
    }
    pile.clear();
}