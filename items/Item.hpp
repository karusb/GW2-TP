#ifndef _BAZ_GW2TP_ITEM_HPP
#define _BAZ_GW2TP_ITEM_HPP
#include <iostream>
#include <string>
#include <sstream>
#include <stdio.h>
#include <vector>
#include <iomanip>

class Item
{
public:
	Item(std::uint64_t ids = 0, std::uint64_t buyprice = 0, std::uint64_t buyquantity = 0, std::uint64_t sellprice = 0 , std::uint64_t sellquantity = 0);
	Item(std::uint64_t ids, std::int64_t profit, std::uint64_t buyprice, std::uint64_t buyquantity, std::uint64_t sellprice, std::uint64_t sellquantity);

	std::uint64_t buyquantity() const;
	std::uint64_t sellquantity() const;
	std::uint64_t sellprice() const;
	std::uint64_t buyprice() const;
	std::uint64_t getid() const;
	std::int64_t fees;
	std::int64_t profit;

	friend std::ostream& operator<< (std::ostream& output, const Item &i) 
	{
		return (output << i.id <<" " << i.profit << " " << i.buy_price << " " << i.buy_quantity <<" " << i.sell_price << " " << i.sell_quantity);
	}
	
	friend std::ostream& operator<< (std::ostream& output, const std::vector<Item> &v) 
	{
		for (const auto& i : v)
			output << i << std::endl;
		return output;
	}

	friend bool operator!= (Item const &a, Item const &b)
	{
		return !(a.id == b.id && a.profit == b.profit && a.buy_price == b.buy_price && a.buy_quantity == b.buy_quantity && a.sell_price == b.sell_price && a.sell_quantity == b.sell_quantity);
	}

	friend std::istream& operator>> (std::istream& input, Item &i)
	{
		std::string idt, profitt, buypricet, buyquantityt, sellpricet, sellquantityt;
		input >> idt;
		if (idt.empty())
			return input;
		input >> profitt >> buypricet >> buyquantityt >> sellpricet >> sellquantityt;

		i = Item(std::stoull(idt), std::stoll(profitt), std::stoull(buypricet), std::stoull(buyquantityt), std::stoull(sellpricet), std::stoull(sellquantityt));

		return (input);
	}

	friend std::istream& operator>> (std::istream& input, std::vector<Item> &v)
	{
		Item item;
		input >> item;
		
		if (item != Item())
			v.push_back(item);

		return (input);
	}

	friend bool operator< (Item const &a, Item const &b)
	{
		return a.profit > b.profit;
	}

protected:
	std::uint64_t sell_quantity, buy_quantity, sell_price, buy_price, id;
};
enum Rarity
	: std::uint64_t
{
	Junk = 0,
	Basic = 1,
	Fine = 2,
	Masterwork = 3,
	Rare = 4,
	Exotic = 5,
	Ascended = 6,
	Legendary = 7
};

struct ItemIdentifier
{
	std::uint64_t id;
	std::string name;
	Rarity rarity;

	friend bool operator!= (ItemIdentifier const &a, ItemIdentifier const &b)
	{
		return !(a.id == b.id && a.name == b.name);
	}

	friend std::ostream& operator<< (std::ostream& output, const ItemIdentifier &i) 
	{
		return (output << i.id <<" " << static_cast<typename std::underlying_type<Rarity>::type>(i.rarity) << " " << i.name);
	}

	friend std::ostream& operator<< (std::ostream& output, const std::vector<ItemIdentifier> &v) 
	{
		for (const auto& i : v)
			output << i << std::endl;
		return output;
	}

	friend std::istream& operator>> (std::istream& input, ItemIdentifier &i)
	{
		std::string idt;
		std::uint64_t irarity;
		input >> idt >> irarity;
		if (idt.empty())
			return input;
		i.id = std::stoull(idt);
		i.rarity = static_cast<Rarity>(irarity);
		return std::getline(input, i.name);
	}
	
	friend std::istream& operator>> (std::istream& input, std::vector<ItemIdentifier> &v)
	{
		ItemIdentifier item;
		input >> item;
		if (item != ItemIdentifier())
			v.push_back(item);

		return (input);
	}
};
class ItemNameExtended
	: public Item
{
public:
	ItemNameExtended(const Item& item, std::string name, Rarity rarity);
	ItemNameExtended(const Item& item, std::string name, std::string rarity);
	ItemNameExtended(std::uint64_t ids = 0, std::uint64_t buyprice = 0, std::uint64_t buyquantity = 0, std::uint64_t sellprice = 0 , std::uint64_t sellquantity = 0, const std::string& name="");
	ItemNameExtended(ItemIdentifier identifier, std::uint64_t buyprice, std::uint64_t buyquantity, std::uint64_t sellprice, std::uint64_t sellquantity);
	ItemNameExtended(const Item& item, const ItemIdentifier& identifier);

	const std::string_view getname() const;
	Rarity getrarity() const;
	static Rarity RarityStringToEnum(std::string_view rarity);
private:

	std::string name;
	Rarity rarity = Rarity::Junk;
};
#endif
