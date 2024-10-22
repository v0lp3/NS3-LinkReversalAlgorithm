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

    void startSimulation();
    void parseCLI(int argc, char** argv);

    void setMaxNodes(uint32_t maxNodes);
    void setMaxPackets(uint32_t maxPackets);
    void setSpeed(float speed);

    static SimulationHelper& GetInstance();

  private:
    void setPhysicalEnvironment(uint32_t maxNodes);
    void setPhysicalLayer(bool enablePcap, bool enableAscii);
    void setNetworkLayer();
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