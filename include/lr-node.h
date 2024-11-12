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

/**
 * \class LrNode
 * @brief Represents a node in the Link Reversal Routing protocol.
 *
 * This class models a node with a height attribute that should be unique for each node.
 * The height determines the flow of packets between nodes in the network.
 * This LR node should be used always with a MobilityModel.
 */
class LrNode : public Node
{
  private:
    double m_height;

  public:
    /**
     * @brief Default constructor for the LrNode class.
     *
     * This constructor initializes an LrNode instance with a default height value.
     */
    LrNode();

    /**
     * @brief Parameterized constructor for the LrNode class.
     *
     * This constructor initializes an LrNode instance with a specified height value.
     * @param height The height value to assign to the node.
     */
    LrNode(double height);

    /**
     * @brief Get the type ID.
     * @return The TypeId of the object.
     */
    static TypeId GetTypeId(void);

    /**
     * @brief Gets the height of the LrNode.
     * @return The current height value of the node.
     */
    double GetHeight() const;

    /**
     * @brief Sets the height of the LrNode.
     * @param height The new height value to assign to the node.
     */
    void SetHeight(double height);

    /**
     * @brief Calculates the Euclidean distance from this LrNode to another LrNode.
     *
     * This method computes the distance between the current node and another specified
     * node using their positions retrieved from their respective MobilityModel objects.
     * If either of the nodes does not have an associated MobilityModel the distance could not be
     * computed.
     *
     * @param node A Ptr to another LrNode for which the distance is to be calculated.
     * @return The Euclidean distance between the two nodes if both have a MobilityModel,
     *         otherwise returns -1.
     */
    double GetDistanceFrom(Ptr<LrNode> node) const;

    /**
     * @brief Retrieves the IPv4 address associated with this LrNode.
     *
     * @return The IPv4Address assigned to the node.
     */
    Ipv4Address GetIpv4Address();
};

#endif