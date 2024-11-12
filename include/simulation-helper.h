#ifndef LINK_REVERSAL_HELPER_H
#define LINK_REVERSAL_HELPER_H

#include "lr-node-container.h"

#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/ipv4-address-helper.h"
#include "ns3/mobility-module.h"
#include "ns3/network-module.h"
#include "ns3/wifi-module.h"
#include "ns3/yans-wifi-helper.h"

using namespace ns3;

/**
 * \class SimulationHelper
 * @brief Provides various utilities for managing the simulation environment.
 *
 * This class is primarily responsible for setting up the simulation environment,
 * parsing command-line input, and installing the Link Reversal Routing protocol.
 * It is also used to benchmark the simulation by counting the number of packets
 * sent, received, and failed.
 */
class SimulationHelper
{
  public:
    LrNodeContainer nodes;
    NetDeviceContainer devices;
    Ipv4InterfaceContainer interfaces;

    uint32_t m_total_packet = 0;
    uint32_t m_success = 0;
    uint32_t m_failure = 0;

    bool m_enableBenchmark = false;

    std::pair<Time, Time> m_benchmark_times = {Seconds(0), Seconds(0)};

    /**
     * @brief Starts the simulation with the configured parameters.
     *
     * This method initializes the simulation environment calling the other methods in this class.
     */
    void startSimulation();

    /**
     * @brief Parses command-line input arguments.
     *
     * This method extracts and processes command-line arguments to configure
     * the simulation settings dynamically.
     *
     * @param argc Number of command-line arguments.
     * @param argv Array of command-line argument strings.
     */
    void parseCLI(int argc, char** argv);

    /**
     * @brief Sets the maximum number of nodes in the simulation.
     *
     * @param maxNodes The maximum number of nodes to be used in the simulation.
     */
    void setMaxNodes(uint32_t maxNodes);

    /**
     * @brief Sets the maximum number of packets for the simulation.
     *
     * This value controls how many packets can be sent or generated
     * during the simulation.
     *
     * @param maxPackets The maximum number of packets allowed.
     */
    void setMaxPackets(uint32_t maxPackets);

    /**
     * @brief Sets the speed of nodes in the simulation.
     *
     * Configures the mobility model to use the specified speed for node movement.
     *
     * @param speed The speed value to set for the nodes.
     */
    void setSpeed(float speed);

    /**
     * @brief Provides a singleton instance of the SimulationHelper class.
     *
     * This method returns a reference to a static instance of the SimulationHelper class,
     * ensuring that only one instance of the helper exists throughout the simulation's
     * lifetime.
     *
     * \return A reference to the single SimulationHelper instance.
     */
    static SimulationHelper& GetInstance();

  private:
    /**
     * @brief Configures the physical environment for the simulation.
     *
     * This method sets up the physical layout and mobility models for the
     * simulation based on the maximum number of nodes.
     *
     * @param maxNodes The maximum number of nodes to configure in the environment.
     */
    void setPhysicalEnvironment(uint32_t maxNodes);

    /**
     * @brief Configures the physical layer settings for the simulation.
     *
     * This method sets up the Wi-Fi network using 802.11ax standard in ad-hoc mode,
     * with a fixed RSS loss model to simulate a constant signal strength. It creates
     * a wireless channel with a constant speed propagation delay model and installs
     * network devices on the nodes. Optionally, it enables PCAP and ASCII tracing.
     *
     * @param enablePcap A flag indicating whether to enable PCAP tracing for packet capture.
     * @param enableAscii A flag indicating whether to enable ASCII tracing for packet logging.
     */
    void setPhysicalLayer(bool enablePcap, bool enableAscii);

    /**
     * @brief Configures the network layer for the simulation.
     *
     * This method sets up the network layer by installing the internet stack on the nodes,
     * assigning IP addresses to the devices, and configuring the Link Reversal Routing protocol
     * for each node. It initializes the nodes with a specified IPv4 address range and associates
     * a custom routing protocol with each node's IPv4 object.
     */
    void setNetworkLayer();

    /**
     * @brief Configures the application layer for the simulation.
     *
     * This method sets up a UDP client-server communication model between the source node
     * and the sink node using the specified port. It installs a packet sink application on
     * the sink node to receive packets and configures a UDP client on the source node to
     * send packets to the sink.
     */
    void setApplicationLayer();

    uint32_t m_maxNodes = 10;
    uint32_t m_sinkNodeId = 0;
    uint32_t m_sourceNodeId = 0;
    uint32_t m_simulationDuration = 100;
    uint32_t m_maxRange = 25;
    uint32_t m_initialDistance = 20;
    uint32_t m_maxPackets = 100;
    float m_speed = 1.0;
    bool m_enablePcap = false;
    bool m_enableAscii = false;
};

#endif