/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
    This is part of a simple discover protocol in NDN.
    Coded by Mateus Sousa (UFBA-Brazil-2017)

    Version 1.0
 **/

#ifndef NDN_CONSUMER_CBR_H
#define NDN_CONSUMER_CBR_H

#include "ns3/ndnSIM/model/ndn-common.hpp"

#include "beacon-app.hpp"

namespace ns3 {
namespace ndn {

/**
 * @ingroup ndn-apps
 * @brief Ndn application for sending out Interest packets at a "constant" rate (Poisson process)
 */
class BeaconSender : public Beacon {
public:
  static TypeId
  GetTypeId();

  /**
   * \brief Default constructor
   * Sets up randomizer function and packet sequence number
   */
  BeaconSender();
  virtual ~BeaconSender();

protected:
  /**
   * \brief Constructs the Interest packet and sends it using a callback to the underlying NDN
   * protocol
   */
  virtual void
  ScheduleNextPacket();

  /**
   * @brief Set type of frequency randomization
   * @param value Either 'none', 'uniform', or 'exponential'
   */
  void
  SetRandomize(const std::string& value);


  /**
   * @brief Get type of frequency randomization
   * @returns either 'none', 'uniform', or 'exponential'
   */
  std::string
  GetRandomize() const;

protected:
  double m_frequency; // Frequency of interest packets (in hertz)
  bool m_isOri; // Is a origin?

};

} // namespace ndn
} // namespace ns3

#endif
