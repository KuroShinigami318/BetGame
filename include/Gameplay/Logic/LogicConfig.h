#pragma once

struct LogicConfig
{
	enum class Difficulty
	{
		Hard = 1,
		Medium,
		Easy
	};
	float normBidHitRate = 1.f;
	float defaultHitRate = 0.5f;
	float hitRateStep = 0.01f;
	uint32_t defaultBid = 1000;
	uint16_t cardCount = 0;
	Difficulty difficulty = Difficulty::Hard;
};