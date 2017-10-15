#include <iostream>
#include <string>
#include <sstream>
#include <stdio.h>
class item
{
public:
	item(int ids = 0, int buyprice = 0, int buyquantity = 0, int sellprice = 0 , int sellquantity = 0);
	~item();
	item& operator=(const item &a);
	int buyquantity();
	int sellquantity();
	int sellprice();
	int buyprice();
	int getid();
	int profit;
	friend std::ostream& operator<< (std::ostream& output, const item &i) 
	{
		return (output << i.id <<" " << i.profit << " " << i.buy_price << " " << i.buy_quantity <<" " << i.sell_price << " " << i.sell_quantity);
	}
	friend std::istream& operator>> (std::istream& input, item &i)
	{
		std::string idt, profitt, buypricet, buyquantityt, sellpricet, sellquantityt;
		input >> idt >> profitt >> buypricet >> buyquantityt >> sellpricet >> sellquantityt;
		i.id = stoi(idt);
		i.profit = stoi(profitt);
		i.buy_price = stoi(buypricet);
		i.buy_quantity = stoi(buyquantityt);
		i.sell_price = stoi(sellpricet);
		i.sell_quantity = stoi(sellquantityt);
		return (input);
	}
	friend bool operator< (item const &a, item const &b)
	{
		return a.profit > b.profit;
	}
	


protected:
	int sell_quantity, buy_quantity, sell_price, buy_price, id;

};
item::item(int ids, int buyprice, int buyquantity, int sellprice, int sellquantity)
{
	id = ids;
	buy_price = buyprice;
	sell_price = sellprice;
	buy_quantity = buyquantity;
	sell_quantity = sellquantity;
	profit = sellprice - buyprice - (sellprice*0.1 + sellprice*0.05);

}
item::~item()
{
}
item& item::operator= (const item &a)
{

	//item result(a.id,a.buy_price,a.buy_quantity,a.sell_price,a.sell_quantity);
	
	id = a.id;
	profit = a.profit;
	buy_price = a.buy_price;
	buy_quantity = a.buy_quantity;
	sell_price = a.sell_price;
	sell_quantity = a.sell_quantity;
	
	return (*this);
}
int item::buyquantity() {return buy_quantity; }
int item::sellquantity() { return sell_quantity; }
int item::sellprice() {return sell_price; }
int item::buyprice() { return buy_price; }
int item::getid() { return id; }