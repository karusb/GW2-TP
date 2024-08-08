#include "Api.hpp"
#include <sstream>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <thread>
#include "curl/HttpClientCurl.hpp"
#include "JsonHelper.hpp"

GW2API::GW2API(uint8_t maxRequestCountPerCall, IdDatabase& idDb, NameDatabase& nameDb, PriceDatabase& priceDb)
	: ITEM_REQ_COUNT(maxRequestCountPerCall)
	, idDb(idDb)
	, nameDb(nameDb)
	, priceDb(priceDb)
{}

bool GW2API::HttpGet(const std::string& url, std::string& response)
{
	HTTPClientCurl httpClient;
	httpClient.AddHeader("Host: api.guildwars2.com");
	return httpClient.Get(url, response);
}

void GW2API::PreloadItemNames(const std::vector<Item>& source, std::vector<ItemNameExtended>& to, int count)
{
	std::vector<Item> trimmedsource = {source.begin(), source.begin() + count};
	std::vector<std::uint64_t> ids;
	for (const auto& item : trimmedsource)
		ids.push_back(item.getid());
	auto details = PullItemNames(ids);
	
	for (const auto& detail : details)
		to.push_back({*std::find_if(trimmedsource.begin(), trimmedsource.end(), [&detail](const auto& item){ return detail.id == item.getid(); }), detail});
}

std::pair<std::string,std::string> GW2API::PullItemDetails(std::uint64_t it)
{
	std::string fixlink = "https://api.guildwars2.com/v2/items/";
	std::string requestitemid = std::to_string(it);
	fixlink.append(requestitemid);
	std::string stringresponse = "";
	HttpGet(fixlink, stringresponse);
	return std::make_pair(ParseJSON(&stringresponse, "name"), ParseJSON(&stringresponse, "rarity"));
}

ItemNameExtended GW2API::ExtendItem(const Item& item)
{
	auto pair = PullItemDetails(item.getid());
	ItemNameExtended extendedItem(item, pair.first, pair.second);
	return extendedItem;
}

Item GW2API::PullItemPrice(std::uint64_t it)
{
	std::string fixlink = "https://api.guildwars2.com/v2/commerce/prices/";
	std::string requestitemid = std::to_string(it);
	std::string id, buy_quantity, sell_quantity, buy_price, sell_price;

	fixlink.append(requestitemid);

	std::string stringresponse = "";
	HttpGet(fixlink, stringresponse);

	id = ParseJSON(&stringresponse, "id");
	Item brokenitem;
	if (id == "nan")
		return brokenitem;
	std::cout << std::endl;
	buy_quantity = ParseJSON(&stringresponse, "quantity", "buys");
	buy_price = ParseJSON(&stringresponse, "unit_price", "buys");
	sell_quantity = ParseJSON(&stringresponse, "quantity", "sells");
	sell_price = ParseJSON(&stringresponse, "unit_price", "sells");
	Item anitem(std::stoull(id), stoi(buy_price), stoi(buy_quantity), stoi(sell_price), stoi(sell_quantity));
	return anitem;
}

bool GW2API::PullItemPrices(int itempos)
{
	std::string fixlink = "https://api.guildwars2.com/v2/commerce/prices?ids=";
	std::string requestitemid;
	std::string currID = "?", bcurrID = "?";
	std::string id, buy_quantity, sell_quantity, buy_price, sell_price;

	for (std::size_t i = 0 + itempos; i < ITEM_REQ_COUNT + itempos && i < idDb.Get().size(); i++)
	{
		fixlink.append(std::to_string(idDb.Get()[i]));
		fixlink.append(",");
	}

	std::string stringresponse = "";
	HttpGet(fixlink, stringresponse);

	for (std::size_t i = 0 + itempos; i < ITEM_REQ_COUNT + itempos && i <  idDb.Get().size(); i++)
	{
		currID = "";
		currID.append("\"id\": ");
		currID.append(std::to_string(idDb.Get()[i]));

		id = ParseJSON(&stringresponse, "id", currID);
		buy_quantity = ParseJSON(&stringresponse, "quantity", "buys", currID);
		buy_price = ParseJSON(&stringresponse, "unit_price", "buys", currID);
		sell_quantity = ParseJSON(&stringresponse, "quantity", "sells", currID);
		sell_price = ParseJSON(&stringresponse, "unit_price", "sells", currID);
		bcurrID = currID;
		if (id == "nan")
			return false;
		Item anitem(stoull(id), stoi(buy_price), stoi(buy_quantity), stoi(sell_price), stoi(sell_quantity));
		priceDb.Get().push_back(anitem);
	}
	return true;
}

void GW2API::InitItemsListDB()
{
	std::string stringresponse = "";
	HttpGet("https://api.guildwars2.com/v2/commerce/prices", stringresponse);
	stringresponse.erase(std::remove_if(stringresponse.begin(), stringresponse.end(), &IsMarkedForRemoval), stringresponse.end());
	
	idDb.WriteRaw(stringresponse);
}

