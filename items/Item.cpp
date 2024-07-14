#include "Item.hpp"

Item::Item(int ids, int buyprice, int buyquantity, int sellprice, int sellquantity)
{
	id = ids;
	buy_price = buyprice;
	sell_price = sellprice;
	buy_quantity = buyquantity;
	sell_quantity = sellquantity;
	profit = sellprice - buyprice - (sellprice*0.1 + sellprice*0.05);

}
Item& Item::operator= (const Item &a)
{
	id = a.id;
	profit = a.profit;
	buy_price = a.buy_price;
	buy_quantity = a.buy_quantity;
	sell_price = a.sell_price;
	sell_quantity = a.sell_quantity;
	
	return (*this);
}
int Item::buyquantity() const {return buy_quantity; }
int Item::sellquantity() const { return sell_quantity; }
int Item::sellprice() const {return sell_price; }
int Item::buyprice() const { return buy_price; }
int Item::getid() const { return id; }

ItemNameExtended::ItemNameExtended(const Item& item, const std::string& name)
	: Item(item)
	, name(name)
{}
ItemNameExtended::ItemNameExtended(int ids, int buyprice, int buyquantity, int sellprice, int sellquantity, const std::string& name)
	: Item(ids, buyprice, buyquantity, sellprice, sellquantity)
	, name(name)
{}

const std::string_view ItemNameExtended::getname() const
{
	return name;
}