#ifndef SERVER_H
#define SERVER_H

#include <omnetpp.h>

using namespace omnetpp;

class Server : public cSimpleModule
{
  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;

  private:
    int         serverId;
    int         queueLength;
    int         servedCustomer;
    double      serviceRate;
    cQueue      queue;
    cMessage    *endServiceEvent;
    simtime_t   totalBusyTime;
    simtime_t   lastServicefinished;
    simtime_t   totalWaitingTime;

  public:
    int getQueueLength() const { return queue.getLength(); }
    int getServedCustomer() const { return servedCustomer; }
    simtime_t getTotalBusyTime() const { return totalBusyTime; }
};

#endif // SERVER_H
