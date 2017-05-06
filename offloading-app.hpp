/*
 This scripts tries to simulate an offloading in VANETs environment;

 The main steps are:
                    1º CONSUMER start the discovery to perform the offloading;
                    2º Surrogates replies the requests;
                    3º CONSUMER sends the data to the surrogates
                    4º Surrogates process the tasks and send it back;
                    5º CONSUMER received the processed tasks.

 Authors:
        Mateus Sousa (UFBA)

NOTE: Is necessary to perform more tests

*/

#ifndef OFFLOADING_H
#define OFFLOADING_H

#include "ns3/ndnSIM/model/ndn-common.hpp"
#include "ns3/ndnSIM/apps/ndn-app.hpp"

namespace ns3{
namespace ndn{
  class Offloading : public ndn::App{
    public:
          static TypeId
          GetTypeId();

          Offloading();
          virtual ~Offloading();

          virtual void
          StartApplication();

          // (overridden from ndn::App) Processing when application is stopped
          virtual void
          StopApplication();

          // (overridden from ndn::App) Callback that will be called when Interest arrives
          virtual void
          OnInterest(std::shared_ptr<const ndn::Interest> interest);

          // (overridden from ndn::App) Callback that will be called when Data arrives
          virtual void
          OnData(std::shared_ptr<const ndn::Data> contentObject);

    protected:
          void
          SendInterest();
          void
          Server();
          void
          Process();
          // void
          // SendData(std::shared_ptr<const ndn::Data> data, uint32_t PayloadSize);

          bool isServer;
          uint32_t pktLentgh;
          uint32_t offladSuccess;
          uint32_t numberOfSurrogates;
          double processingTime;
  };
}
}

#endif
