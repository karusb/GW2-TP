#if WIN32
//#include <windows.h>
//#pragma comment(lib, "ws2_32.lib")
#endif
#include <iostream>
#include <sstream>
#include <fstream>
#include "curl/HttpClientCurl.hpp"
#include "BazPO.hpp"
#include <stdio.h>
#include <cstring>
#include <algorithm>
#include "items/Item.hpp"
#include "items/Database.hpp"
#include "items/JsonHelper.hpp"
#include <chrono>
#include <thread>

using namespace std;
bool HttpGet(const std::string& url, std::string& response);
Item ParseJSON_ITEM(int id);
bool InitJSON_ITEMSDB(Item *items, int *itemslist, int itempos);
string ParseJSON_ITEMName(int it);
void InitItemsListDB(string itemdbname);

namespace
{
	int ITEM_COUNT = 27480;
	int ITEM_REQ_COUNT = 180;
	int SELLQ_Limit = 550;
	int BUYQ_Limit = 550;
	int BUYG_MIN = 0;
	int BUYG_MAX = 9999999;
	int *itemslist;
	Item items[30000];
	std::vector<Item> favitems;
	string stringresponse;
	string APIH = "Authorization: Bearer ";
}

bool HttpGet(const std::string& url, std::string& response)
{
	HTTPClientCurl httpClient;
	httpClient.AddHeader("Host: api.guildwars2.com");
	return httpClient.Get(url, response);
}

bool LoadItemsList()
{
	ifstream filecheck;
	// LOAD THE LIST OF ITEMS TO BE FETCHED
	filecheck.open("items.list");
	if (filecheck.is_open())
	{
		filecheck.close();
		ITEM_COUNT = itemdbintegrity("items.list") - 1;
		itemslist = new int[ITEM_COUNT];
		itemlistload("items.list", itemslist, ITEM_COUNT, ::InitItemsListDB);
		return true;
	}
	return false;
}

void RebuildItemsList()
{
	InitItemsListDB("items.list");
	ITEM_COUNT = itemdbintegrity("items.list") - 1;
	itemslist = new int[ITEM_COUNT];
	itemlistload("items.list", itemslist, ITEM_COUNT, ::InitItemsListDB);
}

bool LoadItemPriceDatabase()
{
	ifstream filecheck;

	filecheck.open("items.db");
	if (filecheck.is_open())
	{
		filecheck.close();
		itemdbload("items.db", items, ITEM_COUNT);
		if (itemdbintegrity("items.db") == ITEM_COUNT)
			cout << "INTEGRITY CHECK PASSED" << endl;
		else
			cout << itemdbintegrity("items.db") << " : Your item database is outdated please rebuild DB" << endl;
	}
	else
		return false;
	return true;
}

