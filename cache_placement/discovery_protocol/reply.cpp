/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
    This is part of a simple discover protocol in NDN.
    Coded by Mateus Sousa (UFBA-Brazil-2017)

    Version 1.0
 **/

#include "reply.hpp"
#include "ns3/log.h"
#include "ns3/string.h"
#include "ns3/uinteger.h"
#include "ns3/packet.h"
#include "ns3/simulator.h"

#include "model/ndn-l3-protocol.hpp"
#include "helper/ndn-fib-helper.hpp"

#include <memory>

NS_LOG_COMPONENT_DEFINE("ndn.Reply");

namespace ns3 {
namespace ndn {

NS_OBJECT_ENSURE_REGISTERED(Reply);

TypeId
Reply::GetTypeId(void)
{
  static TypeId tid =
    TypeId("ns3::ndn::Reply")
      .SetGroupName("Ndn")
      .SetParent<App>()
      .AddConstructor<Reply>()      
      ;
  return tid;
}

Reply::Reply()
{
  NS_LOG_FUNCTION_NOARGS();
}

// inherited from Application base class.
void
Reply::StartApplication()
{
  NS_LOG_FUNCTION_NOARGS();
  App::StartApplication();
  m_prefix = "/HELLO";
  FibHelper::AddRoute(GetNode(), m_prefix, m_face, 0);
}

void
Reply::StopApplication()
{
  NS_LOG_FUNCTION_NOARGS();

  App::StopApplication();
}

// Global array to holds the neighbours's index
std::vector<int> neighbours;
// The neighbourhood LifeTime
std::vector<int> life;

// This function verify if the node still are in the neighbourhood
void
Reply::check_life_cicle(int ele) {
      bool isInNeighbourhood = false;
      for (uint32_t i = 0; i < neighbours.size(); i++) {
          if (neighbours[i] == ele){
              isInNeighbourhood = true;
              break;
          }else if (life[i] < 1) {
                life.erase(life.begin()+i);
                NS_LOG_INFO("[-] He's out -> " << neighbours[i]);
                neighbours.erase(neighbours.begin()+i);
                break;
          }else{
                life[i]--;
                isInNeighbourhood = true;
          }
      }

      if (!isInNeighbourhood) {
          neighbours.push_back(ele);
          life.push_back(2);
          // Just debugging
          NS_LOG_INFO("[+] New neighbour added -> " << neighbours[neighbours.size()-1]);
      }
}

void
Reply::OnInterest(shared_ptr<const Interest> interest)
{
  App::OnInterest(interest); // tracing inside

  NS_LOG_FUNCTION(this << interest);

  if (!m_active)
    return;

  NS_LOG_INFO("Received /HELLO/ message " << interest->getName());
  std::stringstream id;
  id << interest->getName().get(-1);
  // uint32_t index = (uint32_t)str.str();
  int tmp = atoi(id.str().c_str());
  NS_LOG_INFO("Index is > " << tmp);

  // Call check_life_cicle to see whos still remains in my area
  check_life_cicle(tmp);
}

} // namespace ndn
} // namespace ns3
