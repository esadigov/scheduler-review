#include "SimScheluder.hpp"
#include <iostream>

static uint32_t id_counter;

Scheduler::SimScheduler::SimScheduler(Smp::String8 ObjectName, Azstsim::Kernel::TimeKeeper* TimeKeeper) 
    : SimService(ObjectName)
    , TimeKeeper_(TimeKeeper)
{
    
}

void Scheduler::SimScheduler::Step(Smp::Duration simulationTime)
{
    // std::cout << "In Step" << std::endl;
    // for(auto i : SimulationTimeEventQueue_) {
    //     std::cout << "key: " << i.first << " event id: " << i.second.Id_ << std::endl;
    // }

    if(!ImmediateEventQueue_.empty()) {
        for(auto it = ImmediateEventQueue_.begin(); it != ImmediateEventQueue_.end();)
        {
            currentExecutingEvent_ = &(*it); // get raw pointer
            currentExecutingEvent_->EntryPoint_->Execute();
            currentExecutingEvent_ = nullptr;

            it = ImmediateEventQueue_.erase(it);
        }
    }

    auto it = SimulationTimeEventQueue_.find(simulationTime);

    if(it == SimulationTimeEventQueue_.end()) { // no event in queue
        return;
    }

    currentExecutingEvent_ = &(it->second);

    currentExecutingEvent_->EntryPoint_->Execute();

    currentExecutingEvent_ = nullptr;

    if(it->second.repeat_ < 0) { // cyclic event
        Scheduler::SimEvent event(std::move(it->second));
        SimulationTimeEventQueue_.erase(it);
        SimulationTimeEventQueue_.emplace(
            (TimeKeeper_->GetSimulationTime() + event.cycleTime_)
            ,std::move(event)
        );

        return;
    }

    it->second.repeat_--;

    if(it->second.repeat_ < 0){
        SimulationTimeEventQueue_.erase(it);

        return;
    }

    Scheduler::SimEvent event(std::move(it->second));
    SimulationTimeEventQueue_.erase(it);
    SimulationTimeEventQueue_.emplace(
        (TimeKeeper_->GetSimulationTime() + event.cycleTime_)
        ,std::move(event)
    );

    return;
}

/// Add an immediate event to the scheduler.
/// An immediate event is an event that will be added as a
/// simulation time event (with simulation delta time of 0) at the
/// end of the list of immediate events; the list of immediate
/// events is processed in order of insertion and before any
/// non-immediate event. After adding the new Event to the
/// scheduler, it returns the EventId identifying the added Event
/// As an immediate event, it will be executed when the scheduler
/// processes its simulation time events again, but not immediately
/// in the call to AddImmediateEvent(). When the simulator is in
/// Standby state, simulation time does not progress, and
/// simulation time events (including immediate events) are not
/// processed.
/// @remarks To execute an entry point immediately without going
/// through the scheduler, its Execute() method can be called.
/// @remarks For events scheduled at the same time (including
/// immediate events), the Scheduler policy is to always execute
/// those first that have been posted first ("First scheduled,
/// first executed").
/// @param   entryPoint Entry point to call from event.
/// @return  Event identifier that can be used to change or remove
/// event.
Smp::Services::EventId Scheduler::SimScheduler::AddImmediateEvent(
    const Smp::IEntryPoint* entryPoint)
{
    Smp::Services::EventId id = id_counter++;
    ImmediateEventQueue_.emplace_back(entryPoint, 0, 0, id);

    return id;
}