void RebuildItemPriceDatabase()
{
	ofstream itemsout;
	// REBUILD DATABASE ALGO
	for (int i = 0; i < ITEM_COUNT; i += ITEM_REQ_COUNT)
	{
		if (!InitJSON_ITEMSDB(items, itemslist, i))
			{
				cout << "|" << flush;
				std::this_thread::sleep_for(std::chrono::milliseconds(20000));
				if (!InitJSON_ITEMSDB(items, itemslist, i))
				{
					cout << "1 retry wasn't enough at " << i << flush;
					throw;
				}
			}
		if (i % 600 == 0)
			cout << "=" << flush;
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	cout << " Download Complete..." << endl;
	itemsout.open("items.db");
	for (int i = 0; i < ITEM_COUNT; i++)
	{
		itemsout << items[i] << endl;
	}
	itemsout.close();
	cout << " Database saved successfully..." << endl;
	// REBUILD DATABASE ALGO
	if (itemdbintegrity("items.db") == ITEM_COUNT)
		cout << "INTEGRITY CHECK PASSED" << endl;
	else
		cout << itemdbintegrity("items.db") << " : Your item database may not be correct please rebuild DB" << endl;

	std::sort(items, items + ITEM_COUNT);
}

void SortProfitableItems()
{
	for (int i = 0; i < ITEM_COUNT; i++)
		if (items[i].buyquantity() >= BUYQ_Limit && items[i].sellquantity() >= SELLQ_Limit && items[i].buyprice() <= BUYG_MAX && items[i].buyprice() >= BUYG_MIN)
			favitems.push_back(items[i]);

	std::sort(favitems.begin(), favitems.end());
}

void PrintItem(int number, const Item& item, std::string_view name)
{
	cout << "[ " << number << " ]" << name << " Profit : " << item.profit << endl
			<< " Buy Quantity : " << item.buyquantity() << " Sell Quantity : " << item.sellquantity() << endl
			<< " Buy Price : " << item.buyprice() << " Sell Price : " << item.sellprice() << endl << endl;
}

void PrintItemWithFetch(int number, const Item& item)
{
	PrintItem(number, item, ParseJSON_ITEMName(item.getid()));
}

void PrintExtendedItem(int number, const ItemNameExtended& item)
{
	PrintItem(number, item, item.getname());
}

int main(int argc, const char* argv[])
{
	// Variables
	int requestitem = 0;
	int user_choice[20], user_choice_count = 0;
	bool favselect = false;
	Item user_favitems[50];

	ofstream itemsout;
	ofstream itemtpout;
	string user_in = "0";

	if (!LoadItemsList())
	{
		cout << "Item list cannot be found, rebuilding..." << endl;
		RebuildItemsList();
	}

	cout << "Currently " << ITEM_COUNT << " items can be sold or bought from TP" << endl;

	if (!LoadItemPriceDatabase())
	{
		cout << "Item Price Database was not found! Rebuilding database, this may take a while..." << endl;
		RebuildItemPriceDatabase();
	}

	// UI BEGIN

	cout << " [1] LIST [2] LIVE UPDATE FAV [3] SELECT FAVOURITES [4] CHANGE LIMITS [5] REBUILD DATABASE [esc] QUIT" << endl;
	cin >> user_in;
	do
	{
		if (user_in == "1")
		{
			cout << "How many items to show? ";
			cin >> user_in;
			int parsesize = stoi(user_in);

			SortProfitableItems();

			for (int i = 0; i < parsesize && i < favitems.size(); i++)
				PrintItemWithFetch(i, favitems[i]);

			cout << " TYPE u to UPDATE FETCHED LIST or any key to continue" << endl;
			cin >> user_in;
			if (user_in == "u" || user_in == "U")
			{
				while (user_in != "esc")
				{
					for (int i = 0; i < parsesize; i++)
						favitems[i] = ParseJSON_ITEM(favitems[i].getid());
					
					std::sort(favitems.begin(), favitems.end());

					for (int i = 0; i < parsesize; i++)
						PrintItemWithFetch(i, favitems[i]);

					cout << "Press any key then enter to continue or type esc to stop" << endl;
					cin >> user_in;
				}
			}
		}

		if (user_in == "3")
		{
			cout << " ENTER THE ITEM NUMBER(S) SHOWN IN [] FINISH WITH esc" << endl;
			user_choice_count = 0;

			while (user_in != "esc")
			{
				cin >> user_in;
				if (user_in != "esc")
				{
					user_choice[user_choice_count] = stoi(user_in);
					user_choice_count += 1;
				}
			}
			cout << " You have selected : ";
			itemtpout.open("favourites.fav");
			itemtpout << user_choice_count << endl;
			for (int i = 0; i < user_choice_count; i++)
			{
				cout << user_choice[i] << " ";
				itemtpout << favitems[user_choice[i]].getid() << endl;
			}
			itemtpout.close();
			cout << endl;
			favselect = true;
		}

		if (user_in == "2")
		{
			fstream favouriteinstream("favourites.fav", std::ios_base::in);
			string favstreaminS;
			if (favouriteinstream.is_open())
			{
				while (user_in != "esc")
				{
					favouriteinstream >> user_choice_count;
					for (int i = 0; i < user_choice_count; i++)
					{
						favouriteinstream >> favstreaminS;
						user_favitems[i] = ParseJSON_ITEM(stoi(favstreaminS));
					}
					std::sort(user_favitems, user_favitems + user_choice_count);

					for (int i = 0; i < user_choice_count; i++)
						PrintItemWithFetch(i, user_favitems[i]);

					favouriteinstream.seekg(0);

					cout << "Press any key then enter to continue or type esc to stop" << endl;
					cin >> user_in;
				}
				favouriteinstream.close();
			}
			else
				cout << " Select favourites first!" << endl;
		}

		if (user_in == "4")
		{
			cout << "What would you like to change? [1] Quantity Limits [2] Price Limits";
			cin >> user_in;
			if (user_in == "1")
			{
				cout << "Current MIN Limits : BUYING QUANTITY " << BUYQ_Limit << " SELLING QUANTITY " << SELLQ_Limit << endl;
				cout << "Input new limits and separate them with space" << endl;
				cin >> BUYQ_Limit >> SELLQ_Limit;
			}
			else if (user_in == "2")
			{
				cout << "Current Price Limits : MIN PRICE " << BUYG_MIN << " MAX PRICE " << BUYG_MAX << endl;
				cout << "Input new limits and separate them with space" << endl;
				cin >> BUYG_MIN >> BUYG_MAX;
			}
		}
		if (user_in == "5")
		{
			cout << "Downloading tradable item list... " << endl;
			RebuildItemsList();
			
			cout << "Downloading item prices... " << endl;
			RebuildItemPriceDatabase();
		}

		cout << " [1] LIST [2] LIVE UPDATE FAV [3] SELECT FAVOURITES [4] CHANGE LIMITS [5] REBUILD DATABASE [esc] QUIT" << endl;
		cin >> user_in;
	} while (user_in != "esc");

	return 0;
}

string ParseJSON_ITEMName(int it)
{
	string fixlink = "https://api.guildwars2.com/v2/items/";
	string requestitemid = to_string(it);
	fixlink.append(requestitemid);
	stringresponse = "";
	HttpGet(fixlink, stringresponse);
	return ParseJSON(&stringresponse, "name");
}

Item ParseJSON_ITEM(int it)
{
	string fixlink = "https://api.guildwars2.com/v2/commerce/prices/";
	string requestitemid = to_string(it);
	string id, buy_quantity, sell_quantity, buy_price, sell_price;

	fixlink.append(requestitemid);
	// MAKE THE CONNECTION
	stringresponse = "";
	HttpGet(fixlink, stringresponse);

	// PARSE RESPONSE
	id = ParseJSON(&stringresponse, "id");
	Item brokenitem;
	if (id == "nan")
		return brokenitem;
	cout << endl;
	buy_quantity = ParseJSON(&stringresponse, "quantity", "buys");
	buy_price = ParseJSON(&stringresponse, "unit_price", "buys");
	sell_quantity = ParseJSON(&stringresponse, "quantity", "sells");
	sell_price = ParseJSON(&stringresponse, "unit_price", "sells");
	Item anitem(stoi(id), stoi(buy_price), stoi(buy_quantity), stoi(sell_price), stoi(sell_quantity));
	return anitem;
}

bool InitJSON_ITEMSDB(Item *items, int *itemslist, int itempos)
{
	string fixlink = "https://api.guildwars2.com/v2/commerce/prices?ids=";
	string requestitemid;
	string currID = "?", bcurrID = "?";
	string id, buy_quantity, sell_quantity, buy_price, sell_price;
	// STR to CHAR
	for (int i = 0 + itempos; i < ITEM_REQ_COUNT + itempos && i < ITEM_COUNT; i++)
	{
		fixlink.append(to_string(itemslist[i]));
		fixlink.append(",");
	}
	// MAKE THE CONNECTION
	stringresponse = "";
	HttpGet(fixlink, stringresponse);

	// PARSE RESPONSE
	for (int i = 0 + itempos; i < ITEM_REQ_COUNT + itempos && i < ITEM_COUNT; i++)
	{
		currID = "";
		currID.append("\"id\": ");
		currID.append(to_string(itemslist[i]));

		id = ParseJSON(&stringresponse, "id", currID);
		buy_quantity = ParseJSON(&stringresponse, "quantity", "buys", currID);
		buy_price = ParseJSON(&stringresponse, "unit_price", "buys", currID);
		sell_quantity = ParseJSON(&stringresponse, "quantity", "sells", currID);
		sell_price = ParseJSON(&stringresponse, "unit_price", "sells", currID);
		bcurrID = currID;
		if (id == "nan")
			return false;
		Item anitem(stoi(id), stoi(buy_price), stoi(buy_quantity), stoi(sell_price), stoi(sell_quantity));
		items[i] = anitem;
	}
	return true;
}

void InitItemsListDB(string itemdbname)
{
	ofstream Fitemslist;
	HttpGet("https://api.guildwars2.com/v2/commerce/prices", stringresponse);
	Fitemslist.open(itemdbname);
	Fitemslist.seekp(0);
	for (int i = 0; i < stringresponse.length(); i++)
	{
		if (stringresponse[i] == '[' || stringresponse[i] == ' ' || stringresponse[i] == ']')
			stringresponse.erase(i, 1);
	}

	Fitemslist << stringresponse;
	Fitemslist.close();
}
