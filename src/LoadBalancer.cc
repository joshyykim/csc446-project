#include <omnetpp.h>
#include "LoadBalancer.h"
#include "Server.h"

using namespace omnetpp;

Define_Module(LoadBalancer);

void LoadBalancer::initialize()
{
    numServers = par("numServers");
    maxCustomers = par("maxCustomers");
    selectionSize = par("selectionSize");

    numGeneratedCustomer = 0;
    numServedCustomers = 0;
    maxWorkload = 0;
    sumWorkload = 0;
    avgSystemTime = 0;
    totalSystemTime = 0;
    totalServiceTime = 0;
    lastServer = 0;
}

void LoadBalancer::handleMessage(cMessage *msg)
{
    if (msg->arrivedOn("serverIn"))
    {
        // Customer has been processed by a server, record the system time
        simtime_t systemTime = simTime() - msg->getTimestamp();
        totalSystemTime += systemTime;

        // Emit the average system time
        avgSystemTime = totalSystemTime.dbl() / std::max(1, numServedCustomers);

        // Delete the completed customer message
        delete msg;

        // Calculate the maximum workload and average workload
        for (int i = 0; i < numServers; i++)
        {
            Server *serverModule = check_and_cast<Server *>(gate("serverOut", i)->getPathEndGate()->getOwnerModule());
            int workload = serverModule->getQueueLength();
            EV_INFO << "SERVER[" <<i << "]" << "has " << workload << "length of queue" << endl;
            maxWorkload = std::max(maxWorkload, workload);
            sumWorkload += workload;
        }

        numServedCustomers = 0;
        for (int i = 0; i < numServers; i++)
        {
            Server *serverModule = check_and_cast<Server *>(gate("serverOut", i)->getPathEndGate()->getOwnerModule());
            int servedCustomer = serverModule->getServedCustomer();
            numServedCustomers += servedCustomer;
        }

        EV_INFO << "numServedCustomers:   " << numServedCustomers << endl;
        if (numServedCustomers >= maxCustomers)
        {
            endSimulation();
        }
    }
    else
    {
        // Incoming customer
        msg->setTimestamp(); // Set the timestamp for calculating the system time later
        numGeneratedCustomer++;

        std::string balancingMethod = par("balancingMethod");

        if (balancingMethod == "RoundRobinLoadBalancer")
        {
            // Round Robin
            send(msg, "serverOut", lastServer);
            lastServer++;
            lastServer = lastServer % numServers;
        }
        else
        {
            // Select selectionSize servers randomly
            std::vector<int> selectedServers;
            for (int i = 0; i < selectionSize; i++)
            {
                int randomIndex = intuniform(0, numServers - 1);
                while (std::find(selectedServers.begin(), selectedServers.end(), randomIndex) != selectedServers.end())
                {
                    randomIndex = intuniform(0, numServers - 1);
                }
                selectedServers.push_back(randomIndex);
            }

            // Find the server with the shortest queue
            int minQueueIndex = selectedServers[0];
            Server *serverModule = check_and_cast<Server *>(gate("serverOut", minQueueIndex)->getPathEndGate()->getOwnerModule());
            int minQueueLength = serverModule->getQueueLength();

            for (int i = 1; i < selectionSize; i++)
            {
                int serverIndex = selectedServers[i];
                serverModule = check_and_cast<Server *>(gate("serverOut", serverIndex)->getPathEndGate()->getOwnerModule());
                int queueLength = serverModule->getQueueLength();
                if (queueLength < minQueueLength)
                {
                    minQueueIndex = serverIndex;
                    minQueueLength = queueLength;
                }
            }

            // Send the customer to the server with the shortest queue
            send(msg, "serverOut", minQueueIndex);
        }
    }
}


void LoadBalancer::finish()
{
    for (int i=0; i < numServers; i++)
    {
        Server *serverModule = check_and_cast<Server *>(gate("serverOut", i)->getPathEndGate()->getOwnerModule());
        simtime_t totalBusyTime = serverModule->getTotalBusyTime();
        totalServiceTime += totalBusyTime;
    }

    double avgSystemTimePerServer = totalSystemTime.dbl() / numServers;
    double avgWorkload = avgWorkload / numServers;

    double lambda = numGeneratedCustomer / simTime();
    double rho = totalServiceTime.dbl() / simTime().dbl() / 10;
    double mu = lambda / (numServers * rho);
    double Lq = sumWorkload / (double) (numGeneratedCustomer * numServers);

    EV << "Total time spent for simulation:     " << simTime() << endl;
    EV << "numServedCustomers:                  " << numServedCustomers << endl;
    EV << "numGeneratedCustomer:                " << numGeneratedCustomer << endl;
    EV << "totalSystemTime:                     " << totalSystemTime << endl;
    EV << endl;
    EV << "Arrival rate(lambda):                " << lambda << endl;
    EV << "Service utilization(rho):            " << rho << endl;
    EV << "Service rate(mu):                    " << mu << endl;
    EV << endl;
    EV << "Maximum workload:                    " << maxWorkload << endl;
    EV << "Average queue length(Lq):            " << Lq << endl;
    EV << "Average system time:                 " << totalSystemTime.dbl() / numGeneratedCustomer << endl;
    EV << endl;
}