std::vector<ItemIdentifier> GW2API::PullItemNames(const std::vector<std::uint64_t>& items)
{
	std::string fixlink = "https://api.guildwars2.com/v2/items?ids=";
	std::string response = "";
	std::string key = "\"id\": ";

	for (const auto& item : items)
	{
		fixlink.append(std::to_string(item));
		fixlink.append(",");
	}

	HttpGet(fixlink, response);

	std::vector<ItemIdentifier> itemIdentifiers;

	auto name = ParseJSON(&response, "name");
	if (name == "nan")
		return std::vector<ItemIdentifier>{};

	while (!response.empty() && !Value(FromToken(key, response)).empty())
	{
		std::string idKey = key;
		auto id = Value(FromToken(idKey, response)).substr(1);
		auto rarity = ToToken(",", FromToken("\"rarity\":", response, 10), 0);
		response = response.substr(response.find(idKey.append(id)) + idKey.size());

		ItemIdentifier itemIdentifier{std::stoull(id), name, ItemNameExtended::RarityStringToEnum(rarity)};
		itemIdentifiers.push_back(itemIdentifier);

		auto details = FromToken("\"details\": {", ToToken("}", response));
		if (!details.empty())
			response = response.substr(response.find(details) + details.size());

		name = ParseJSON(&response, "name");
		response = response.substr(response.find(name) + name.size());
	}
	// some id's do not return any response
	// when queried, item API says the id is invalid, tough that's the reported item list from TP API
	// for example https://api.guildwars2.com/v2/items?ids=14907,14900,14913,14915,14918 returns only 1 result
	return itemIdentifiers;
}

bool GW2API::PullItemPrices(std::vector<Item>&items, std::vector<ItemIdentifier>&identifierList, int itempos)
{
	std::string fixlink = "https://api.guildwars2.com/v2/commerce/prices?ids=";
	std::string requestitemid;
	std::string currID = "?", bcurrID = "?";
	std::string id, buy_quantity, sell_quantity, buy_price, sell_price;

	for (int i = 0 + itempos; i < ITEM_REQ_COUNT + itempos && i < identifierList.size(); i++)
	{
		fixlink.append(std::to_string(identifierList[i].id));
		fixlink.append(",");
	}

	std::string stringresponse = "";
	HttpGet(fixlink, stringresponse);

	for (const auto& identifier : identifierList)
	{
		currID = "";
		currID.append("\"id\": ");
		currID.append(std::to_string(identifier.id));

		id = ParseJSON(&stringresponse, "id", currID);
		
		buy_quantity = ParseJSON(&stringresponse, "quantity", "buys", currID);
		buy_price = ParseJSON(&stringresponse, "unit_price", "buys", currID);
		sell_quantity = ParseJSON(&stringresponse, "quantity", "sells", currID);
		sell_price = ParseJSON(&stringresponse, "unit_price", "sells", currID);
		bcurrID = currID;
		if (id == "nan")
			return false;

		ItemNameExtended anitem(identifier, stoi(buy_price), stoi(buy_quantity), stoi(sell_price), stoi(sell_quantity));
		items.push_back(anitem);
	}
	return true;
}

void GW2API::InitExtendedItemsListDB()
{
	std::vector<std::uint64_t> ids;
	std::string line;
	std::string response;

	std::cout << "Downloading item IDs...";
	HttpGet("https://api.guildwars2.com/v2/commerce/prices", response);
	std::cout << "DONE" << std::endl;
	
	response.erase(std::remove_if(response.begin(), response.end(), &IsMarkedForRemoval), response.end());

	std::stringstream idStream(response);
	while (!idStream.eof())
	{
		idStream >> line;
		ids.push_back(std::stoull(line));
	}

	std::cout << "Total number of items " << ids.size() << std::endl
			  << "Downloading item names..." << std::endl;

	std::ofstream db = nameDb.DbOutStream();
	db.seekp(0);
	db << std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()) << std::endl;

	for (int i = 0; i < ids.size(); i += ITEM_REQ_COUNT)
	{
		auto end = ids.begin();
		if (i + ITEM_REQ_COUNT > ids.size())
			end = ids.end();
		else 
			end = ids.begin() + i + ITEM_REQ_COUNT;
		std::vector<std::uint64_t> trimmedIds = {ids.begin() + i, end};
		auto itemIdentifiers = PullItemNames(trimmedIds);
		if (itemIdentifiers.empty())
		{
			std::cout << "|" << std::flush;
			std::this_thread::sleep_for(std::chrono::milliseconds(10000));
			itemIdentifiers = PullItemNames(trimmedIds);
			
			if (itemIdentifiers.empty())
				throw;
		}

		db << itemIdentifiers;

		if (i % 600 == 0)
			std::cout << "=" << std::flush;
	}

	std::cout << "DONE" << std::endl;
}

void GW2API::InitItemPriceDatabase()
{
	priceDb.Get().clear();
	for (int i = 0; i < idDb.Get().size(); i += ITEM_REQ_COUNT)
	{
		if (!PullItemPrices(i))
			{
				std::cout << "|" << std::flush;
				std::this_thread::sleep_for(std::chrono::milliseconds(20000));
				if (!PullItemPrices(i))
				{
					std::cout << "1 retry wasn't enough at " << i << std::flush;
					throw;
				}
			}
		if (i % 600 == 0)
			std::cout << "=" << std::flush;
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	std::cout << " Download Complete..." << std::endl;

	priceDb.WriteBack();

	std::cout << " Database saved successfully..." << std::endl;
}
