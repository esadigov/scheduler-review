#include <iostream>
#include <thread>
#include "SimScheluder.hpp"
#include "SimInvalidObjectName.hpp"

void immediateEventHandler() 
{
    std::cout << "Hello from immediate event handler" << std::endl;

    return;
}

void SimulationTimeEventHandler1() 
{
    std::cout << "Event 1 executed" << std::endl;

    return;
}

void SimulationTimeEventHandler2() 
{
    std::cout << "Event 2 executed" << std::endl;

    return;
}

int main(void)
{
    Scheduler::SimScheduler* scheduler = nullptr;
    Azstsim::Kernel::TimeKeeper* timekeeper = new Azstsim::Kernel::TimeKeeper(
        "Timekepper"
        ,"Default"
    );

    try{
        scheduler = new Scheduler::SimScheduler("FirstScheduler", timekeeper);

        std::cout << "Scheduler constructed, Jolly good to go..." << std::endl;
    } catch (SimExceptions::SimInvalidObjectName& ex) {
        std::cout << ex.GetMessage() << std::endl;
    }

    SimpleEntryPoint* entry1 = new SimpleEntryPoint("Entry1", "lorem ipsum1", nullptr, immediateEventHandler);
    scheduler->AddImmediateEvent(entry1);
    
    SimpleEntryPoint* entry2 = new SimpleEntryPoint("Entry2", "lorem ipsum2", nullptr, SimulationTimeEventHandler1);
    scheduler->AddSimulationTimeEvent(
        entry2
        ,(timekeeper->GetSimulationTime())
        ,5000
        ,2
    );

    SimpleEntryPoint* entry3 = new SimpleEntryPoint("Entry3", "lorem ipsum3", nullptr, SimulationTimeEventHandler2);
    scheduler->AddSimulationTimeEvent(
        entry3
        ,(timekeeper->GetSimulationTime())
        ,3000
        ,4
    );

    for(int i=0;i<10;i++){
        scheduler->Step(scheduler->GetNextScheduledEventTime());
    }

    
    return 0;
}