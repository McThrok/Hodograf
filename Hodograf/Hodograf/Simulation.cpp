#include "Simulation.h"

void Simulation::Init()
{
	time = 0;
	simulationSpeed = 1;
	delta_time = 0.03;

	omega = 3.14;
	alpha = 0;
	R = 3;
	L = 10;
	e0 = 0.00;

	minX = { 0,5 };
	maxX = { 50,15 };

	minXt = { 0,-1 };
	maxXt = { 50,1 };

	minXtt = { 0,-1 };
	maxXtt = { 50,1 };

	minState = { 5,-1 };
	maxState = { 15,1 };

	Reset();
	paused = false;
}

void Simulation::Reset()
{
	disturbed_L = max(L, R);
	alpha = 0;

	x.clear();
	xt.clear();
	xtt.clear();
	state.clear();

	x.push_back(ImVec2(0, R + L));
	x.push_back(ImVec2(delta_time, R + L));
}

void Simulation::Update(float dt)
{
	//return;
	if (paused)
		return;

	time += dt / 1000;
	float timePerStep = delta_time / simulationSpeed;

	while (time >= timePerStep)
	{
		Update();
		time -= timePerStep;
	}
}

void Simulation::Update()
{
	DisturbL();
	alpha += omega * delta_time;
	while (alpha > 2 * XM_PI) alpha -= 2 * XM_PI;

	float sa = sinf(alpha);
	float time = x[x.size() - 1].x + delta_time;

	x.push_back(ImVec2(time, cosf(alpha) * R + sqrtf(disturbed_L * disturbed_L - sa * sa * R * R)));

	if (x.size() >= 9)
	{
		int c = x.size();
		xt.push_back(ImVec2(
			x[c - 5].x,
			x[c - 9].y / 280
			- 4 * x[c - 8].y / 105
			+ x[c - 7].y / 5
			- 4 * x[c - 6].y / 5
			+ 4 * x[c - 4].y / 5
			- x[c - 3].y / 5
			+ 4 * x[c - 2].y / 105
			- x[c - 1].y / 280
		));

		xtt.push_back(ImVec2(
			x[c - 5].x,
			-x[c - 9].y / 560
			+ 8 * x[c - 8].y / 325
			- x[c - 7].y / 5
			+ 8 * x[c - 6].y / 5
			- 205 * x[c - 5].y / 72
			+ 8 * x[c - 4].y / 5
			- x[c - 3].y / 5
			+ 8 * x[c - 2].y / 325
			- x[c - 1].y / 560
		));

		state.push_back(ImVec2(x[c - 5].y, xt[xt.size() - 1].y));
	}

}

void Simulation::DisturbL()
{
	if (e0 > 0)
	{
		auto nd = normal_distribution<float>(0, e0);
		disturbed_L = L + nd(gen);
	}
	else
	{
		disturbed_L = L;
	}

	disturbed_L = max(disturbed_L, R);
}