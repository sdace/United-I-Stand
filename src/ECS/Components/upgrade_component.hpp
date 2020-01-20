#pragma once

enum UpgradeType
{
    SPARTAN_UPGRADE,
    WHEEL_UPGRADE
};

struct UpgradeComponent
{
    UpgradeComponent(UpgradeType upgrade_type) : upgrade_type(upgrade_type) {}
    UpgradeType upgrade_type;
};