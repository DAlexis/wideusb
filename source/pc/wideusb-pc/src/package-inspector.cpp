#include "wideusb-pc/package-inspector.hpp"
#include "wideusb/communication/binary/channel.hpp"
#include "wideusb/communication/binary/network.hpp"
#include "wideusb/communication/binary/transport.hpp"

#include <sstream>
#include <iostream>
#include <iomanip>

using namespace std;

PackageInspector::PackageInspector(Verbosity verbosity,
                 std::shared_ptr<IChannelLayer> channel,
                 std::shared_ptr<INetworkLayer> network,
                 std::shared_ptr<ITransportLayer> transport) :
    m_channel(channel ? channel : std::make_shared<ChannelLayerBinary>()),
    m_network(network ? network : std::make_shared<NetworkLayerBinary>()),
    m_transport(transport ? transport : std::make_shared<TransportLayerBinary>()),
    m_created(std::chrono::steady_clock::now()),
    m_verbosity(verbosity)
{

}

void PackageInspector::inspect_package(const PBuffer data, const std::string& context_msg)
{
    if (data->size() == 0)
        return;
    ostringstream oss;

    PBuffer copy = data->clone();
    BufferAccessor accessor(copy);

    std::vector<DecodedFrame> frames = m_channel->decode(accessor);
    if (m_verbosity == Verbosity::laconic && frames.empty())
        return;
    oss << "┌ Package inspection:" << endl;
    oss << "│ frames count: " << frames.size() << endl;
    for (const auto& frame : frames)
    {
        std::vector<DecodedPacket> packets = m_network->decode(frame.frame);
        oss << "│  ╓ packets in frame: " << packets.size() << endl;
        for (size_t i = 0; i != packets.size(); i++)
        {
            if (m_verbosity == Verbosity::full)
            {
                oss << "│  ║  ┌ packet " << i << ":" << endl;
                oss << "│  ║  │ options.receiver " << packets[i].options.receiver << endl;
                oss << "│  ║  │ options.sender   " << packets[i].options.sender << endl;
                oss << "│  ║  │ options.ttl      " << int(packets[i].options.ttl) << endl;
            } else {
                oss << "│  ║  ┌ pack from " << packets[i].options.sender << " to " << packets[i].options.receiver << " (ttl=" << int(packets[i].options.ttl) << ")" << endl;
            }

                std::vector<DecodedSegment> segments = m_transport->decode(packets[i].packet);
                oss << "│  ║  │ segments count:  " << segments.size() << endl;

            for (size_t j = 0; j != segments.size(); j++)
            {
                if (m_verbosity == Verbosity::full)
                {
                    oss << "│  ║  │ segment " << j << ":" << endl;
                    oss << "│  ║  │ │ segment_id         " << segments[j].segment_id << endl;
                    oss << "│  ║  │ │ ack_for_segment_id " << segments[j].ack_for_segment_id << endl;
                    oss << "│  ║  │ │ port               " << segments[j].port << endl;
                    oss << "│  ║  │ │ need_ack           " << ((segments[j].flags & DecodedSegment::Flags::need_ack) != 0) << endl;
                    oss << "│  ║  │ │ is_ack             " << (((segments[j].flags & DecodedSegment::Flags::is_ack) != 0) ? "YES" : "no") << endl;
                    oss << "│  ║  │ │ data size          " << segments[j].segment.size() << endl;
                    oss << "│  ║  │ └ contents           ";
                    size_t len = std::min(m_max_buffer_len_to_print, segments[j].segment.size());
                    for (size_t k = 0; k < len; k++)
                    {
                        oss << std::hex << std::setfill('0') << std::setw(2) << int(segments[j].segment[k]) << " ";
                    }
                    if (segments[j].segment.size() > m_max_buffer_len_to_print)
                    {
                        oss << "...";
                    }
                    oss << std::dec << std::endl;
                } else {
                    oss << "│  ║  │ seg id " << segments[j].segment_id << ", port " << segments[j].port
                        << (((segments[j].flags & DecodedSegment::Flags::need_ack) != 0) ? " need ack" : "") << " size " << segments[j].segment.size();
                    if ((segments[j].flags & DecodedSegment::Flags::is_ack) != 0)
                    {
                        oss << ", is ack to " << segments[j].ack_for_segment_id;
                    }
                    oss << std::endl;
                }
                oss << "│  ║  └───────────────────────" << endl;
            }
            oss << "│  ╚═════════════════" << packets[i].options.ttl << endl;
        }
    }
    std::cout << "[" << std::chrono::duration_cast<std::chrono::milliseconds> (std::chrono::steady_clock::now() - m_created).count() << "ms] "
              <<  context_msg << ", len = " << data->size() << std::endl;
    std::cout << oss.str() << std::endl;

}
