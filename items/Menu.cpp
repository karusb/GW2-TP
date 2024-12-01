#include "Menu.hpp"
#include <iostream>
#include <utility>
#include "termcolor/termcolor.hpp"

Menu& Menu::Add(const std::string& input, const std::string& description, const std::function<void(std::string_view input)>& action)
{
    map.insert({{input}, {input, description, action, false, ""}});
    return *this;
}

Menu& Menu::AddContinous(const std::string& input, const std::string& description, const std::function<void(std::string_view input)>& action, const std::string& exit)
{
    map.insert({{input}, {input, description, action, true, exit}});
    return *this;
}

Menu& Menu::Default(const std::function<void(std::string_view input)>& action)
{
    MenuOption option {"", "", action, false, ""};
    defaultOption.emplace(option);
    return *this;
}

Menu& Menu::DefaultContinuous(const std::function<void(std::string_view input)>& action, const std::string& exit)
{
    MenuOption option {"", "", action, true, exit};
    defaultOption.emplace(option);
    return *this;
}

Menu& Menu::ExitOn(const std::string& input, const std::string& description)
{
    MenuOption option {input, description, nullptr, false, ""};
    exitOption.emplace(option);
    return *this;
}

Menu& Menu::Run()
{
    if (exitOption)
        DoContinouslyUntil(exitOption->input, [this](auto input){ ProcessOptions(input); });
    else
    {
        std::string input;
        std::cin >> input;
        ProcessOptions(input);
    }

    return *this;
}

Menu& Menu::PrintOptions()
{
    if (!printPrefix.empty())
        std::cout << termcolor::bright_blue << printPrefix << ": ";
    for (auto pair : map)
        std::cout << termcolor::bright_blue << "[" << pair.first << "] " << pair.second.description << " ";
    if (exitOption)
        std::cout << termcolor::red << "[" << exitOption->input << "] " << exitOption->description << " ";
    std::cout << termcolor::reset << std::endl;
    return *this;
}

void Menu::RunOptionContinously(std::string& input, const MenuOption& option)
{
    do
    {
        option.action(input);
        input.clear();
        std::cin >> input;
    }
    while(!std::equal(option.exit.begin(), option.exit.end(), input.begin(), input.end()));
}

void Menu::ProcessOptions(std::string& input)
{
    while (map.find(input) != map.end())
    {
        MenuOption& option = map.at(input);
        if (option.continuous)
            RunOptionContinously(input, option);
        else
            option.action(input);
        input.clear();
    }

    if (defaultOption)
    {
        if (defaultOption->continuous)
            RunOptionContinously(input, *defaultOption);
        else
            defaultOption->action(input);
    }
}

void Menu::DoContinouslyUntil(const std::string& exitInput, const std::function<void(std::string& input)>& action)
{
    if (printOptions)
        PrintOptions();
    std::string input;
    std::cin >> input;
    while(!std::equal(exitInput.begin(), exitInput.end(), input.begin(), input.end()))
    {
        action(input);
        input.clear();
        if (printOptions)
            PrintOptions();
        std::cin >> input;
    }
}