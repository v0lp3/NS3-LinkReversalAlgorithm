#ifndef LINK_REVERSAL_ROUTING_H
#define LINK_REVERSAL_ROUTING_H

#include "ns3/ipv4-routing-protocol.h"
#include "ns3/ipv4.h"
#include "ns3/node-container.h"
#include "ns3/packet.h"

using namespace ns3;

class LinkReversalRouting : public Ipv4RoutingProtocol
{
  public:
    static TypeId GetTypeId(void);
    LinkReversalRouting();
    ~LinkReversalRouting();

    Ptr<Node> m_node;
    Ptr<Ipv4> m_ipv4;

    virtual Ptr<Ipv4Route> RouteOutput(Ptr<Packet> packet,
                                       const Ipv4Header& header,
                                       Ptr<NetDevice> oif,
                                       Socket::SocketErrno& sockerr) override;

    virtual bool RouteInput(Ptr<const Packet> packet,
                            const Ipv4Header& header,
                            Ptr<const NetDevice> idev,
                            const UnicastForwardCallback& ucb,
                            const MulticastForwardCallback& mcb,
                            const LocalDeliverCallback& lcb,
                            const ErrorCallback& ecb) override;

    virtual void SetIpv4(Ptr<Ipv4> ipv4) override;
    virtual void PrintRoutingTable(Ptr<OutputStreamWrapper> stream, Time::Unit unit) const override;

    void NotifyInterfaceUp(uint32_t interface) override;
    void NotifyInterfaceDown(uint32_t interface) override;
    void NotifyAddAddress(uint32_t interface, Ipv4InterfaceAddress address) override;
    void NotifyRemoveAddress(uint32_t interface, Ipv4InterfaceAddress address) override;
    void SetNode(Ptr<Node> node);
};

#endif