#pragma once
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>
using namespace glm;

namespace eng {

    struct PerspectiveParams
    {
        int		width        = 0;
        int		height       = 0;
        float	fov          = 0.f;
        float	nearPlane    = 0.f;
        float	farPlane     = 0.f;
        float   aspectRatio  = 0.f;
    };

    class Camera
    {
    public:
        Camera() = default;
        ~Camera() = default;
        //Camera(const PerspectiveParams& perspectiveParams);

        void InitializeCamera(const vec3& worldUp, const PerspectiveParams& perspectiveParams);
        void InitializeCamera(const vec3& worldUp);

        void Update(float deltaTime);
        void SetCameraId(const int id);
        const int GetId() const;

        vec3& Position();
        vec3& Direction();

        const vec3& Right();
        const vec3& Up();
        const mat4& Rotation();
        const mat4& ProjectionMatrix();
        const mat4& ViewMatrix();

    private:
        PerspectiveParams	m_perspectiveParams; // This needs to be deep-copied

        vec3	m_cameraPosition;
        vec3	m_cameraDirection;
        vec3	m_cameraRight;
        vec3	m_cameraUp;

        mat3	m_cameraBaseVectorWorldSpace;
        mat4	m_cameraRotation;

        mat4	m_cameraProjectionMatrix;
        mat4	m_cameraViewMatrix;

        int     m_id = -1;
        char    m_code = '0';
    };

    class ActiveCamera
    {
    public:
        ActiveCamera() = default;
        ~ActiveCamera() = default;

        ActiveCamera(const ActiveCamera&) = delete;
        ActiveCamera& operator = (const ActiveCamera&) = delete;
        ActiveCamera(const ActiveCamera&&) = delete;
        ActiveCamera& operator = (const ActiveCamera&&) = delete;

        void Update(float deltaTime);

        void SetActiveCamera(Camera& activeCamera);
        Camera* GetActiveCamera();


    private:
        Camera* m_currentCamera = nullptr;
    };
}