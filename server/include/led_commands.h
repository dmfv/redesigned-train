#pragma once

#include <string>
#include <memory>
#include <unordered_set>

#include "led_manager.h"

// abstract class for basic command
class BaseCommand {
public:
    BaseCommand(std::shared_ptr<LedManager>& ledManager, const std::string& cmdName = "baseCommand") : ledManager(ledManager), cmdName(cmdName) { }
    virtual ~BaseCommand() { }
    // must return true if the command executed successfully, otherwise false
    virtual bool run(const std::string& input, std::string& output) = 0;
    const std::string& getName() const { return cmdName; }
protected:
    std::shared_ptr<LedManager> ledManager;
    const std::string cmdName;
};

class GetLedStateCommand : public BaseCommand {
public:
    GetLedStateCommand(std::shared_ptr<LedManager>& ledManager) : BaseCommand(ledManager, "get-led-state\n") {  }
    virtual ~GetLedStateCommand() { }
    virtual bool run(const std::string& input, std::string& output) override {
        // verify input
        if (input != cmdName)
            return false;
        // get led state and set output
        output = "OK " + ledManager->getLedStateString();
        return true;
    }
};

class GetLedColorCommand : public BaseCommand {
public:
    GetLedColorCommand(std::shared_ptr<LedManager>& ledManager) : BaseCommand(ledManager, "get-led-color\n") { }
    virtual ~GetLedColorCommand() { }
    virtual bool run(const std::string& input, std::string& output) override {
        bool commandWasExecuted = false;
        // verify input
        if (input != cmdName)
            return false;
        // get led state and set output
        output = "OK " + ledManager->getLedColorString();
        return true;
    }
};

class GetLedRateCommand : public BaseCommand {
public:
    GetLedRateCommand(std::shared_ptr<LedManager>& ledManager) : BaseCommand(ledManager, "get-led-rate\n") { }
    virtual ~GetLedRateCommand() { }
    virtual bool run(const std::string& input, std::string& output) override {
        bool commandWasExecuted = false;
        // verify input
        if (input != cmdName)
            return false;
        output = "OK " + std::to_string(ledManager->getLedRate());
        return true;
    }
};

class SetLedStateCommand : public BaseCommand {
public:
    SetLedStateCommand(std::shared_ptr<LedManager>& ledManager) : BaseCommand(ledManager, "set-led-state") { }
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
};

class SetLedColorCommand : public BaseCommand {
public:
    SetLedColorCommand(std::shared_ptr<LedManager>& ledManager) : BaseCommand(ledManager, "set-led-color") { }
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
};

class SetLedRateCommand : public BaseCommand {
public:
    SetLedRateCommand(std::shared_ptr<LedManager>& ledManager) : BaseCommand(ledManager, "set-led-rate") { }
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
};


// custom implementation of 'chain of responsibility' pattern to provide easier way adding and modifying commands
// usage example:
// create command container with necessary commands
// run execute when necessary
class CommandContainer {
public:
    CommandContainer(std::unordered_set<std::unique_ptr<BaseCommand>>&& commandsList = {}) : commandsList(std::move(commandsList)) { }   

    virtual ~CommandContainer() { }
    // return true if at least one command finished successfully (processed data correctly)
    bool execute(const std::string& argument, std::string& output) {
        bool cmdWasExecuted = false;
        for (auto cmd = commandsList.begin(); cmd != commandsList.end(); ++cmd) {
            cmdWasExecuted = (*cmd)->run(argument, output);
            // std::cout << "Command executed: " << (*cmd)->getName() << std::endl; // debug
            if (cmdWasExecuted) break;
        }
        if (!cmdWasExecuted) 
            output = "FAILED: unknown command \"" + argument + "\"";
        // std::cout << "Command was executed status: " << cmdWasExecuted << std::endl; // debug
        return cmdWasExecuted;
    }

private:
    std::unordered_set<std::unique_ptr<BaseCommand>> commandsList;
};