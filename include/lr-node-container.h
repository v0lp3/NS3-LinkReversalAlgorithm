#ifndef LR_NODE_CONTAINER_H
#define LR_NODE_CONTAINER_H

#include "lr-node.h"

#include "ns3/core-module.h"
#include "ns3/ipv4.h"
#include "ns3/node-container.h"
#include "ns3/object.h"
#include "ns3/pointer.h"

#include <set>

using namespace ns3;

class LrNodeContainer : public NodeContainer, public Object
{
  public:
    uint m_maxRange;

    static TypeId GetTypeId(void);

    LrNodeContainer();

    void SetMaxRange(uint32_t maxRange);

    void Create(uint32_t n, uint32_t sinkID);
    void ReverseLink(Ptr<LrNode> node);

    Ptr<LrNode> Get(int i);
    Ptr<LrNode> GetNodeFromIPv4(Ipv4Address address);
    Ptr<LrNode> GetNextHop(Ptr<LrNode> actualNode, Ipv4Address source, Ipv4Address destination);

    Ptr<LrNodeContainer> GetNodeNeighbours(Ptr<LrNode> node);
    Ptr<LrNodeContainer> GetInboundNeighbours(Ptr<LrNode> node);
    Ptr<LrNodeContainer> GetOutBoundNeighbours(Ptr<LrNode> node);

  private:
    std::set<double> m_idHeights;
};

#endif