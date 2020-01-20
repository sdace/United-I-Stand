#pragma once

#include "ECS/Components/upgrade_component.hpp"

class UpgradeUnlockedEvent
{
public:
    UpgradeUnlockedEvent(UpgradeType upgrade_type) : upgrade_type(upgrade_type) {}
    UpgradeType upgrade_type;
};
