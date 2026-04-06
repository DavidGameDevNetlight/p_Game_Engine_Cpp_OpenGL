#pragma once
#include <eng.h>
#include <iostream>

using namespace eng;

class Game : public Application
{
public:
	/// <summary>
	/// The `Init()` separates "object exists" from "object ready for use"
	/// </summary>
	/// <returns></returns>
	bool Init() 					override;
	void Update(float deltaTime) 	override;
	void Destroy() 					override;

private:
	Material	m_material;
	
	// Always initialize pointers to nullptr. If a process fails early, this member contains garbage.
	Mesh*		m_mesh			= nullptr;		// original std::unique_ptr<eng::Mesh>
};