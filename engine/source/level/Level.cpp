#include "Level.h"

namespace eng
{
    void Level::UpdateGameActors(float deltaTime)
    {
        for (size_t i = 0; i < m_gameActorsCount; i++)
        {
            if (m_listOfGameActors[i] != nullptr)
                m_listOfGameActors[i]->Update(deltaTime);
        }
    }

    void Level::Clear()
    {
        for (size_t i = 0; i < m_gameActorsCount; i++)
        {
            delete m_listOfGameActors[i]; // Deleting the heap-allocated memory at each position.
            m_listOfGameActors[i] = nullptr; // nulling to prevent a crash if Clear is called again, preventing "dangling pointers"
        }
    }

    // Factory method for creating GameActors
    GameActor* Level::CreateGameActor(const string& name, GameActor* parent)
    {
        if (m_gameActorsCount >= m_gameActorsListCapacity)
        {
            //TODO Trigger resizing, move operator.
        }
        auto newActor = new GameActor();
        newActor->SetName(name);
        SetParent(newActor, parent);
        m_listOfGameActors[m_gameActorsCount] = newActor;
        m_gameActorsCount++;
        return newActor;
    }

    //TODO
    bool Level::SetParent(GameActor* object, GameActor* parent)
    {
        // 24. 3.2 Set parent implementation, https://www.udemy.com/course/game-engine-development-with-c-and-opengl/learn/lecture/53054419#overview
        bool result = false;
        auto currentParent = object->GetParent();
        ///////////////////////////////////////////
        /// Case 1: Setting GameActor as Root
        ///////////////////////////////////////////
        if (parent == nullptr)
        {
            if (currentParent != nullptr)
            {
                result = true;
            }
            /*
             * 1. Obj in scene root
             * 2. Obj just created
             */
        }
        else
        {
            // Locate Obj in current parent list
            // Avoid circular dependency
            result = true;
        }
        return result;
    }

}