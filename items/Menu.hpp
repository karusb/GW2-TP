#ifndef _BAZ_GW2TP_MENU_HPP
#define _BAZ_GW2TP_MENU_HPP

#include <string>
#include <functional>
#include <map>
#include <optional>

class Menu
{
public:
    Menu() = default;
    Menu(const std::string& menuName)
        : printPrefix(menuName)
    {};
    virtual ~Menu() = default;

    Menu& Add(const std::string& input, const std::string& description, const std::function<void(std::string_view input)>& action);
    Menu& AddContinous(const std::string& input, const std::string& description, const std::function<void(std::string_view input)>& action, const std::string& exit ="q");
    Menu& Default(const std::function<void(std::string_view input)>& action);
    Menu& DefaultContinuous(const std::function<void(std::string_view input)>& action, const std::string& exit ="q");
    Menu& ExitOn(const std::string& input, const std::string& description);
    Menu& Run();
    Menu& PrintOptions();
    Menu& DisablePrintOptions() {printOptions = false; return *this;};
private:
    struct MenuOption
    {
        MenuOption(const std::string& input, const std::string& description, const std::function<void(std::string_view input)>& action, bool continuous = false, const std::string& exit = "q")
            : input(input)
            , description(description)
            , action(action)
            , continuous(continuous)
            , exit(exit)
        {}

        const std::string& input;
        const std::string& description;
        const std::function<void(std::string_view input)>& action;
        bool continuous = false;
        const std::string& exit = "q";
    };
    void ProcessOptions(std::string& input);
    void RunOptionContinously(std::string& input, const MenuOption& option);
    void DoContinouslyUntil(const std::string& exitInput, const std::function<void(std::string& input)>& action = nullptr);

    std::map<std::string, MenuOption> map;
    std::optional<MenuOption> defaultOption;
    std::optional<MenuOption> exitOption;
    bool printOptions = true;
    std::string printPrefix = "";
};

#endif