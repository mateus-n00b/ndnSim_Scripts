/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2011-2015  Regents of the University of California.
 *
 * This file is part of ndnSIM. See AUTHORS for complete list of ndnSIM authors and
 * contributors.
 *
 * ndnSIM is free software: you can redistribute it and/or modify it under the terms
 * of the GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 *
 * ndnSIM is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * ndnSIM, e.g., in COPYING.md file.  If not, see <http://www.gnu.org/licenses/>.
 **/

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/internet-module.h"
#include "ns3/wifi-80211p-helper.h"
#include "ns3/wave-mac-helper.h"
#include "ns3/netanim-module.h"

#include "ns3/ndnSIM-module.h"

using namespace std;
namespace ns3 {

NS_LOG_COMPONENT_DEFINE("ndn.mateus");

void showDistance(Ptr<MobilityModel> model1, Ptr<MobilityModel> model2) {
    std::cout << "[SYSTEM] Distance from server to client => " << model1->GetDistanceFrom (model2) << std::endl;
}

void RecvPkt(void) {
    NS_LOG_UNCOND("Received one packet!");
}

int
main(int argc, char* argv[])
{
  // disable fragmentation
  std::string phyMode ("OfdmRate6MbpsBW10MHz");
  // Config::SetDefault("ns3::WifiRemoteStationManager::FragmentationThreshold", StringValue("2200"));
  // Config::SetDefault("ns3::WifiRemoteStationManager::RtsCtsThreshold", StringValue("2200"));
  // Config::SetDefault("ns3::WifiRemoteStationManager::NonUnicastMode", StringValue("OfdmRate24Mbps"));
  std::string traceFile;
  uint32_t numberOfNodes = 25;
  // set the seed
  srand(time(NULL));
  uint32_t the_client = rand() % numberOfNodes + 0;
  uint32_t the_server = rand() % numberOfNodes + 0;
  while (the_server == the_client) {
    the_server = rand() % numberOfNodes + 0;
  }

  CommandLine cmd;
  cmd.AddValue ("traceFile", "Ns2 trace de mobilidade", traceFile);
  cmd.AddValue("numberOfNodes","Numero de nos",numberOfNodes);
  cmd.Parse(argc, argv);

  if (traceFile.empty ())
    {
      std::cout << "Usage of " << argv[0] << " :\n\n"
      "./waf --run \"vanets_tcp"
      " --traceFile=src/mobility/examples/default.ns_movements"
      "NOTE: ns2-traces-file could be an absolute or relative path.\n"
      "NOTE 2: Number of nodes present in the trace file must match with the command line argument and must\n"
      "        be a positive number. Note that you must know it before to be able to load it.\n\n"
      "NOTE 3: Duration must be a positive number. Note that you must know it before to be able to load it.\n\n";

      return 0;
    }

  NodeContainer nodes;
  nodes.Create(numberOfNodes);

  /*

          PHY LAYER

  */
  // WifiHelper wifi = WifiHelper::Default();
  // // wifi.SetRemoteStationManager ("ns3::AarfWifiManager");
  // wifi.SetStandard(WIFI_PHY_STANDARD_80211a);
  // wifi.SetRemoteStationManager("ns3::ConstantRateWifiManager", "DataMode",
  //                              StringValue("OfdmRate24Mbps"));
  //
  // YansWifiChannelHelper wifiChannel; // = YansWifiChannelHelper::Default ();
  // wifiChannel.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");
  // // wifiChannel.AddPropagationLoss("ns3::ThreeLogDistancePropagationLossModel");
  // wifiChannel.AddPropagationLoss("ns3::NakagamiPropagationLossModel");
  //
  // // YansWifiPhy wifiPhy = YansWifiPhy::Default();
  // YansWifiPhyHelper wifiPhyHelper = YansWifiPhyHelper::Default();
  // wifiPhyHelper.SetChannel(wifiChannel.Create());
  // wifiPhyHelper.Set("TxPowerStart", DoubleValue(5));
  // wifiPhyHelper.Set("TxPowerEnd", DoubleValue(5));
  YansWifiPhyHelper wifiPhy =  YansWifiPhyHelper::Default ();
  YansWifiChannelHelper wifiChannel = YansWifiChannelHelper::Default ();
  wifiChannel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
  wifiChannel.AddPropagationLoss ("ns3::NakagamiPropagationLossModel");
  Ptr<YansWifiChannel> channel = wifiChannel.Create ();
  wifiPhy.SetChannel (channel);


  /*

            MAC LAYER (VANETs)

  */
  NqosWaveMacHelper wifi80211pMac = NqosWaveMacHelper::Default ();
  Wifi80211pHelper wifi80211p = Wifi80211pHelper::Default ();

  wifi80211p.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
                                      "DataMode",StringValue (phyMode),
                                      "ControlMode",StringValue (phyMode));
  ////////////////
  // 1. Install wifi80211p
  NetDeviceContainer wifiNetDevices = wifi80211p.Install(wifiPhy, wifi80211pMac, nodes);

  // 2. Install Mobility model
  MobilityHelper mobility;
  Ns2MobilityHelper ns2 = Ns2MobilityHelper (traceFile);
  ns2.Install ();

  mobility.Install(nodes);

  // 3. Install NDN stack
  NS_LOG_INFO("Installing NDN stack");
  ndn::StackHelper ndnHelper;
  // ndnHelper.AddNetDeviceFaceCreateCallback (WifiNetDevice::GetTypeId (), MakeCallback(RecvPkt));
  ndnHelper.SetOldContentStore("ns3::ndn::cs::Lru", "MaxSize", "1000");
  ndnHelper.SetDefaultRoutes(true);
  ndnHelper.Install(nodes);

  // Set BestRoute strategy
  ndn::StrategyChoiceHelper::Install(nodes, "/", "/localhost/nfd/strategy/multicast");


  ndn::GlobalRoutingHelper ndnGlobalRoutingHelper;
  ndnGlobalRoutingHelper.InstallAll();

  // 4. Set up applications
  NS_LOG_INFO("Installing Applications");
  std::cout << "[SYSTEM] Client index => "<< the_client << std::endl;

  ndn::AppHelper consumerHelper("ns3::ndn::ConsumerCbr");
  consumerHelper.SetPrefix("/ufba");
  consumerHelper.SetAttribute("Frequency", StringValue("100"));
  consumerHelper.SetAttribute("Randomize",StringValue("uniform"));
  consumerHelper.Install(nodes.Get(the_client));

  std::cout << "[SYSTEM] Server index => "<< the_server << std::endl;

  ndn::AppHelper producerHelper("ns3::ndn::Producer");
  producerHelper.SetPrefix("/ufba");
  producerHelper.SetAttribute("PayloadSize", StringValue("1200"));
  producerHelper.Install(nodes.Get(the_server));

  // Get the distance between the nodes
  Ptr<MobilityModel> model1 = nodes.Get(the_server)->GetObject<MobilityModel>();
  Ptr<MobilityModel> model2 = nodes.Get(the_client)->GetObject<MobilityModel>();

  Simulator::Schedule(Seconds(1),showDistance,model1,model2);
  ////////////////

  // ndn::L3RateTracer::InstallAll("resultados.tr", Seconds(1.0));
  AnimationInterface anim ("ndn-animation.xml");

  Simulator::Stop(Seconds(15.0));

  // L2RateTracer::InstallAll("drop-trace.txt", Seconds(1.0));

  Simulator::Run();
  Simulator::Destroy();

  return 0;
}

} // namespace ns3

int
main(int argc, char* argv[])
{
  return ns3::main(argc, argv);
}