/// Add event to scheduler that is called based on simulation time.
/// If the provided Simulation Time is more than MAX_INT – ‘current
/// simulation time’ , it throws an InvalidEventTime exception, the
/// Event is not added to the scheduler and never executed. If the
/// provided Simulation Time is -1, the event is created but not
/// posted (i.e. not on the timeline). It will be posted only when
/// SetEventSimulationTime is used to define a valid execution
/// time. If the provided Simulation Time is a negative value less
/// than -1, it throws an InvalidEventTime exception, the Event is
/// not added to the scheduler and never executed. If Repeat is not
/// zero and CycleTime is a value less than -1 or is 0, it throws
/// an InvalidCycleTime exception, the Event is not added to the
/// scheduler and never executed; If the provided Simulation Time
/// is -1, the event is created but not posted on the timeline; If
/// after the event execution the Cycle Time is -1 and repeat count
/// is not 0, then the event is not re-posted on the timeline ; An
/// event not posted on the timeline will be posted only until
/// SetEventSimulationTime is used to define a valid execution
/// time; After adding the new Event to the scheduler, it returns
/// the EventId identifying the added Event.
/// @remarks For events scheduled at the same time (including
/// immediate events), the Scheduler policy is to always execute
/// those first that have been posted first ("First scheduled,
/// first executed").
/// @param   entryPoint Entry point to call from event.
/// @param   simulationTime Duration from now when to trigger the
/// event for the first time.
/// This must not be a negative simulation time, as this would be
/// in the past.
/// @param   cycleTime Duration between two triggers of the event.
/// For non-cyclic Events, the cycle time parameter is stored, but
/// not used;
/// For cyclic Events, an InvalidCycleTime exception is thrown if
/// the cycle time is is a value less than -1 or is 0.
/// @param   repeat Number of times the event shall be repeated, or
/// 0 for a single event, or -1 for no limit.
/// An Event with repeat=0 is non-cyclic and executes only once;
/// An Event with repeat=0 is removed automatically after its
/// triggering; 
/// An Event with repeat>0 is cyclic, and repeats ‘repeat’ times;
/// An Event with repeat>0 is removed automatically after it has
/// been triggered ‘repeat+1’ times;
/// An Event with repeat<0 is cyclic forever;
/// An Event with repeat<0 is never removed from the scheduler
/// unless explicitly requested using the RemoveEvent() method.
/// @return  Event identifier that can be used to change or remove
/// event.
/// @throws  Smp::Services::InvalidCycleTime
/// @throws  Smp::Services::InvalidEventTime
Smp::Services::EventId Scheduler::SimScheduler::AddSimulationTimeEvent(
    const Smp::IEntryPoint* entryPoint,
    Smp::Duration simulationTime,
    Smp::Duration cycleTime,
    Smp::Int64 repeat)
{
    if((simulationTime > MAX_INT - TimeKeeper_->GetSimulationTime())
        || (simulationTime < -1)) 
        {
        throw SimExceptions::SimInvalidEventTime(this);
    } 
    else if ((repeat != 0)
        && ((cycleTime < -1) || (cycleTime == 0))) 
        {
        throw SimExceptions::SimInvalidCycleTime(this);
    }
    else if (simulationTime == -1) // will be posted only when SetEventSimulationTime 
        {                          // is used to define a valid execution time 
        // ???
    }

    Smp::Services::EventId id = id_counter++;
    SimulationTimeEventQueue_.emplace(
        Smp::Duration(simulationTime + TimeKeeper_->GetSimulationTime())
        ,SimEvent(entryPoint, cycleTime, repeat, id)
    );

    return id;
}

/// Add event to scheduler that is called based on mission time.
/// If the Mission Time is less than the current mission time, it
/// throws an InvalidEventTime exception, the Event is not added to
/// the scheduler and never executed; If Repeat is not zero and
/// CycleTime is a value less than 1, it throws an InvalidCycleTime
/// exception, the Event is not added to the scheduler and never
/// executed; After adding the new Event to the scheduler, it
/// returns the EventId identifying the added Event.
/// @remarks For events scheduled at the same time (including
/// immediate events), the Scheduler policy is to always execute
/// those first that have been posted first ("First scheduled,
/// first executed").
/// @param   entryPoint Entry point to call from event.
/// @param   missionTime Absolute mission time when to trigger the
/// event for the first time.
/// This must not be a mission time in the past.
/// @param   cycleTime Duration between two triggers of the event.
/// For non-cyclic Events, the cycle time parameter is stored, but
/// not used;
/// For cyclic Events, an InvalidCycleTime exception is thrown if
/// the cycle time is a value less than 1.
/// @param   repeat Number of times the event shall be repeated, or
/// 0 for a single event, or -1 for no limit.
/// An Event with repeat=0 is non-cyclic and executes only once; 
/// An Event with repeat=0 is removed automatically after its
/// triggering; 
/// An Event with repeat>0 is cyclic, and repeats ‘repeat’ times;
/// An Event with repeat>0 is removed automatically after it has
/// been triggered ‘repeat+1’ times;
/// An Event with repeat<0 is cyclic forever;
/// An Event with repeat<0 is never removed from the scheduler
/// unless explicitly requested using the RemoveEvent() method.
/// @return  Event identifier that can be used to change or remove
/// event.
/// @throws  Smp::Services::InvalidCycleTime
/// @throws  Smp::Services::InvalidEventTime
Smp::Services::EventId Scheduler::SimScheduler::AddMissionTimeEvent(
    const Smp::IEntryPoint* entryPoint,
    Smp::Duration missionTime,
    Smp::Duration cycleTime,
    Smp::Int64 repeat) 
{
    if(missionTime < TimeKeeper_->GetMissionTime()) {
        throw SimExceptions::SimInvalidEventTime(this);
    }
    else if((repeat > 0) && (cycleTime < 1)) {
        throw SimExceptions::SimInvalidCycleTime(this);
    }
    
    Smp::Duration NextExecutionTime 
        = (missionTime - TimeKeeper_->GetMissionTime() + TimeKeeper_->GetSimulationTime());

    Smp::Services::EventId id = id_counter++;
    SimulationTimeEventQueue_.emplace(
        std::make_pair(Smp::Duration(NextExecutionTime)
        ,SimEvent(entryPoint, cycleTime, repeat, id)) 
    );
    

    return id;
}

