#include <iostream>
#include <sstream>
#include <fstream>
#include "curl/HttpClientCurl.hpp"
#include "BazPO.hpp"
#include "items/Api.hpp"
#include "items/App.hpp"
#include "items/Item.hpp"
#include "items/Database.hpp"
#include "items/JsonHelper.hpp"

int main(int argc, const char* argv[])
{
	std::uint8_t ITEM_REQ_COUNT = 180;

	IdDatabase idDb("items.list");
	NameDatabase nameDb("items.exlist");
	PriceDatabase priceDb("items.db");
	FavouritesDatabase favDb {"favourites.fav"};
	SortParameters parameters;

	BazPO::Cli cli(argc, argv, "Guild Wars 2 Trading Post Tool / GW2-TP v2.0");
	
	cli.flag("-e",[&](const auto& option)
	{ 
		idDb.Erase();
		nameDb.Erase();
		priceDb.Erase();
		std::cout << "ALL DATABASES ARE ERASED!" << std::endl;
	} , "Erase all databases before starting up (except favourite database)", "--erase");
	cli.flag("-ef",[&](const auto& option)
	{ 
		favDb.Erase();
		std::cout << "FAVOURITE DATABASE IS ERASED!" << std::endl;
	} , "Erase favorite database before starting up", "--erasefavs");
	cli.option("-r",[&](const auto& option)
	{ 
		ITEM_REQ_COUNT = option.template valueAs<std::uint8_t>();
		std::cout << "USING CUSTOM API REQUEST LIMIT: " << ITEM_REQ_COUNT << std::endl;
	} , "--requests", "Amount of items per API request when downloading databases", "180").withMaxValueCount(1)
	.constrain(std::make_pair(1, 199));
	cli.option("-bq",[&](const auto& option)
	{ 
		parameters.BUYQ_Limit = option.template valueAs<std::uint64_t>();
		std::cout << "USING BUY QUANTITY LIMIT: " << parameters.BUYQ_Limit << std::endl;
	} , "--buyquantity", "Minimum amount of BUY orders required for an item", "550").withMaxValueCount(1);
	cli.option("-sq",[&](const auto& option)
	{ 
		parameters.SELLQ_Limit = option.template valueAs<std::uint64_t>();
		std::cout << "USING SELL QUANTITY LIMIT: " << parameters.SELLQ_Limit << std::endl;
	} , "--sellquantity", "Minimum amount of SELL orders required for an item", "550").withMaxValueCount(1);
	cli.option("-p",[&](const auto& option)
	{ 
		auto values = option.template valuesAs<std::uint64_t>();
		parameters.BUYG_MIN = values[0]; 
		parameters.BUYG_MAX = values[1]; 
		std::cout << "USING BUY PRICE LIMITS MIN-MAX: " << parameters.BUYG_MIN << " - " << parameters.BUYG_MAX << std::endl;
	} , "--price", "Buying price limits in format [min max]", "0 9999999", BazPO::OptionType::MultiValue).withMaxValueCount(2).constrain(std::make_pair(0, 9999999));
	cli.parse();

	GW2API api(ITEM_REQ_COUNT, idDb, nameDb, priceDb);
	GW2TPAPP app(api, parameters, idDb, nameDb, priceDb, favDb);

	app.UserInitialize();
	app.MainMenu();

	return 0;
}