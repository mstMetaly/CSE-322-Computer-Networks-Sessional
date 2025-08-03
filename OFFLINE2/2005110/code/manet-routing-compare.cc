/*
 * Copyright (c) 2011 University of Kansas
 *
 * SPDX-License-Identifier: GPL-2.0-only
 *
 * Author: Justin Rohrer <rohrej@ittc.ku.edu>
 *
 * James P.G. Sterbenz <jpgs@ittc.ku.edu>, director
 * ResiliNets Research Group  https://resilinets.org/
 * Information and Telecommunication Technology Center (ITTC)
 * and Department of Electrical Engineering and Computer Science
 * The University of Kansas Lawrence, KS USA.
 *
 * Work supported in part by NSF FIND (Future Internet Design) Program
 * under grant CNS-0626918 (Postmodern Internet Architecture),
 * NSF grant CNS-1050226 (Multilayer Network Resilience Analysis and Experimentation on GENI),
 * US Department of Defense (DoD), and ITTC at The University of Kansas.
 */

/*
 * This example program allows one to run ns-3 DSDV, AODV, or OLSR under
 * a typical random waypoint mobility model.
 *
 * By default, the simulation runs for 200 simulated seconds, of which
 * the first 50 are used for start-up time.  The number of nodes is 50.
 * Nodes move according to RandomWaypointMobilityModel with a speed of
 * 20 m/s and no pause time within a 300x1500 m region.  The WiFi is
 * in ad hoc mode with a 2 Mb/s rate (802.11b) and a Friis loss model.
 * The transmit power is set to 7.5 dBm.
 *
 * It is possible to change the mobility and density of the network by
 * directly modifying the speed and the number of nodes.  It is also
 * possible to change the characteristics of the network by changing
 * the transmit power (as power increases, the impact of mobility
 * decreases and the effective density increases).
 *
 * By default, OLSR is used, but specifying a value of 2 for the protocol
 * will cause AODV to be used, and specifying a value of 3 will cause
 * DSDV to be used.
 *
 * By default, there are 10 source/sink data pairs sending UDP data
 * at an application rate of 2.048 Kb/s each.    This is typically done
 * at a rate of 4 64-byte packets per second.  Application data is
 * started at a random time between 50 and 51 seconds and continues
 * to the end of the simulation.
 *
 * The program outputs a few items:
 * - packet receptions are notified to stdout such as:
 *   <timestamp> <node-id> received one packet from <src-address>
 * - each second, the data reception statistics are tabulated and output
 *   to a comma-separated value (csv) file
 * - some tracing and flow monitor configuration that used to work is
 *   left commented inline in the program
 */

#include "ns3/aodv-module.h"
#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/dsdv-module.h"
#include "ns3/dsr-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/network-module.h"
#include "ns3/olsr-module.h"
#include "ns3/yans-wifi-helper.h"

#include <fstream>
#include <iostream>

using namespace ns3;
using namespace dsr;

NS_LOG_COMPONENT_DEFINE("manet-routing-compare");

/**
 * Routing experiment class.
 *
 * It handles the creation and run of an experiment.
 */
class RoutingExperiment
{
  public:
    RoutingExperiment();
    /**
     * Run the experiment.
     */
    void Run();

    /**
     * Handles the command-line parameters.
     * \param argc The argument count.
     * \param argv The argument vector.
     */
    void CommandSetup(int argc, char** argv);

  private:
    /**
     * Setup the receiving socket in a Sink Node.
     * \param addr The address of the node.
     * \param node The node pointer.
     * \return the socket.
     */
    Ptr<Socket> SetupPacketReceive(Ipv4Address addr, Ptr<Node> node);
    /**
     * Receive a packet.
     * \param socket The receiving socket.
     */
    void ReceivePacket(Ptr<Socket> socket);
    /**
     * Compute the throughput.
     */
    void CheckThroughput();

    uint32_t port{9};            //!< Receiving port number.
    uint32_t bytesTotal{0};      //!< Total received bytes.
    uint32_t packetsReceived{0}; //!< Total received packets.
    uint32_t m_nodes{20};        // nodes count added
    uint32_t m_packets{100};     // packets per second
    uint32_t m_speed{5};         // speed of node

    std::string m_CSVfileName{"manet-routing.output.csv"}; //!< CSV filename.
    int m_nSinks{10};                                      //!< Number of sink nodes.
    std::string m_protocolName{"AODV"};                    //!< Protocol name.
    double m_txp{7.5};                                     //!< Tx power.
    bool m_traceMobility{true};                            //!< Enable mobility tracing.
    bool m_flowMonitor{true};                              //!< Enable FlowMonitor.
};

