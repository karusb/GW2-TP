#include "Item.hpp"

Item::Item(std::uint64_t ids, std::uint64_t buyprice, std::uint64_t buyquantity, std::uint64_t sellprice, std::uint64_t sellquantity)
	: id(ids)
	, profit(sellprice - buyprice - (sellprice*0.1 + sellprice*0.05))
	, buy_price(buyprice)
	, buy_quantity(buyquantity)
	, sell_price(sellprice)
	, sell_quantity(sellquantity)
{}

Item::Item(std::uint64_t ids, std::int64_t profit, std::uint64_t buyprice, std::uint64_t buyquantity, std::uint64_t sellprice, std::uint64_t sellquantity)
	: id(ids)
	, profit(profit)
	, buy_price(buyprice)
	, buy_quantity(buyquantity)
	, sell_price(sellprice)
	, sell_quantity(sellquantity)
{}

std::uint64_t Item::buyquantity() const { return buy_quantity; }
std::uint64_t Item::sellquantity() const { return sell_quantity; }
std::uint64_t Item::sellprice() const { return sell_price; }
std::uint64_t Item::buyprice() const { return buy_price; }
std::uint64_t Item::getid() const { return id; }

ItemNameExtended::ItemNameExtended(const Item& item, std::string name)
	: Item(item)
	, name(name)
{}
ItemNameExtended::ItemNameExtended(std::uint64_t ids, std::uint64_t buyprice, std::uint64_t buyquantity, std::uint64_t sellprice, std::uint64_t sellquantity, const std::string& name)
	: Item(ids, buyprice, buyquantity, sellprice, sellquantity)
	, name(name)
{}

ItemNameExtended::ItemNameExtended(ItemIdentifier identifier, std::uint64_t buyprice, std::uint64_t buyquantity, std::uint64_t sellprice, std::uint64_t sellquantity)
	: Item(identifier.id, buyprice, buyquantity, sellprice, sellquantity)
	, name(identifier.name)
{}
const std::string_view ItemNameExtended::getname() const
{
	return name;
}