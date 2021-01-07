#ifndef NETWORKING_HPP
#define NETWORKING_HPP

#include "communication/networking-interfaces.hpp"
#include "buffer.hpp"

#include <vector>
#include <optional>

class NetworkFilter
{
public:
    /**
     * @brief Enable listening gived address. Accept if (incoming target & mask) == (addr & mask)
     * @param addr
     * @param mask
     */
    void listen_address(Address addr, Address mask);
    bool is_acceptable(const Address& addr);

private:
    struct Target
    {
        Address addr;
        Address mask;
    };
    std::vector<Target> m_targets;
};

class NetworkManager
{
public:

};

#endif // NETWORKING_HPP
