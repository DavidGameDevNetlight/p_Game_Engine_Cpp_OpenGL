#pragma once
#include <string>
#include <type_traits>
#include <concepts>
#include "GameActor.h"
using std::string;
using std::derived_from;
//using std::enable_if;
//using std::is_base_of; //https://www.geeksforgeeks.org/cpp/stdis_base_of-template-in-c-with-examples/

namespace eng {

    constexpr size_t INITIALIZER_LIST_SIZE = 8;

    class Level {
    public:
        void UpdateGameActors(float deltaTime);
        void Clear();

        // Factory method for creating GameActors
        GameActor* CreateGameActor(const string& name, GameActor* parent = nullptr);
        bool SetParent(GameActor* object, GameActor* parent);

        // Template Factory
        template<typename T>
        requires derived_from<T, GameActor>
        T* CreateGameActor(const std::string& name, GameActor* parent = nullptr) {

            if (m_gameActorsCount >= m_gameActorsListCapacity)
            {
                //TODO Trigger resizing, move operator.
            }

            auto newActor = new T();
            newActor->SetName(name);
            SetParent(newActor, parent);
            m_listOfGameActors[m_gameActorsCount] = newActor;
            m_gameActorsCount++;
            return newActor;
        }

    private:
        size_t          m_gameActorsCount                           = 0;
        size_t          m_gameActorsListCapacity                    = INITIALIZER_LIST_SIZE;
        GameActor*      m_listOfGameActors[INITIALIZER_LIST_SIZE]   = {nullptr};
    };

}