RoutingExperiment::RoutingExperiment()
{
}

static inline std::string
PrintReceivedPacket(Ptr<Socket> socket, Ptr<Packet> packet, Address senderAddress)
{
    std::ostringstream oss;

    oss << Simulator::Now().GetSeconds() << " " << socket->GetNode()->GetId();

    if (InetSocketAddress::IsMatchingType(senderAddress))
    {
        InetSocketAddress addr = InetSocketAddress::ConvertFrom(senderAddress);
        oss << " received one packet from " << addr.GetIpv4();
    }
    else
    {
        oss << " received one packet!";
    }
    return oss.str();
}

// RoutingExperiment class er ReceivePacket func
void
RoutingExperiment::ReceivePacket(Ptr<Socket> socket)
{
    Ptr<Packet> packet;
    Address senderAddress;
    while ((packet = socket->RecvFrom(senderAddress)))
    {
        bytesTotal += packet->GetSize();
        packetsReceived += 1;
        NS_LOG_UNCOND(PrintReceivedPacket(socket, packet, senderAddress));
    }
}

// RoutingExperiment class er checkThroughput func
// this writes the throughput to the output file
void
RoutingExperiment::CheckThroughput()
{
    double kbs = (bytesTotal * 8.0) / 1000; // kilobits/second
    bytesTotal = 0;

    std::ofstream out(m_CSVfileName, std::ios::app); // writing to the csv file

    out << (Simulator::Now()).GetSeconds() << "," << kbs << "," << packetsReceived << ","
        << m_nSinks << "," << m_protocolName << "," << m_txp << "" << std::endl;

    out.close();
    packetsReceived = 0; // packets received within 1s zero kore deya hoilo
    // CheckThroughput function is called periodically, every 1 second in simulation time.
    // This schedules an event to occur at a specific time during the simulation.
    // This specifies the function (CheckThroughput) to be called when the event is triggered.
    Simulator::Schedule(Seconds(1.0), &RoutingExperiment::CheckThroughput, this);
}

// RoutingExperiment class er SetupPacketReceive func
Ptr<Socket>
RoutingExperiment::SetupPacketReceive(Ipv4Address addr, Ptr<Node> node)
{
    TypeId tid = TypeId::LookupByName("ns3::UdpSocketFactory");
    Ptr<Socket> sink = Socket::CreateSocket(node, tid);
    InetSocketAddress local = InetSocketAddress(addr, port);
    sink->Bind(local);
    sink->SetRecvCallback(MakeCallback(&RoutingExperiment::ReceivePacket, this));

    return sink;
}

// RoutingExperiment er command setup function
// number of nodes, packets per second, speed of nodes
/*
void
RoutingExperiment::CommandSetup(int argc, char** argv)
{
    CommandLine cmd(__FILE__);
    //cmd.AddValue("CSVfileName", "The name of the CSV output file name", m_CSVfileName);
    //cmd.AddValue("traceMobility", "Enable mobility tracing", m_traceMobility);
    //cmd.AddValue("protocol", "Routing protocol (OLSR, AODV, DSDV, DSR)", m_protocolName);
    //cmd.AddValue("flowMonitor", "enable FlowMonitor", m_flowMonitor);
    // cmd.Parse(argc, argv);

    // std::vector<std::string> allowedProtocols{"AODV"}; // allowed protocol changed

    // if (std::find(std::begin(allowedProtocols), std::end(allowedProtocols), m_protocolName) ==
    //     std::end(allowedProtocols))
    // {
    //     NS_FATAL_ERROR("No such protocol:" << m_protocolName);
    // }

}
*/

void
RoutingExperiment::CommandSetup(int argc, char** argv)
{
    CommandLine cmd(__FILE__);

    // Add values to command line for number of nodes, packets per second, and speed
    cmd.AddValue("nodes", "Number of nodes", m_nodes);
    cmd.AddValue("packets", "Packets per second", m_packets);
    cmd.AddValue("speed", "Speed of nodes (m/s)", m_speed);

    // Parse the command line arguments
    cmd.Parse(argc, argv);

    // Print the values that have been parsed
    std::cout << "Number of nodes: " << m_nodes << std::endl;
    std::cout << "Packets per second: " << m_packets << std::endl;
    std::cout << "Node speed: " << m_speed << " m/s" << std::endl;
}

int
main(int argc, char* argv[])
{
    RoutingExperiment experiment;
    experiment.CommandSetup(argc, argv);
    experiment.Run();

    return 0;
}

