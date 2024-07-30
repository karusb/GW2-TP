#include "App.hpp"
#include <algorithm>
#include "termcolor/termcolor.hpp"

GW2TPAPP::GW2TPAPP(GW2API& api, IdDatabase& idDb, NameDatabase& nameDb, PriceDatabase& priceDb, FavouritesDatabase& favDb)
    : api(api)
    , idDb(idDb)
    , nameDb(nameDb)
    , priceDb(priceDb)
    , favDb(favDb)
{}

GW2TPAPP::~GW2TPAPP()
{
    std::cout << termcolor::reset;
}

void GW2TPAPP::Printer::EncodeRarity(Rarity rarity)
{
    if (rarity == Rarity::Basic)
        std::cout << termcolor::grey;
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
            << termcolor::bright_green << " Profit : " << item.profit << termcolor::reset
            << termcolor::bright_red << " Fees before profit : " << item.fees << termcolor::reset << std::endl;

    if (!shortList)
    {
	    std::cout << " Buy Quantity : " << item.buyquantity() << " Buy Price : " << item.buyprice() << std::endl 
            << " Sell Quantity : " << item.sellquantity() << " Sell Price : " << item.sellprice() << std::endl << std::endl;
    }
            // << " BUY: Quantity  Price   |  SELL: Quantity  Price" << termcolor::reset << std::endl
            // << termcolor::green << "       " << item.buyquantity() << "    " << item.buyprice() << termcolor::reset << "         "
            // << termcolor::red << "       " <<  item.sellquantity() << "    " << item.sellprice() << termcolor::reset << std::endl;

			// << " Buy Quantity : " << item.buyquantity() << " Sell Quantity : " << item.sellquantity() << std::endl
			// << " Buy Price : " << item.buyprice() << " Sell Price : " << item.sellprice() << std::endl << std::endl;
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
		std::cout << "Last Update " << std::put_time(std::localtime(&timePoint), "%F %T.\n") << std::flush;

	if (!idDb.Load())
	{
		std::cout << "Item list cannot be found, rebuilding..." << std::endl;
		RebuildItemsList();
	}

	std::cout << "Currently " << idDb.Get().size() << " items can be sold or bought from TP" << std::endl;

	if (!priceDb.Load())
	{
		std::cout << "Item Price Database was not found! Rebuilding database, this may take a while..." << std::endl;
		RebuildItemPriceDatabase();
	}
	else 
		DbIntegrityNotify();
}

void GW2TPAPP::EraseAllDatabases()
{
	idDb.Erase();
	priceDb.Erase();
	nameDb.Erase();
    favDb.Erase();
}

void GW2TPAPP::DatabaseMenuPrint()
{
	std::string offlineNotice;
	auto point = nameDb.Load();
	if (point == 0)
		offlineNotice = "(Not Built)";
	else
	{
		offlineNotice = "(Availabe)";
		std::cout << "Last Offline Update " << std::put_time(std::localtime(&point), "%F %T.\n") << std::flush;
	}
	auto dbTime = std::chrono::system_clock::to_time_t(priceDb.UpdateTime());
	std::cout << "Last Price Update " << std::put_time(std::localtime(&dbTime), "%F %T.\n") << std::flush;
	std::cout << "DATABASE: [1] OFFLINE" << offlineNotice << " [2] REBUILD PRICES [3] REBUILD ALL [r] RETURN" << std::endl;

}

