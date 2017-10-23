#include "stdafx.h"

#include <windows.h>
#include <iostream>
#include <vector>
#include <locale>
#include <sstream>
#include <ctime>
#include <fstream>
#include <cmath>
#include <math.h>
#include <curl/curl.h>
#include <stdio.h>
#include <algorithm>
#include "items.cpp"

//#define _WINSOCK_DEPRECATED_NO_WARNINGS_
using namespace std;
#pragma comment(lib,"ws2_32.lib")
int ITEM_COUNT = 24476;
int ITEM_REQ_COUNT = 100;
int SELLQ_Limit=550;
int BUYQ_Limit=550;
int BUYG_MIN = 0;
int BUYG_MAX = 9999999;

char buffer[10000];
char *APIHc;
string website_HTML;
string stringresponse;

string APIH = "Authorization: Bearer ";
//****************************************************
// Functions

int getstringinputsizeJSON(string *inputfile, string tobefound, string tobefoundwithin = "?", string tobefoundwithinfirst = "?");
int getstringinputpositionJSON(string *inputfile, string tobefound, string tobefoundwithin = "?", string tobefoundwithinfirst ="?");
string ParseJSON(string *inputfile, string tobefound, string tobefoundwithin = "?", string tobefoundwithinfirst = "?" );
item ParseJSON_ITEM(int id);
void InitJSON_ITEMSDB(item* items,int* itemslist,int itempos);
string ParseJSON_ITEMName(int it);
void get_Website(char *url);
size_t CURLWrite_Callback_String(void *contents, size_t size, size_t nmemb, std::string *s);
void itemlistload(string filename,int *itemlist);
void itemdbload(string filename, item *itemdb);
int itemdbintegrity(string filename);
void InitItemsListDB(string itemdbname);

