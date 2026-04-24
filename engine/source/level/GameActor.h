#pragma once
#include <string>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

using std::string;
using glm::vec3;
using glm::vec4;
using glm::mat4;
using glm::quat;

namespace eng
{

    constexpr size_t INITIALIZER_CHILD_COUNT = 8;

    struct Transform
    {
        quat rotation       {1.0f, 0.0f, 0.0f, 0.0f};
        vec3 translation    {0};
        //vec3 rotation       {0};
        vec3 scale          {1};
    };

    class GameActor
    {
    public:
        virtual             ~GameActor() = default;
        virtual void        Update(float);

        const   string&     GetName() const;
                void        MarkForDestroy();
                void        SetName(const string& name);
                bool        IsActive() const;

                GameActor*  GetParent();
        /////////////////////////////////
        /// Transform
        /////////////////////////////////
        [[nodiscard]] Transform& GetTransform(); // The nodiscard makes the compiler alert that this result was not used
        [[nodiscard]] mat4 GetLocalTransformMatrix() const;
        [[nodiscard]] mat4 GetWorldTransformMatrix() const;

    protected:
        // Preventing Direct instantiation
        GameActor() = default;

    private:
        /*
         * This private members remain inaccessible by the derived classes.
         */
        string      m_name      = "Unnamed";
        GameActor*  m_parent    = nullptr;
        bool        m_isAlive   = true;

        /////////////////////////////////
        /// Children
        /////////////////////////////////
        size_t      m_childCount                        = INITIALIZER_CHILD_COUNT;
        GameActor*  m_children[INITIALIZER_CHILD_COUNT] = {nullptr}; //TODO - array initialization cheatsheet

        /////////////////////////////////
        /// Transform
        /////////////////////////////////
        Transform   m_transform;

        friend class Level;
    };
}
