#pragma once

#include "airwin_consolidated_base.h"
#include <functional>
#include <string>

class AirwinRegistryEntry
{
public:

    using AirwinEffectConstructor = std::function<AudioEffectX* (float samplingRate)>;

    AirwinRegistryEntry(
        const std::string& name,
        const std::string& category,
        int unusedOrdering,
        bool isMono,
        const std::string& whatText,
        int numberParams,
        const std::string& unusedDate,
        AirwinEffectConstructor effectConstructor)
    {
        _effectConstructor = effectConstructor;
        _name = new std::string(name);
        _category = new std::string(category);
        _whatText = new std::string(whatText);
        _isMono = isMono;
        _numberOfParams = numberParams;
    }
    AirwinRegistryEntry(const AirwinRegistryEntry& copy)
    {
        _effectConstructor = copy.GetEffectConstructor();
        _name = new std::string(copy.GetName());
        _category = new std::string(copy.GetCategory());
        _whatText = new std::string(copy.GetWhatText());
        _isMono = copy.IsMono();
        _numberOfParams = copy.GetNumberOfParams();
    }
    ~AirwinRegistryEntry()
    {
        delete _name;
        delete _category;
        delete _whatText;

        //NOTE*** The effects pointer must be deleted! This is a simple initialization problem; but
        //        that's for the 1500 effects that this guy has! So, we'll make a loop to delete them
        //        in the registry destructor.
    }

    std::string GetName() const { return *_name; }
    std::string GetCategory() const { return *_category; }
    std::string GetWhatText() const { return *_whatText; }
    bool IsMono() const { return _isMono; }
    int GetNumberOfParams() const { return _numberOfParams; }
    AudioEffectX* CreateEffect(float samplingRate) const { return _effectConstructor(samplingRate); }

protected:

    AirwinEffectConstructor GetEffectConstructor() const { return _effectConstructor; }

private:

    std::string* _name;
    std::string* _category;
    std::string* _whatText;
    bool _isMono;    
    int _numberOfParams;
    AirwinEffectConstructor _effectConstructor;
};