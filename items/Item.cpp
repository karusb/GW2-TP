#include "Item.hpp"

Item::Item(std::uint64_t ids, std::uint64_t buyprice, std::uint64_t buyquantity, std::uint64_t sellprice, std::uint64_t sellquantity)
	: id(ids)
	, buy_price(buyprice)
	, buy_quantity(buyquantity)
	, sell_price(sellprice)
	, sell_quantity(sellquantity)
	, fees(0.1 * sellprice + 0.05 * sellprice + 0.1 * buyprice + 0.05 * buyprice)
	, profit(sellprice - buyprice - fees)
{}

Item::Item(std::uint64_t ids, std::int64_t profit, std::uint64_t buyprice, std::uint64_t buyquantity, std::uint64_t sellprice, std::uint64_t sellquantity)
	: id(ids)
	, buy_price(buyprice)
	, buy_quantity(buyquantity)
	, sell_price(sellprice)
	, sell_quantity(sellquantity)
	, fees(0.1 * sellprice + 0.05 * sellprice + 0.1 * buyprice + 0.05 * buyprice)
	, profit(sellprice - buyprice - fees)
{}

std::uint64_t Item::buyquantity() const { return buy_quantity; }
std::uint64_t Item::sellquantity() const { return sell_quantity; }
std::uint64_t Item::sellprice() const { return sell_price; }
std::uint64_t Item::buyprice() const { return buy_price; }
std::uint64_t Item::getid() const { return id; }

ItemNameExtended::ItemNameExtended(const Item& item, std::string name, Rarity rarity)
	: Item(item)
	, name(name)
	, rarity(rarity)
{}

ItemNameExtended::ItemNameExtended(const Item& item, std::string name, std::string rarity)
	: Item(item)
	, name(name)
	, rarity(RarityStringToEnum(rarity))
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

Rarity ItemNameExtended::getrarity() const
{
	return rarity;
}

Rarity ItemNameExtended::RarityStringToEnum(std::string_view rarityString)
{
    if (rarityString == "\"Junk\"")
        return Rarity::Junk;
    else if (rarityString == "\"Basic\"")
        return Rarity::Basic;
    else if (rarityString == "\"Fine\"")
        return Rarity::Fine;
    else if (rarityString == "\"Masterwork\"")
        return Rarity::Masterwork;
    else if (rarityString == "\"Rare\"")
        return Rarity::Rare;
    else if (rarityString == "\"Exotic\"")
        return Rarity::Exotic;
    else if (rarityString == "\"Ascended\"")
        return Rarity::Ascended;
    else if (rarityString == "\"Legendary\"")
        return Rarity::Legendary;
    return Rarity::Junk;
}
