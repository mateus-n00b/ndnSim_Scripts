#include "ns3/mobility-module.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/ndnSIM-module.h"
#include "ns3/packet.h"
#include "ns3/netanim-module.h"
// #include "ns3/ndn-cs-tracer.hpp"

namespace ns3{
int main(int argc, char *argv[]) {

	// LogComponentEnable("ndn.Consumer",LOG_LEVEL_INFO);
	NodeContainer nodes;
	nodes.Create(5);

	// PHY
	PointToPointHelper p2p;
	p2p.Install(nodes.Get(0),nodes.Get(1));
	p2p.Install(nodes.Get(0),nodes.Get(4));
	p2p.Install(nodes.Get(1),nodes.Get(2));
	p2p.Install(nodes.Get(2),nodes.Get(3));

	Config::SetDefault ("ns3::PointToPointNetDevice::DataRate", StringValue ("1Mbps"));
  Config::SetDefault ("ns3::PointToPointChannel::Delay", StringValue ("10ms"));
  Config::SetDefault ("ns3::DropTailQueue::MaxPackets", StringValue ("50"));


	MobilityHelper mobility;
	mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
			"MinX", DoubleValue (95.0),
			"MinY", DoubleValue (80.0),
			"DeltaX", DoubleValue (10.0),
			"DeltaY", DoubleValue (5.0),
			"GridWidth", UintegerValue (10),
			"LayoutType", StringValue ("RowFirst"));
	mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
	mobility.InstallAll();

	// NETWORK STACK
	ndn::StackHelper ndnHelper;
	ndnHelper.SetDefaultRoutes(true);
	ndnHelper.SetOldContentStore("ns3::ndn::cs::Lru"); // don't set up max size here, will use default value = 100
	ndnHelper.InstallAll();


	// ndn::StrategyChoiceHelper::InstallAll("/", "/localhost/nfd/strategy/let");
	for (size_t i = 0; i < 5; i++) {
			if ( i!= 3)
					ndn::StrategyChoiceHelper::Install(nodes.Get(i),"/", "/localhost/nfd/strategy/lsif-strategy");
			else
					ndn::StrategyChoiceHelper::Install(nodes.Get(i),"/HELLO", "/localhost/nfd/strategy/lsif-strategy");
	}

	ApplicationContainer container;
	ApplicationContainer beacon;

	ndn::AppHelper beacon_client ("ns3::ndn::Beacon");
	// Instalando beacon app
	for (size_t i = 0; i < 5; i++) {
		beacon.Add(beacon_client.Install(nodes.Get(i))); // Utilizando os containers para instalar
																													// o app Beacon nos nodes!
	}

	ndn::AppHelper consumerHelper ("ns3::ndn::ConsumerCbr");
	consumerHelper.SetAttribute("MaxSeq",IntegerValue(3));
	consumerHelper.SetAttribute("Frequency",StringValue("1"));
	consumerHelper.SetAttribute ("LifeTime", StringValue ("3s"));
	consumerHelper.SetAttribute("Prefix",StringValue("/data"));
	container.Add(consumerHelper.Install(nodes.Get(0)));

	beacon.Start(Seconds(1));
	beacon.Stop(Seconds(19));

	container.Start(Seconds(5));
	container.Stop(Seconds(25));

	ndn::AppHelper producer ("ns3::ndn::Producer");
	// ndn::AppHelper beacon_server ("ns3::ndn::Beacon");
	producer.SetAttribute("PayloadSize",UintegerValue(1024));
	producer.SetAttribute("Prefix",StringValue("/data"));
	producer.Install(nodes.Get(3));
	// beacon_server.Install(nodes.Get(3));

	// ndn::CsTracer::InstallAll("cs-trace.txt", Seconds(1));
	AnimationInterface anim ("attack-sim.xml");

	// ndnGlobalRoutingHelper.AddOrigins("/", nodes.Get(0));
	// ndnGlobalRoutingHelper.CalculateRoutes();

	ndn::AppDelayTracer::Install(nodes.Get(0),"app-delays-trace.txt");
	// ndn::L3RateTracer::InstallAll("resultados.tr", Seconds(1.0));

	Simulator::Stop(Seconds(30));
	Simulator::Run();
	Simulator::Destroy();
	return 0;
}
}

int
main(int argc, char* argv[])
{
	return ns3::main(argc, argv);
}
