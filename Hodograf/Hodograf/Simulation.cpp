#include "Simulation.h"

void Simulation::Init()
{
	time = 0;
	simulationSpeed = 1;
	delta_time = 0.001;

	omega = 0.1;
	alpha = 0;
	R = 5;
	L = 10;
	e0 = 0.01;

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

	x.push_back(R + L);
	xt.push_back(0);
	xtt.push_back(0);

	x.push_back(R + L);
	xt.push_back(0);
	xtt.push_back(0);
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
	x.push_back(cosf(alpha) * R + sqrtf(disturbed_L * disturbed_L - sa * sa * R * R));
	xt.push_back((x[x.size() - 1] - x[x.size() - 2]) / (2 * delta_time));
	xtt.push_back((xt[xt.size() - 1] - xt[xt.size() - 2]) / (2 * delta_time));
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