// Routing Experiment class er run func
void
RoutingExperiment::Run()
{
    Packet::EnablePrinting();

    // blank out the last output file and write the column headers
    std::ofstream out(m_CSVfileName);
    out << "SimulationSecond,"
        << "ReceiveRate,"
        << "PacketsReceived,"
        << "NumberOfSinks,"
        << "RoutingProtocol,"
        << "TransmissionPower" << std::endl;
    out.close();

    int nWifis = m_nodes; // replaced by numberOfNodes
    m_nSinks = m_nodes / 2;

    double TotalTime = 120.0;
    // need to calculate rate from numer of packets per second
    uint32_t tempRate = m_packets * 64 * 8; // packet size 64 bytes, calculating rate
    std::stringstream ssRate;
    ssRate << tempRate << "bps"; // Convert tempRate to string and append "bps"
    std::string rate = ssRate.str();
    // std::string rate("2048bps");
    std::string phyMode("DsssRate11Mbps");
    std::string tr_name("manet-routing-compare");
    int nodeSpeed = m_speed; // in m/s //replaced by node speed
    int nodePause = 0;       // in s

    Config::SetDefault("ns3::OnOffApplication::PacketSize", StringValue("64"));
    Config::SetDefault("ns3::OnOffApplication::DataRate", StringValue(rate));

    // Set Non-unicastMode rate to unicast mode
    Config::SetDefault("ns3::WifiRemoteStationManager::NonUnicastMode", StringValue(phyMode));

    NodeContainer adhocNodes;
    adhocNodes.Create(nWifis);

    // setting up wifi phy and channel using helpers
    WifiHelper wifi;
    wifi.SetStandard(WIFI_STANDARD_80211b);

    YansWifiPhyHelper wifiPhy;
    YansWifiChannelHelper wifiChannel;
    wifiChannel.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");
    wifiChannel.AddPropagationLoss("ns3::FriisPropagationLossModel");
    wifiPhy.SetChannel(wifiChannel.Create());

    // Add a mac and disable rate control
    WifiMacHelper wifiMac;
    wifi.SetRemoteStationManager("ns3::ConstantRateWifiManager",
                                 "DataMode",
                                 StringValue(phyMode),
                                 "ControlMode",
                                 StringValue(phyMode));

    wifiPhy.Set("TxPowerStart", DoubleValue(m_txp));
    wifiPhy.Set("TxPowerEnd", DoubleValue(m_txp));

    wifiMac.SetType("ns3::AdhocWifiMac");
    NetDeviceContainer adhocDevices = wifi.Install(wifiPhy, wifiMac, adhocNodes);

    MobilityHelper mobilityAdhoc;
    int64_t streamIndex = 0; // used to get consistent mobility across scenarios

    ObjectFactory pos;
    pos.SetTypeId("ns3::RandomRectanglePositionAllocator");
    pos.Set("X", StringValue("ns3::UniformRandomVariable[Min=0.0|Max=300.0]"));
    pos.Set("Y", StringValue("ns3::UniformRandomVariable[Min=0.0|Max=1500.0]"));

    Ptr<PositionAllocator> taPositionAlloc = pos.Create()->GetObject<PositionAllocator>();
    streamIndex += taPositionAlloc->AssignStreams(streamIndex);

    std::stringstream ssSpeed;
    ssSpeed << "ns3::UniformRandomVariable[Min=0.0|Max=" << nodeSpeed << "]";
    std::stringstream ssPause;
    ssPause << "ns3::ConstantRandomVariable[Constant=" << nodePause << "]";
    mobilityAdhoc.SetMobilityModel("ns3::RandomWaypointMobilityModel",
                                   "Speed",
                                   StringValue(ssSpeed.str()),
                                   "Pause",
                                   StringValue(ssPause.str()),
                                   "PositionAllocator",
                                   PointerValue(taPositionAlloc));
    mobilityAdhoc.SetPositionAllocator(taPositionAlloc);
    mobilityAdhoc.Install(adhocNodes);
    streamIndex += mobilityAdhoc.AssignStreams(adhocNodes, streamIndex);

    AodvHelper aodv;
    OlsrHelper olsr;
    DsdvHelper dsdv;
    DsrHelper dsr;
    DsrMainHelper dsrMain;
    Ipv4ListRoutingHelper list;
    InternetStackHelper internet;

    if (m_protocolName == "OLSR")
    {
        list.Add(olsr, 100);
        internet.SetRoutingHelper(list);
        internet.Install(adhocNodes);
    }
    else if (m_protocolName == "AODV")
    {
        list.Add(aodv, 100);
        internet.SetRoutingHelper(list);
        internet.Install(adhocNodes);
    }
    else if (m_protocolName == "DSDV")
    {
        list.Add(dsdv, 100);
        internet.SetRoutingHelper(list);
        internet.Install(adhocNodes);
    }
    else if (m_protocolName == "DSR")
    {
        internet.Install(adhocNodes);
        dsrMain.Install(dsr, adhocNodes);
        if (m_flowMonitor)
        {
            NS_FATAL_ERROR("Error: FlowMonitor does not work with DSR. Terminating.");
        }
    }
    else
    {
        NS_FATAL_ERROR("No such protocol:" << m_protocolName);
    }

    NS_LOG_INFO("assigning ip address");

    Ipv4AddressHelper addressAdhoc;
    addressAdhoc.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer adhocInterfaces;
    adhocInterfaces = addressAdhoc.Assign(adhocDevices);

    OnOffHelper onoff1("ns3::UdpSocketFactory", Address());
    onoff1.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1.0]"));
    onoff1.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0.0]"));

    for (int i = 0; i < m_nSinks; i++)
    {
        Ptr<Socket> sink = SetupPacketReceive(adhocInterfaces.GetAddress(i), adhocNodes.Get(i));

        AddressValue remoteAddress(InetSocketAddress(adhocInterfaces.GetAddress(i), port));
        onoff1.SetAttribute("Remote", remoteAddress);

        Ptr<UniformRandomVariable> var = CreateObject<UniformRandomVariable>();
        ApplicationContainer temp = onoff1.Install(adhocNodes.Get(i + m_nSinks));
        temp.Start(Seconds(var->GetValue(100.0, 101.0)));
        temp.Stop(Seconds(TotalTime));
    }

    std::stringstream ss;
    ss << nWifis;
    std::string nodes = ss.str();

    std::stringstream ss2;
    ss2 << nodeSpeed;
    std::string sNodeSpeed = ss2.str();

    std::stringstream ss3;
    ss3 << nodePause;
    std::string sNodePause = ss3.str();

    std::stringstream ss4;
    ss4 << rate;
    std::string sRate = ss4.str();

    // NS_LOG_INFO("Configure Tracing.");
    // tr_name = tr_name + "_" + m_protocolName +"_" + nodes + "nodes_" + sNodeSpeed + "speed_" +
    // sNodePause + "pause_" + sRate + "rate";

    // AsciiTraceHelper ascii;
    // Ptr<OutputStreamWrapper> osw = ascii.CreateFileStream(tr_name + ".tr");
    // wifiPhy.EnableAsciiAll(osw);
    AsciiTraceHelper ascii;
    MobilityHelper::EnableAsciiAll(ascii.CreateFileStream(tr_name + ".mob"));

    FlowMonitorHelper flowmonHelper;
    Ptr<FlowMonitor> flowmon;
    if (m_flowMonitor)
    {
        flowmon = flowmonHelper.InstallAll();
    }

    NS_LOG_INFO("Run Simulation.");

    CheckThroughput();

    Simulator::Stop(Seconds(TotalTime));
    Simulator::Run();

    if (m_flowMonitor)
    {
        flowmon->SerializeToXmlFile(tr_name + ".flowmon", false, false);

        //  csv file code
        std::string reportFolder = "scratch/report/";
        std::string filename = reportFolder + "manet-report.csv";
        // Check if the file exists
        std::ifstream fileCheck(filename);

        std::ofstream csvFile;
        csvFile.open(filename, std::ios::app); // Use a file name based on `tr_name`

        // Write the header for CSV file
        if (!fileCheck)
        {
            csvFile << "Speed,PacketsPerSecond,Nodes,NetworkThroughputAvg,EndToEndDelayAvg,"
                       "PacketDeliveryRatioAvg,PacketDropRatioAvg\n";
        }

        /*
        // Initialize sums for the averages
        double totalThroughput = 0;
        double totalDelay = 0;
        double totalPdr = 0;
        double totalPdrDrop = 0;
        int flowCount = 0;

        Ptr<Ipv4FlowClassifier> classifier =
            DynamicCast<Ipv4FlowClassifier>(flowmonHelper.GetClassifier());
        std::map<FlowId, FlowMonitor::FlowStats> stats = flowmon->GetFlowStats();

        // Iterate through each flow to accumulate values
        for (auto& flow : stats)
        {
            Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow(flow.first);
            uint32_t txBytes = flow.second.txBytes;
            uint32_t rxBytes = flow.second.rxBytes;
            uint32_t txPackets = flow.second.txPackets;
            uint32_t rxPackets = flow.second.rxPackets;
            uint32_t lostPackets = flow.second.lostPackets;

            // Calculate Throughput (Mbps)
            double throughput = rxBytes * 8.0 /
                                (flow.second.timeLastRxPacket.GetSeconds() -
                                 flow.second.timeFirstTxPacket.GetSeconds()) /
                                1e6; // Mbps

            // Calculate End-to-End Delay (Average delay per received packet)
            double delay = rxPackets > 0 ? flow.second.delaySum.GetSeconds() / rxPackets : 0;

            // Calculate Packet Delivery Ratio (PDR)
            double pdr = (txPackets > 0) ? (rxPackets / (double)txPackets) : 0;

            // Calculate Packet Drop Ratio (PDR drop)
            double pdrDrop = (txPackets > 0) ? (lostPackets / (double)txPackets) : 0;

            // Accumulate the values
            totalThroughput += throughput;
            totalDelay += delay;
            totalPdr += pdr;
            totalPdrDrop += pdrDrop;

            flowCount++; // Increase the flow count
        }

        // Calculate averages
        double throughput = (flowCount > 0) ? (totalThroughput / flowCount) : 0;
        double endToEndDelay = (flowCount > 0) ? (totalDelay / flowCount) : 0;
        double pDelivaryRatio = (flowCount > 0) ? (totalPdr / flowCount) : 0;
        double pDropRatio = (flowCount > 0) ? (totalPdrDrop / flowCount) : 0;
        */
        

        
        //  Initialize sums for all relevant parameters
        int64_t totalTxBytes = 0;
        int64_t totalRxBytes = 0;
        int64_t totalTxPackets = 0;
        int64_t totalRxPackets = 0;
        int64_t totalLostPackets = 0;
        int64_t totalDelaySum = 0; // For sum of delays (to calculate average delay later)
        int flowCount = 0;
        int64_t totalTime = 0;

        // Access the flow statistics
        Ptr<Ipv4FlowClassifier> classifier =
            DynamicCast<Ipv4FlowClassifier>(flowmonHelper.GetClassifier());
        std::map<FlowId, FlowMonitor::FlowStats> stats = flowmon->GetFlowStats();

        // Iterate through each flow to accumulate values
        for (auto& flow : stats)
        {
            // Extract flow stats
            uint32_t txBytes = flow.second.txBytes;
            uint32_t rxBytes = flow.second.rxBytes;
            uint32_t txPackets = flow.second.txPackets;
            uint32_t rxPackets = flow.second.rxPackets;
            uint32_t lostPackets = flow.second.lostPackets;
            double delaySum = flow.second.delaySum.GetSeconds();

            // Accumulate the values
            totalTxBytes += txBytes;
            totalRxBytes += rxBytes;
            totalTxPackets += txPackets;
            totalRxPackets += rxPackets;
            totalLostPackets += lostPackets;
            totalDelaySum += delaySum;

             // Calculate the flow time: time from first packet transmitted to last packet received
            double flowTime = flow.second.timeLastRxPacket.GetSeconds() - flow.second.timeFirstTxPacket.GetSeconds();
            if(flowTime < 0)
                flowTime = flowTime * (-1);
            totalTime += flowTime;  // Accumulate flow time

            flowCount++; // Increase the flow count
        }

       
        //Throughput (Mbps) = (Total Rx Bytes * 8) / (Total time for received packets)

        double throughput = (totalRxBytes * 8.0) / (totalTime * 1e6); // Mbps

        //End-to-End Delay (Average delay per received packet)
        double endToEndDelay = (totalRxPackets > 0) ? (totalDelaySum / (double)totalRxPackets) : 0;

        //Packet Delivery Ratio (PDR)
        double pDelivaryRatio = (totalTxPackets > 0) ? (totalRxPackets / (double)totalTxPackets) : 0;

        //Packet Drop Ratio (PDR drop)
        double pDropRatio = (totalTxPackets > 0) ? (totalLostPackets / (double)totalTxPackets) : 0;
        

        // Write the values to the CSV file
        csvFile << m_speed << "," << m_packets << "," << m_nodes << "," << throughput << ","
                << endToEndDelay << "," << pDelivaryRatio << "," << pDropRatio << "\n";

        // Close the CSV file
        csvFile.close();
    }

    Simulator::Destroy();
}
