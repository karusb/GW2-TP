#include "App.hpp"
#include <algorithm>
#include <limits>
#include "termcolor/termcolor.hpp"
#include "Menu.hpp"

GW2TPAPP::GW2TPAPP(GW2API& api, SortParameters& parameters, IdDatabase& idDb, NameDatabase& nameDb, PriceDatabase& priceDb, FavouritesDatabase& favDb)
    : api(api)
    , idDb(idDb)
    , nameDb(nameDb)
    , priceDb(priceDb)
    , favDb(favDb)
    , sortParameters(parameters)
{
    // Standard font
    std::cout << termcolor::bright_green << R"(
  ____           __        __             
 | __ )  __ _ ___\ \      / /_ _ _ __ ___ 
 |  _ \ / _` |_  /\ \ /\ / / _` | '__/ _ \
 | |_) | (_| |/ /  \ V  V / (_| | | |  __/
 |____/ \__,_/___|  \_/\_/ \__,_|_|  \___|
   ______        ______     _____ ____    
  / ___\ \      / /___ \   |_   _|  _ \   
 | |  _ \ \ /\ / /  __) |____| | | |_) |  
 | |_| | \ V  V /  / __/_____| | |  __/   
  \____|  \_/\_/  |_____|    |_| |_|      
                                          )"
    << std::endl << termcolor::reset;
}

GW2TPAPP::~GW2TPAPP()
{
    std::cout << termcolor::reset;
}

void GW2TPAPP::Printer::EncodeRarity(Rarity rarity)
{
    if (rarity == Rarity::Basic)
        std::cout << termcolor::bright_grey;
    else if (rarity == Rarity::Fine)
        std::cout << termcolor::blue;
    else if (rarity == Rarity::Masterwork)
        std::cout << termcolor::green;
    else if (rarity == Rarity::Rare)
        std::cout << termcolor::bright_yellow;
    else if (rarity == Rarity::Exotic)
        std::cout << termcolor::yellow;
    else if (rarity == Rarity::Ascended)
        std::cout << termcolor::magenta;
    else if (rarity == Rarity::Legendary)
        std::cout << termcolor::bright_magenta;
}

void GW2TPAPP::Printer::PrintItem(int number, const Item& item, std::string_view name, Rarity rarity, bool shortList)
{
	std::cout << "[ " << number << " ] ";
    EncodeRarity(rarity);
    std::cout << name  << termcolor::reset << std::endl
            << termcolor::bright_red << " Fees before profit : " << item.fees << termcolor::reset 
            << termcolor::bright_green << " Profit : " << item.profit << termcolor::reset 
            << std::endl;

    if (!shortList)
    {
	    std::cout << " Buy Quantity : " << item.buyquantity() << " Buy Price : " << item.buyprice() << std::endl 
            << " Sell Quantity : " << item.sellquantity() << " Sell Price : " << item.sellprice() << std::endl << std::endl;
    }
}

void GW2TPAPP::Printer::PrintItemWithFetch(int number, const Item& item, GW2API& api, bool shortList)
{
    auto details = api.PullItemDetails(item.getid());
	PrintItem(number, item, details.first, ItemNameExtended::RarityStringToEnum(details.second), shortList);
}

void GW2TPAPP::Printer::PrintExtendedItem(int number, const ItemNameExtended& item, bool shortList)
{
	PrintItem(number, item, item.getname(), item.getrarity(), shortList);
}

