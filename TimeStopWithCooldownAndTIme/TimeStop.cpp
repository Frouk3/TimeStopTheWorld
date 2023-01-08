#include "TimeStop.h"
#include <cSlowRateManager.h>
#include <shared.h>
#include <common.h>
#include <GameMenuStatus.h>
#include "IniReader.h"
#include <Windows.h>
#include <Pl0000.h>
#include <cGameUIManager.h>
#include <ARGB32.h>
#include <cmath>

void TimeStop::Update() noexcept
{
	DWORD base = shared::base;
	GameMenuStatus GameMenuState = (GameMenuStatus)g_GameMenuStatus;
	bool IsForegroundWindow = *(bool*)(base + 0x19D509C);
	Pl0000* player = (Pl0000*)g_cGameUIManager.m_pPl;
	static cSlowRateManager* SlowRateManager = &g_cSlowRateManager;
	if (IsForegroundWindow && GameMenuState == InGame)
	{
		if (!SlowRateManager)
			return;

		static bool once = false;
		float ticks = SlowRateManager->m_fTicks;
		if (shared::IsKeyPressed(key))
		{
			if (ticks - LastTimeActive > ((Upgradeable * 1000.0f) / 5.0f) * SlowRateManager->m_fTickRate) // abt cooldown is bit of too much
			{
				LastTimeActive = ticks;
				StartCooldown = false;
			}
			if (StartCooldown)
				return;

			if (player->m_nCurrentAction == 69 || player->m_nCurrentAction == 70) // zangeki, inter zangeki mode
				return;

			Enabled = SlowRateManager->GetSlowRate(0) == 1.0f;
			if (shared::random(1, 40) == 1 && Enabled)
			{
				Upgradeable += shared::random(1, 5);
				Core_PlaySound("core_se_btl_battery_blue", 1);
			}
			once = false;
			if (!Enabled)
			{
				SlowRateManager->SetSlowRate(0, 1.0f);
				SlowRateManager->SetSlowRate(1, 1.0f);
				SlowRateManager->SetSlowRate(2, 1.0f);

				*(unsigned int*)(base + 0x17EA074) &= ~0x8000;
			}
			if (Enabled)
			{
				*(unsigned int*)(base + 0x17EA074) |= 0x8000;

				if (needToPlaySound) Core_PlaySound("core_se_btl_slow_in", 1);

				LastTimeStopTicks = ticks;
			}
			else
			{
				StartCooldown = true;

				if (needToPlaySound) Core_PlaySound("core_se_btl_slow_out", 1);
			}
			SaveConfig();
		}
		if (Enabled && ticks - LastTimeStopTicks > (Upgradeable * 1000.0f) * SlowRateManager->m_fTickRate)
		{
			StartCooldown = true;
			Enabled = false;
			LastTimeStopTicks = ticks;

			if (needToPlaySound) Core_PlaySound("core_se_btl_slow_out", 1);
		}
		if (Enabled)
		{
			SlowRateManager->SetSlowRate(0, 1.0f * 3.33564095e-9f);
			SlowRateManager->SetSlowRate(1, 1.0f / 3.33564095e-9f);
			SlowRateManager->SetSlowRate(2, 1.0f * 3.33564095e-9f);
			*(unsigned int*)(base + 0x17EA074) |= 0x8000;
		}
		if (!Enabled && !once)
		{
			SlowRateManager->SetSlowRate(0, 1.0f);
			SlowRateManager->SetSlowRate(1, 1.0f);
			SlowRateManager->SetSlowRate(2, 1.0f);
			StartCooldown = true;
			LastTimeActive = ticks;
			once = true;
			*(unsigned int*)(base + 0x17EA074) &= ~0x8000;
		}

		static float lastTime = 0.0f;
		if (Enabled && ticks - LastTimeStopTicks > (Upgradeable * (1000.0f * SlowRateManager->m_fTickRate)) - 4000.0f * SlowRateManager->m_fTickRate)
		{
			if (ticks - lastTime > 1000.0f * SlowRateManager->m_fTickRate)
			{
				if (playTickSounds) Core_PlaySound("core_se_sys_title_start", 1);

				lastTime = ticks;
			}
		}
	}
	if (Enabled && GameMenuState == InMenu)
	{
		Enabled = false;
		SlowRateManager->ResetSlowRate();
	}

	if (!Enabled)
		Sleep(20);
}

void TimeStop::LoadConfig() noexcept
{
	CIniReader iniReader("TimeStop.ini");

	Upgradeable = iniReader.ReadInteger("TimeStop", "upgrade", 2);
	key = iniReader.ReadInteger("TimeStop", "key", 84);
	needToPlaySound = iniReader.ReadInteger("TimeStop", "playSound", 1) == 1;
	playTickSounds = iniReader.ReadInteger("TimeStop", "playTickSound", 1) == 1;
}

void TimeStop::SaveConfig() noexcept
{
	CIniReader iniReader("TimeStop.ini");

	iniReader.WriteInteger("TimeStop", "upgrade", Upgradeable);
	iniReader.WriteInteger("TimeStop", "key", key);
	iniReader.WriteInteger("TimeStop", "playSound", needToPlaySound);
	iniReader.WriteInteger("TimeStop", "playTickSound", playTickSounds);
}