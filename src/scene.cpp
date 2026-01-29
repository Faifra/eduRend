#include "Scene.h"
#include "QuadModel.h"
#include "OBJModel.h"
#include "cube.h"

Scene::Scene(
    ID3D11Device* dxdevice,
    ID3D11DeviceContext* dxdevice_context,
    int window_width,
    int window_height) :
    m_dxdevice(dxdevice),
    m_dxdevice_context(dxdevice_context),
    m_window_width(window_width),
    m_window_height(window_height)
{
}

void Scene::OnWindowResized(
    int new_width,
    int new_height)
{
    m_window_width = new_width;
    m_window_height = new_height;
}

OurTestScene::OurTestScene(
    ID3D11Device* dxdevice,
    ID3D11DeviceContext* dxdevice_context,
    int window_width,
    int window_height) :
    Scene(dxdevice, dxdevice_context, window_width, window_height)
{
    InitTransformationBuffer();
    InitLightCameraBuffer();
    InitMaterialBuffer();

}

//
// Called once at initialization
//
void OurTestScene::Init()
{
    m_camera = new Camera(
        45.0f * fTO_RAD,        // field-of-view (radians)
        (float)m_window_width / m_window_height,    // aspect ratio
        1.0f,                    // z-near plane
        500.0f);                 // z-far plane

    // Move camera to (0,0,5)
    m_camera->MoveTo({ 0, 0, 5 });

    // Create objects
    m_quad = new QuadModel(m_dxdevice, m_dxdevice_context);
    m_sponza = new OBJModel("assets/crytek-sponza/sponza.obj",
        m_dxdevice, m_dxdevice_context);

    m_cube = new cube(m_dxdevice, m_dxdevice_context); // Sun
    n_cube = new cube(m_dxdevice, m_dxdevice_context); // Earth
    v_cube = new cube(m_dxdevice, m_dxdevice_context); // Moon
}

//
// Called every frame
//
void OurTestScene::Update(
    float dt,
    const InputHandler& input_handler)
{

    m_camera->Update(dt, input_handler);

    // Quad model-to-world transformation
    m_quad_transform = mat4f::translation(0, 0, 0) *
        mat4f::rotation(-m_angle, 0.0f, 1.0f, 0.0f) *
        mat4f::scaling(1.5f);

    // Sponza model-to-world transformation
    m_sponza_transform = mat4f::translation(0, -5, 0) *
        mat4f::rotation(fPI / 2, 0.0f, 1.0f, 0.0f) *
        mat4f::scaling(0.05f);

    //
    // Solar system animation
    //

    // Update the rotation and orbit angles
    m_cube_rotation_angle += dt * 0.5f;      // Sun rotation
    n_cube_orbit_angle += dt * 1.0f;         // Earth orbit around Sun
    n_cube_rotation_angle += dt * 3.0f;      // Earth rotation
    v_cube_orbit_angle += dt * 5.0f;         // Moon orbit around Earth
    v_cube_rotation_angle += dt * 0.5f;      // Moon rotation

    // Sun stays at center but rotates
    m_cube_transform =
        mat4f::translation(0, 0, 0) *
        mat4f::rotation(m_cube_rotation_angle, 0.0f, 1.0f, 0.0f) *
        mat4f::scaling(2.0f);

    // Earth orbit around Sun
    float n_orbitX = n_orbit_radius * cos(n_cube_orbit_angle);
    float n_orbitZ = n_orbit_radius * sin(n_cube_orbit_angle);

    n_cube_transform =
        mat4f::scaling(0.7f) *
        mat4f::rotation(n_cube_rotation_angle, 0.0f, 1.0f, 0.0f) *
        mat4f::translation(n_orbitX, 0, n_orbitZ);

    // Moon orbit around Earth
    float v_orbitX = v_orbit_radius * cos(v_cube_orbit_angle);
    float v_orbitZ = v_orbit_radius * sin(v_cube_orbit_angle);

    v_cube_transform =
        mat4f::scaling(0.4f) *
        mat4f::rotation(v_cube_rotation_angle, 0.0f, 1.0f, 0.0f) *
        mat4f::translation(v_orbitX, 0, v_orbitZ);

    // Increment the rotation angle.
    m_angle += m_angular_velocity * dt;

    // Print fps
    m_fps_cooldown -= dt;
    if (m_fps_cooldown < 0.0)
    {
        std::cout << "fps " << (int)(1.0f / dt) << std::endl;
        m_fps_cooldown = 2.0;
    }
}

