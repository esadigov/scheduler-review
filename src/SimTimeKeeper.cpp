#include "SimTimeKeeper.hpp"
#include "Smp/ISimulator.h"

#include <stdexcept>

namespace Azstsim::Kernel {

using namespace Smp;
using namespace Smp::Services;

void TimeKeeper::Connect(ISimulator* simulator)
{
    if (m_State != Smp::ComponentStateKind::CSK_Configured)
        throw std::runtime_error("Connect called in wrong state");

    m_State     = Smp::ComponentStateKind::CSK_Connected;
    m_EventMgr  = simulator ? simulator->GetEventManager() : nullptr;
    m_Scheduler = simulator ? simulator->GetScheduler()     : nullptr;

    if (m_EventMgr) {
        m_PreSimEp = std::make_unique<SimpleEntryPoint>(
            std::string(GetName()) + ".PreSim", "Pre Simulation Time Change",
            this, [this]() { onPreSimTimeChange_(); });

        m_PostSimEp = std::make_unique<SimpleEntryPoint>(
            std::string(GetName()) + ".PostSim", "Post Simulation Time Change",
            this, [this]() { onPostSimTimeChange_(); });

        m_EventMgr->Subscribe(IEventManager::SMP_PreSimTimeChangeId,  m_PreSimEp.get());
        m_EventMgr->Subscribe(IEventManager::SMP_PostSimTimeChangeId, m_PostSimEp.get());
    }
}

void TimeKeeper::SetSimulationTime(Duration simulationTime)
{
    if (!m_InPreSimTimeChange)
        return;

    if (simulationTime < m_SimTime)
        throw std::runtime_error("ASmp::InvalidSimulationTimeException()");

    if (m_Scheduler) {
        Duration next = m_Scheduler->GetNextScheduledEventTime();
        if (simulationTime > next)
            throw std::runtime_error("ASmp::InvalidSimulationTimeException()");
    }

    m_SimTime = simulationTime;
}

void TimeKeeper::SetEpochTime(DateTime epochTime)
{
    m_EpochBase = epochTime - m_SimTime;
    if (m_EventMgr)
        m_EventMgr->Emit(IEventManager::SMP_EpochTimeChangedId, true);
}

void TimeKeeper::SetMissionStartTime(DateTime missionStart)
{
    m_MissionStart = missionStart;
    if (m_EventMgr)
        m_EventMgr->Emit(IEventManager::SMP_MissionTimeChangedId, true);
}

void TimeKeeper::SetMissionTime(Duration missionTime)
{
    m_MissionStart = GetEpochTime() - missionTime;
    if (m_EventMgr)
        m_EventMgr->Emit(IEventManager::SMP_MissionTimeChangedId, true);
}

} // namespace Azstsim::Kernel
