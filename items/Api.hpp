#ifndef _BAZ_GW2TP_API_HPP
#define _BAZ_GW2TP_API_HPP
#include <string>
#include <functional>
#include "Item.hpp"
#include "Database.hpp"
#include <chrono>


class GW2API
{
public:
    GW2API(uint8_t maxRequestCountPerCall, IdDatabase& idDb, NameDatabase& nameDb, PriceDatabase& priceDb);
    virtual ~GW2API() = default;

    bool HttpGet(const std::string& url, std::string& response);

    ItemNameExtended ExtendItem(const Item& item);
    void PreloadItemNames(const std::vector<Item>& source, std::vector<ItemNameExtended>& to, int count);

    void InitExtendedItemsListDB();
    void InitItemPriceDatabase();
    void InitItemsListDB();
    std::pair<std::string,std::string> PullItemDetails(std::uint64_t it);
    Item PullItemPrice(std::uint64_t it);

private:
    std::vector<ItemIdentifier> PullItemNames(const std::vector<std::uint64_t>& items);
    std::vector<ItemNameExtended> PullItemNames(const std::vector<Item>& items);
    bool PullItemPrices(int itempos);
    bool PullItemPrices(std::vector<Item>&items, std::vector<ItemIdentifier>&identifierList, int itempos);

    IdDatabase& idDb;
    NameDatabase& nameDb;
    PriceDatabase& priceDb;

    uint8_t ITEM_REQ_COUNT;
};


#endif