/// Add event to scheduler that is called based on epoch time.
/// If the Epoch Time is less than the current epoch time it throws
/// an InvalidEventTime exception, the Event is not added to the
/// scheduler and never executed; If Repeat is not zero and
/// CycleTime is a value less than 1, it throws an InvalidCycleTime
/// exception, the Event is not added to the scheduler and never
/// executed; After adding the new Event to the scheduler, it
/// returns the EventId identifying the added Event.
/// @remarks For events scheduled at the same time (including
/// immediate events), the Scheduler policy is to always execute
/// those first that have been posted first ("First scheduled,
/// first executed").
/// @param   entryPoint Entry point to call from event.
/// @param   epochTime Epoch time when to trigger the event for the
/// first time.
/// This must not be an epoch time in the past.
/// @param   cycleTime Duration between two triggers of the event.
/// For non-cyclic Events, the cycle time parameter is stored, but
/// not used;
/// For cyclic Events, an InvalidCycleTime exception is thrown if
/// the cycle time is a value less than 1.
/// @param   repeat Number of times the event shall be repeated, or
/// 0 for a single event, or -1 for no limit.
/// An Event with repeat=0 is non-cyclic and executes only once; 
/// An Event with repeat=0 is removed automatically after its
/// triggering; 
/// An Event with repeat>0 is cyclic, and repeats ‘repeat’ times;
/// An Event with repeat>0 is removed automatically after it has
/// been triggered ‘repeat+1’ times;
/// An Event with repeat<0 is cyclic forever;
/// An Event with repeat<0 is never removed from the scheduler
/// unless explicitly requested using the RemoveEvent() method.
/// @return  Event identifier that can be used to change or remove
/// event.
/// @throws  Smp::Services::InvalidCycleTime
/// @throws  Smp::Services::InvalidEventTime
Smp::Services::EventId Scheduler::SimScheduler::AddEpochTimeEvent(
    const Smp::IEntryPoint* entryPoint,
    Smp::DateTime epochTime,
    Smp::Duration cycleTime,
    Smp::Int64 repeat)
{
    if(epochTime < TimeKeeper_->GetEpochTime()) {
        throw SimExceptions::SimInvalidEventTime(this);
    }
    else if((repeat != 0) && (cycleTime < 1)) {
        throw SimExceptions::SimInvalidCycleTime(this);
    }

    Smp::Duration NextExecutionTime 
        = (epochTime - TimeKeeper_->GetEpochTime() + TimeKeeper_->GetSimulationTime());

    Smp::Services::EventId id = id_counter++;
    SimulationTimeEventQueue_.emplace(
        std::make_pair(Smp::Duration(NextExecutionTime)
        ,SimEvent(entryPoint, cycleTime, repeat, id)) 
    );


    return id;
}