//
// Called every frame, after update
//
void OurTestScene::Render()
{
    // Bind transformation_buffer to slot b0 of the VS
    m_dxdevice_context->VSSetConstantBuffers(0, 1, &m_transformation_buffer);

    // Obtain the matrices needed for rendering from the camera
    m_view_matrix = m_camera->WorldToViewMatrix();
    m_projection_matrix = m_camera->ProjectionMatrix();

    // Extract camera position
    float3 cam_pos;
    cam_pos.x = -m_view_matrix.col[3].x;
    cam_pos.y = -m_view_matrix.col[3].y;
    cam_pos.z = -m_view_matrix.col[3].z;

    // Simple light
    float t = m_angle;
    float3 light_pos = float3(5.0f * cos(t), 3.0f, 5.0f * sin(t));

    // Update and bind light buffer (PS b0)
    UpdateLightCameraBuffer(light_pos, cam_pos);
    m_dxdevice_context->PSSetConstantBuffers(0, 1, &m_light_buffer);

    UpdateMaterialBuffer(
        float3(0.2f, 0.2f, 0.2f),   // ambient
        float3(1.0f, 1.0f, 1.0f),   // diffuse
        float3(1.0f, 1.0f, 1.0f),   // specular
        32.0f);                     // shininess

    m_dxdevice_context->PSSetConstantBuffers(1, 1, &m_material_buffer);

    //
    // Render Quad
    //
    UpdateTransformationBuffer(m_quad_transform, m_view_matrix, m_projection_matrix);
    m_quad->Render();

    //
    // Render Sponza
    //
    UpdateTransformationBuffer(m_sponza_transform, m_view_matrix, m_projection_matrix);
    m_sponza->Render();

    // Render blue sun
    UpdateMaterialBuffer(
        float3(0.0f, 0.0f, 0.2f),   // Ambient (dark blue)
        float3(0.0f, 0.0f, 1.0f),   // Diffuse (blue)
        float3(1.0f, 1.0f, 1.0f),   // Specular (white)
        32.0f                       // Shininess
    );

    m_dxdevice_context->PSSetConstantBuffers(1, 1, &m_material_buffer);

    UpdateTransformationBuffer(m_cube_transform, m_view_matrix, m_projection_matrix);
    m_cube->Render();

    //
    // Earth world transform (relative to Sun)
    //
    mat4f n_cube_world_transform = m_cube_transform * n_cube_transform;
    UpdateTransformationBuffer(n_cube_world_transform, m_view_matrix, m_projection_matrix);
    n_cube->Render();

    //
    // Moon world transform (relative to Earth)
    //
    mat4f v_cube_world_transform = n_cube_world_transform * v_cube_transform;
    UpdateTransformationBuffer(v_cube_world_transform, m_view_matrix, m_projection_matrix);
    v_cube->Render();
}

void OurTestScene::Release()
{
    SAFE_DELETE(m_quad);
    SAFE_DELETE(m_sponza);
    SAFE_DELETE(m_camera);

    SAFE_DELETE(m_cube);
    SAFE_DELETE(n_cube);
    SAFE_DELETE(v_cube);

    SAFE_RELEASE(m_transformation_buffer);
    SAFE_RELEASE(m_light_buffer);
    SAFE_RELEASE(m_material_buffer);
}

void OurTestScene::OnWindowResized(
    int new_width,
    int new_height)
{
    if (m_camera)
        m_camera->SetAspect(float(new_width) / new_height);

    Scene::OnWindowResized(new_width, new_height);
}

void OurTestScene::InitTransformationBuffer()
{
    HRESULT hr;
    D3D11_BUFFER_DESC matrixBufferDesc = { 0 };
    matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    matrixBufferDesc.ByteWidth = sizeof(TransformationBuffer);
    matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    matrixBufferDesc.MiscFlags = 0;
    matrixBufferDesc.StructureByteStride = 0;
    ASSERT(hr = m_dxdevice->CreateBuffer(&matrixBufferDesc, nullptr, &m_transformation_buffer));
}

void OurTestScene::UpdateTransformationBuffer(
    mat4f ModelToWorldMatrix,
    mat4f WorldToViewMatrix,
    mat4f ProjectionMatrix)
{
    // Map the resource buffer, obtain a pointer and then write our matrices to it
    D3D11_MAPPED_SUBRESOURCE resource;
    m_dxdevice_context->Map(m_transformation_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
    TransformationBuffer* matrixBuffer = (TransformationBuffer*)resource.pData;
    matrixBuffer->ModelToWorldMatrix = ModelToWorldMatrix;
    matrixBuffer->WorldToViewMatrix = WorldToViewMatrix;
    matrixBuffer->ProjectionMatrix = ProjectionMatrix;
    m_dxdevice_context->Unmap(m_transformation_buffer, 0);
}

void OurTestScene::InitLightCameraBuffer()
{
    D3D11_BUFFER_DESC desc = {};
    desc.Usage = D3D11_USAGE_DYNAMIC;
    desc.ByteWidth = sizeof(LightCameraBuffer);
    desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    m_dxdevice->CreateBuffer(&desc, nullptr, &m_light_buffer);
}

void OurTestScene::UpdateLightCameraBuffer(float3 light_pos, float3 camera_pos)
{
    D3D11_MAPPED_SUBRESOURCE mapped;
    m_dxdevice_context->Map(m_light_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);

    LightCameraBuffer* data = (LightCameraBuffer*)mapped.pData;
    data->light_pos = float4(light_pos, 1.0f);
    data->camera_pos = float4(camera_pos, 1.0f);

    m_dxdevice_context->Unmap(m_light_buffer, 0);
}

void OurTestScene::InitMaterialBuffer()
{
    D3D11_BUFFER_DESC desc = {};
    desc.Usage = D3D11_USAGE_DYNAMIC;
    desc.ByteWidth = sizeof(MaterialBuffer);
    desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    m_dxdevice->CreateBuffer(&desc, nullptr, &m_material_buffer);
}

void OurTestScene::UpdateMaterialBuffer(
    float3 AmbientColor,
    float3 DiffuseColor,
    float3 SpecularColor,
    float Shininess)
{
    D3D11_MAPPED_SUBRESOURCE mapped;
    m_dxdevice_context->Map(m_material_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);

    MaterialBuffer* data = (MaterialBuffer*)mapped.pData;
    data->AmbientColor = float4(AmbientColor, 1.0f);
    data->DiffuseColor = float4(DiffuseColor, 1.0f);
    data->SpecularColor = float4(SpecularColor, 1.0f);
    data->Shininess = float4(Shininess, 0, 0, 0);

    m_dxdevice_context->Unmap(m_material_buffer, 0);
}
