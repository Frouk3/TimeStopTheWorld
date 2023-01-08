#pragma once

namespace TimeStop
{
	inline unsigned int key = 84;
	inline bool Enabled = false;
	inline int Upgradeable = 2;
	inline float LastTimeActive = 0.0f;
	inline bool StartCooldown = false;
	inline float LastTimeStopTicks = 0.0f;
	inline bool needToPlaySound = true;
	inline bool playTickSounds = true;

	void Update() noexcept;
	void LoadConfig() noexcept;
	void SaveConfig() noexcept;
}