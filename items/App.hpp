#ifndef _BAZ_GW2TP_APP_HPP
#define _BAZ_GW2TP_APP_HPP
#include <functional>
#include "Api.hpp"
#include "Item.hpp"
#include "Database.hpp"

struct SortParameters
{
    std::uint64_t SELLQ_Limit = 550;
	std::uint64_t BUYQ_Limit = 550;
	std::uint64_t BUYG_MIN = 0;
	std::uint64_t BUYG_MAX = 9999999;
};
class GW2TPAPP
{
public:
    GW2TPAPP(GW2API& api, SortParameters& parameters, IdDatabase& idDb, NameDatabase& nameDb, PriceDatabase& priceDb, FavouritesDatabase& favDb);
    ~GW2TPAPP();
    struct Printer
    {
        static void EncodeRarity(Rarity rarity);
        static void PrintItem(int number, const Item& item, std::string_view name, Rarity rarity, bool shortList);
        static void PrintItemWithFetch(int number, const Item& item, GW2API& api, bool shortList);
        static void PrintExtendedItem(int number, const ItemNameExtended& item, bool shortList);
    };

    void UserInitialize();
    void MainMenu();
    void EraseAllDatabases();

private: 
    void DatabaseMenuPrint();
    void DatabaseMenu();
    void LimitsMenu();
    void LiveListMenu();
    void OfflineListMenu();
    void LiveFavouritesMenu();
    void SelectFavouritesMenu();
    void DbIntegrityNotify();
    void RebuildItemsList();
    void RebuildExtendedItemsList();
    void RebuildItemPriceDatabase();  
    
    GW2API& api;
    IdDatabase& idDb;
    NameDatabase& nameDb;
    PriceDatabase& priceDb;
    FavouritesDatabase& favDb;
    SortParameters& sortParameters;

    bool shortList = false;
};

#endif
