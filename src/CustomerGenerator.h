#ifndef CUSTOMERGENERATOR_H
#define CUSTOMERGENERATOR_H

#include <omnetpp.h>

using namespace omnetpp;

class CustomerGenerator : public cSimpleModule
{
  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;

  private:
    double meanArrivalRate;
    cMessage *generateCustomerEvent;
};

#endif // CUSTOMERGENERATOR_H
