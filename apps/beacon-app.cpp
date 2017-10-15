/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
    This is part of a simple discover protocol in NDN.
    Coded by Mateus Sousa (UFBA-Brazil-2017)

    Version 1.0
 **/

#include "beacon-app.hpp"
#include "ns3/ptr.h"
#include "helper/ndn-fib-helper.hpp"
#include "ns3/log.h"
#include "ns3/simulator.h"
#include "ns3/packet.h"
#include "ns3/callback.h"
#include "ns3/string.h"
#include "ns3/boolean.h"
#include "ns3/uinteger.h"
#include "ns3/integer.h"
#include "ns3/double.h"

#include "utils/ndn-ns3-packet-tag.hpp"
#include "utils/ndn-rtt-mean-deviation.hpp"

#include <ndn-cxx/lp/tags.hpp>

#include <boost/lexical_cast.hpp>
#include <boost/ref.hpp>

NS_LOG_COMPONENT_DEFINE("ndn.BeaconApp");

namespace ns3 {
namespace ndn {

NS_OBJECT_ENSURE_REGISTERED(Beacon);

TypeId
Beacon::GetTypeId(void)
{
	static TypeId tid =
			TypeId("ns3::ndn::BeaconApp")
			.SetGroupName("Ndn")
			.SetParent<App>()
			;

	return tid;
}

Beacon::Beacon()
: m_rand(CreateObject<UniformRandomVariable>())
, m_seq(0)
, m_seqMax(0) // don't request anything
{
	NS_LOG_FUNCTION_NOARGS();

	m_rtt = CreateObject<RttMeanDeviation>();
}

void
Beacon::SetRetxTimer(Time retxTimer)
{
	m_retxTimer = retxTimer;
	if (m_retxEvent.IsRunning()) {
		// m_retxEvent.Cancel (); // cancel any scheduled cleanup events
		Simulator::Remove(m_retxEvent); // slower, but better for memory
	}

	// schedule even with new timeout
	m_retxEvent = Simulator::Schedule(m_retxTimer, &Beacon::CheckRetxTimeout, this);
}

Time
Beacon::GetRetxTimer() const
{
	return m_retxTimer;
}

void
Beacon::CheckRetxTimeout()
{
	Time now = Simulator::Now();

	Time rto = m_rtt->RetransmitTimeout();
	// NS_LOG_DEBUG ("Current RTO: " << rto.ToDouble (Time::S) << "s");

	while (!m_seqTimeouts.empty()) {
		SeqTimeoutsContainer::index<i_timestamp>::type::iterator entry =
				m_seqTimeouts.get<i_timestamp>().begin();
		if (entry->time + rto <= now) // timeout expired?
		{
			uint32_t seqNo = entry->seq;
			m_seqTimeouts.get<i_timestamp>().erase(entry);
			OnTimeout(seqNo);
		}
		else
			break; // nothing else to do. All later packets need not be retransmitted
	}

	m_retxEvent = Simulator::Schedule(m_retxTimer, &Beacon::CheckRetxTimeout, this);
}

// Application Methods
void
Beacon::StartApplication() // Called at time specified by Start
{
	NS_LOG_FUNCTION_NOARGS();

	// do base stuff
	App::StartApplication();
	NS_LOG_INFO("[+] Starting for node " << GetNode()->GetId());
	ScheduleNextPacket();
}

void
Beacon::StopApplication() // Called at time specified by Stop
{
	NS_LOG_FUNCTION_NOARGS();

	// cancel periodic packet generation
	Simulator::Cancel(m_sendEvent);

	// cleanup base stuff
	App::StopApplication();
}

void
Beacon::sendPacket()
{
	if (!m_active)
		return;

	NS_LOG_FUNCTION_NOARGS();

	uint32_t seq = std::numeric_limits<uint32_t>::max(); // invalid

	while (m_retxSeqs.size()) {
		seq = *m_retxSeqs.begin();
		m_retxSeqs.erase(m_retxSeqs.begin());
		break;
	}

	if (seq == std::numeric_limits<uint32_t>::max()) {
		if (m_seqMax != std::numeric_limits<uint32_t>::max()) {
			if (m_seq >= m_seqMax) {
				return; // we are totally done
			}
		}

		seq = m_seq++;
	}

	// Here I set my ID in the special interest
	std::stringstream ID;
	ID << GetNode()->GetId();
	m_interestName = "/HELLO/"+ID.str();

	shared_ptr<Name> nameWithSequence = make_shared<Name>(m_interestName);
	// nameWithSequence->appendSequenceNumber(seq);
	//

	shared_ptr<Interest> interest = make_shared<Interest>();
	interest->setNonce(m_rand->GetValue(0, std::numeric_limits<uint32_t>::max()));
	interest->setName(*nameWithSequence);

	time::milliseconds interestLifeTime(m_interestLifeTime.GetMilliSeconds());
	interest->setInterestLifetime(interestLifeTime);

	WillSendOutInterest(seq);

	m_transmittedInterests(interest, this, m_face);
	m_appLink->onReceiveInterest(*interest);
	NS_LOG_INFO("node(" << ID.str() << ") sent a beacon!");

	ScheduleNextPacket();
}

void
Beacon::OnTimeout(uint32_t sequenceNumber)
{
	NS_LOG_FUNCTION(sequenceNumber);
	// std::cout << Simulator::Now () << ", TO: " << sequenceNumber << ", current RTO: " <<
	// m_rtt->RetransmitTimeout ().ToDouble (Time::S) << "s\n";

	m_rtt->IncreaseMultiplier(); // Double the next RTO
	m_rtt->SentSeq(SequenceNumber32(sequenceNumber),
			1); // make sure to disable RTT calculation for this sample
	m_retxSeqs.insert(sequenceNumber);
	ScheduleNextPacket();
}

void
Beacon::WillSendOutInterest(uint32_t sequenceNumber)
{
	NS_LOG_DEBUG("Trying to add " << sequenceNumber << " with " << Simulator::Now() << ". already "
			<< m_seqTimeouts.size() << " items");

	m_seqTimeouts.insert(SeqTimeout(sequenceNumber, Simulator::Now()));
	m_seqFullDelay.insert(SeqTimeout(sequenceNumber, Simulator::Now()));

	m_seqLastDelay.erase(sequenceNumber);
	m_seqLastDelay.insert(SeqTimeout(sequenceNumber, Simulator::Now()));

	m_seqRetxCounts[sequenceNumber]++;

	m_rtt->SentSeq(SequenceNumber32(sequenceNumber), 1);
}

} // namespace ndn
} // namespace ns3
