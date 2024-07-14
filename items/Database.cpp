#include "Database.hpp"
#include <sstream>
#include <stdio.h>
#include <iostream>
#include <fstream>


void itemlistload(std::string filename, int *itemlist, int count, const std::function<void(std::string)>& initItemsListDB)
{
	std::fstream itemliststream(filename, std::ios_base::in);
	std::string itemliststring;
	if (!itemliststream.is_open())
	{
		std::cout << "item file not open bob";
		initItemsListDB(filename);
	}
	for (int i = 0; i < count; i++)
	{
		itemliststream >> itemliststring;
		itemlist[i] = std::stoi(itemliststring);
	}
	itemliststream.close();
}
void itemdbload(std::string filename, Item *itemdb, int count)
{
	std::fstream itemdbstream(filename, std::ios_base::in);
	std::string itemdbstring;
	if (!itemdbstream.is_open())
		std::cout << "itemdb file not open bob";
	else
	{
		for (int i = 0; i < count; i++)
		{
			if (!itemdbstream.eof())
				itemdbstream >> itemdb[i];
		}
	}
	itemdbstream.close();
}
int itemdbintegrity(std::string filename)
{
	std::fstream itemdbstream(filename, std::ios_base::in);
	std::string itemdbstring;
	int line = 0;
	if (!itemdbstream.is_open())
		return -1;
	else
	{
		while (std::getline(itemdbstream, itemdbstring))
			++line;
		itemdbstream.close();
		return line;
	}
}