/// Add event to scheduler that is called based on Zulu time.
/// If the given Zulu Time is less than the current Zulu time, it
/// throws an InvalidEventTime exception, the Event is not added to
/// the scheduler and never executed; If Repeat is not zero and
/// CycleTime is a value less than 1, it throws an InvalidCycleTime
/// exception, the Event is not added to the scheduler and never
/// executed; After adding the new Event to the scheduler, it
/// returns the EventId identifying the added Event.
/// @remarks For events scheduled at the same time (including
/// immediate events), the Scheduler policy is to always execute
/// those first that have been posted first ("First scheduled,
/// first executed").
/// @param   entryPoint Entry point to call from event.
/// @param   zuluTime Absolute (Zulu) time when to trigger the
/// event for the first time.
/// This must not be a time in the past.
/// @param   cycleTime Duration between two triggers of the event.
/// For non-cyclic Events, the cycle time parameter is stored, but
/// not used;
/// For cyclic Events, an InvalidCycleTime exception is thrown if
/// the cycle time is a value less than 1.
/// @param   repeat Number of times the event shall be repeated, or
/// 0 for a single event, or -1 for no limit.
/// An Event with repeat=0 is non-cyclic and executes only once; 
/// An Event with repeat=0 is removed automatically after its
/// triggering; 
/// An Event with repeat>0 is cyclic, and repeats ‘repeat’ times;
/// An Event with repeat>0 is removed automatically after it has
/// been triggered ‘repeat+1’ times;
/// An Event with repeat<0 is cyclic forever;
/// An Event with repeat<0 is never removed from the scheduler
/// unless explicitly requested using the RemoveEvent() method.
/// @return  Event identifier that can be used to change or remove
/// event.
/// @throws  Smp::Services::InvalidCycleTime
/// @throws  Smp::Services::InvalidEventTime
Smp::Services::EventId Scheduler::SimScheduler::AddZuluTimeEvent(
    const Smp::IEntryPoint* entryPoint,
    Smp::DateTime zuluTime,
    Smp::Duration cycleTime,
    Smp::Int64 repeat)
{

}

/// Add event to scheduler that is called based on Zulu time.
/// An event with repeat=0 is not cyclic. It will be removed
/// automatically after is has been triggered. An event with
/// repeat>0 is cyclic, and will be repeated repeat times.
/// Therefore, it will be called repeat+1 times, and then it will
/// be removed automatically. An event with repeat=-1 is cyclic as
/// well, but it will be triggered forever, unless it is removed
/// from the scheduler using the RemoveEvent() method. For a cyclic
/// event, the cycleTime needs to be positive. Otherwise, an
/// InvalidCycleTime exception is thrown. For non-cyclic events, it
/// is stored but not used. The zuluTimedelay must not be negative.
/// Otherwise, an InvalidEventTime exception is thrown.
/// @remarks For events scheduled at the same time (including
/// immediate events), the Scheduler policy is to always execute
/// those first that have been posted first ("First scheduled,
/// first executed").
/// @param   entryPoint Entry point to call from event.
/// @param   zuluTimeDelay Relative (Zulu) time when to trigger the
/// event for the first time.
/// This must not be a negative duration.
/// @param   cycleTime Duration between two triggers of the event.
/// When repeat is not zero, this must be a positive duration.
/// @param   repeat Number of times the event shall be repeated, or
/// 0 for a single event, or -1 for no limit.
/// @return  Event identifier that can be used to change or remove
/// event.
/// @throws  Smp::Services::InvalidCycleTime
/// @throws  Smp::Services::InvalidEventTime
Smp::Services::EventId Scheduler::SimScheduler::AddRelativeZuluTimeEvent(
    const Smp::IEntryPoint* entryPoint,
    Smp::Duration zuluTimeDelay,
    Smp::Duration cycleTime,
    Smp::Int64 repeat) 
{

}