int main(void) {
	////////////////////////
	// Variables
	int requestitem = 0;
	int user_choice[20], user_choice_count = 0;
	int favj = 0;
	bool favselect = false;
	item items[30000];
	item favitems[10000];
	item user_favitems[50];
	int *itemslist;
	ofstream itemsout;
	ofstream itemtpout;
	ifstream APIin;
	ifstream filecheck;
	string user_in = "0";
	string API_KEY;
	//string EXAMPLE = "\{ /r/n\"id\": 19684,/r/n\"whitelisted\" : false,/r/n\"buys\" : {/r/n	\"quantity\": 145975,/r/n		\"unit_price\" : 7018},/r/n\"sells\" : {/r/n		\"quantity\": 126,/r/n			\"unit_price\" : 7019/r/n	}}";
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// READ API KEY FROM FILE
	//APIin.open("APIkey.txt");
	//if (!APIin.is_open()) cout << "APIkey.txt was not read" << endl;
	//APIin >> API_KEY;
	//APIin.close();
	//APIH.append(API_KEY);

	APIHc = new char[APIH.length() + 1];
	strcpy_s(APIHc, APIH.length() + 1, APIH.c_str());
	//if (API_KEY == "XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXXXXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX") cout << "Please provide your API KEY in the APIKey.txt file" << endl;
	

	// LOAD THE LIST OF ITEMS TO BE FETCHED
	filecheck.open("items.list");
	if (filecheck.is_open())
	{
		filecheck.close();
		ITEM_COUNT = itemdbintegrity("items.list") - 1;
		itemslist = new int[ITEM_COUNT];
		itemlistload("items.list", itemslist);
	}
	else
	{
		filecheck.close();
		cout << "Item list cannot be found, rebuilding..." << endl;
		InitItemsListDB("items.list");
		ITEM_COUNT = itemdbintegrity("items.list") - 1;
		itemslist = new int[ITEM_COUNT];
		itemlistload("items.list", itemslist);

	}
	
	cout << "Currently " << ITEM_COUNT << " can be sold or bought from TP" << endl;
	filecheck.open("items.db");
	if (filecheck.is_open())
	{
		filecheck.close();
		itemdbload("items.db", items);
		if (itemdbintegrity("items.db") == ITEM_COUNT) cout << "INTEGRITY CHECK PASSED" << endl;
		else cout << itemdbintegrity("items.db") << " : Your item database may not be correct please rebuild DB" << endl;
	}
	else
	{

		cout << "Item Price Database was not found to rebuilding database..." << endl;
		// REBUILD DATABASE ALGO
		for (int i = 0; i < ITEM_COUNT; i += ITEM_REQ_COUNT)
		{
			InitJSON_ITEMSDB(items, itemslist, i);
			if (i % 600 == 0) cout << "=";
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
		if (itemdbintegrity("items.db") == ITEM_COUNT) cout << "INTEGRITY CHECK PASSED" << endl;
		else cout << itemdbintegrity("items.db") << " : Your item database may not be correct please rebuild DB" << endl;
	}
	// UI BEGIN 

		// OLD DB ALGO
		//for (int i = 0; i < 300; i++)cout << itemslist[i];
		cout << "\n\n\n";
		// CONTROL DATABASE - ONLY REBUILD DB IF REQUESTED (user control)
		//requestitem = 19684;
		//itemsout.open("items.db");
		//for (int i = 0; i < ITEM_COUNT; i++)
		//{
		//	requestitem = itemslist[i];
		//	items[i] = ParseJSON_ITEM(requestitem);
		//	cout << items[i] << endl;
		//	itemsout << items[i] << endl;
		//}
		//itemsout.close();
		// OLD DB ALGO


		std::sort(items, items + ITEM_COUNT);			

		cout << " [1] LIST [2] LIVE UPDATE FAV [3] SELECT FAVOURITES [4] CHANGE LIMITS [5] REBUILD DATABASE [esc] QUIT" << endl;
		cin >> user_in;
		do
		{
			if (user_in == "1")
			{
				cout << "How many items to show? ";
				cin >> user_in;
				int parsesize = stoi(user_in);
				//itemtpout.open("TP.txt");
				for (int i = 0; i < ITEM_COUNT; i++)
				{
					if (items[i].buyquantity() >= BUYQ_Limit && items[i].sellquantity() >= SELLQ_Limit && items[i].buyprice() <= BUYG_MAX && items[i].buyprice() >= BUYG_MIN)
					{
						favitems[favj] = items[i];
						favj += 1;
						//itemtpout << items[i] << endl;
					}
				}
				//itemtpout.close();
				for (int i =0; i < parsesize && i < favj;i++)cout << "[ " << i <<" ]"<< ParseJSON_ITEMName(favitems[i].getid()) << " Profit : " << favitems[i].profit << endl << " Buy Quantity : " << favitems[i].buyquantity() << " Sell Quantity : " << favitems[i].sellquantity() << endl << " Buy Price : " << favitems[i].buyprice() << " Sell Price : " << favitems[i].sellprice() << endl;
				cout << " TYPE u to UPDATE FETCHED LIST or any key to continue" << endl;
				cin >> user_in;
				if (user_in == "u" || user_in == "U")
				{
					while (user_in != "esc")
					{
						for (int i = 0; i < parsesize; i++) {
							favitems[i] = ParseJSON_ITEM(favitems[i].getid());
						}
						std::sort(favitems, favitems + favj);
						for (int i = 0; i < parsesize; i++)cout << "[ " << i << " ]" << ParseJSON_ITEMName(favitems[i].getid()) << " Profit : " << favitems[i].profit << endl << " Buy Quantity : " << favitems[i].buyquantity() << " Sell Quantity : " << favitems[i].sellquantity() << endl << " Buy Price : " << favitems[i].buyprice() << " Sell Price : " << favitems[i].sellprice() << endl;

						cout << "Press any key then enter to continue or type esc to stop" << endl;
						cin >> user_in;
					}
				}
			}

			if (user_in == "3")
			{
				cout << " ENTER THE ITEM NUMBER(S) SHOWN IN [] FINISH WITH esc" << endl;
				user_choice_count = 0;

				while (user_in != "esc") {
					cin >> user_in;
					if (user_in != "esc") {
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
				if (favouriteinstream.is_open()) {
					
					while (user_in != "esc")
					{
						favouriteinstream >> user_choice_count;
						for (int i = 0; i < user_choice_count; i++) {
							favouriteinstream >> favstreaminS;
							user_favitems[i] = ParseJSON_ITEM(stoi(favstreaminS));
						}
						std::sort(user_favitems, user_favitems + user_choice_count);
						for(int i=0 ; i<user_choice_count ; i++)cout << "[ " << i << " ]" << ParseJSON_ITEMName(user_favitems[i].getid()) << " Profit : " << user_favitems[i].profit << endl << " Buy Quantity : " << user_favitems[i].buyquantity() << " Sell Quantity : " << user_favitems[i].sellquantity() << endl << " Buy Price : " << user_favitems[i].buyprice() << " Sell Price : " << user_favitems[i].sellprice() << endl;
						favouriteinstream.seekg(0);

						cout << "Press any key then enter to continue or type esc to stop" << endl;
						cin >> user_in;
					}
					favouriteinstream.close();
				}
				else cout << " Select favourites first!" << endl;
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
				InitItemsListDB("items.list");
				ITEM_COUNT = itemdbintegrity("items.list") - 1;
				itemslist = new int[ITEM_COUNT];
				itemlistload("items.list", itemslist);
				cout << "Downloading item prices... " << endl;
				// REBUILD DATABASE ALGO
				for (int i = 0; i < ITEM_COUNT; i += ITEM_REQ_COUNT)
				{
					InitJSON_ITEMSDB(items, itemslist, i);
					if (i % 600 == 0) cout << "=";
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
				if (itemdbintegrity("items.db") == ITEM_COUNT) cout << "INTEGRITY CHECK PASSED" << endl;
				else cout << itemdbintegrity("items.db") << " : Your item database may not be correct please rebuild DB" << endl;
				std::sort(items, items + ITEM_COUNT);
			}

			cout << " [1] LIST [2] LIVE UPDATE FAV [3] SELECT FAVOURITES [4] CHANGE LIMITS [5] REBUILD DATABASE [esc] QUIT" << endl;
			cin >> user_in;
		} while (user_in != "esc");
	
	return 0;
}

void itemlistload(string filename, int *itemlist)
{
	fstream itemliststream(filename,std::ios_base::in);
	string itemliststring;
	if (!itemliststream.is_open())
	{
		cout << "item file not open bob";
		InitItemsListDB(filename);
	}
	for (int i = 0; i < ITEM_COUNT; i++)
	{
		itemliststream >> itemliststring;
		itemlist[i] = stoi(itemliststring);
	}
	itemliststream.close();
}
void itemdbload(string filename, item *itemdb)
{
	fstream itemdbstream(filename, std::ios_base::in);
	string itemdbstring;
	if (!itemdbstream.is_open())cout << "itemdb file not open bob";
	else
	{
		for (int i = 0; i < ITEM_COUNT; i++)
		{
			if(!itemdbstream.eof())itemdbstream >> itemdb[i];
		}
	}
	itemdbstream.close();
}
int itemdbintegrity(string filename)
{
	fstream itemdbstream(filename, std::ios_base::in);
	string itemdbstring;
	int line = 0;
	if (!itemdbstream.is_open())return -1;
	else
	{
		while (std::getline(itemdbstream, itemdbstring))++line;
		itemdbstream.close();
		return line;
	}
	
}
//****************************************************
string ParseJSON(string *inputfile, string tobefound, string tobefoundwithin, string tobefoundwithinfirst)
{
	int size, pos;
	stringstream parsedstringstream;
	string parsedstring;
	
	if(inputfile->find("no such id")!=string::npos)return "nan";
	size = getstringinputsizeJSON(inputfile, tobefound,tobefoundwithin,tobefoundwithinfirst);
	pos = getstringinputpositionJSON(inputfile, tobefound, tobefoundwithin,tobefoundwithinfirst);
	//cout << size << pos << endl;
	for (int i = 0; i < size; i++)parsedstringstream << inputfile->at(i + pos);
	parsedstring = parsedstringstream.str();
	//cout << "Parsedstring output:" << parsedstring << endl;
	return parsedstring;
}
string ParseJSON_ITEMName(int it)
{
	string fixlink = "https://api.guildwars2.com/v2/items/";
	string requestitemid = to_string(it);
	fixlink.append(requestitemid);
	char *newfixlink = new char[fixlink.length() + 1];
	strcpy_s(newfixlink, fixlink.length() + 1, fixlink.c_str());
	stringresponse = "";
	get_Website(newfixlink);
	return ParseJSON(&stringresponse, "name");
}
item ParseJSON_ITEM(int it)
{
	string fixlink = "https://api.guildwars2.com/v2/commerce/prices/";
	string requestitemid = to_string(it);
	string id, buy_quantity, sell_quantity, buy_price, sell_price;
	//STR to CHAR
	
	fixlink.append(requestitemid);
	char *newfixlink = new char[fixlink.length() + 1];
	strcpy_s(newfixlink,fixlink.length() +1,fixlink.c_str());
	// MAKE THE CONNECTION
	stringresponse = "";
	get_Website(newfixlink);

	//cout << "Response read successfully." << endl;
	//cout << stringresponse << endl;

	// PARSE RESPONSE 
	//cout << "Parse Response" << endl;
	id = ParseJSON(&stringresponse, "id");
	item brokenitem;
	if (id == "nan") return brokenitem;
	cout << endl;
	buy_quantity = ParseJSON(&stringresponse, "quantity", "buys");
	buy_price = ParseJSON(&stringresponse, "unit_price", "buys");
	sell_quantity = ParseJSON(&stringresponse, "quantity", "sells");
	sell_price = ParseJSON(&stringresponse, "unit_price", "sells");
	//cout << "FOUND :" << id << " " << buy_price << " " << buy_quantity << " " << sell_price << " " << sell_quantity << endl;
	//cout << "Item:";
	item anitem(stoi(id),stoi(buy_price),stoi(buy_quantity),stoi(sell_price),stoi(sell_quantity));
	return anitem;
}
void InitJSON_ITEMSDB(item* items,int* itemslist,int itempos)
{
	string fixlink = "https://api.guildwars2.com/v2/commerce/prices?ids=";
	string requestitemid;
	string currID = "?",bcurrID = "?";
	string id, buy_quantity, sell_quantity, buy_price, sell_price;
	//STR to CHAR
	for (int i = 0+itempos; i < ITEM_REQ_COUNT+itempos && i < ITEM_COUNT; i++)
	{
		fixlink.append(to_string(itemslist[i]));
		fixlink.append(",");
	}
	
	char *newfixlink = new char[fixlink.length() + 1];
	strcpy_s(newfixlink, fixlink.length() + 1, fixlink.c_str());
	// MAKE THE CONNECTION
	stringresponse = "";
	get_Website(newfixlink);

	//cout << "Response read successfully." << endl;
	//cout << stringresponse << endl;

	// PARSE RESPONSE 
	//cout << "Parse Response" << endl;
	for (int i = 0 + itempos; i < ITEM_REQ_COUNT + itempos && i < ITEM_COUNT; i++)
	{
		currID = "";
		currID.append("\"id\": ");
		currID.append(to_string(itemslist[i]));

		id = ParseJSON(&stringresponse, "id",currID);
		buy_quantity = ParseJSON(&stringresponse, "quantity", "buys",currID);
		buy_price = ParseJSON(&stringresponse, "unit_price", "buys",currID);
		sell_quantity = ParseJSON(&stringresponse, "quantity", "sells",currID);
		sell_price = ParseJSON(&stringresponse, "unit_price", "sells",currID);
		//cout << "FOUND :" << id << " " << buy_price << " " << buy_quantity << " " << sell_price << " " << sell_quantity << endl;
		//cout << "Item:";
		bcurrID = currID;
		//if (i < 99 + itempos && i < ITEM_COUNT - 1)
		//{

		//}
		item anitem(stoi(id), stoi(buy_price), stoi(buy_quantity), stoi(sell_price), stoi(sell_quantity));
		items[i] = anitem;
	}
}
void InitItemsListDB(string itemdbname)
{
	ofstream Fitemslist;
	get_Website("https://api.guildwars2.com/v2/commerce/prices");
	Fitemslist.open(itemdbname);
	Fitemslist.seekp(0);
	for (int i = 0; i < stringresponse.length(); i++)
	{
		if (stringresponse[i] == '[' || stringresponse[i] == ' ' || stringresponse[i] == ']')stringresponse.erase(i, 1);
	}

	Fitemslist << stringresponse;
	Fitemslist.close();
}
/* int getstringinputsizeJSON (string inputfile, string tobefound)
@Description  Counts the data size of the variable given in the config file
@libDependencies - string.h
@param string *inputfile - A reference to the string to be searched
@param string tobefound - A string that is to be found in inputfile
@return int - Returns the data size after the given variable to the end of line

@notice - Doesn't work with integer variables, create another function and change find variable for eolpos to "," only.
@example &inputfile
"rpcAddress": "192.168.1.1",
@example tobefound = \"rpcAddress\": \"
@example return = int 11

*/
int getstringinputsizeJSON(string *inputfile, string tobefound,string tobefoundwithin, string tobefoundwithinfirst)
{
	size_t strpos=0;
	int size = 0;
	size_t eolpos,eolpos2;
	size_t datastartpos;
	
	if(tobefoundwithin!="?" && tobefoundwithinfirst =="?")strpos = inputfile->find(tobefoundwithin);
	if (tobefoundwithinfirst != "?")strpos = inputfile->find(tobefoundwithinfirst);
	if(tobefoundwithin != "?" && tobefoundwithinfirst != "?")strpos = inputfile->find(tobefoundwithin,strpos);
	strpos = inputfile->find(tobefound,strpos);
	datastartpos = inputfile->find(":", strpos);
	eolpos = inputfile->find(",", datastartpos);
	eolpos2 = inputfile->find("\n", datastartpos);
	bool eol = false;
	for (int i = datastartpos + 2; i < inputfile->size() && !eol; i++)
	{

		//cout << i;
		if (i == eolpos || i==eolpos2)eol = true;
		else
		{
			size += 1;
			//cout << size;
		}
	}
	return size;
}
/* int getstringinputsizeJSON (string inputfile, string tobefound)
@Description  Counts the data size of the variable given in the config file
@libDependencies - string.h
@param string *inputfile - A reference to the string to be searched
@param string tobefound - A string that is to be found in inputfile
@return int - Returns the data size after the given variable to the end of line

@notice - Doesn't work with integer variables, create another function and change find variable for eolpos to "," only.
@example &inputfile
"rpcAddress": "192.168.1.1",
@example tobefound = \"rpcAddress\": \"
@example return = int 11

*/
int getstringinputpositionJSON(string *inputfile, string tobefound, string tobefoundwithin, string tobefoundwithinfirst)
{
	size_t strpos = 0;
	int size = 0;
	size_t eolpos,datastartpos;
	if (tobefoundwithin != "?")strpos = inputfile->find(tobefoundwithin);
	if (tobefoundwithinfirst != "?")strpos = inputfile->find(tobefoundwithinfirst);
	if (tobefoundwithin != "?" && tobefoundwithinfirst != "?")strpos = inputfile->find(tobefoundwithin, strpos);
	strpos = inputfile->find(tobefound, strpos);
	datastartpos = inputfile->find(":", strpos);
	eolpos = inputfile->find(",", datastartpos);

	return datastartpos+2;
}

// CURL CODE TO MAKE THE HTTPS REQUESTS
void get_Website(char *url) {
	CURL *curl;
	CURLcode res;
	string get_http, get_auth;
	

	//get_auth = "GET /v2/tokeninfo/ HTTP/1.1\r\nHost: " + url + "\r\nAuthorization: Bearer " + API_KEY + "\r\nConnection: close\r\n\r\n";
	//get_http = "GET /v2/commerce/listings/19684/ HTTP/1.1\r\nHost: " + url + "\r\nConnection: close\r\n\r\n";
	//size_t request_len = get_auth.size;

	curl_global_init(CURL_GLOBAL_DEFAULT);
	curl = curl_easy_init();
	if (curl) {
		struct curl_slist *chunk = NULL;
		chunk = curl_slist_append(chunk, "Host: api.guildwars2.com");
		chunk = curl_slist_append(chunk, APIHc);
		res = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
		curl_easy_setopt(curl, CURLOPT_URL, url);
		//curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CURLWrite_Callback_String);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &stringresponse);
		/* Perform the request, res will get the return code */
		res = curl_easy_perform(curl);
		//cout << res << endl;
		/* Check for errors */
		if (res != CURLE_OK)
			fprintf(stderr, "curl_easy_perform() failed: %s\n",
				curl_easy_strerror(res));

		/* always cleanup */
		curl_easy_cleanup(curl);
		curl_slist_free_all(chunk);
	}

	curl_global_cleanup();
	

}
size_t CURLWrite_Callback_String(void *contents, size_t size, size_t nmemb, std::string *s)
{
	size_t newLength = size*nmemb;
	size_t oldLength = s->size();
	try
	{
		s->resize(oldLength + newLength);
	}
	catch (std::bad_alloc &e)
	{
		//handle memory problem
		return 0;
	}

	std::copy((char*)contents, (char*)contents + newLength, s->begin() + oldLength);
	return size*nmemb;
}