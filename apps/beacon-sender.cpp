/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
    This is part of a simple discover protocol in NDN.
    Coded by Mateus Sousa (UFBA-Brazil-2017)

    Version 1.0
 **/

#include "beacon-sender.hpp"
#include "ns3/ptr.h"
#include "ns3/log.h"
#include "ns3/simulator.h"
#include "ns3/packet.h"
#include "ns3/callback.h"
#include "ns3/string.h"
#include "ns3/boolean.h"
#include "ns3/uinteger.h"
#include "ns3/integer.h"
#include "ns3/double.h"
#include "helper/ndn-fib-helper.hpp"


NS_LOG_COMPONENT_DEFINE("ndn.Beacon");

namespace ns3 {
namespace ndn {

NS_OBJECT_ENSURE_REGISTERED(BeaconSender);

TypeId
BeaconSender::GetTypeId(void)
{
	static TypeId tid =
			TypeId("ns3::ndn::Beacon")
			.SetGroupName("Ndn")
			.SetParent<Beacon>()
			.AddConstructor<BeaconSender>()

			.AddAttribute("Time", "Time to start the offloading", StringValue("1.0"),
					MakeDoubleAccessor(&BeaconSender::m_frequency), MakeDoubleChecker<double>())
						.AddAttribute("isOrigin", "Is the node a producer?",BooleanValue(false),
								MakeBooleanAccessor(&BeaconSender::m_isOri),	MakeBooleanChecker())
								.AddAttribute("MaxSeq", "Maximum sequence number to request",
										IntegerValue(std::numeric_limits<uint32_t>::max()),
										MakeIntegerAccessor(&BeaconSender::m_seqMax), MakeIntegerChecker<uint32_t>())

							;
	return tid;
}

BeaconSender::BeaconSender()
: m_frequency(1.0) // Useless
, m_isOri(false) // Useless
{
	NS_LOG_FUNCTION_NOARGS();
	m_seqMax = std::numeric_limits<uint32_t>::max();
}

BeaconSender::~BeaconSender()
{
}

void
BeaconSender::ScheduleNextPacket()
{
	int interval = rand() % 200+10;
	double timing = (double)(interval/100.0);
	// double mean = 8.0 * m_payloadSize / m_desiredRate.GetBitRate ();
	// std::cout << "next: " << Simulator::Now().ToDouble(Time::S) + mean << "s\n";
	if (!m_sendEvent.IsRunning()){
		m_sendEvent = Simulator::Schedule(Seconds(timing),&Beacon::sendPacket, this);
	}

}

} // namespace ndn
} // namespace ns3