/// Update when an existing simulation time event on the scheduler
/// shall be triggered.
/// The simulationTime must not be negative. Otherwise, the event
/// will never be executed, but immediately removed.
/// When the given event Id is not a valid identifier of a
/// scheduler event, the method throws an exception of type
/// InvalidEventId. In case an event is registered under the given
/// event Id but it is not a simulation time event, the method
/// throws an exception of type InvalidEventId as well.
/// @param   event Identifier of event to modify.
/// @param   simulationTime Duration from now when to trigger event.
/// If the simulation time is negative, the event will never be
/// executed but instead be removed immediately from the scheduler.
/// @throws  Smp::Services::InvalidEventId
void Scheduler::SimScheduler::SetEventSimulationTime(
    Smp::Services::EventId event,
    Smp::Duration simulationTime) 
{
    // In my implementation all events are converted to simulation time events
    // So this function will never throw an exception because of different time kind events
    // See 5.3.3l-2(a)/(b)

    if(simulationTime < -1) {
        for(auto it = SimulationTimeEventQueue_.begin(); it != SimulationTimeEventQueue_.end(); it++) {
            if(it->second.Id_ == event) {
                SimulationTimeEventQueue_.erase(it);

                return;
            }
        }
    }

    for(auto it = SimulationTimeEventQueue_.begin(); it != SimulationTimeEventQueue_.end(); it++) {
        if(it->second.Id_ == event) {
            
            if(simulationTime == -1) {
                PendingEventQueue_.emplace_back(it->second);
                SimulationTimeEventQueue_.erase(it);

                return;
            }
            
            std::multimap<Smp::Duration, Scheduler::SimEvent>::node_type node = SimulationTimeEventQueue_.extract(it);
            node.key() = simulationTime;
            SimulationTimeEventQueue_.insert(std::move(node));
            
            return;
        }
    }

    for(auto it = PendingEventQueue_.begin(); it != PendingEventQueue_.end(); it++) {
        if(it->Id_ == event) {
            SimulationTimeEventQueue_.emplace(simulationTime, std::move(*it));
            PendingEventQueue_.erase(it);

            return;
        }
    }


    throw SimExceptions::SimInvalidEventId(event, this);
}

/// Update when an existing mission time event on the scheduler
/// shall be triggered.
/// The missionTime must not be before the current mission time of
/// the ITimeKeeper service. Otherwise, the event will never be
/// executed, but immediately removed.
/// When the given event Id is not a valid identifier of a
/// scheduler event, the method throws an exception of type
/// InvalidEventId. In case an event is registered under the given
/// event Id but it is not a mission time event, the method throws
/// an exception of type InvalidEventId as well.
/// @param   event Identifier of event to modify.
/// @param   missionTime Absolute mission time when to trigger
/// event.
/// If the mission time is before the current mission time, the
/// event will never be executed but instead be removed immediately
/// from the scheduler.
/// @throws  Smp::Services::InvalidEventId
void Scheduler::SimScheduler::SetEventMissionTime(
    Smp::Services::EventId event,
    Smp::Duration missionTime) 
{

}

/// Update when an existing epoch time event on the scheduler (an
/// event that has been registered using AddEpochTimeEvent()) shall
/// be triggered.
/// The epochTime must not be before the current epoch time of the
/// ITimeKeeper service. Otherwise, the event will never be
/// executed, but immediately removed.
/// When the given event Id is not a valid identifier of a
/// scheduler event, the method throws an exception of type
/// InvalidEventId. In case an event is registered under the given
/// event Id but it is not an epoch time event, the method throws
/// an exception of type InvalidEventId as well.
/// @param   event Identifier of event to modify.
/// @param   epochTime Epoch time when to trigger event.
/// If the epoch time is before the current epoch time, the event
/// will never be executed but instead be removed immediately from
/// the scheduler.
/// @throws  Smp::Services::InvalidEventId
void Scheduler::SimScheduler::SetEventEpochTime(
    Smp::Services::EventId event,
    Smp::DateTime epochTime) 
{

}

/// Update when an existing zulu time event on the scheduler shall
/// be triggered.
/// The zuluTime must not be before the current Zulu time of the
/// ITimeKeeper service. Otherwise, the event will never be
/// executed, but immediately removed.
/// When the given event Id is not a valid identifier of a
/// scheduler event, the method throws an exception of type
/// InvalidEventId. In case an event is registered under the given
/// event Id but it is not a zulu time event, the method throws an
/// exception of type InvalidEventId as well.
/// @param   event Identifier of event to modify.
/// @param   zuluTime Absolute (Zulu) time when to trigger event.
/// If the zulu time is before the current zulu time, the event
/// will never be executed but instead be removed immediately from
/// the scheduler.
/// @throws  Smp::Services::InvalidEventId
void Scheduler::SimScheduler::SetEventZuluTime(
    Smp::Services::EventId event,
    Smp::DateTime zuluTime) 
{

}

