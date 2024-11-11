#include "../include/lr-routing-protocol.h"

#include "../include/simulation-helper.h"

NS_LOG_COMPONENT_DEFINE("LinkReversalRouting");
NS_OBJECT_ENSURE_REGISTERED(LinkReversalRouting);

TypeId
LinkReversalRouting::GetTypeId(void)
{
    static TypeId tid = TypeId("ns3::LinkReversalRouting")
                            .SetParent<Ipv4RoutingProtocol>()
                            .SetGroupName("Internet")
                            .AddConstructor<LinkReversalRouting>();
    return tid;
}

LinkReversalRouting::LinkReversalRouting()
{
    NS_LOG_FUNCTION(this);
}

LinkReversalRouting::~LinkReversalRouting()
{
    NS_LOG_FUNCTION(this);
}

Ptr<Ipv4Route>
LinkReversalRouting::RouteOutput(Ptr<Packet> packet,
                                 const Ipv4Header& header,
                                 Ptr<NetDevice> idev,
                                 Socket::SocketErrno& sockerr)
{
    Ipv4Address destination = header.GetDestination();
    Ptr<LrNode> actualLrNode = m_ipv4->GetObject<LrNode>();

    // Here, as in RouteInput, it would be faster to retrieve the IPv4 directly from m_ipv4, but for
    // consistency and code uniformity, I chose to do it this way.
    Ipv4Address actualNodeIpv4 = actualLrNode->GetIpv4Address();

    NS_LOG_DEBUG("Generated packet from " << actualNodeIpv4 << " to " << destination
                                          << " id: " << packet->GetUid());

    SimulationHelper& instance = SimulationHelper::GetInstance();

    // When the benchmark is enabled, only a single packet is delivered.
    // So we can use a simple vector for store the times.
    if (instance.m_enableBenchmark == true)
    {
        instance.m_benchmark_times.first = Simulator::Now();
    }

    instance.m_total_packet++;

    // If a node have no one to forward it will try to reverse the links, but this is only tried one
    // time
    if (instance.nodes.GetOutBoundNeighbours(actualLrNode)->GetN() == 0)
        instance.nodes.ReverseLink(actualLrNode);

    Ptr<LrNode> nextHop = instance.nodes.GetNextHop(actualLrNode, header.GetSource(), destination);

    // This occurs when the node has no available nodes to forward the packet to, even after the
    // link reversal process.
    if (nextHop == nullptr)
    {
        instance.m_failure++;
        sockerr = Socket::ERROR_NOROUTETOHOST;
        return nullptr;
    }

    Ptr<Ipv4Route> route = Create<Ipv4Route>();

    route->SetDestination(destination);
    route->SetGateway(instance.interfaces.GetAddress(nextHop->GetId()));

    // nodes haves only one interface in addition to the loopback interface, so the value is
    // hardocded..
    route->SetOutputDevice((idev != nullptr) ? idev : m_node->GetDevice(1));

    route->SetSource(actualNodeIpv4);

    sockerr = Socket::ERROR_NOTERROR;
    return route;
}

bool
LinkReversalRouting::RouteInput(Ptr<const Packet> packet,
                                const Ipv4Header& header,
                                Ptr<const NetDevice> idev,
                                const UnicastForwardCallback& ucb,
                                const MulticastForwardCallback& mcb,
                                const LocalDeliverCallback& lcb,
                                const ErrorCallback& ecb)
{
    Ptr<LrNode> actualLrNode = m_ipv4->GetObject<LrNode>();

    Ipv4Address destination = header.GetDestination();
    Ipv4Address actualNodeIpv4 = actualLrNode->GetIpv4Address();

    uint32_t iif = m_ipv4->GetInterfaceForDevice(idev);

    SimulationHelper& instance = SimulationHelper::GetInstance();

    if (m_ipv4->IsDestinationAddress(destination, iif))
    {
        NS_LOG_DEBUG("Packet arrived at destination " << destination
                                                      << " id: " << packet->GetUid());
        lcb(packet, header, iif);

        if (instance.m_enableBenchmark == true)
        {
            instance.m_benchmark_times.second = Simulator::Now();
            Time elapsed = instance.m_benchmark_times.second - instance.m_benchmark_times.first;
            NS_LOG_UNCOND("Elapsed time: " << elapsed.GetSeconds());
        }

        instance.m_success++;
        return true;
    }

    Ptr<LrNodeContainer> outbounds = instance.nodes.GetOutBoundNeighbours(actualLrNode);
    if (outbounds->GetN() == 0)
    {
        NS_LOG_DEBUG("No outbound neighbours, reversing link");
        instance.nodes.ReverseLink(actualLrNode);
    }

    Ptr<Ipv4Route> route = Create<Ipv4Route>();
    Ptr<LrNode> nextHop = instance.nodes.GetNextHop(actualLrNode, header.GetSource(), destination);

    // time to live
    uint8_t ttl = header.GetTtl();

    if (nextHop == nullptr)
    {
        NS_LOG_DEBUG("No route to host, packet id: " << packet->GetUid());
        instance.m_failure++;
        ecb(packet, header, Socket::ERROR_NOROUTETOHOST);
        return false;
    }
    else if (ttl == 0 && nextHop->GetIpv4Address() != destination)
    {
        NS_LOG_DEBUG("TTL expired, packet id: " << packet->GetUid());
        instance.m_failure++;
        return false;
    }

    NS_LOG_DEBUG("Forwarding packet from "
                 << actualNodeIpv4 << " (source " << header.GetSource() << ") to "
                 << instance.interfaces.GetAddress(nextHop->GetId()) << " ttl " << (uint32_t)ttl
                 << " id: " << packet->GetUid());

    route->SetDestination(destination);
    route->SetGateway(instance.interfaces.GetAddress(nextHop->GetId()));
    route->SetOutputDevice((m_ipv4->GetNetDevice(1)));

    Ipv4Header modifiedHeader = header;
    modifiedHeader.SetSource(actualNodeIpv4);

    ucb(route, packet, modifiedHeader);

    return true;
}

void
LinkReversalRouting::SetIpv4(Ptr<Ipv4> ipv4)
{
    NS_LOG_FUNCTION(this << ipv4);
    m_ipv4 = ipv4;
}

void
LinkReversalRouting::PrintRoutingTable(Ptr<OutputStreamWrapper> stream, Time::Unit unit) const
{
    NS_LOG_FUNCTION(this << stream);
}

void
LinkReversalRouting::NotifyInterfaceUp(uint32_t interface)
{
    NS_LOG_FUNCTION(this << interface);
}

void
LinkReversalRouting::NotifyInterfaceDown(uint32_t interface)
{
    NS_LOG_FUNCTION(this << interface);
}

void
LinkReversalRouting::NotifyAddAddress(uint32_t interface, Ipv4InterfaceAddress address)
{
    NS_LOG_FUNCTION(this << interface << address);
}

void
LinkReversalRouting::NotifyRemoveAddress(uint32_t interface, Ipv4InterfaceAddress address)
{
    NS_LOG_FUNCTION(this << interface << address);
}

void
LinkReversalRouting::SetNode(Ptr<Node> node)
{
    m_node = node;
}
