#include "TestGameActor.h"

TestGameActor::TestGameActor()
{
    std::cout << "I've been default-initialized" << std::endl;
}

TestGameActor::~TestGameActor()
{
    std::cout << "I've been destroyed" << std::endl;
}

void TestGameActor::Update(float deltaTime)
{
    GameActor::Update(deltaTime);
}