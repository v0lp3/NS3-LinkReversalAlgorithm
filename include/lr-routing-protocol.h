#ifndef LINK_REVERSAL_ROUTING_H
#define LINK_REVERSAL_ROUTING_H

#include "ns3/ipv4-routing-protocol.h"
#include "ns3/ipv4.h"
#include "ns3/node-container.h"
#include "ns3/packet.h"

using namespace ns3;

/**
 * \class LinkReversalRouting
 * @brief Implements the logic for the Link Reversal Routing protocol.
 *
 * This class handles the routing of packets between nodes in the network.
 * The two virtual methods, RouteInput and RouteOutput, perform similar functions,
 * with the key difference being that RouteInput is called when a packet arrives
 * at a node, while RouteOutput is called when a packet is generated at the node.
 */
class LinkReversalRouting : public Ipv4RoutingProtocol
{
  public:
    static TypeId GetTypeId(void);
    LinkReversalRouting();
    ~LinkReversalRouting();

    Ptr<Node> m_node;
    Ptr<Ipv4> m_ipv4;

    /**
     * @brief Handles packet generation at the node.
     *
     * This method determines how packets generated at this node are routed
     * out. It selects the appropriate outgoing interface and next hop for
     * sending the packet to its destination.
     *
     * @param packet The packet to be routed.
     * @param header The IPv4 header associated with the packet.
     * @param oif The output interface (optional).
     * @param sockerr Output parameter to indicate socket error.
     * @return A Ptr to an Ipv4Route if the packet can be routed, or null otherwise.
     */
    virtual Ptr<Ipv4Route> RouteOutput(Ptr<Packet> packet,
                                       const Ipv4Header& header,
                                       Ptr<NetDevice> oif,
                                       Socket::SocketErrno& sockerr) override;
    /**
     * @brief Handles packets arriving at the node.
     *
     * This method determines how incoming packets are routed when they arrive
     * at a node. It takes care of forwarding the packet to the next hop or
     * delivering it locally.
     *
     * @param packet The incoming packet.
     * @param header The IPv4 header associated with the packet.
     * @param idev The input network device on which the packet was received.
     * @param ucb The unicast forward callback.
     * @param mcb The multicast forward callback.
     * @param lcb The local delivery callback.
     * @param ecb The error callback in case of routing failure.
     * @return True if the packet was handled, false otherwise.
     */
    virtual bool RouteInput(Ptr<const Packet> packet,
                            const Ipv4Header& header,
                            Ptr<const NetDevice> idev,
                            const UnicastForwardCallback& ucb,
                            const MulticastForwardCallback& mcb,
                            const LocalDeliverCallback& lcb,
                            const ErrorCallback& ecb) override;

    virtual void SetIpv4(Ptr<Ipv4> ipv4) override;
    void SetNode(Ptr<Node> node);

    virtual void PrintRoutingTable(Ptr<OutputStreamWrapper> stream, Time::Unit unit) const override;
    void NotifyInterfaceUp(uint32_t interface) override;
    void NotifyInterfaceDown(uint32_t interface) override;
    void NotifyAddAddress(uint32_t interface, Ipv4InterfaceAddress address) override;
    void NotifyRemoveAddress(uint32_t interface, Ipv4InterfaceAddress address) override;
};

#endif