void GW2TPAPP::UserInitialize()
{
    auto timePoint = nameDb.Load();
    if (timePoint == 0)
        std::cout << "OFFLINE database not built!" << std::endl;
    else
        std::cout << "Last Offline Database Update " << std::put_time(std::localtime(&timePoint), "%F %T.\n") << std::flush;

    try 
    {
        if (!idDb.Load())
        {
            std::cout << termcolor::bright_red << "Item list cannot be found, rebuilding..."  << termcolor::reset << std::endl;
            RebuildItemsList();
        }
    }
    catch (...)
    {
        std::cout << termcolor::bright_red << "ID database is corrupted! Erasing database and rebuilding..."  << termcolor::reset << std::endl;
        idDb.Erase();
        RebuildItemsList();
    }

    std::cout << termcolor::yellow << "Currently " << idDb.Get().size() << " items are recorded in the database" << termcolor::reset << std::endl;

    try 
    {
        if (!priceDb.Load())
        {
            std::cout << termcolor::bright_red << "Item Price Database was not found! Rebuilding database, this may take a while..." << termcolor::reset << std::endl;
            RebuildItemPriceDatabase();
        }
        else 
            DbIntegrityNotify();
    }
    catch (...)
    {
        std::cout << termcolor::bright_red << "Item Price Database is corrupted! Erasing database and rebuilding..."  << termcolor::reset << std::endl;
        priceDb.Erase();
        RebuildItemPriceDatabase();
        DbIntegrityNotify();
    }
}

void GW2TPAPP::EraseAllDatabases()
{
	idDb.Erase();
	priceDb.Erase();
	nameDb.Erase();
}

void GW2TPAPP::MainMenu()
{
    Menu menu;
    menu.Add("1", "LIVE", [this](auto){ LiveListMenu(); })
        .Add("s", "SEARCH", [this](auto){ SearchMenu(); })
        .Add("2", "OFFLINE", [this](auto){ OfflineListMenu(); })
        .Add("3", "FAVOURITES", [this](auto){ FavouritesMenu(); })
        .Add("4", "LIMITS", [this](auto){ LimitsMenu(); })
        .Add("5", "DATABASE", [this](auto){ DatabaseMenu(); })
        .Add("c", "SHORTEN", [this](auto)
        { 
            shortList = !shortList;
            std::cout << termcolor::bright_magenta << "Shorten " << (shortList ? "enabled." : "disabled.") << termcolor::reset << std::endl;
        })
        .ExitOn("q", "QUIT")
        .Run();
}

void GW2TPAPP::DatabaseMenuPrint()
{
	auto point = nameDb.Load();
	if (point == 0)
		std::cout << termcolor::red << "Last Offline Update " << ("(Not Built)") << std::endl << termcolor::reset << std::flush;
	else
		std::cout << termcolor::green << "Last Offline Update " << std::put_time(std::localtime(&point), "%F %T.\n") << std::flush;
	auto dbTime = std::chrono::system_clock::to_time_t(priceDb.UpdateTime());
	std::cout << termcolor::green << "Last Price Update " << std::put_time(std::localtime(&dbTime), "%F %T.\n") << termcolor::reset << std::flush;
}

void GW2TPAPP::DatabaseMenu()
{
    Menu menu("DATABASE");
    std::cout << termcolor::yellow << "!!! INFORMATION !!! Make sure to rebuild all databases on major updates" << termcolor::reset << std::endl;
    DatabaseMenuPrint();
    menu.Add("1", "REBUILD OFFLINE", [this](auto)
        {
            nameDb.Erase();
			RebuildExtendedItemsList();
        })
        .Add("2", "REBUILD PRICES", [this](auto)
        {
            idDb.Erase();
			priceDb.Erase();

			std::cout << "Downloading tradable item list... " << termcolor::reset << std::endl;
			RebuildItemsList();
			
			std::cout << termcolor::yellow << "Downloading item prices, this may take a while... " << termcolor::reset << std::endl;
			RebuildItemPriceDatabase();
        })
        .Add("3", "REBUILD ALL", [this](auto)
        {
            EraseAllDatabases();
			std::cout << termcolor::yellow << "Rebuilding all databases, this may take a while depending on API availability" << termcolor::reset << std::endl;
			std::cout << "Downloading tradable item list... " << termcolor::reset << std::endl;
			RebuildItemsList();
			RebuildExtendedItemsList();
			std::cout << termcolor::yellow << "Downloading item prices, this may take a while... " << termcolor::reset << std::endl;
			RebuildItemPriceDatabase();
        })
        .Add("4", "ERASE FAVOURITES", [this](auto){ favDb.Erase(); })
        .ExitOn("r", "RETURN")
        .Default([this](auto){ DatabaseMenuPrint(); })
        .Run();
}

