#include "include/lr-node-container.h"
#include "include/lr-node.h"
#include "include/simulation-helper.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("LRA-Simulation");

int
main(int argc, char* argv[])
{
    NS_LOG_UNCOND("__Project__\tLINK REVERSAL ALGORITHM SIMULATION");
    NS_LOG_UNCOND("__Author__\tAndrea Maugeri");

    SimulationHelper& instance = SimulationHelper::GetInstance();

    instance.parseCLI(argc, argv);
   
    instance.startSimulation();

    NS_LOG_UNCOND("Simulation completed.");
    
    NS_LOG_UNCOND("Total packets: " << instance.m_total_packet);
    NS_LOG_UNCOND("Success: " << instance.m_success);
    NS_LOG_UNCOND("Failure: " << instance.m_failure);
}