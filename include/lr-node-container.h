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

/**
 * \class LrNodeContainer
 * @brief Container for LrNode objects.
 *
 * The LrNodeContainer class is a specialization of the NodeContainer, designed to store
 * LrNode objects. It offers a quick way to handle simulations by allowing direct access
 * to each node and enabling comparisons between nodes.
 *
 * The LrNodeContainer also sets a range to determine whether a node is considered a
 * neighbor or not.
 */
class LrNodeContainer : public NodeContainer, public Object
{
  public:
    uint m_maxRange;

    /**
     * @brief Get the type ID.
     * @return The TypeId of the object.
     */
    static TypeId GetTypeId(void);

    LrNodeContainer();

    /**
     * @brief Sets the maximum range for the LrNodeContainer.
     *
     * @param maxRange The maximum range to set.
     */
    void SetMaxRange(uint32_t maxRange);

    /**
     * @brief Creates a specified number of nodes and assigns them heights, with one designated sink
     * node.
     *
     * This method creates `n` nodes and assigns each a unique height. The node identified by
     * `sinkID` is assigned a height of 0.0, indicating it is the sink node. The other nodes are
     * assigned random heights, ensuring no two nodes have the same height.
     *
     * @param n The number of nodes to be created.
     * @param sinkID The ID of the node that will act as the sink (assigned a height of 0.0).
     */
    void Create(uint32_t n, uint32_t sinkID);

    /**
     * @brief Reverses the link of the given node by adjusting its height based on its inbound
     * neighbors.
     *
     * This method first finds the inbound neighbors of the given node. If there are no inbound
     * neighbors, the function returns immediately. It then finds the inbound neighbor with the
     * maximum height and adjusts the height of the given node accordingly to mantain height unique.
     *
     * If the maximum height node itself has inbound neighbors, the height of the given node is set
     * to mantain the order of heights.
     *
     * @param node The node for which the link is to be reversed.
     */
    void ReverseLink(Ptr<LrNode> node);

    /**
     * @brief Retrieves an LrNode pointer from the NodeContainer given the index.
     *
     * @param i Index of the node to retrieve.
     * @return Ptr<LrNode> Pointer to the LrNode at the specified index.
     */
    Ptr<LrNode> Get(int i);

    /**
     * @brief Retrieves a node based on its IPv4 address.
     *
     * This method iterates through all nodes in the current container and returns the node whose
     * IPv4 address matches the specified address. If no node is found with the given IPv4 address,
     * it returns nullptr.
     *
     * @param address The IPv4 address of the node to be found.
     * @return Ptr<LrNode> The node with the matching IPv4 address, or nullptr if no such node is
     * found.
     */
    Ptr<LrNode> GetNodeFromIPv4(Ipv4Address address);

    /**
     * @brief Determines the next hop node for routing from the actual node to the destination.
     *
     * This method retrieves the outbound neighbors of the actual node and attempts to find the best
     * next hop node that brings the packet closer to the destination. It ensures that the next hop
     * is not the source and attempts to directly reach the destination if possible.
     *
     * If no valid next hop is found, it returns nullptr.
     *
     * @param actualNode The current node from which the next hop is being determined.
     * @param source The IPv4 address of the source node.
     * @param destination The IPv4 address of the destination node.
     * @return Ptr<LrNode> The next hop node, or nullptr if no suitable next hop is found.
     */
    Ptr<LrNode> GetNextHop(Ptr<LrNode> actualNode, Ipv4Address source, Ipv4Address destination);

    /**
     * @brief Retrieves the neighboring nodes of a specified LrNode within a certain range and that
     * satisfy a specific filter condition.
     *
     * This function iterates over all nodes in the current LrNodeContainer and identifies
     * nodes that are within a specified maximum range (m_maxRange) from the given node.
     * Nodes are considered neighbors if they are within the maximum range and have a
     * different ID than the specified node. The neighbors are added to a new
     * LrNodeContainer if the filter criteria is satisfied, which is then returned.
     *
     * @param node A Ptr to the LrNode for which neighboring nodes are to be found.
     * @param filter A function that determines whether a neighbor should be included in the
     * result.
     * @return A Ptr to an LrNodeContainer containing all nodes that are considered
     *         neighbors of the specified node.
     */
    Ptr<LrNodeContainer> GetNodeNeighbours(Ptr<LrNode> node,
                                           std::function<bool(Ptr<LrNode>)> filter);

    /**
     * @brief Retrieves the inbound neighboring nodes of the given node.
     *
     * Inbound neighbors are those that have a higher height value compared to the given node.
     * This method uses the GetFilteredNeighbours function to filter nodes that have a greater
     * height than the given node, indicating that they are inbound neighbors.
     *
     * @param node The node for which inbound neighbors are to be found.
     * @return Ptr<LrNodeContainer> A container holding the inbound neighboring nodes, which are
     *         nodes with a height greater than the height of the given node.
     */
    Ptr<LrNodeContainer> GetInboundNeighbours(Ptr<LrNode> node);

    /**
     * @brief Retrieves the outbound neighboring nodes of the given node.
     *
     * Outbound neighbors are those that have a height value less than or equal to the given node.
     * This method uses the GetFilteredNeighbours function to filter nodes that have a height less
     * than or equal to the given node, indicating that they are outbound neighbors.
     *
     * @param node The node for which outbound neighbors are to be found.
     * @return Ptr<LrNodeContainer> A container holding the outbound neighboring nodes, which are
     *         nodes with a height less than or equal to the height of the given node.
     */
    Ptr<LrNodeContainer> GetOutBoundNeighbours(Ptr<LrNode> node);

  private:
    std::set<double> m_idHeights;
};

#endif