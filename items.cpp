class item
{
public:
	item();
	item(int ids, int buyprice, int buyquantity, int sellprice, int sellquantity);
	~item();
	int buyquantity();
	int sellquantity();
	int sellprice();
	int buyprice();
	int id();
	int flip_profit();
protected:
	int sell_quantity, buy_quantity, sell_price, buy_price, id,flip_profit;

};
item::item()
{
	sell_quantity = 0;
	buy_quantity = 0;
	sell_price = 0;
	buy_price = 0;
	id = 0;
}
item::item(int ids, int buyprice, int buyquantity, int sellprice, int sellquantity)
{
	id = ids;
	buy_price = buyprice;
	buy_quantity = buyquantity;
	sell_quantity = sellquantity;
	sell_price = sellprice;
	flip_profit = sellprice - buyprice - (sellprice * 0.1 + sellprice*0.05);
}
item::~item()
{
}
int item::buyquantity() {return buy_quantity; }
int item::sellquantity() { return sell_quantity; }
int item::sellprice() {return sell_price; }
int item::buyprice() { return buy_price; }
int item::id() { return id; }
int item::flip_profit() { return flip_profit };