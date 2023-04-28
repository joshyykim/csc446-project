network LoadBalancingNetwork
{
    parameters:
        int numServers; // Number of servers (m)
        int selectionSize; // Number of servers to select randomly (d)
        double meanArrivalRate; // Customer arrival rate (lambda)
        double serviceRate; // Server service rate (mu)
        string balancingMethod;

    submodules:
        loadBalancer: LoadBalancer {
            numServers = parent.numServers;
            selectionSize = parent.selectionSize;
            balancingMethod = parent.balancingMethod;
        }
        server[numServers]: Server {
            serviceRate = parent.serviceRate;
        }
        customerGenerator: CustomerGenerator {
            meanArrivalRate = parent.meanArrivalRate;
            @display("p=32,34");
        }

    connections allowunconnected:
        // Connect the load balancer to the servers
        for i=0..numServers-1 {
            loadBalancer.serverOut[i] --> server[i].serverIn;
            server[i].serverOut --> loadBalancer.serverIn[i];
        }

        // Connect the customer generator to the load balancer
        customerGenerator.customerOut --> loadBalancer.customerIn;
}

simple LoadBalancer
{
    parameters:
    int numServers; // Number of servers (m)
    int maxCustomers = default(1000);
    int selectionSize; // Number of servers to select randomly (d)
    string balancingMethod = default("RandMin"); // Load balancing method (RandMin, PureRand, or RR)
    
    gates:
        input customerIn;
        input serverIn[numServers];
        output serverOut[numServers];
}

simple Server
{
    parameters:
        double serviceRate; // Server service rate (mu)
        int queueLength = 0 @editable;
        int servedCustomer = 0 @editable;
    
    gates:
        input serverIn;
        output serverOut;
}

simple CustomerGenerator
{
    parameters:
        double meanArrivalRate; // Customer arrival rate (lambda)
    
    gates:
        output customerOut;
}
