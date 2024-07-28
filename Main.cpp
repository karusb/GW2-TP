#if WIN32
//#include <windows.h>
//#pragma comment(lib, "ws2_32.lib")
#endif
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

	BazPO::Cli cli(argc, argv, "Guild Wars 2 Trading Post Tool / GW2-TP v2.0");
	
	cli.flag("-e",[&](const auto& option)
	{ 
		idDb.Erase();
		nameDb.Erase();
		priceDb.Erase();
		favDb.Erase();

		std::cout << "ALL DATABASES ARE ERASED!" << std::endl;
	} , "--erase", "Erase all databases before starting up");
	cli.option("-r",[&](const auto& option)
	{ 
		ITEM_REQ_COUNT = option.valueAs<std::uint8_t>();
		std::cout << "USING CUSTOM API REQUEST LIMIT: " << ITEM_REQ_COUNT << std::endl;
	} , "--requests", "Amount of items per API request when downloading databases").withMaxValueCount(1)
	.constrain(std::make_pair(1, 199));

	cli.parse();

	GW2API api (ITEM_REQ_COUNT, idDb, nameDb, priceDb);
	GW2TPAPP app(api, idDb, nameDb, priceDb, favDb);

	app.UserInitialize();
	app.MainMenu();

	return 0;
}