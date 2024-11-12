#include "../include/simulation-helper.h"

#include "../include/lr-routing-protocol.h"

void
SimulationHelper::setPhysicalLayer(bool enablePcap, bool enableAscii)
{
    WifiHelper wifi;
    wifi.SetStandard(WIFI_STANDARD_80211ax);

    WifiMacHelper wifiMac;
    wifiMac.SetType("ns3::AdhocWifiMac");

    YansWifiPhyHelper phy;
    YansWifiChannelHelper channel = YansWifiChannelHelper::Default();
    channel.AddPropagationLoss("ns3::FixedRssLossModel",
                               "Rss",
                               DoubleValue(-10)); // Strong constant signal
    channel.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");

    phy.SetChannel(channel.Create());
    this->devices = wifi.Install(phy, wifiMac, this->nodes);

    if (enablePcap)
        phy.EnablePcapAll("lra-simulation");

    if (enableAscii)
        phy.EnableAsciiAll("lra-simulation");
}

void
SimulationHelper::setPhysicalEnvironment(uint32_t maxNodes)
{
    uint32_t gridWidth = 20000; // 20km in meters
    uint32_t numColumns = std::ceil(std::sqrt(maxNodes));

    MobilityHelper mobility;
    mobility.SetPositionAllocator("ns3::GridPositionAllocator",
                                  "MinX",
                                  DoubleValue(0.0),
                                  "MinY",
                                  DoubleValue(0.0),
                                  "DeltaX",
                                  DoubleValue(this->m_initialDistance),
                                  "DeltaY",
                                  DoubleValue(this->m_initialDistance),
                                  "GridWidth",
                                  UintegerValue(numColumns),
                                  "LayoutType",
                                  StringValue("RowFirst"));

    mobility.SetMobilityModel(
        "ns3::RandomWalk2dMobilityModel",
        "Mode",
        StringValue("Time"),
        "Time",
        StringValue("2s"),
        "Speed",
        StringValue("ns3::ConstantRandomVariable[Constant=" + std::to_string(this->m_speed) + "]"),
        "Bounds",
        RectangleValue(Rectangle(0.0, gridWidth, 0.0, gridWidth)));

    mobility.Install(this->nodes);
}

void
SimulationHelper::setNetworkLayer()
{
    InternetStackHelper internet;
    internet.Install(this->nodes);

    Ipv4AddressHelper ipv4;
    ipv4.SetBase("10.1.0.0", "255.255.0.0");

    this->interfaces = ipv4.Assign(this->devices);

    for (uint32_t i = 0; i < this->nodes.GetN(); i++)
    {
        Ptr<LinkReversalRouting> lr = CreateObject<LinkReversalRouting>();
        Ptr<LrNode> node = this->nodes.Get(i);
        node->GetObject<Ipv4>()->SetRoutingProtocol(lr);
        lr->SetNode(node);
    }
}

