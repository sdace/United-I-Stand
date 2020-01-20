#pragma once

#include <bitset>

using namespace std;

struct Entity
{
    static Entity NULL_ENTITY;
    size_t Id;
    size_t Version;

    friend bool operator==(const Entity& lhs, const Entity& rhs)
    {
        return ((lhs.Id == rhs.Id) && (lhs.Version == rhs.Version));
    }
};

namespace std
{
    template<> struct hash<Entity>
    {
        std::size_t operator()(Entity const& entity) const noexcept
        {
            return entity.Id;
        }
    };
}

const size_t max_components = 32;
typedef bitset<max_components> ComponentSet;