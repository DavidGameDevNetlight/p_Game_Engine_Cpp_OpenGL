#pragma once
#include <eng.h>
#include "level/GameActor.h"

using namespace eng;

class TestGameActor : public GameActor
{
public:
    TestGameActor();
    ~TestGameActor() override;
    void Update(float) override;

};