void
SimulationHelper::parseCLI(int argc, char* argv[])
{
    CommandLine cmd;

    cmd.AddValue("nodes", "Number of nodes in the simulation.", this->m_maxNodes);
    cmd.AddValue("packets", "Number of packets to send in the simulation.", this->m_maxPackets);
    cmd.AddValue("sink", "ID of the sink node (node that receive the packet).", this->m_sinkNodeId);
    cmd.AddValue("source",
                 "ID of the source node (node that send the packets).",
                 this->m_sourceNodeId);
    cmd.AddValue("duration", "Simulation duration in seconds.", this->m_simulationDuration);
    cmd.AddValue("range", "Max communication range between nodes.", this->m_maxRange);
    cmd.AddValue("distance", "Initial distance between nodes.", this->m_initialDistance);
    cmd.AddValue("pcap", "Enable Pcap tracing", this->m_enablePcap);
    cmd.AddValue("ascii", "Enable ascii tracing", this->m_enableAscii);
    cmd.AddValue("speed", "Change the speed of nodes", this->m_speed);
    cmd.AddValue("benchmark",
                 "Execute benchmarks and output result in a file",
                 this->m_enableBenchmark);

    cmd.Parse(argc, argv);

    if (this->m_sinkNodeId >= this->m_maxNodes || this->m_sourceNodeId >= this->m_maxNodes)
    {
        NS_LOG_UNCOND("Node ID must be less than the number of nodes");
        exit(0);
    }

    if (this->m_simulationDuration <= 0)
    {
        NS_LOG_UNCOND("Simulation duration must be greater than 0");
        exit(0);
    }

    if (this->m_maxRange <= 0)
    {
        NS_LOG_UNCOND("Max range must be greater than 0");
        exit(0);
    }

    if (this->m_initialDistance <= 0)
    {
        NS_LOG_UNCOND("Initial distance must be greater than 0");
        exit(0);
    }

    if (this->m_maxPackets <= 0)
    {
        NS_LOG_UNCOND("Max packets must be greater than 0");
        exit(0);
    }

    if (this->m_speed < 0)
    {
        NS_LOG_UNCOND("Speed must be positive");
        exit(0);
    }

    if (this->m_simulationDuration < this->m_maxPackets)
    {
        NS_LOG_UNCOND("Duration must be greater than the number of packets ~(1 packets/second)");
        this->m_maxPackets = this->m_simulationDuration;
    }

    while (this->m_sourceNodeId == this->m_sinkNodeId)
    {
        this->m_sinkNodeId = rand() % this->m_maxNodes;
    };
}

void
SimulationHelper::setApplicationLayer()
{
    uint16_t port = 9;

    Address sinkAddress(InetSocketAddress(interfaces.GetAddress(this->m_sinkNodeId), port));
    PacketSinkHelper packetSinkHelper("ns3::UdpSocketFactory", sinkAddress);
    ApplicationContainer sinkApps = packetSinkHelper.Install(nodes.Get(this->m_sinkNodeId));

    sinkApps.Start(Seconds(0));
    sinkApps.Stop(Seconds(this->m_simulationDuration));

    UdpClientHelper udpClient(sinkAddress);

    udpClient.SetAttribute("MaxPackets", UintegerValue(this->m_maxPackets));
    udpClient.SetAttribute("Interval", TimeValue(Seconds(1.0)));
    udpClient.SetAttribute("PacketSize", UintegerValue(1024));

    ApplicationContainer clientApps = udpClient.Install(this->nodes.Get(this->m_sourceNodeId));

    clientApps.Start(Seconds(0.0));
    clientApps.Stop(Seconds(this->m_simulationDuration));
}

void
SimulationHelper::startSimulation()
{
    NS_LOG_UNCOND("Total nodes:\t" << this->m_maxNodes);
    NS_LOG_UNCOND("Source node id:\t" << this->m_sourceNodeId);
    NS_LOG_UNCOND("Sink node id:\t" << this->m_sinkNodeId);

    this->nodes.SetMaxRange(this->m_maxRange);
    this->nodes.Create(this->m_maxNodes, this->m_sinkNodeId);

    this->setPhysicalLayer(this->m_enablePcap, this->m_enableAscii);
    this->setPhysicalEnvironment(this->m_maxNodes);
    this->setNetworkLayer();
    this->setApplicationLayer();

    Simulator::Stop(Seconds(this->m_simulationDuration));
    Simulator::Run();
    Simulator::Destroy();
}

void
SimulationHelper::setMaxNodes(uint32_t maxNodes)
{
    this->m_maxNodes = maxNodes;
}

void
SimulationHelper::setMaxPackets(uint32_t maxPackets)
{
    this->m_maxPackets = maxPackets;
}

void
SimulationHelper::setSpeed(float speed)
{
    this->m_speed = speed;
}

SimulationHelper&
SimulationHelper::GetInstance()
{
    static SimulationHelper instance;
    return instance;
}