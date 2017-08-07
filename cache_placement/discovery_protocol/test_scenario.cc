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

	LogComponentEnable("ndn.Consumer",LOG_LEVEL_INFO);
	NodeContainer nodes;
	nodes.Create(4);

	// PHY
	PointToPointHelper p2p;
	p2p.Install(nodes.Get(0),nodes.Get(1));
	p2p.Install(nodes.Get(1),nodes.Get(2));
	p2p.Install(nodes.Get(2),nodes.Get(3));

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

	// ndn::StrategyChoiceHelper::InstallAll("/traffic", "/localhost/nfd/strategy/let");
	ndn::StrategyChoiceHelper::InstallAll("/traffic", "/localhost/nfd/strategy/multicast");

	ApplicationContainer container;

	ndn::AppHelper temp ("ns3::ndn::Beacon");
	container.Add(temp.Install(nodes.Get(2)));
	container.Start(Seconds(0));
	container.Stop(Seconds(2));

	ndn::AppHelper consumer ("ns3::ndn::Beacon");
	// ndn::AppHelper consumer ("ns3::ndn::ConsumerCbr");
	// ndn::AppHelper consumer ("ns3::ndn::Offloading");
	// consumer.SetPrefix("/traffic");
	// consumer.SetAttribute("Frequency",DoubleValue(1));
	// consumer.SetAttribute("MaxSeq",IntegerValue(5));
	consumer.Install(nodes.Get(3));

	ndn::AppHelper producer ("ns3::ndn::Reply");
	// ndn::AppHelper producer ("ns3::ndn::Producer");
	// producer.SetPrefix("/HELLO");
	// producer.SetAttribute("PayloadSize",UintegerValue(1024));
	producer.Install(nodes.Get(0));

	// ndn::AppHelper attacker("Hijacker");
	// attacker.Install(nodes.Get(1));


	// ndn::CsTracer::InstallAll("cs-trace.txt", Seconds(1));
	AnimationInterface anim ("attack-sim.xml");

	// ndnGlobalRoutingHelper.AddOrigins("/prefix/sub", nodes.Get(1));
	// ndnGlobalRoutingHelper.CalculateRoutes();

	ndn::AppDelayTracer::Install(nodes.Get(3),"app-delays-trace.txt");
	// ndn::L3RateTracer::InstallAll("resultados.tr", Seconds(1.0));

	Simulator::Stop(Seconds(15));
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
