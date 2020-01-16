#pragma once
#include <d3d11.h>
#include <vector>
#include <math.h> 
#include <DirectXMath.h>
#include <SimpleMath.h>
#include "Graphics/Vertex.h"
#include <random>
#include "Graphics/MyImGui.h"

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

	vector<ImVec2> x;
	vector<ImVec2> xt;
	vector<ImVec2> xtt;
	vector<ImVec2> state;

	ImVec2 minX, maxX;
	ImVec2 minXt, maxXt;
	ImVec2 minXtt, maxXtt;
	ImVec2 minState, maxState;

	float omega, L, R, alpha, disturbed_L, e0;

	mt19937 gen{ std::random_device{}() };

	void DisturbL();

	void Init();
	void Reset();
	void Update(float dt);
	void Update();
};

