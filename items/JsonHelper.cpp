#include "JsonHelper.hpp"
#include <sstream>
//****************************************************
std::string ParseJSON(std::string *inputfile, std::string tobefound, std::string tobefoundwithin, std::string tobefoundwithinfirst)
{
	size_t size, pos;
	std::stringstream parsedstringstream;
	std::string parsedstring;

	if (inputfile->find("no such id")!= std::string::npos || inputfile->find("<html") != std::string::npos)
		return "nan";
	size = getstringinputsizeJSON(inputfile, tobefound, tobefoundwithin, tobefoundwithinfirst);
	pos = getstringinputpositionJSON(inputfile, tobefound, tobefoundwithin, tobefoundwithinfirst);
	for (int i = 0; i < size; i++)
		parsedstringstream << inputfile->at(i + pos);
	parsedstring = parsedstringstream.str();
	return parsedstring;
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
size_t getstringinputsizeJSON(std::string *inputfile, std::string tobefound, std::string tobefoundwithin, std::string tobefoundwithinfirst)
{
	size_t strpos = 0;
	size_t size = 0;
	size_t eolpos, eolpos2;
	size_t datastartpos;

	if (tobefoundwithin != "?" && tobefoundwithinfirst == "?")
		strpos = inputfile->find(tobefoundwithin);
	if (tobefoundwithinfirst != "?")
		strpos = inputfile->find(tobefoundwithinfirst);
	if (tobefoundwithin != "?" && tobefoundwithinfirst != "?")
		strpos = inputfile->find(tobefoundwithin, strpos);
	strpos = inputfile->find(tobefound, strpos);
	datastartpos = inputfile->find(":", strpos);
	eolpos = inputfile->find(",", datastartpos);
	eolpos2 = inputfile->find("\n", datastartpos);
	bool eol = false;
	for (auto i = datastartpos + 2; i < inputfile->size() && !eol; i++)
	{

		// cout << i;
		if (i == eolpos || i == eolpos2)
			eol = true;
		else
		{
			size += 1;
			// cout << size;
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
size_t getstringinputpositionJSON(std::string *inputfile, std::string tobefound, std::string tobefoundwithin, std::string tobefoundwithinfirst)
{
	size_t strpos = 0;
	size_t size = 0;
	size_t eolpos, datastartpos;
	if (tobefoundwithin != "?")
		strpos = inputfile->find(tobefoundwithin);
	if (tobefoundwithinfirst != "?")
		strpos = inputfile->find(tobefoundwithinfirst);
	if (tobefoundwithin != "?" && tobefoundwithinfirst != "?")
		strpos = inputfile->find(tobefoundwithin, strpos);
	strpos = inputfile->find(tobefound, strpos);
	datastartpos = inputfile->find(":", strpos);
	eolpos = inputfile->find(",", datastartpos);

	return datastartpos + 2;
}

bool IsMarkedForRemoval(char c)
{
    switch(c)
    {
    case '[':
    case ' ':
    case ']':
    case ',':
        return true;
    default:
        return false;
    }
}

std::string FromToken(const std::string& key, const std::string& value, std::size_t offset)
{
	if (const auto& loc = value.find(key); loc != std::string::npos)
		return value.substr(value.find(key) + offset);
	else 
		return {};
}

std::string ToToken(const std::string& key, const std::string& value, std::size_t offset)
{
	if (const auto& loc = value.find(key); loc != std::string::npos)
		return value.substr(0, value.find(key) + offset);
	else 
		return {};
}

std::string StringValue(const std::string& value) 
{ 
	return ToToken(",", FromToken(":", value, 2), -1);
}

std::string Value(const std::string& value) 
{
	return ToToken(",", FromToken(":", value, 1));
}