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

		i.id = std::stoull(idt);
		i.profit = std::stoll(profitt);
		i.buy_price = std::stoull(buypricet);
		i.buy_quantity = std::stoull(buyquantityt);
		i.sell_price = std::stoull(sellpricet);
		i.sell_quantity = std::stoull(sellquantityt);
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
struct ItemIdentifier
{
	std::uint64_t id;
	std::string name;

	friend bool operator!= (ItemIdentifier const &a, ItemIdentifier const &b)
	{
		return !(a.id == b.id && a.name == b.name);
	}
	friend std::ostream& operator<< (std::ostream& output, const ItemIdentifier &i) 
	{
		return (output << i.id <<" " << i.name);
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
		input >> idt;
		if (idt.empty())
			return input;
		i.id = std::stoull(idt);
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
	ItemNameExtended(const Item& item, std::string name);
	ItemNameExtended(std::uint64_t ids = 0, std::uint64_t buyprice = 0, std::uint64_t buyquantity = 0, std::uint64_t sellprice = 0 , std::uint64_t sellquantity = 0, const std::string& name="");
	ItemNameExtended(ItemIdentifier identifier, std::uint64_t buyprice, std::uint64_t buyquantity, std::uint64_t sellprice, std::uint64_t sellquantity);

	const std::string_view getname() const;

private:
	std::string name;
};
#endif
