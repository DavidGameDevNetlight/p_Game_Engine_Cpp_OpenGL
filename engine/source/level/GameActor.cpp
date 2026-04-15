#include "GameActor.h"

#include <iostream>
#include <ostream>

#include "glm/gtx/matrix_transform_2d.hpp"

namespace eng
{
    void GameActor::Update(const float deltaTime)
    {
        /*
         * Note:
         * The original uses vector and iterator 23. 3.1 Scene and GameObject
         * As of now, the parent will iterate all the array
         */
        for (size_t i = 0; i < m_childCount; i++)
        {
            if (m_children[i] != nullptr)
            {
                if (m_children[i]->IsActive())
                {
                    m_children[i]->Update(deltaTime);
                }
            }
        }
    }

    const string&  GameActor::GetName() const
    {
        return m_name;
    }

    void GameActor::SetName(const string& name)
    {
        m_name = name;
        std::cout<< "My name is:" << m_name << std::endl;
    }

    bool GameActor::IsActive() const
    {
        return m_isAlive;
    }

    GameActor* GameActor::GetParent()
    {
        return m_parent;
    }

    void GameActor::MarkForDestroy() {
        m_isAlive = false;
    }

    /////////////////////////////////
    /// Transform
    /////////////////////////////////

    Transform& GameActor::GetTransform()
    {
        return m_transform;
    }

    mat4 GameActor::GetLocalTransformMatrix() const {
        mat4 m; // Assume identity initialization

        // Shorthand for readability
        const float x = m_transform.rotation.x;
        const float y = m_transform.rotation.y;
        const float z = m_transform.rotation.z;
        const float w = m_transform.rotation.w;

        const float sx = m_transform.scale.x;
        const float sy = m_transform.scale.y;
        const float sz = m_transform.scale.z;

        // Column 0 (Right) * Scale X
        m[0][0] = (1.0f - 2.0f * (y * y + z * z)) * sx;
        m[0][1] = (2.0f * (x * y + z * w)) * sx;
        m[0][2] = (2.0f * (x * z - y * w)) * sx;
        m[0][3] = 0.0f;

        // Column 1 (Up) * Scale Y
        m[1][0] = (2.0f * (x * y - z * w)) * sy;
        m[1][1] = (1.0f - 2.0f * (x * x + z * z)) * sy;
        m[1][2] = (2.0f * (y * z + x * w)) * sy;
        m[1][3] = 0.0f;

        // Column 2 (Forward) * Scale Z
        m[2][0] = (2.0f * (x * z + y * w)) * sz;
        m[2][1] = (2.0f * (y * z - x * w)) * sz;
        m[2][2] = (1.0f - 2.0f * (x * x + y * y)) * sz;
        m[2][3] = 0.0f;

        // Column 3 (Translation)
        m[3][0] = m_transform.translation.x;
        m[3][1] = m_transform.translation.y;
        m[3][2] = m_transform.translation.z;
        m[3][3] = 1.0f;

        return m;
    }

    mat4 GameActor::GetWorldTransformMatrix() const
    {
        // TODO consider parent transformation.
        return GetLocalTransformMatrix();
    }
}
