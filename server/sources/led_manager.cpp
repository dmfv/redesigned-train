#include "led_manager.h"

LedManager::LedManager(LedState initState, 
                       LedColor initColor,
                       int initLedRate) 
: state(initState), 
  color(initColor), 
  refrashRateHz(1) 
{
}
    
const std::string LedManager::getLedStateString() const { 
    std::string str;
    switch (state) {
        case LedState::on:
            str = "on";
            break;
        case LedState::off:
            str = "off";
            break;
    }
    return str;
}

// TODO: change enum class to sophisticated data structure (maybe map or unordered_map<enum, string>?)
const std::string LedManager::getLedColorString() const { 
    std::string str;
    switch (color) {
        case LedColor::red:
            str = "red";
            break;
        case LedColor::green:
            str = "green";
            break;
        case LedColor::blue:
            str = "blue";
            break;
    }
    return str;
}

LedManager::LedState LedManager::string2LedState(const std::string& str) const {
    if (str == "on") {
        return LedState::on;
    } else if (str == "off") {
        return LedState::off;
    }
    return LedState::unknownState;
}

LedManager::LedColor LedManager::string2LedColor(const std::string& str) const {
    if (str == "red") {
        return LedColor::red;
    } else if (str == "green") {
        return LedColor::green;
    } else if (str == "blue") {
        return LedColor::blue;
    }
    return LedColor::unknownColor;
}

void LedManager::setLedState(LedState newState) {
    if (newState != state) {
        state = newState;
    }
}

void LedManager::setLedColor(LedColor newColor) {
    if (newColor != color) {
        color = newColor;
    }
}

void LedManager::setLedRate(int newRateHz) {
    if (newRateHz != refrashRateHz) {
        refrashRateHz = newRateHz;
    }
}
