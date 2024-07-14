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
int Item::buyquantity() {return buy_quantity; }
int Item::sellquantity() { return sell_quantity; }
int Item::sellprice() {return sell_price; }
int Item::buyprice() { return buy_price; }
int Item::getid() { return id; }