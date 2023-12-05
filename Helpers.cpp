#include "headers/Helpers.h"
// Реалізація функції для порівняння ключів
size_t KeyHash::operator()(const StateSymbolPair &k) const
{
    return std::hash<int>()(k.first) ^ std::hash<char>()(k.second);
}

// Реалізація функції для порівняння значень
bool KeyEqual::operator()(const StateSymbolPair &lhs, const StateSymbolPair &rhs) const
{
    return lhs.first == rhs.first && lhs.second == rhs.second;
}