void GW2TPAPP::LimitsMenu()
{
    Menu menu("LIMITS");
    menu.Add("1", "QUANTITY LIMITS", [this](auto)
        {
            std::cout << termcolor::bright_yellow << "Current MIN Limits :" << termcolor::bright_white << std::endl 
             << " BUYING QUANTITY " << termcolor::bright_green << sortParameters.BUYQ_Limit << termcolor::bright_white << " SELLING QUANTITY "  << termcolor::bright_red << sortParameters.SELLQ_Limit << termcolor::reset << std::endl;
            
            Menu menu; 
            menu.Add("c", "CHANGE", [this](auto)
            {
                std::cout << termcolor::bright_yellow << "Input new quantity limits and separate them with space" << termcolor::reset << std::endl;
                std::cin >> sortParameters.BUYQ_Limit >> sortParameters.SELLQ_Limit;
            })
            .Add("d", "DEFAULTS", [this](auto)
            {
                std::cout << termcolor::bright_yellow << "New price limits set to default" << termcolor::reset << std::endl;
                auto defaultParameters = SortParameters();
                sortParameters.BUYQ_Limit = defaultParameters.BUYQ_Limit;
                sortParameters.SELLQ_Limit = defaultParameters.SELLQ_Limit;
            })
            .PrintOptions()
            .Run();

        })
        .Add("2", "PRICE LIMITS", [this](auto)
        {
            std::cout << termcolor::bright_yellow << "Current BUY Price Limits :" << termcolor::bright_white << std::endl
            << " MIN PRICE " << termcolor::bright_red << sortParameters.BUYG_MIN << termcolor::bright_white << " MAX PRICE "  << termcolor::bright_green << sortParameters.BUYG_MAX << termcolor::reset << std::endl;
            
            Menu menu; 
            menu.Add("c", "CHANGE", [this](auto)
            {
                std::cout << termcolor::bright_yellow << "Input new price limits and separate them with space" << termcolor::reset << std::endl;
                std::cin >> sortParameters.BUYG_MIN >> sortParameters.BUYG_MAX;
            })
            .Add("d", "DEFAULTS", [this](auto)
            {
                std::cout << termcolor::bright_yellow << "New price limits set to default" << termcolor::reset << std::endl;
                auto defaultParameters = SortParameters();
                sortParameters.BUYG_MIN = defaultParameters.BUYG_MIN;
                sortParameters.BUYG_MAX = defaultParameters.BUYG_MAX;
            })
            .PrintOptions()
            .Run();

        })
        .Add("3", "QUANTITY DIFFERENCE", [this](auto)
        {
            std::cout << "Filters out items where the absolute difference of their buy quantity and sell quantity exceeds the quantity difference value" << std::endl;
            std::cout << termcolor::bright_yellow << "Current Quantity Difference :" << termcolor::bright_white << std::endl
            << " Maximum Quantity Difference " << termcolor::bright_green << sortParameters.QDIFF_MAX << termcolor::reset << std::endl;
            
            Menu menu; 
            menu.Add("c", "CHANGE", [this](auto)
            {
                std::cout << termcolor::bright_yellow << "Input new quantity limit" << termcolor::reset << std::endl;
                std::cin >> sortParameters.QDIFF_MAX;
            })
            .Add("max", "MAX", [this](auto)
            {
                std::cout << termcolor::bright_yellow << "Maxed quantity difference" << termcolor::reset << std::endl;
                sortParameters.QDIFF_MAX = 9999999;
            })
            .Add("d", "DEFAULT", [this](auto)
            {
                std::cout << termcolor::bright_yellow << "New quantity difference limit set to default" << termcolor::reset << std::endl;
                auto defaultParameters = SortParameters();
                sortParameters.QDIFF_MAX = defaultParameters.QDIFF_MAX;
            })
            .PrintOptions()
            .Run();

        })
        .ExitOn("r", "RETURN")
        .Run();
}

