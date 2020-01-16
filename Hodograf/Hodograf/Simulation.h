#pragma once
#include <d3d11.h>
#include <vector>
#include <math.h> 
#include <DirectXMath.h>
#include <SimpleMath.h>
#include "Graphics/Vertex.h"
#include <random>

using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;

class Simulation
{
public:

	float delta_time;
	float time;
	bool paused;

	float simulationSpeed;

	vector<float> x;
	vector<float> xt;
	vector<float> xtt;

	float omega, L, R, alpha, disturbed_L,e0;

	mt19937 gen{ std::random_device{}() };

	void DisturbL();

	void Init();
	void Reset();
	void Update(float dt);
	void Update();
};

