#include "Camera.h"

#include <iostream>
#include <ostream>

#include "Engine.h"

namespace eng {
    /*Camera::Camera(const PerspectiveParams& perspectiveParams)
    {
        m_perspectiveParams.width = perspectiveParams.width;
        m_perspectiveParams.height = perspectiveParams.height;
        m_perspectiveParams.fov = perspectiveParams.fov;
        m_perspectiveParams.nearPlane = perspectiveParams.nearPlane;
        m_perspectiveParams.farPlane = perspectiveParams.farPlane;
        m_perspectiveParams.aspectRatio = perspectiveParams.aspectRatio;
        std::cout << "fov:" << m_perspectiveParams.fov << '\n';
    }*/

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Camera
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Camera::InitializeCamera(const vec3& worldUp) {


        const Window windowInPixels = Engine::GetInstance().GetWindow();

        m_perspectiveParams.width        = windowInPixels.pixelWidth;
        m_perspectiveParams.height       = windowInPixels.pixelHeight;
        m_perspectiveParams.fov          = 45.0f;
        m_perspectiveParams.nearPlane    = 0.1f;
        m_perspectiveParams.farPlane     = 300.f;
        m_perspectiveParams.aspectRatio  = static_cast<float>(windowInPixels.pixelWidth) / static_cast<float>(windowInPixels.pixelHeight);

        m_id = 0;
        m_cameraPosition 	= vec3(10.0f, 10.0f, 10.0f);
        m_cameraDirection	= normalize(vec3(-1.0f, -1.0f, -1.0f));
        m_cameraRight	    = normalize(cross(m_cameraDirection, worldUp));
        m_cameraUp		    = normalize(cross(m_cameraRight, m_cameraDirection));

        m_cameraBaseVectorWorldSpace    = mat3(m_cameraRight, m_cameraUp, -m_cameraDirection);
        m_cameraRotation			 = transpose(m_cameraBaseVectorWorldSpace);

        m_code = 'x';
    }

    void Camera::InitializeCamera(const vec3& worldUp, const PerspectiveParams& perspectiveParams)
    {
        m_perspectiveParams.width = perspectiveParams.width;
        m_perspectiveParams.height = perspectiveParams.height;
        m_perspectiveParams.fov = perspectiveParams.fov;
        m_perspectiveParams.nearPlane = perspectiveParams.nearPlane;
        m_perspectiveParams.farPlane = perspectiveParams.farPlane;
        m_perspectiveParams.aspectRatio = perspectiveParams.aspectRatio;
        std::cout << "fov:" << m_perspectiveParams.fov << '\n';

        m_id = 0;
        m_cameraPosition 	= vec3(20.0f, 20.0f, 20.0f);
        m_cameraDirection	= normalize(vec3(-1.0f, -1.0f, -1.0f));
        m_cameraRight	    = normalize(cross(m_cameraDirection, worldUp));
        m_cameraUp		    = normalize(cross(m_cameraRight, m_cameraDirection));

        m_cameraBaseVectorWorldSpace    = mat3(m_cameraRight, m_cameraUp, -m_cameraDirection);
        m_cameraRotation			 = transpose(m_cameraBaseVectorWorldSpace);

        m_code = 'x';
        m_cameraType = ECameraType::Dynamic;
    }

    void Camera::SetCameraId(const int id) { m_id = id; }

    int Camera::GetId() const { return m_id; }
    ECameraType Camera::GetActiveCameraType() const { return m_cameraType; }

    vec3& Camera::Position() { return m_cameraPosition; }
    vec3& Camera::Direction() { return m_cameraDirection; }

    const vec3& Camera::Right() { return m_cameraRight; }
    const vec3& Camera::Up() { return m_cameraUp;}
    const mat4& Camera::Rotation() { return m_cameraRotation; }
    const mat4& Camera::ProjectionMatrix() { return m_cameraProjectionMatrix; }
    const mat4& Camera::ViewMatrix() { return m_cameraViewMatrix; }

    void Camera::Rotate(const vec2& deltaMovement)
    {
        m_cameraDirection += vec3(deltaMovement.x, -deltaMovement.y, 0.0f);
        m_cameraDirection = normalize(m_cameraDirection);
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Active Camera
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ActiveCamera::SetActiveCamera(Camera& activeCamera)
    {
        m_currentCamera = &activeCamera;
        std::cout << "ActiveCamera Set " << m_currentCamera->GetId() << std::endl;
        std::cout << m_currentCamera->Position().x << " camera.Pos.X\n";
        std::cout << m_currentCamera->Position().y << " camera.Pos.Y\n";
        std::cout << m_currentCamera->Position().z << " camera.Pos.Z\n";

    }

    Camera* ActiveCamera::GetActiveCamera()
    {
        return m_currentCamera;
    }

    void ActiveCamera::UpdateViewDirection(const vec2& deltaMovement)
    {
        if (m_currentCamera->GetActiveCameraType() == ECameraType::Static)
            return;

        m_currentCamera->Rotate(deltaMovement);
    }

    void ActiveCamera::Update(float deltaTime) {
        m_currentCamera->Update(deltaTime);
    }

    void Camera::Update(float deltaTime)
    {
        vec3 worldUp = Engine::GetInstance().GetWorld().worldUp;

        m_cameraRight	    = normalize(cross(m_cameraDirection, worldUp));
        m_cameraUp		    = normalize(cross(m_cameraRight, m_cameraDirection));

        m_cameraBaseVectorWorldSpace    = mat3(m_cameraRight, m_cameraUp, -m_cameraDirection);
        m_cameraRotation			 = transpose(m_cameraBaseVectorWorldSpace);

        m_cameraBaseVectorWorldSpace    = mat3(m_cameraRight, m_cameraUp, -m_cameraDirection);
        m_cameraRotation			 = transpose(m_cameraBaseVectorWorldSpace);

        m_cameraViewMatrix				= m_cameraRotation * translate(-m_cameraPosition);
        m_cameraProjectionMatrix        =  perspective(radians(m_perspectiveParams.fov),
                                                            m_perspectiveParams.aspectRatio,
                                                            m_perspectiveParams.nearPlane,
                                                            m_perspectiveParams.farPlane);
    }

    void Camera::ResizeAspectRatio(const int newWidth, const int newHeight)
    {
        m_perspectiveParams.width       = newWidth;
        m_perspectiveParams.height      = newHeight;
        m_perspectiveParams.aspectRatio = static_cast<float>(newWidth) / static_cast<float>(newHeight);
    }

} // eng