void GW2TPAPP::LiveListMenu()
{
    int parsesize = GetSizeChoice();
    auto sortedItems = Sorter::SortProfitableItems(priceDb, sortParameters.BUYQ_Limit, sortParameters.SELLQ_Limit, sortParameters.BUYG_MAX, sortParameters.BUYG_MIN, sortParameters.QDIFF_MAX);

    std::vector<ItemNameExtended> extendedItems;
    std::cout << termcolor::yellow << "Loading item names..." << termcolor::reset;
    api.PreloadItemNames(sortedItems, extendedItems, parsesize);
    std::cout << termcolor::bright_green << "Done" << termcolor::reset << std::endl;

    for (int i = 0; i < parsesize && i < extendedItems.size(); i++)
        Printer::PrintExtendedItem(i, extendedItems[i], shortList);

    std::cout << termcolor::yellow << " Type 'u' to update the prices in the list or any other key to continue"  << termcolor::reset << std::endl;
    Menu menu;
    menu.AddContinous("u", "UPDATE", [this, &extendedItems, &parsesize](auto)
        {
            for (int i = 0; i < parsesize; i++)
                extendedItems[i] = {extendedItems[i], api.PullItemPrice(extendedItems[i].getid())};
            std::cout << std::endl;
            std::sort(extendedItems.begin(), extendedItems.end());

            for (int i = 0; i < parsesize && i < extendedItems.size(); i++)
                Printer::PrintExtendedItem(i, extendedItems[i], shortList);

            std::cout << termcolor::yellow << "Type any key to update or type 'r' to stop"  << termcolor::reset << std::endl;
        }, "r")
        .Run();
}

void GW2TPAPP::SearchMenu()
{
    auto foundItems = SearchItems();
    for (auto i = 0; i < foundItems.size(); ++i)
        Printer::PrintExtendedItem(i, foundItems[i], shortList);

    std::cout << termcolor::yellow << " Type 'u' to update the prices in the list or any other key to continue"  << termcolor::reset << std::endl;
    Menu menu;
    menu.AddContinous("u", "UPDATE", [this, &foundItems](auto)
        {
            for (int i = 0; i < foundItems.size(); i++)
                foundItems[i] = {foundItems[i], api.PullItemPrice(foundItems[i].getid())};
            std::cout << std::endl;
            std::sort(foundItems.begin(), foundItems.end());

            for (int i = 0; i < foundItems.size(); i++)
                Printer::PrintExtendedItem(i, foundItems[i], shortList);

            std::cout << termcolor::yellow << "Type any key to update or type 'r' to stop"  << termcolor::reset << std::endl;
        }, "r")
        .Run();
}

void GW2TPAPP::OfflineListMenu()
{
    if (nameDb.Exists())
    {
        int parsesize = GetSizeChoice();
        auto sortedItems = Sorter::SortProfitableItems(priceDb, sortParameters.BUYQ_Limit, sortParameters.SELLQ_Limit, sortParameters.BUYG_MAX, sortParameters.BUYG_MIN, sortParameters.QDIFF_MAX);

        for (int i = 0; i < parsesize && i < sortedItems.size(); i++)
            Printer::PrintExtendedItem(i, nameDb.ExtendItemFromDB(sortedItems[i]), shortList);
    }
    else
    {
        std::cout << termcolor::bright_red << "Offline database not available! Downloading..."  << termcolor::reset << std::endl;
        RebuildExtendedItemsList();
        OfflineListMenu();
    }
}

