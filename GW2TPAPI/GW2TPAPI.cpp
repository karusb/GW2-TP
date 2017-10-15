#include "stdafx.h"
#include <string.h>
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
#define ITEM_COUNT 24465
int SELLQ_Limit=550;
int BUYQ_Limit=550;

char buffer[10000];
string website_HTML;
string stringresponse;
////////////////////////
// Constant strings
const string API_KEY = "F0415A36-8665-6049-AE2A-F97254BABE69E79410B8-61C4-49E3-B09D-693344F57B30";
const string API_Link = "api.guildwars2.com/v2"; // ADD all links in a class?
//****************************************************
// Functions
int getstringinputsizeJSON(string *inputfile, string tobefound, string tobefoundwithin = "?");
int getstringinputpositionJSON(string *inputfile, string tobefound, string tobefoundwithin = "?");
string ParseJSON(string *inputfile, string tobefound, string tobefoundwithin = "?");
item ParseJSON_ITEM(int id);
string ParseJSON_ITEMName(int it);
void get_Website(char *url);
size_t CURLWrite_Callback_String(void *contents, size_t size, size_t nmemb, std::string *s);
void itemlistload(string filename,int *itemlist);
void itemdbload(string filename, item *itemdb);

int main(void) {
	////////////////////////
	// Variables
	int requestitem = 0;
	int user_choice[20], user_choice_count = 0;
	int favj = 0;
	bool favselect = false;
	item items[ITEM_COUNT];
	item favitems[5000];
	item user_favitems[50];
	int *itemslist;
	ofstream itemsout;
	ofstream itemtpout;
	itemslist = new int[ITEM_COUNT];
	string user_in = "0";
	//string EXAMPLE = "\{ /r/n\"id\": 19684,/r/n\"whitelisted\" : false,/r/n\"buys\" : {/r/n	\"quantity\": 145975,/r/n		\"unit_price\" : 7018},/r/n\"sells\" : {/r/n		\"quantity\": 126,/r/n			\"unit_price\" : 7019/r/n	}}";
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Below is the main code for checking the IP in intervals.
	itemlistload("items.txt", itemslist);

	cout << "Rebuild DB? (Y/N) " << endl;
	cin >> user_in;
	if (user_in == "n" || user_in == "N")
	{
		itemdbload("itemsdb.txt", items);
	}
	else
	{
		//for (int i = 0; i < 300; i++)cout << itemslist[i];
		cout << "\n\n\n";
		// CONTROL DATABASE - ONLY REBUILD DB IF REQUESTED (user control)
		requestitem = 19684;
		itemsout.open("itemsdb.txt");
		for (int i = 0; i < ITEM_COUNT; i++)
		{
			requestitem = itemslist[i];
			items[i] = ParseJSON_ITEM(requestitem);
			cout << items[i] << endl;
			itemsout << items[i] << endl;
		}
		itemsout.close();
	}

		
		std::sort(items, items + ITEM_COUNT);
		itemtpout.open("TP.txt");
		for (int i = 0; i < ITEM_COUNT; i++)
		{
			if(items[i].buyquantity() >= BUYQ_Limit && items[i].sellquantity() >= SELLQ_Limit)itemtpout << items[i] << endl;
		}
		itemtpout.close();

		cout << " [1] LIST [2] LIVE UPDATE FAV [3] SELECT FAVOURITES [4] CHANGE SELL QUANTITY LIMITS [5] REBUILD DATABASE [esc] QUIT" << endl;
		cin >> user_in;
		do
		{
			if (user_in == "1")
			{
				cout << "How many items to parse? ";
				cin >> user_in;
				int parsesize = stoi(user_in);
			
				for (int i = 0; i < ITEM_COUNT; i++)
				{
					if (items[i].buyquantity() >= BUYQ_Limit && items[i].sellquantity() >= SELLQ_Limit)
					{
						favitems[favj] = items[i];
						favj += 1;
					}
				}
				for (int i =0; i < parsesize && i < favj;i++)cout << "[ " << i <<" ]"<< ParseJSON_ITEMName(favitems[i].getid()) << " Profit : " << favitems[i].profit << endl << " Buy Quantity : " << favitems[i].buyquantity() << " Sell Quantity : " << favitems[i].sellquantity() << endl << " Buy Price : " << favitems[i].buyprice() << " Sell Price : " << favitems[i].sellprice() << endl;
			}
			if (user_in == "3")
			{
				cout << " ENTER THE ITEM NUMBER(S) SHOWN IN [] FINISH WITH esc" << endl;


				while (user_in != "esc") {
					cin >> user_in;
					if (user_in != "esc") {
						user_choice[user_choice_count] = stoi(user_in);
						user_choice_count += 1;
					}
				}
				cout << " You have selected : ";
				for (int i = 0; i < user_choice_count; i++)cout << user_choice[i] << " ";
				cout << endl;
				favselect = true;
			}
			if (user_in == "2")
			{
				if (favselect) {
					while (user_in != "esc")
					{
						for (int i = 0; i < user_choice_count; i++) {
							user_favitems[i] = ParseJSON_ITEM(favitems[user_choice[i]].getid());
						}
						std::sort(user_favitems, user_favitems + user_choice_count);
						for(int i=0 ; i<user_choice_count ; i++)cout << "[ " << user_choice[i] << " ]" << ParseJSON_ITEMName(user_favitems[i].getid()) << " Profit : " << user_favitems[i].profit << endl << " Buy Quantity : " << user_favitems[i].buyquantity() << " Sell Quantity : " << user_favitems[i].sellquantity() << endl << " Buy Price : " << user_favitems[i].buyprice() << " Sell Price : " << user_favitems[i].sellprice() << endl;


						cout << "Press any key then enter to continue or type esc to stop" << endl;
						cin >> user_in;
					}
				}
				else cout << " Select favourites first!" << endl;
			}
			if (user_in == "4")
			{
				cout << "Current MIN Limits : BUYING QUANTITY " << BUYQ_Limit << " SELLING QUANTITY " << SELLQ_Limit << endl;
				cout << "Input new limits and separate them with space" << endl;
				cin >> BUYQ_Limit >> SELLQ_Limit;
			}
			cout << " [1] LIST [2] LIVE UPDATE FAV [3] SELECT FAVOURITES [4] CHANGE SELL QUANTITY LIMITS [5] REBUILD DATABASE [esc] QUIT" << endl;
			cin >> user_in;
		} while (user_in != "esc");
		// COMPARISON OF SELL QUANTITIES
		// GET ITEM NAME FOR SHOWING
		


		


	return 0;
}

