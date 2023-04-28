#ifndef LOADBALANCER_H
#define LOADBALANCER_H

#include <omnetpp.h>

using namespace omnetpp;

class LoadBalancer : public cSimpleModule
{
  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;

  private:
    int numServers;
    int maxCustomers;
    int numServedCustomers;
    int numGeneratedCustomer;
    int selectionSize;
    int maxWorkload;
    int sumWorkload;
    int lastServer;
    simtime_t avgSystemTime;
    simtime_t totalSystemTime;
    simtime_t totalServiceTime;
    std::vector<int> serverQueueLengths;
};

#endif // LOADBALANCER_H