/// Update cycle time of an existing event on the scheduler.
/// For a cyclic event, the cycleTime needs to be positive. For
/// non-cyclic events, it is stored but not used.
/// When the given event is not a valid identifier of a scheduler
/// event, the method throws an exception of type InvalidEventId.
/// @param   event Identifier of event to modify.
/// @param   cycleTime Duration between two triggers of the event.
/// For a cyclic event, this needs to be a positive duration.
/// Otherwise, an exception of type InvalidCycleTime is thrown.
/// @throws  Smp::Services::InvalidEventId
/// @throws  Smp::Services::InvalidCycleTime
void Scheduler::SimScheduler::SetEventCycleTime(
    Smp::Services::EventId event,
    Smp::Duration cycleTime) 
{
    // TODO: search in pending events
    if(PendingEventQueue_.empty()) {
        goto SearchInSimulationEvents_;
    }

    for(auto it = PendingEventQueue_.begin(); it != PendingEventQueue_.end(); it++) {
        if(it->Id_ == event) {
            if((it->repeat_ != 0) && ((it->cycleTime_ == 0) || (it->cycleTime_ < -1))) {
                throw SimExceptions::SimInvalidCycleTime(this);
            }
            it->cycleTime_ = cycleTime;

            return;
        }
    }

    SearchInSimulationEvents_:
    for(auto it = SimulationTimeEventQueue_.begin(); it != SimulationTimeEventQueue_.end(); it++) {
        if(it->second.Id_ == event) {            
            if((it->second.repeat_ != 0) && ((it->second.cycleTime_ == 0) || (it->second.cycleTime_ < -1))) {
                throw SimExceptions::SimInvalidCycleTime(this);
            }
            it->second.cycleTime_ = cycleTime;

            return;
        }
    }

    throw SimExceptions::SimInvalidEventId(event, this);
}

/// Update the repeat counter of an existing event on the scheduler.
/// An event with repeat=0 is not cyclic. It will be removed
/// automatically after is has been triggered. An event with
/// repeat>0 is cyclic, and will be repeated 'repeat' times.
/// Therefore, it will be called repeat+1 times, and then it will
/// be removed automatically. An event with repeat=-1 is cyclic as
/// well, but it will be triggered forever, unless it is removed
/// from the scheduler using the RemoveEvent() method. For a cyclic
/// event, the cycleTime needs to be positive. For non-cyclic
/// events, it is ignored.
/// When the given event is not a valid identifier of a scheduler
/// event, the method throws an exception of type InvalidEventId.
/// @param   event Identifier of event to modify.
/// @param   repeat Number of times the event shall be repeated, or
/// 0 for a single event, or -1 for no limit.
/// An event with a repeat different from 0 is called cyclic. For
/// such an event, a positive cycle time has to be defined before.
/// Otherwise, an exception of type InvalidCycleTime is thrown.
/// @throws  Smp::Services::InvalidEventId
void Scheduler::SimScheduler::SetEventRepeat(
    Smp::Services::EventId event,
    Smp::Int64 repeat) 
{
    // TODO: immediate event repeat queue problem
    // if(ImmediateEventQueue_.empty()) {
    //     goto SearchInSimulationEvents_;
    // }

    // TODO: search in pending events

    // SearchInSimulationEvents_:
    for(auto it = SimulationTimeEventQueue_.begin(); it != SimulationTimeEventQueue_.end(); it++) {
        if(it->second.Id_ == event) {
            if((repeat != 0) && ((it->second.cycleTime_ == 0) || (it->second.cycleTime_ < -1))) {
                throw SimExceptions::SimInvalidCycleTime(this);
            }
            it->second.repeat_ = repeat;

            return;
        }
    }

    // throw InvalidEventId
}

