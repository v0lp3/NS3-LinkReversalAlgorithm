#ifndef LR_NODE_H
#define LR_NODE_H

#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/ipv4-address-helper.h"
#include "ns3/mobility-model.h"
#include "ns3/node.h"
#include "ns3/pointer.h"
#include "ns3/type-id.h"
#include "ns3/vector.h"

using namespace ns3;

class LrNode : public Node
{
  private:
    double m_height;

  public:
    LrNode();
    LrNode(double height);

    static TypeId GetTypeId(void);

    double GetHeight() const;

    void SetHeight(double height);

    double GetDistanceFrom(Ptr<LrNode> node) const;

    Ipv4Address GetIpv4Address();
};

#endif