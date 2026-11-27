/**
 * @file TextUtil.hpp
 * @brief Small string helpers shared by the parser and the game text.
 */
#ifndef TEXTUTIL_HPP_INCLUDED
#define TEXTUTIL_HPP_INCLUDED

#include <string>
#include <vector>

/**
 * @brief Trims, lowercases and collapses runs of whitespace to one space.
 */
std::string normalize(const std::string& s);

/**
 * @brief Joins words for a sentence: "a", "a and b", "a, b and c".
 */
std::string joinList(const std::vector<std::string>& items);

#endif  // TEXTUTIL_HPP_INCLUDED