void itemlistload(string filename, int *itemlist)
{
	fstream itemliststream(filename,std::ios_base::in);
	string itemliststring;
	if (!itemliststream.is_open())cout << "item file not open bob";
	for (int i = 0; i < 24465; i++)
	{
		itemliststream >> itemliststring;
		itemlist[i] = stoi(itemliststring);
	}
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
}
//****************************************************
string ParseJSON(string *inputfile, string tobefound, string tobefoundwithin)
{
	int size, pos;
	stringstream parsedstringstream;
	string parsedstring;
	
	if(inputfile->find("no such id")!=string::npos)return "nan";
	size = getstringinputsizeJSON(inputfile, tobefound,tobefoundwithin);
	pos = getstringinputpositionJSON(inputfile, tobefound, tobefoundwithin);
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
int getstringinputsizeJSON(string *inputfile, string tobefound,string tobefoundwithin)
{
	size_t strpos=0;
	int size = 0;
	size_t eolpos;
	size_t datastartpos;
	
	if(tobefoundwithin!="?")strpos = inputfile->find(tobefoundwithin);
	strpos = inputfile->find(tobefound,strpos);
	datastartpos = inputfile->find(":", strpos);
	eolpos = inputfile->find(",", datastartpos);
	bool eol = false;
	for (int i = datastartpos + 2; i < inputfile->size() && !eol; i++)
	{

		//cout << i;
		if (i == eolpos)eol = true;
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
int getstringinputpositionJSON(string *inputfile, string tobefound, string tobefoundwithin)
{
	size_t strpos = 0;
	int size = 0;
	size_t eolpos,datastartpos;
	if (tobefoundwithin != "?")strpos = inputfile->find(tobefoundwithin);
	strpos = inputfile->find(tobefound, strpos);
	datastartpos = inputfile->find(":", strpos);
	eolpos = inputfile->find(",", datastartpos);

	return datastartpos+2;
}

// Winsock function that sends http request to the given string url and reads to response to char buffer that is defined globally.
// RETURN STRING?
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
		chunk = curl_slist_append(chunk, "Authorization: Bearer F0415A36-8665-6049-AE2A-F97254BABE69E79410B8-61C4-49E3-B09D-693344F57B30");
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