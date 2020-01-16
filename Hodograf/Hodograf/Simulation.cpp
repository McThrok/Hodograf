#include "Simulation.h"

void Simulation::Init()
{
	time = 0;
	simulationSpeed = 1;
	delta_time = 0.01;

	omega = 0.1;
	alpha = 0;
	R = 3;
	L = 10;
	e0 = 0.01;

	minX = { 0,-5 };
	maxX = { 100,5 };

	minXt = { 0,-5 };
	maxXt = { 100,5 };

	minXtt = { 0,-5 };
	maxXtt = { 100,5 };

	minState = { -5,-5 };
	maxState = { 5,5 };

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
	xt.push_back(ImVec2(0, 0));
	xtt.push_back(ImVec2(0, 0));
	state.push_back(ImVec2(x[0].y, xt[0].y));

	x.push_back(ImVec2(delta_time, R + L));
	xt.push_back(ImVec2(delta_time, 0));
	xtt.push_back(ImVec2(delta_time, 0));
	state.push_back(ImVec2(x[1].y, xt[1].y));
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
	int count = x.size();
	float time = x[count - 1].x + delta_time;

	x.push_back(ImVec2(time,cosf(alpha) * R + sqrtf(disturbed_L * disturbed_L - sa * sa * R * R)));
	xt.push_back(ImVec2(time,(x[count - 1].y - x[count - 2].y) / (2 * delta_time) ));
	xtt.push_back(ImVec2(time,(xt[count - 1].y - xt[count - 2].y) / (2 * delta_time)));
	state.push_back(ImVec2(x[count - 1].y, xt[count - 1].y));
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