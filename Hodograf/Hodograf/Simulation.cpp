#include "Simulation.h"

void Simulation::Init()
{
	time = 0;
	simulationSpeed = 1;
	delta_time = 0.03;

	omega = 3.14/2;
	alpha = 0;
	R = 3;
	L = 10;
	e0 = 0.00;

	minX = { 0,5 };
	maxX = { 50,15 };

	minXt = { 0,-10 };
	maxXt = { 50,10 };

	minXtt = { 0,-30 };
	maxXtt = { 50,30 };

	minState = { 5,-10 };
	maxState = { 15,10 };

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

	int d = diffOffset;
	if (x.size() >= 9 * d)
	{
		int c = x.size();
		xt.push_back(ImVec2(
			x[c - 5 * d].x,
			(
				x[c - 9 * d].y / 280
				- 4 * x[c - 8 * d].y / 105
				+ x[c - 7 * d].y / 5
				- 4 * x[c - 6 * d].y / 5
				+ 4 * x[c - 4 * d].y / 5
				- x[c - 3 * d].y / 5
				+ 4 * x[c - 2 * d].y / 105
				- x[c - 1 * d].y / 280
				) / (delta_time * d)
		));

		xtt.push_back(ImVec2(
			x[c - 5 * d].x,
			(
				-x[c - 9 * d].y / 560
				+ 8 * x[c - 8 * d].y / 315
				- x[c - 7 * d].y / 5
				+ 8 * x[c - 6 * d].y / 5
				- 205 * x[c - 5 * d].y / 72
				+ 8 * x[c - 4 * d].y / 5
				- x[c - 3 * d].y / 5
				+ 8 * x[c - 2 * d].y / 315
				- x[c - 1 * d].y / 560
				) /( delta_time * d * delta_time * d)
		));

		state.push_back(ImVec2(x[c - 5 * d].y, xt[xt.size() - 1].y));
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