#ifndef _BAZ_GW2TP_DATABASE_HPP
#define _BAZ_GW2TP_DATABASE_HPP
#include <string>
#include <functional>
#include "Item.hpp"

void itemlistload(std::string filename, int *itemlist, int count, const std::function<void(std::string)>&);
void itemdbload(std::string filename, Item *itemdb, int count);
int itemdbintegrity(std::string filename);
#endif