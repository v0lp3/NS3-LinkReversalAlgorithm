#include "../include/lr-node.h"

NS_LOG_COMPONENT_DEFINE("LrNode");
NS_OBJECT_ENSURE_REGISTERED(LrNode);

TypeId
LrNode::GetTypeId(void)
{
    static TypeId tid =
        TypeId("ns3::LrNode").SetParent<Node>().SetGroupName("Network").AddConstructor<LrNode>();

    return tid;
}

LrNode::LrNode()
{
    m_height = 0;
}

LrNode::LrNode(double height)
{
    m_height = height;
}

double
LrNode::GetHeight() const
{
    return m_height;
}

void
LrNode::SetHeight(double height)
{
    m_height = height;
}

Ipv4Address
LrNode::GetIpv4Address()
{
    return this->GetObject<Ipv4>()->GetAddress(1, 0).GetLocal();
}

double
LrNode::GetDistanceFrom(Ptr<LrNode> node) const
{
    if (this->GetObject<MobilityModel>() && node->GetObject<MobilityModel>())
    {
        Vector from = this->GetObject<MobilityModel>()->GetPosition();
        Vector to = node->GetObject<MobilityModel>()->GetPosition();
        return sqrt(pow(from.x - to.x, 2) + pow(from.y - to.y, 2));
    }

    return -1;
}
