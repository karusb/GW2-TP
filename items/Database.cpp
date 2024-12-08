#include "Database.hpp"
#include <sstream>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <algorithm>

std::uint64_t DatabaseIntegrityChecker::CheckLines(std::string filename)
{
	std::fstream db(filename, std::ios_base::in);
	std::string string;
	std::uint64_t line = 0;
	if (!db.is_open())
		return -1;
	else
	{
		while (std::getline(db, string))
			++line;
		db.close();
		return line;
	}
}

bool DatabaseIntegrityChecker::Exists(std::string filename)
{
	std::ifstream filecheck;
	filecheck.open(filename);
	return filecheck.is_open();
}

NameDatabase::NameDatabase(std::string filename)
	: Database(filename)
{}

bool NameDatabase::Write(const std::vector<ItemIdentifier>&itemList)
{
	std::fstream file(filename);

	if (file.is_open())
	{
		file << std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()) << std::endl;
		file << itemList;
	}
	else 
		return false;
	return true;
}

std::time_t NameDatabase::LoadItemListDB()
{
	std::fstream file(filename, std::ios_base::in);	
	std::time_t timeDuration = 0;
	storage.clear();
	if (file.is_open())
	{
		file >> timeDuration;
		while (!file.eof())
			file >> storage;
	}
	return timeDuration;
}

std::time_t NameDatabase::Load()
{
	if (Exists())
		return LoadItemListDB();
	return 0;
}

ItemNameExtended NameDatabase::ExtendItemFromDB(const Item& item)
{
	for (const auto& exitem : storage)
		if (exitem.id == item.getid())
			return {item, exitem.name, exitem.rarity};
	std::string unknownItemName = "Unknown Item ID ";
	unknownItemName.append(std::to_string(item.getid()));
	return {item, unknownItemName, Rarity::Junk};
}

IdDatabase::IdDatabase(std::string filename)
	: Database(filename)
{}

bool IdDatabase::itemlistload()
{
	std::fstream db(filename, std::ios_base::in);
	std::string string;
	storage.clear();
	if (!db.is_open())
		return false;

	db >> string;
	while (!db.eof() && !string.empty())
	{
		storage.push_back(std::stoi(string));
		db >> string;
	}

	db.close();
	return true;
}

bool IdDatabase::Load()
{
	if (Exists())
		return itemlistload();
	return false;
}

void IdDatabase::WriteRaw(std::string contents)
{
	std::ofstream db = DbOutStream();
	db.seekp(0);
	db << contents;
	db.close();
}

PriceDatabase::PriceDatabase(std::string filename)
	: Database(filename)
{}

std::time_t PriceDatabase::itemdbload()
{
	std::fstream db(filename, std::ios_base::in);
	std::string string;
	std::time_t timeDuration = 0;
	storage.clear();
	if (!db.is_open())
		std::cout << "Price Database file cannot be opened";
	else
	{
		db >> timeDuration;
		while (!db.eof())
			db >> storage;
	}
	db.close();
	return timeDuration;
}

std::time_t PriceDatabase::Load()
{
	if (Exists())
		dbTimePoint = std::chrono::system_clock::from_time_t(itemdbload());
	else
		return 0;
	return std::chrono::system_clock::to_time_t(dbTimePoint);
}

void PriceDatabase::WriteBack()
{
	std::ofstream db = DbOutStream();
	dbTimePoint = std::chrono::system_clock::now();
	db << std::chrono::system_clock::to_time_t(dbTimePoint) << std::endl;
	db << storage;
	db.close();
}

Item PriceDatabase::ItemFromDB(std::uint64_t id)
{
	for (const auto& item : storage)
		if (item.getid() == id)
			return item;
	return {};
}

FavouritesDatabase::FavouritesDatabase(std::string filename)
	: Database(filename)
{}

bool FavouritesDatabase::Write(const std::vector<std::uint64_t>& items)
{
	std::ofstream db;

	db.open(filename);
	if (!db.is_open())
		return false;
	for (const auto& item : items)
		db << item << std::endl;
	db.close();
	return true;
}

bool FavouritesDatabase::Append(const std::vector<std::uint64_t> &items)
{
    storage.reserve(storage.size() + items.size());
	storage.insert(storage.end(), items.begin(), items.end());
	return Write(storage);
}

bool FavouritesDatabase::Delete(const std::vector<std::uint64_t> &items)
{
	for (auto& item : items)
	{
		auto foundItem = std::find_if(storage.begin(), storage.end(), [&item](auto &storageItem){ return  item == storageItem;});
		if (foundItem != storage.end())
			storage.erase(foundItem);
	}
	
    return Write(storage);
}

bool FavouritesDatabase::Load()
{
	std::fstream db(filename, std::ios_base::in);
	std::string string;
	storage.clear();
	if (!db.is_open())
		return false;

	db >> string;
	while (!db.eof() && !string.empty())
	{
		storage.push_back(std::stoi(string));
		db >> string;
	}
	return true;
}

std::vector<Item> Sorter::SortProfitableItems(PriceDatabase& db, std::uint64_t BUYQ_Limit, std::uint64_t SELLQ_Limit, std::uint64_t BUYG_MAX, std::uint64_t BUYG_MIN, std::uint64_t QDIFF_MAX)
{
	std::vector<Item> favitems;
	
	for (const auto& item : db.Get())
		if (item.buyquantity() >= BUYQ_Limit && item.sellquantity() >= SELLQ_Limit && item.buyprice() <= BUYG_MAX && item.buyprice() >= BUYG_MIN && std::abs(item.buyquantity() - item.sellquantity() <= QDIFF_MAX))
			favitems.push_back(item);

	std::sort(favitems.begin(), favitems.end());
	return favitems;
}

std::vector<ItemNameExtended> Search::NameContains(const std::vector<ItemNameExtended>& items, std::string name)
{
	std::vector<ItemNameExtended> containingItems;
	std::vector<std::string> words;
	std::string word;
	std::stringstream stream(name);
	while (std::getline(stream, word, ' '))
		words.push_back(word);
	auto predicate = [&words](auto& item)
	{
		std::string itemName { item.getname() };
		std::vector<std::string> searchWords = words;
		std::transform(itemName.begin(), itemName.end(), itemName.begin(),[](auto c){ return std::tolower(c); });
		for (auto& word : searchWords)
			std::transform(word.begin(), word.end(), word.begin(),[](auto c){ return std::tolower(c); });
		return std::all_of(searchWords.begin(), searchWords.end(), [&itemName](auto& word){return itemName.find(word) != std::string::npos;});
	};
	auto marker = items.begin();
	auto it = std::find_if(marker, items.end(), predicate);
	while (it != items.end())
	{
		containingItems.push_back(*it);
		marker = it;
		it = std::find_if(marker + 1, items.end(), predicate);
	}

	return containingItems;
}