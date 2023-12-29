#ifndef HELPERS_H
#define HELPERS_H

#include <functional>

// Визначення StateSymbolPair (якщо це ще не зроблено)
using StateSymbolPair = std::pair<int, char>;

// Функція для порівняння ключів у unordered_map
struct KeyHash
{
    size_t operator()(const StateSymbolPair &k) const;
};

// Функція для порівняння значень у unordered_map
struct KeyEqual
{
    bool operator()(const StateSymbolPair &lhs, const StateSymbolPair &rhs) const;
};

#endif // HELPERS_H