#ifndef _BAZ_GW2TP_DATABASE_HPP
#define _BAZ_GW2TP_DATABASE_HPP
#include <string>
#include <functional>
#include "Item.hpp"
#include <chrono>
#include <fstream>

class DatabaseIntegrityChecker
{
public:
    static std::uint64_t CheckLines(std::string filename);
    static bool Exists(std::string filename);
};

template<typename T> 
class Database
{
public:
    explicit Database(std::string filename)
    	: filename(filename)
    {}
    virtual ~Database() = default;

    std::string_view Filename()
    {
	    return filename;
    }

    std::ofstream DbOutStream()
    {
	    return std::ofstream(filename);
    }
    
    std::ifstream DbInStream()
    {
	    return std::ifstream(filename);
    }

    void Erase()
    {
        std::remove(filename.c_str());
    }

    bool Exists()
    {
        return DatabaseIntegrityChecker::Exists(filename);
    }

    std::vector<T>& Get()
    {
	    return storage;
    }

    std::chrono::system_clock::time_point UpdateTime()
    {
	    return dbTimePoint;
    }
    
protected:
    std::string filename;
    std::vector<T> storage;
    std::chrono::system_clock::time_point dbTimePoint;
};

class IdDatabase
    : public Database<std::uint64_t>
{
public:
    explicit IdDatabase(std::string filename);

    bool Load();
    void WriteRaw(std::string contents);
    
private:
    bool itemlistload();

};
class NameDatabase
    : public Database<ItemIdentifier>
{
public:
    explicit NameDatabase(std::string filename);

    bool Write(const std::vector<ItemIdentifier>&itemList);
    std::time_t Load();
    ItemNameExtended ExtendItemFromDB(const Item& item);

private:
    std::time_t LoadItemListDB();
};

class PriceDatabase
    : public Database<Item>
{
public:
    struct Sorter
    {
        static std::vector<Item> SortProfitableItems(PriceDatabase& db, std::uint64_t BUYQ_Limit, std::uint64_t SELLQ_Limit, std::uint64_t BUYG_MAX, std::uint64_t BUYG_MIN);
    };
    explicit PriceDatabase(std::string filename);

    bool Load();
    void WriteBack();

private:
    std::time_t itemdbload();
};

class FavouritesDatabase
    : public Database<std::uint64_t>
{
public:
    explicit FavouritesDatabase(std::string filename);

    bool Write(const std::vector<std::uint64_t>& items);
    bool Load();
};

#endif