/// Remove an event from the scheduler.
/// An event with repeat=0 is removed automatically after it has
/// been triggered.
/// When the given event is not a valid identifier of a scheduler
/// event, the method throws an exception of type InvalidEventId.
/// @param   event Event identifier of the event to remove.
/// @throws  Smp::Services::InvalidEventId
void Scheduler::SimScheduler::RemoveEvent(
    Smp::Services::EventId event) 
{   
    if(GetCurrentEventId() == event) {
        return;
    }

    if(ImmediateEventQueue_.empty()) {
        goto SearchInPendingEvents;
    }

    for(auto it = ImmediateEventQueue_.begin(); it != ImmediateEventQueue_.end(); it++) {
        if(it->Id_ == event) {       
            ImmediateEventQueue_.erase(it);
            return;
        }
    }

    SearchInPendingEvents:
    if(PendingEventQueue_.empty()) {
        goto SearchInSimulationEvents_;
    }

    for(auto it = PendingEventQueue_.begin(); it != PendingEventQueue_.end(); it++) {
        if(it->Id_ == event) {
            PendingEventQueue_.erase(it);
            return;
        }
    }

    SearchInSimulationEvents_:
    for(auto it = SimulationTimeEventQueue_.begin(); it != SimulationTimeEventQueue_.end(); it++) {
        if(it->second.Id_ == event) {
            if(GetCurrentEventId() == event) {
                SetEventRepeat(event, 0);
                return;
            }
            else {
                SimulationTimeEventQueue_.erase(it);
                return;
            } 
        }
    }

    throw SimExceptions::SimInvalidEventId(event, this);
}

/// Return the ID of the event currently executed by the scheduler.
/// If not event is executed, this function returns -1.
/// The operation returns -1 when no event is being executed.
/// @return  Event Id or -1 if no event is being executed.
Smp::Services::EventId Scheduler::SimScheduler::GetCurrentEventId() const 
{
    if(SimulationTimeEventQueue_.empty()) {
        return -1;
    }

    if(currentExecutingEvent_ != nullptr) {
        return currentExecutingEvent_->Id_;
    }

    return -1;
}

/// Get the time of next scheduled Event.
/// This function can only provide the time of the next event at
/// the time of invocation. There is no guarantee that no events
/// will be inserted between the method invocation and the
/// (currently) next event. Events scheduled in Zulu Time are not
/// considered, as these Events do not have a fixed defined
/// Simulation Time.
/// @return  Time of the next event on the scheduler.
Smp::Duration Scheduler::SimScheduler::GetNextScheduledEventTime() const 
{
    /*
    NOTE 1 Events scheduled in Zulu Time are not
        considered, as these Events do not have a fixed
        defined Simulation Time.
    NOTE 2 In case of Zulu Events executed, other Events
        may schedule Events prior to the time returned,
        hence the Scheduler does not guarantee that no
        other Events may be executed prior to the time
        returned from GetNextScheduledEventTime().
    */
    if(SimulationTimeEventQueue_.empty()) {
        return -1;
    }

    if(currentExecutingEvent_ == nullptr) {
        return SimulationTimeEventQueue_.begin()->first;
    }

    else if(SimulationTimeEventQueue_.size() == 1) {
        if(SimulationTimeEventQueue_.begin()->second.repeat_ == 0) {
            // return what?
        }
        return SimulationTimeEventQueue_.begin()->first;
    }
    
    return SimulationTimeEventQueue_.begin()->first;
}

/// Returns true if the corresponding event has been scheduled
/// but its start time does not yet elapse; or if it is not yet removed
/// by RemoveEvent; or if it is currently executing; or if it has not
/// yet finished its repeat count;
/// false otherwise.
/// @param   eventId Event identifier to check for.
/// @return  True if the event with the given eventId is scheduled
/// (i.e. will be called again), false otherwise.
Smp::Bool Scheduler::SimScheduler::IsEventScheduled(
    Smp::Services::EventId eventId) const 
{
    if(ImmediateEventQueue_.empty()) {
        goto SearchInSimulationEvents_;
    }

    for(auto it = ImmediateEventQueue_.begin(); it != ImmediateEventQueue_.end(); it++) {
        if(it->Id_ == eventId) {       
            return Smp::Bool(true);
        }
    }

    SearchInSimulationEvents_:
    for(auto it = SimulationTimeEventQueue_.begin(); it != SimulationTimeEventQueue_.end(); it++) {
        if(it->second.Id_ == eventId) {
            return Smp::Bool(true);
        }
    }

    return Smp::Bool(false);
}