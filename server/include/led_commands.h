#pragma once

#include <list>
#include <string>
#include <memory>
// #include <functional> // for new modification (get rid of LedManager ptr in Command classes and change to)

#include "led_manager.h"

// abstract class for basic command
class BaseCommand {
public:
    BaseCommand(LedManager* ledManager) : ledManager(ledManager) { }
    virtual ~BaseCommand() {}
    // must return true if the command executed successfully, otherwise false
    virtual bool run(const std::string& input, std::string& output) = 0;
protected:
    LedManager* ledManager;
};

class GetLedStateCommand : public BaseCommand {
public:
    GetLedStateCommand(LedManager* ledManager) : BaseCommand(ledManager) { }
    virtual ~GetLedStateCommand() { }
    virtual bool run(const std::string& input, std::string& output) override {
        // verify input
        // if (input.substr(0, 14) != "get-led-state\n")
        if (input != "get-led-state\n")
            return false;
        // get led state and set output
        output = "OK " + ledManager->getLedStateString();
        return true;
    }
};

class GetLedColorCommand : public BaseCommand {
public:
    GetLedColorCommand(LedManager* ledManager) : BaseCommand(ledManager) { }
    virtual ~GetLedColorCommand() { }
    virtual bool run(const std::string& input, std::string& output) override {
        bool commandWasExecuted = false;
        // verify input
        if (input != "get-led-color\n")
            return false;
        // get led state and set output
        output = "OK " + ledManager->getLedColorString();
        return true;
    }
};

class GetLedRateCommand : public BaseCommand {
public:
    GetLedRateCommand(LedManager* ledManager) : BaseCommand(ledManager) { }
    virtual ~GetLedRateCommand() { }
    virtual bool run(const std::string& input, std::string& output) override {
        bool commandWasExecuted = false;
        // verify input
        if (input != "get-led-rate\n")
            return false;
        output = "OK " + std::to_string(ledManager->getLedRate());
        return true;
    }
};

class SetLedStateCommand : public BaseCommand {
public:
    SetLedStateCommand(LedManager* ledManager) : BaseCommand(ledManager) { }
    virtual ~SetLedStateCommand() { }
    virtual bool run(const std::string& input, std::string& output) override {
        bool commandWasExecuted = false;
        // verify input
        if (input.substr(0, cmdName.size()) != cmdName)
            return false;
        LedManager::LedState state = ledManager->string2LedState(input.substr(cmdName.size(), input.size() - cmdName.size() - 1));
        if (state == LedManager::LedState::unknownState) {
            output = "FAILED: wrong argument (state name)";
        } else {
            ledManager->setLedState(state);
            output = "OK";
        }
        return true;
    }
private:
    inline static const std::string cmdName = "set-led-state"; // remove inline if compiled with c++14 or lower
};

class SetLedColorCommand : public BaseCommand {
public:
    SetLedColorCommand(LedManager* ledManager) : BaseCommand(ledManager) { }
    virtual ~SetLedColorCommand() { }
    virtual bool run(const std::string& input, std::string& output) override {
        bool commandWasExecuted = false;
        // verify input
        if (input.substr(0, cmdName.size()) != cmdName)
            return false;
        LedManager::LedColor color = ledManager->string2LedColor(input.substr(cmdName.size(), input.size() - cmdName.size() - 1)); // -1 to remove last \n symbols
        if (color == LedManager::LedColor::unknownColor) {
            output = "FAILED: wrong argument (color name)";
        } else {
            ledManager->setLedColor(color);
            output = "OK";
        }
        return true;
    }
private:
    inline static const std::string cmdName = "set-led-color"; // remove inline if compiled with c++14 or lower
};

class SetLedRateCommand : public BaseCommand {
public:
    SetLedRateCommand(LedManager* ledManager) : BaseCommand(ledManager) { }
    virtual ~SetLedRateCommand() { }
    virtual bool run(const std::string& input, std::string& output) override {
        bool commandWasExecuted = false;
        // verify input
        if (input.substr(0, cmdName.size()) != cmdName)
            return false;
        int refreshRate = atoi(input.substr(cmdName.size(), input.size() - cmdName.size() - 1).c_str()); 
        if (refreshRate > 5 || refreshRate < 0) {
            output = "FAILED: wrong argument (led rate should be between 0 and 5)";
        } else {
            ledManager->setLedRate(refreshRate);
            output = "OK";
        }
        return true;
    }
private:
    inline static const std::string cmdName = "set-led-rate";
};


// custom implementation of 'chain of responsibility' pattern to provide easier way adding and modifying commands
// usage example:
// create command container with necessary commands
// run execute when necessary
class CommandContainer {
public:
    CommandContainer(const std::list<BaseCommand*>& commandsList = {}) : commandsList(commandsList) { }   

    virtual ~CommandContainer() {
        commandsList.remove_if( [] (BaseCommand* elem) { delete elem; return true; } ); // use this instead of clear() won't clean memory
    }

    bool execute(const std::string& argument, std::string& output) {
        bool cmdWasExecuted = false;
        for (auto cmd = commandsList.begin(); cmd != commandsList.end(); ++cmd) {
            cmdWasExecuted = (*cmd)->run(argument, output);
            if (cmdWasExecuted) break;
        }
        return cmdWasExecuted;
    }

private:
    std::list<BaseCommand*> commandsList;
};