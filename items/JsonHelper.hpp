#include <string>
//****************************************************
std::string ParseJSON(std::string *inputfile, std::string tobefound, std::string tobefoundwithin = "?", std::string tobefoundwithinfirst = "?");

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
int getstringinputsizeJSON(std::string *inputfile, std::string tobefound, std::string tobefoundwithin = "?", std::string tobefoundwithinfirst = "?");
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
int getstringinputpositionJSON(std::string *inputfile, std::string tobefound, std::string tobefoundwithin = "?", std::string tobefoundwithinfirst = "?");

bool IsMarkedForRemoval(char c);

std::string FromToken(const std::string& key, const std::string& value, std::size_t offset = 0);
std::string ToToken(const std::string& key, const std::string& value, std::size_t offset = 0);
std::string StringValue(const std::string& value);
std::string Value(const std::string& value);