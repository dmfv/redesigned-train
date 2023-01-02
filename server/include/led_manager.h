#pragma once
#include <string>

// class the main responsibility is manipulate led 
class LedManager {
public:
    enum class LedState {
        on,
        off,
        unknownState, 
    };
    enum class LedColor {
        red,
        green,
        blue,
        unknownColor, 
    };

    LedManager(LedState initState = LedState::off, 
               LedColor initColor = LedColor::red,
               int initLedRate = 1);

    const LedState& getLedState() const { return state; }
    const LedColor& getLedColor() const { return color; }
    const int getLedRate() const {return refrashRateHz; }
    
    const std::string getLedStateString() const;
    
    // TODO: change enum class to sophisticated data structure (maybe map or unordered_map<enum, string>?)
    const std::string getLedColorString() const;
    LedState string2LedState(const std::string& str) const;
    LedColor string2LedColor(const std::string& str) const;
    void setLedState(LedState newState);
    void setLedColor(LedColor newColor);
    void setLedRate(int newRateHz);

private:
    // led switcher timer with lambda
    LedState state;
    LedColor color;
    int refrashRateHz;
};
