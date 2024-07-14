#ifndef _BAZ_GW2TP_ITEM_HPP
#define _BAZ_GW2TP_ITEM_HPP
#include <iostream>
#include <string>
#include <sstream>
#include <stdio.h>

class Item
{
public:
	Item(int ids = 0, int buyprice = 0, int buyquantity = 0, int sellprice = 0 , int sellquantity = 0);
	~Item() = default;

	Item& operator=(const Item &a);
	int buyquantity() const;
	int sellquantity() const;
	int sellprice() const;
	int buyprice() const;
	int getid() const;
	int profit;

	friend std::ostream& operator<< (std::ostream& output, const Item &i) 
	{
		return (output << i.id <<" " << i.profit << " " << i.buy_price << " " << i.buy_quantity <<" " << i.sell_price << " " << i.sell_quantity);
	}
	friend std::istream& operator>> (std::istream& input, Item &i)
	{
		std::string idt, profitt, buypricet, buyquantityt, sellpricet, sellquantityt;
		input >> idt >> profitt >> buypricet >> buyquantityt >> sellpricet >> sellquantityt;
		i.id = std::stoi(idt);
		i.profit = std::stoi(profitt);
		i.buy_price = std::stoi(buypricet);
		i.buy_quantity = std::stoi(buyquantityt);
		i.sell_price = std::stoi(sellpricet);
		i.sell_quantity = std::stoi(sellquantityt);
		return (input);
	}
	friend bool operator< (Item const &a, Item const &b)
	{
		return a.profit > b.profit;
	}

protected:
	int sell_quantity, buy_quantity, sell_price, buy_price, id;
};

class ItemNameExtended
	: public Item
{
public:
	ItemNameExtended(const Item& item, const std::string& name);
	ItemNameExtended(int ids = 0, int buyprice = 0, int buyquantity = 0, int sellprice = 0 , int sellquantity = 0, const std::string& name="");

	const std::string_view getname() const;

private:
	std::string name;
};
#endif
