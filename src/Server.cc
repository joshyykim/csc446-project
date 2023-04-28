#include <omnetpp.h>
#include "Server.h"

using namespace omnetpp;

Define_Module(Server);

void Server::initialize()
{
    serviceRate = par("serviceRate");
    endServiceEvent = new cMessage("endOfService");

    lastServicefinished = 0;
    totalWaitingTime = 0;
}

void Server::handleMessage(cMessage *msg)
{
    EV_INFO << "servedCustomer: " << servedCustomer << endl;

    if (msg == endServiceEvent)
    {
        // End of service event
        cMessage *completedCustomer = (cMessage *)queue.pop();
        send(completedCustomer, "serverOut");

        EV_INFO << "now: " << simTime() << "  started: " << completedCustomer->getTimestamp() << endl;
        totalBusyTime += simTime() - std::max(completedCustomer->getTimestamp(), lastServicefinished);

        if (completedCustomer->getTimestamp() < lastServicefinished)
        {
            totalWaitingTime += lastServicefinished - completedCustomer->getTimestamp();
        }

        lastServicefinished = simTime();
        servedCustomer++;

        // Check if there are more customers waiting in the queue
        if (!queue.isEmpty())
        {
            // Schedule the next end of service event
            simtime_t serviceTime = exponential(serviceRate);
            scheduleAt(simTime() + serviceTime, endServiceEvent); // changed to endServiceEvent
        }
    }
    else
    {
        // New customer has arrived
        queue.insert(msg);

        // If the server is idle, start serving the new customer
        if (!endServiceEvent->isScheduled() && queue.getLength() == 1)
        {
            simtime_t serviceTime = exponential(serviceRate);
            scheduleAt(simTime() + serviceTime, endServiceEvent); // changed to endServiceEvent
        }
    }
}

void Server::finish()
{
    recordScalar("#totalBusyTime", totalBusyTime);
    recordScalar("#servedCustomer", servedCustomer);
}
