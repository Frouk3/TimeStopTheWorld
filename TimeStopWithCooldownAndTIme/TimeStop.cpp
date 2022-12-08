#include "TimeStop.h"
#include "game/cSlowRateManager.h"
#include "game/shared.h"
#include "game/GameMenuStatus.h"
#include "IniReader.h"
#include <Windows.h>

void TimeStop::Update() noexcept
{
	DWORD base = (DWORD)GetModuleHandleA(NULL);
	GameMenuStatus GameMenuState = *(GameMenuStatus*)(base + 0x17E9F9C);
	bool IsForegroundWindow = *(bool*)(base + 0x19D509C);
	DWORD player = *(DWORD*)(base + 0x19C1490);
	if (IsForegroundWindow && GameMenuState == InGame)
	{
		auto SlowRateManager = GetcSlowRateManager();
		static bool once = false;
		float ticks = SlowRateManager->m_fTicks;
		if (GetAsyncKeyState(key) & 1)
		{
			if (ticks - LastTimeActive > (Upgradeable * 1000.0f) / 2.0f) // abt cooldown is bit of too much
			{
				LastTimeActive = ticks;
				StartCooldown = false;
			}
			if (StartCooldown)
				return;

			short plAction = *(short*)(player + 0x618);
			if (plAction == 69 || plAction == 70) // zangeki, inter zangeki mode
				return;

			Enabled = SlowRateManager->GetSlowRate(0) == 1.0f;
			if (rand() % 25 == 1)
			{
				Upgradeable += rand() % 5;
				shared::Core_PlaySound("core_se_btl_battery_blue", 1);
			}
			once = false;
			if (!Enabled)
			{
				SlowRateManager->SetSlowRate(0, 1.0f);
				SlowRateManager->SetSlowRate(1, 1.0f);
				SlowRateManager->SetSlowRate(2, 1.0f);
			}
			if (Enabled)
				shared::Core_PlaySound("core_se_btl_slow_in", 1);
			else
			{
				StartCooldown = true;
				shared::Core_PlaySound("core_se_btl_slow_out", 1);
			}
			if (Enabled)
				LastTimeStopTicks = ticks;
			SaveConfig();
		}
		if (Enabled && ticks - LastTimeStopTicks > Upgradeable * 1000.0f)
		{
			StartCooldown = true;
			Enabled = false;
			LastTimeStopTicks = ticks;
			shared::Core_PlaySound("core_se_btl_slow_out", 1);
		}
		if (Enabled)
		{
			SlowRateManager->SetSlowRate(0, 1.0f * 3.33564095e-9f);
			SlowRateManager->SetSlowRate(1, 1.0f / 3.33564095e-9f);
			SlowRateManager->SetSlowRate(2, 1.0f * 3.33564095e-9f);
		}
		if (!Enabled && !once)
		{
			SlowRateManager->SetSlowRate(0, 1.0f);
			SlowRateManager->SetSlowRate(1, 1.0f);
			SlowRateManager->SetSlowRate(2, 1.0f);
			StartCooldown = true;
			LastTimeActive = ticks;
			once = true;
		}
	}
}

void TimeStop::LoadConfig() noexcept
{
	CIniReader iniReader("TimeStop.ini");

	Upgradeable = iniReader.ReadInteger("TimeStop", "upgrade", 2);
	key = iniReader.ReadInteger("TimeStop", "key", 84);
}

void TimeStop::SaveConfig() noexcept
{
	CIniReader iniReader("TimeStop.ini");

	iniReader.WriteInteger("TimeStop", "upgrade", Upgradeable);
	iniReader.WriteInteger("TimeStop", "key", key);
}