void GW2TPAPP::FavouritesMenu()
{
    Menu menu("FAVOURITES");
    menu.Add("1", "SHOW FAVOURITES", [this](auto input){ LiveFavouritesMenu(); })
        .Add("2", "SELECT FAVOURITES", [this](auto input){ SelectFavouritesMenu(); })
        .Add("3", "REMOVE FAVOURITES", [this](auto input){ RemoveFavourites(); })
        .ExitOn("r", "RETURN")
        .Run();
}

void GW2TPAPP::LiveFavouritesMenu()
{
    std::string input;
    if (favDb.Load())
    {
        while (input != "r")
        {
            std::vector<Item> userFavourites;
            for (const auto& fav : favDb.Get())
                userFavourites.push_back(api.PullItemPrice(fav));
            std::cout << std::endl;
            std::sort(userFavourites.begin(), userFavourites.end());

            for (int i = 0; i < favDb.Get().size(); i++)
                Printer::PrintItemWithFetch(i, userFavourites[i], api, shortList);

            std::cout << termcolor::yellow << "Press any key then enter to continue or type 'r' to stop" << termcolor::reset << std::endl;
            std::cin >> input;
        }
    }
    else
        std::cout << termcolor::bright_red << " Select favourites first!" << termcolor::reset << std::endl;
}

void GW2TPAPP::SelectFavouritesMenu()
{
    Menu menu("SELECT FAVOURITES");
    menu.Add("1", "FROM LIST", [this](auto){ SelectFavouritesFromList(); })
        .Add("2", "FROM NAME", [this](auto){ SelectFavouritesFromName(); })
        .ExitOn("r", "RETURN")
        .Run();
}

void GW2TPAPP::SelectFavouritesFromName()
{
    favDb.Load();

    auto foundItems = SearchItems();

    for (auto i = 0; i < foundItems.size(); ++i)
        Printer::PrintExtendedItem(i, foundItems[i], true);

    auto choices = GetChoices();

    std::vector<uint64_t> selectedItems;
    std::cout << termcolor::bright_yellow << " You have selected : " << termcolor::bright_red;
    for (auto choice : choices)
    {
        if (foundItems.size() > choice)
        {
            std::cout << choice << " ";
            selectedItems.push_back(foundItems[choice].getid());
        }
    }
    if (!selectedItems.empty())
        favDb.Append(selectedItems);
}

void GW2TPAPP::SelectFavouritesFromList()
{
    favDb.Load();
    auto sortedItems = Sorter::SortProfitableItems(priceDb, sortParameters.BUYQ_Limit, sortParameters.SELLQ_Limit, sortParameters.BUYG_MAX, sortParameters.BUYG_MIN, sortParameters.QDIFF_MAX);
    int parsesize = GetSizeChoice();
    sortedItems = {sortedItems.begin(), sortedItems.begin() + parsesize};
    if (nameDb.Exists())
    {
        for (auto i = 0; i < sortedItems.size(); ++i)
            Printer::PrintExtendedItem(i, nameDb.ExtendItemFromDB(sortedItems[i]), true);
    }
    else
    {
        for (auto i = 0; i < sortedItems.size(); ++i)
            Printer::PrintItemWithFetch(i, sortedItems[i], api, true);
    }
    
    auto choices = GetChoices();

    std::cout << termcolor::bright_yellow << " You have selected : " << termcolor::bright_red;
    std::vector<uint64_t> selectedItems;
    for (const auto& choice : choices)
    {
        if (sortedItems.size() > choice)
        {
            std::cout << choice << " ";
            selectedItems.push_back(sortedItems[choice].getid());
        }
    }
    if (!selectedItems.empty())
        favDb.Append(selectedItems);

    std::cout << termcolor::reset << std::endl;
}

std::vector<std::uint64_t> GW2TPAPP::GetChoices()
{
    std::string input;
    std::vector<std::uint64_t> choices;
    std::cout << termcolor::yellow << " ENTER THE ITEM NUMBER(S) SHOWN IN [] FINISH WITH ' q'" << termcolor::reset << std::endl;

    while (input != "q")
    {
        std::cin >> input;
        try
        {
            if (input != "q")
                choices.push_back(std::stoi(input));
        }
        catch (const std::invalid_argument&)
        {
            std::cout << termcolor::bright_red << "INVALID INPUT!" << termcolor::reset << std::endl;
            input.clear();
        }
    }
    return choices;
}