void GW2TPAPP::DatabaseMenu()
{
	std::cout << termcolor::yellow << "!!! INFORMATION !!! Make sure to rebuild all databases on major updates" << termcolor::reset << std::endl;
	DatabaseMenuPrint();

	std::string input;
	std::cin >> input;
	while (input != "r")
	{
		if (input == "1")
		{
			nameDb.Erase();
			RebuildExtendedItemsList();
		}
		else if (input == "2")
		{
			idDb.Erase();
			priceDb.Erase();

			std::cout << "Downloading tradable item list... " << std::endl;
			RebuildItemsList();
			
			std::cout << "Downloading item prices, this may take a while... " << std::endl;
			RebuildItemPriceDatabase();
		}
		else if (input == "3")
		{
			EraseAllDatabases();
			std::cout << "Rebuilding all databases, this may take a while depending on API availability" << std::endl;
			std::cout << "Downloading tradable item list... " << std::endl;
			RebuildItemsList();
			RebuildExtendedItemsList();
			std::cout << "Downloading item prices, this may take a while... " << std::endl;
			RebuildItemPriceDatabase();
		}

		DatabaseMenuPrint();
		std::cin >> input;
	};
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

void GW2TPAPP::LimitsMenu()
{
    std::string input;
    std::cout << "What would you like to change? [1] Quantity Limits [2] Price Limits";
    std::cin >> input;
    if (input == "1")
    {
        std::cout << "Current MIN Limits : BUYING QUANTITY " << BUYQ_Limit << " SELLING QUANTITY " << SELLQ_Limit << std::endl;
        std::cout << "Input new limits and separate them with space" << std::endl;
        std::cin >> BUYQ_Limit >> SELLQ_Limit;
    }
    else if (input == "2")
    {
        std::cout << "Current Price Limits : MIN PRICE " << BUYG_MIN << " MAX PRICE " << BUYG_MAX << std::endl;
        std::cout << "Input new limits and separate them with space" << std::endl;
        std::cin >> BUYG_MIN >> BUYG_MAX;
    }
}

void GW2TPAPP::LiveListMenu()
{
    std::string input;
    std::cout << "How many items to show? ";
    std::cin >> input;
    int parsesize = stoi(input);

    auto sortedItems = PriceDatabase::Sorter::SortProfitableItems(priceDb, BUYQ_Limit, SELLQ_Limit, BUYG_MAX, BUYG_MIN);

    std::vector<ItemNameExtended> extendedItems;
    std::cout << "Loading item names...";
    api.PreloadItemNames(sortedItems, extendedItems, parsesize);
    std::cout << "Done" << std::endl;

    for (int i = 0; i < parsesize && i < extendedItems.size(); i++)
        Printer::PrintExtendedItem(i, extendedItems[i], shortList);

    std::cout << " TYPE u to UPDATE FETCHED LIST or any key to continue" << std::endl;
    std::cin >> input;
    if (input == "u" || input == "U")
    {
        while (input != "esc")
        {
            for (int i = 0; i < parsesize; i++)
                extendedItems[i] = extendedItems[i], api.PullItemPrice(extendedItems[i].getid());
            
            std::sort(extendedItems.begin(), extendedItems.end());

            for (int i = 0; i < parsesize && i < extendedItems.size(); i++)
                Printer::PrintExtendedItem(i, extendedItems[i], shortList);

            std::cout << "Press any key then enter to continue or type esc to stop" << std::endl;
            std::cin >> input;
        }
    }

}
void GW2TPAPP::OfflineListMenu()
{
    std::string input;
    std::cout << "How many items to show? ";
    std::cin >> input;
    int parsesize = std::stoi(input);

    auto sortedItems = PriceDatabase::Sorter::SortProfitableItems(priceDb, BUYQ_Limit, SELLQ_Limit, BUYG_MAX, BUYG_MIN);

    for (int i = 0; i < parsesize && i < sortedItems.size(); i++)
    {	
        auto item = nameDb.ExtendItemFromDB(sortedItems[i]);
        Printer::PrintExtendedItem(i, item, shortList);
    }
}

void GW2TPAPP::LiveFavouritesMenu()
{
    std::string input;
    if (favDb.Load())
    {
        while (input != "q")
        {
            std::vector<Item> userFavourites;
            for (const auto& fav : favDb.Get())
                userFavourites.push_back(api.PullItemPrice(fav));

            std::sort(userFavourites.begin(), userFavourites.end());

            for (int i = 0; i < favDb.Get().size(); i++)
                Printer::PrintItemWithFetch(i, userFavourites[i], api, shortList);

            std::cout << "Press any key then enter to continue or type q to stop" << std::endl;
            std::cin >> input;
        }
    }
    else
        std::cout << " Select favourites first!" << std::endl;
}

void GW2TPAPP::SelectFavouritesMenu()
{
    std::string input;
    std::vector<std::uint64_t> choices;
    std::cout << " ENTER THE ITEM NUMBER(S) SHOWN IN [] FINISH WITH q" << std::endl;

    auto sortedItems = PriceDatabase::Sorter::SortProfitableItems(priceDb, BUYQ_Limit, SELLQ_Limit, BUYG_MAX, BUYG_MIN);

    while (input != "q")
    {
        std::cin >> input;
        if (input != "q")
            choices.push_back(std::stoi(input));
    }

    std::cout << " You have selected : ";
    std::vector<uint64_t> selectedItems;
    for (const auto& choice : choices)
    {
        std::cout << choice << " ";
        selectedItems.push_back(sortedItems[choice].getid());
    }
    favDb.Write(selectedItems);

    std::cout << std::endl;
}

void GW2TPAPP::MainMenu()
{
    std::string input = "0";

	while (input != "q")
	{
        std::cout << termcolor::bright_blue 
        <<" [1] LIVE [2] FAVOURITES [3] OFFLINE [4] SELECT FAVOURITES [5] LIMITS [6] DATABASE " << termcolor::reset << std::endl
        << termcolor::magenta << " [s] SHORTEN(" << shortList << ")" << termcolor::reset
        << termcolor::red << " [q] QUIT" << termcolor::reset <<  std::endl;

        std::cin >> input;
		if (input == "1")
            LiveListMenu();
		else if (input == "2")
            LiveFavouritesMenu();
		else if (input == "3")
            OfflineListMenu();
		else if (input == "4")
            SelectFavouritesMenu();
		else if (input == "5")
            LimitsMenu();
		else if (input == "6")
			DatabaseMenu();
        else if (input == "s")
            shortList = !shortList;
        std::cout << termcolor::reset;
	};
}
