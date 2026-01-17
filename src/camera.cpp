#include "Camera.h"

using namespace linalg;



void Camera::MoveTo(const vec3f& position) noexcept
{
    m_position = position;
}

void Camera::Move(const vec3f& direction) noexcept
{
    m_position += direction;
}

void Camera::Rotate(float dx, float dy) noexcept
{
    // Apply sensitivity and accumulate rotation
    m_yaw += dx * m_sensitivity;
    m_pitch += dy * m_sensitivity;

    // Clamp pitch to prevent camera from flipping over
    const float maxPitch = 3.14159265f / 2.0f - 0.01f; // Just under 90 degrees
    if (m_pitch > maxPitch) m_pitch = maxPitch;
    if (m_pitch < -maxPitch) m_pitch = -maxPitch;

    // Keep yaw in a reasonable range (optional, but helps prevent precision issues)
    const float twoPi = 2.0f * 3.14159265f;
    m_yaw = fmodf(m_yaw, twoPi);
}


mat4f Camera::WorldToViewMatrix() const noexcept
{
    // Assuming a camera's position and rotation is defined by matrices T(p) and R,
    // the View-to-World transform is T(p)*R (for a first-person style camera).
    //
    //
    // World-to-View then is the inverse of T(p)*R;
    //        inverse(T(p)*R) = inverse(R)*inverse(T(p)) = transpose(R)*T(-p)
    // Since now there is no rotation, this matrix is simply T(-p)

    // Create rotation matrix using pitch and yaw
    // mat4f::rotation(roll, yaw, pitch)
    mat4f rotation = mat4f::rotation(0.0f, m_yaw, m_pitch);

    mat4f translation = mat4f::translation(-m_position);

    return transpose(rotation) * translation;
}

mat4f Camera::ProjectionMatrix() const noexcept
{
    return mat4f::projection(m_vertical_fov, m_aspect_ratio, m_near_plane, m_far_plane);
}