int GW2TPAPP::GetSizeChoice()
{
    try
    {
        std::string input;
        std::cout << "How many items to show? ";
        std::cin >> input;
        return std::stoi(input);
    }
    catch(const std::invalid_argument&)
    {
        std::cout << termcolor::bright_red << "INVALID INPUT!" << termcolor::reset << std::endl;
        return GetSizeChoice();
    }
}

std::vector<ItemNameExtended> GW2TPAPP::SearchItems()
{
    if (!nameDb.Exists())
    {
        std::cout << termcolor::bright_red << "Name search unavailable, build Offline database first!"  << termcolor::reset << std::endl;
        return {};
    }
    else
    {
        std::cout << termcolor::bright_green << "Item name: "  << termcolor::reset;
        std::string input ;
        std::vector<ItemNameExtended> extendedItems;

        std::cin >> std::ws;
        std::getline(std::cin, input);
        std::cout << termcolor::bright_green << "Searching... "  << termcolor::reset << std::flush;
        for (auto item : priceDb.Get())
            extendedItems.push_back(nameDb.ExtendItemFromDB(item));

        auto foundItems = Search::NameContains(extendedItems, input);

        std::cout << termcolor::bright_yellow << "Found " << foundItems.size() << " results."  << termcolor::reset << std::endl;
        std::sort(foundItems.begin(), foundItems.end());
        return foundItems;
    }
}

void GW2TPAPP::RemoveFavourites()
{
    if (favDb.Load())
    {
        std::vector<ItemNameExtended> userFavourites;
        if (nameDb.Exists())
        {
            for (const auto& fav : favDb.Get())
                userFavourites.push_back(nameDb.ExtendItemFromDB(priceDb.ItemFromDB(fav)));
        }
        else
        {
            for (const auto& fav : favDb.Get())
                userFavourites.push_back(api.ExtendItem(priceDb.ItemFromDB(fav)));
        }
        std::sort(userFavourites.begin(), userFavourites.end());

        for (auto i = 0; i < userFavourites.size(); ++i)
            Printer::PrintExtendedItem(i, userFavourites[i], true);
        
        auto choices = GetChoices();
        
        std::cout << termcolor::bright_yellow << " You have selected : " << termcolor::bright_red;
        std::vector<uint64_t> selectedItems;
        for (const auto& choice : choices)
        {
            std::cout << choice << " ";
            selectedItems.push_back(userFavourites[choice].getid());
        }

        if (!selectedItems.empty())
            favDb.Delete(selectedItems);
    }
    else
        std::cout << termcolor::bright_red << " Select favourites first!" << termcolor::reset << std::endl;
}

void GW2TPAPP::RebuildItemsList()
{
	api.InitItemsListDB();
	idDb.Load();
}

void GW2TPAPP::RebuildExtendedItemsList()
{
	api.InitExtendedItemsListDB();
	nameDb.Load();
}

void GW2TPAPP::DbIntegrityNotify()
{
	if (DatabaseIntegrityChecker::CheckLines("items.db") - 1 == idDb.Get().size())
		std::cout  << termcolor::bright_green << "INTEGRITY CHECK PASSED" << termcolor::reset << std::endl;
	else
		std::cout << termcolor::bright_red << DatabaseIntegrityChecker::CheckLines("items.db") - 1 << " != " << idDb.Get().size() << "Actual vs Expected INCONSISTENCY : Please rebuild all databases" << termcolor::reset << std::endl;
}

void GW2TPAPP::RebuildItemPriceDatabase()
{
    api.InitItemPriceDatabase();
	priceDb.Load();
	DbIntegrityNotify();

	std::sort(priceDb.Get().begin(), priceDb.Get().end());
}
