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
    m_yaw -= dx * m_sensitivity;
    m_pitch -= dy * m_sensitivity;

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
    //		inverse(T(p)*R) = inverse(R)*inverse(T(p)) = transpose(R)*T(-p)
    // Since now there is no rotation, this matrix is simply T(-p)

    // Create rotation matrix using pitch and yaw
    // mat4f::rotation(roll, yaw, pitch)
    mat4f rotation = mat4f::rotation(0.0f, m_yaw, m_pitch);

    mat4f translation = mat4f::translation(-m_position);

    return transpose(rotation) * translation;
}

vec3f Camera::GetForward() const noexcept {
    mat4f rotation = mat4f::rotation(0.0f, m_yaw, m_pitch);

    vec3f forward = normalize(vec3f(
        rotation.col[2].x,  // Third column, x component
        rotation.col[2].y,  // Third column, y component  
        rotation.col[2].z   // Third column, z component
    ));

    return forward;
}
vec3f Camera::GetRight() const noexcept {
    mat4f rotation = mat4f::rotation(0.0f, m_yaw, m_pitch);

    // The right direction is in the first column (col[0])
    vec3f right = normalize(vec3f(
        rotation.col[0].x,  // First column, x component
        rotation.col[0].y,  // First column, y component
        rotation.col[0].z   // First column, z component
    ));

    return right;
}


void Camera::Update(
    float dt,
    const InputHandler& input_handler) {

    // Calculate camera basis vectors for horizontal movement only
    vec3f forward = GetForward();  // Forward in horizontal plane
    vec3f right = GetRight();      // Right in horizontal plane

    // Camera movement
    vec3f moveDirection(0.0f, 0.0f, 0.0f);

    // Forward/Backward movement (W/S) - horizontal plane only
    if (input_handler.IsKeyPressed(Keys::Up) || input_handler.IsKeyPressed(Keys::W))
        moveDirection -= forward;  // Move forward (where camera is looking horizontally)
    if (input_handler.IsKeyPressed(Keys::Down) || input_handler.IsKeyPressed(Keys::S))
        moveDirection += forward;  // Move backward

    // Left/Right strafing (A/D) - horizontal plane only
    if (input_handler.IsKeyPressed(Keys::Right) || input_handler.IsKeyPressed(Keys::D))
        moveDirection += right;    // Strafe right
    if (input_handler.IsKeyPressed(Keys::Left) || input_handler.IsKeyPressed(Keys::A))
        moveDirection -= right;    // Strafe left

    // Up/Down movement (Space/LCtrl) - world space (vertical)
    if (input_handler.IsKeyPressed(Keys::Space))
        moveDirection += vec3f(0.0f, 1.0f, 0.0f);  // Move up in world space
    if (input_handler.IsKeyPressed(Keys::LCtrl))
        moveDirection -= vec3f(0.0f, 1.0f, 0.0f);  // Move down in world space

    // Apply movement if any direction is pressed
   // Check if movement vector has non-zero length
    if (moveDirection.x != 0.0f || moveDirection.y != 0.0f || moveDirection.z != 0.0f)
    {
        // Simple movement - move in the combined direction
        // Diagonals will be faster, but that's often acceptable
        // If you want to prevent faster diagonals, you could normalize:
        // moveDirection = normalize(moveDirection);
        Move(moveDirection * m_camera_velocity * dt);
    }

    long mouse_dx = input_handler.GetMouseDeltaX();
    long mouse_dy = input_handler.GetMouseDeltaY();

    if (mouse_dx != 0 || mouse_dy != 0) {
        Rotate(static_cast<float>(mouse_dx), static_cast<float>(mouse_dy));
    }

}

mat4f Camera::ProjectionMatrix() const noexcept
{
    return mat4f::projection(m_vertical_fov, m_aspect_ratio, m_near_plane, m_far_plane);
}
