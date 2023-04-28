#include <omnetpp.h>
#include "CustomerGenerator.h"

using namespace omnetpp;

Define_Module(CustomerGenerator);

void CustomerGenerator::initialize()
{
    meanArrivalRate = par("meanArrivalRate");
    generateCustomerEvent = new cMessage("generateCustomer");
    scheduleAt(simTime() + poisson(meanArrivalRate), generateCustomerEvent);
}

void CustomerGenerator::handleMessage(cMessage *msg)
{
    if (msg == generateCustomerEvent)
    {
        // Generate a new customer
        cMessage *customer = new cMessage("customer");
        send(customer, "customerOut");

        // Schedule the next customer generation event
        scheduleAt(simTime() + poisson(meanArrivalRate), generateCustomerEvent);
    }
}
