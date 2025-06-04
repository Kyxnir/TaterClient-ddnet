#ifndef GAME_CLIENT_COMPONENTS_STATBOARD_GORES_H
#define GAME_CLIENT_COMPONENTS_STATBOARD_GORES_H

#include <game/client/component.h>

class CStatboardGores : public CComponent
{
private:
    int m_SpawnTick;
    int m_TotalSurvivalTicks;
    int m_LongestSurvivalTicks;
    int m_FreezeCount;
    int m_UnfreezeCount;
    int m_LastHookedPlayer;
    int m_LastHookTick;
    bool m_WasFrozen;
    bool m_aWasFrozen[MAX_CLIENTS];

public:
    CStatboardGores();
    virtual int Sizeof() const override { return sizeof(*this); }
    virtual void OnReset() override;
    virtual void OnNewSnapshot() override;
    void OnHammerUnfreeze();
    float AverageSurvivalTime() const;
    float LongestSurvivalTime() const;
    int FreezeCount() const { return m_FreezeCount; }
    int UnfreezeCount() const { return m_UnfreezeCount; }
};

extern CStatboardGores *g_pStatboardGores;

#endif // GAME_CLIENT_COMPONENTS_STATBOARD_GORES_H
