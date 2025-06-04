#include "statboard_gores.h"

#include <engine/shared/config.h>
#include <game/client/gameclient.h>
#include <game/client/prediction/entities/character.h>

CStatboardGores *g_pStatboardGores = nullptr;

CStatboardGores::CStatboardGores()
{
    g_pStatboardGores = this;
    OnReset();
}

void CStatboardGores::OnReset()
{
    m_SpawnTick = -1;
    m_TotalSurvivalTicks = 0;
    m_LongestSurvivalTicks = 0;
    m_FreezeCount = 0;
    m_UnfreezeCount = 0;
    m_LastHookedPlayer = -1;
    m_LastHookTick = -1;
    m_WasFrozen = false;
    for(int i = 0; i < MAX_CLIENTS; i++)
        m_aWasFrozen[i] = false;
}

void CStatboardGores::OnHammerUnfreeze()
{
    m_UnfreezeCount++;
}

float CStatboardGores::AverageSurvivalTime() const
{
    int Lives = m_LongestSurvivalTicks ? m_TotalSurvivalTicks / m_LongestSurvivalTicks : 0;
    if(m_SpawnTick != -1)
        Lives++;
    if(Lives == 0)
        return 0.0f;
    return (float)m_TotalSurvivalTicks / Lives / Client()->GameTickSpeed();
}

float CStatboardGores::LongestSurvivalTime() const
{
    return (float)m_LongestSurvivalTicks / Client()->GameTickSpeed();
}

void CStatboardGores::OnNewSnapshot()
{
    int Local = m_pClient->m_Snap.m_LocalClientId;
    if(Local < 0)
        return;

    int CurrentTick = Client()->GameTick(g_Config.m_ClDummy);

    bool Alive = m_pClient->m_Snap.m_aCharacters[Local].m_Active;
    if(Alive)
    {
        if(m_SpawnTick < 0)
            m_SpawnTick = CurrentTick;
    }
    else if(m_SpawnTick >= 0)
    {
        int Life = CurrentTick - m_SpawnTick;
        m_TotalSurvivalTicks += Life;
        if(Life > m_LongestSurvivalTicks)
            m_LongestSurvivalTicks = Life;
        m_SpawnTick = -1;
    }

    CClientData &LocalData = m_pClient->m_aClients[Local];
    bool Frozen = LocalData.m_FreezeEnd > CurrentTick || LocalData.m_DeepFrozen || LocalData.m_LiveFrozen;
    if(Frozen && !m_WasFrozen)
        m_FreezeCount++;
    m_WasFrozen = Frozen;

    int Hooked = LocalData.m_Predicted.HookedPlayer();
    if(Hooked != -1)
    {
        m_LastHookedPlayer = Hooked;
        m_LastHookTick = CurrentTick;
    }

    for(int i = 0; i < MAX_CLIENTS; i++)
    {
        bool FrozenI = m_pClient->m_aClients[i].m_FreezeEnd > CurrentTick || m_pClient->m_aClients[i].m_DeepFrozen || m_pClient->m_aClients[i].m_LiveFrozen;
        if(!FrozenI && m_aWasFrozen[i])
        {
            if(i == m_LastHookedPlayer && CurrentTick - m_LastHookTick <= Client()->GameTickSpeed())
            {
                m_UnfreezeCount++;
                m_LastHookedPlayer = -1;
            }
        }
        m_aWasFrozen[i] = FrozenI;
    }
}

