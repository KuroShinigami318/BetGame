#pragma once

struct LogicConfig
{
	float normBidHitRate = 1.f;
	float defaultHitRate = 0.5f;
	float hitRateStep = 0.01f;
	uint32_t defaultBid = 1000;
};