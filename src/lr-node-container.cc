#include "../include/lr-node-container.h"

NS_LOG_COMPONENT_DEFINE("LrNodeContainer");

LrNodeContainer::LrNodeContainer()
    : NodeContainer(),
      Object()
{
    srand(time(NULL));
}

TypeId
LrNodeContainer::GetTypeId(void)
{
    static TypeId tid = TypeId("ns3::LrNodeContainer")
                            .SetParent<Object>()
                            .SetGroupName("Network")
                            .AddConstructor<LrNodeContainer>();

    return tid;
}

Ptr<LrNode>
LrNodeContainer::Get(int i)
{
    return NodeContainer::Get(i)->GetObject<LrNode>();
}

void
LrNodeContainer::Create(uint32_t n, uint32_t sinkID)
{
    for (uint32_t i = 0; i < n; i++)
    {
        double height;

        if (i == sinkID)
        {
            height = 0.0;
        }
        else
        {
            do
                height = rand();
            while (m_idHeights.find(height) != m_idHeights.end());
        }

        m_idHeights.insert(height);
        Ptr<LrNode> node = CreateObject<LrNode>(height);
        NodeContainer::Add(node);
    }
}

Ptr<LrNodeContainer>
LrNodeContainer::GetNodeNeighbours(Ptr<LrNode> node)
{
    Ptr<LrNodeContainer> neighbours = CreateObject<LrNodeContainer>();

    for (uint32_t i = 0; i < this->GetN(); i++)
    {
        Ptr<LrNode> n = this->Get(i);

        if (n->GetId() != node->GetId() && n->GetDistanceFrom(node) <= m_maxRange)
        {
            neighbours->Add(n);
        }
    }

    return neighbours;
}

Ptr<LrNodeContainer>
LrNodeContainer::GetInboundNeighbours(Ptr<LrNode> node)
{
    Ptr<LrNodeContainer> neighbours = this->GetNodeNeighbours(node);
    Ptr<LrNodeContainer> inbound = CreateObject<LrNodeContainer>();

    for (uint32_t i = 0; i < neighbours->GetN(); i++)
    {
        Ptr<LrNode> n = neighbours->Get(i);

        if (n->GetHeight() > node->GetHeight())
        {
            inbound->Add(n);
        }
    }

    return inbound;
}

Ptr<LrNodeContainer>
LrNodeContainer::GetOutBoundNeighbours(Ptr<LrNode> node)
{
    Ptr<LrNodeContainer> neighbours = this->GetNodeNeighbours(node);
    Ptr<LrNodeContainer> outbound = CreateObject<LrNodeContainer>();

    for (uint32_t i = 0; i < neighbours->GetN(); i++)
    {
        Ptr<LrNode> n = neighbours->Get(i);

        if (n->GetHeight() <= node->GetHeight())
        {
            outbound->Add(n);
        }
    }

    return outbound;
}

void
LrNodeContainer::ReverseLink(Ptr<LrNode> node)
{
    Ptr<LrNodeContainer> inboundNeighbours = this->GetInboundNeighbours(node);

    if (inboundNeighbours->GetN() == 0)
        return;

    Ptr<LrNode> maxHeightNode = inboundNeighbours->Get(0);

    for (uint32_t i = 1; i < inboundNeighbours->GetN(); i++)
    {
        Ptr<LrNode> n = inboundNeighbours->Get(i);

        if (n->GetHeight() > maxHeightNode->GetHeight())
        {
            maxHeightNode = n;
        }
    }

    Ptr<LrNodeContainer> inboundNeighboursMaxHeight = this->GetInboundNeighbours(maxHeightNode);

    if (inboundNeighboursMaxHeight->GetN() == 0)
        node->SetHeight(maxHeightNode->GetHeight() + 0.1);
    else
    {
        Ptr<LrNode> minHeightNode = inboundNeighboursMaxHeight->Get(0);

        for (uint32_t i = 1; i < inboundNeighboursMaxHeight->GetN(); i++)
        {
            Ptr<LrNode> n = inboundNeighboursMaxHeight->Get(i);

            if (n->GetHeight() < minHeightNode->GetHeight())
            {
                minHeightNode = n;
            }
        }

        node->SetHeight(maxHeightNode->GetHeight() +
                        (minHeightNode->GetHeight() - maxHeightNode->GetHeight()) / 2);
    }
}

Ptr<LrNode>
LrNodeContainer::GetNodeFromIPv4(Ipv4Address address)
{
    for (uint32_t i = 0; i < this->GetN(); i++)
    {
        Ptr<LrNode> node = this->Get(i);

        if (node->GetIpv4Address() == address)
            return node;
    }

    return nullptr;
}

Ptr<LrNode>
LrNodeContainer::GetNextHop(Ptr<LrNode> actualNode, Ipv4Address source, Ipv4Address destination)
{
    Ptr<LrNodeContainer> outbounds = this->GetOutBoundNeighbours(actualNode);

    if (outbounds->GetN() == 0)
        return nullptr;

    Ptr<LrNode> destinationNode = this->GetNodeFromIPv4(destination);

    if (destinationNode == nullptr)
    {
        NS_LOG_UNCOND("Destination node not found");
        exit(-1);
    }

    Ptr<LrNode> nextHop = outbounds->Get(0);

    for (uint32_t i = 1; i < outbounds->GetN(); i++)
    {
        Ptr<LrNode> node = outbounds->Get(i);
        Ipv4Address nextHopAddress = node->GetIpv4Address();

        if (nextHopAddress == source)
            continue;

        if (nextHopAddress == destination)
            return node;

        else if (destinationNode->GetDistanceFrom(node) < destinationNode->GetDistanceFrom(nextHop))
            nextHop = node;
    }

    if (nextHop->GetIpv4Address() == source)
        return nullptr;
    else
        return nextHop;
}

void
LrNodeContainer::SetMaxRange(uint32_t maxRange)
{
    m_maxRange = maxRange;
}
