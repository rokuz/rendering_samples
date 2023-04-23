#define API_OPENGL
#include "rf.hpp"

using GpuProgram = rf::gl::GpuProgram;
using Mesh = rf::gl::Mesh;
using Texture = rf::gl::Texture;

char const * const kDefaultTexture = "default.png";

class BasicSample
{
public:
  bool Initialize(rf::Window const * window)
  {
    m_window = window;
    m_camera.Initialize(m_window->GetScreenWidth(), m_window->GetScreenHeight());
    m_camera.Setup(glm::vec3(-2.5f, 3.0f, -2.5f), glm::vec3(-1.5f, 2.0f, 1.0f));

    if (!m_program.Initialize({"skinned.vsh.glsl", "skinned.fsh.glsl"}, true /* areFiles */))
      return false;

    auto const desiredAttributesMask = rf::MeshVertexAttribute::Position |
                                       rf::MeshVertexAttribute::Normal |
                                       rf::MeshVertexAttribute::UV0 |
                                       rf::MeshVertexAttribute::Tangent |
                                       rf::MeshVertexAttribute::BoneIndices |
                                       rf::MeshVertexAttribute::BoneWeights;
    if (!m_mesh.Initialize("horse/horse.dae", desiredAttributesMask))
      return false;

    if (m_mesh.GetAttributesMask() != desiredAttributesMask)
    {
      rf::Logger::ToLog(rf::Logger::Error, "Mesh doesn't contain desired attributes.");
      return false;
    }

    m_textures[kDefaultTexture] = std::make_shared<Texture>("default");
    if (!m_textures[kDefaultTexture]->Initialize(std::string(kDefaultTexture)))
      return false;

    m_meshDiffuseTextures.resize(m_mesh.GetGroupsCount());
    for (size_t groupIndex = 0; groupIndex < m_mesh.GetGroupsCount(); ++groupIndex)
    {
      auto const material = m_mesh.GetGroupMaterial(static_cast<int>(groupIndex));
      if (material && !material->m_diffuseTexture.empty())
      {
        auto const it = m_textures.find(material->m_diffuseTexture);
        if (it != m_textures.end())
        {
          m_meshDiffuseTextures[groupIndex] = it->second;
        }
        else
        {
          auto t = std::make_shared<Texture>(material->m_diffuseTexture);
          if (t->Initialize(std::string(material->m_diffuseTexture)))
          {
            m_textures[material->m_diffuseTexture] = t;
            m_meshDiffuseTextures[groupIndex] = t;
          }
          else
          {
            m_meshDiffuseTextures[groupIndex] = m_textures[kDefaultTexture];
          }
        }
      }
    }

    // Basic clipping/culling options.
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    glEnable(GL_SCISSOR_TEST);

    // Basic alpha blending options.
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_BLEND);

    // Basic depth options.
    glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST);

    glViewport(0, 0, m_window->GetScreenWidth(), m_window->GetScreenHeight());
    glScissor(0, 0, m_window->GetScreenWidth(), m_window->GetScreenHeight());

    glCheckError();

    return true;
  }

  void Frame(double timeSinceStart, double elapsedTime, double averageFps)
  {
    m_camera.Update(elapsedTime, m_window->GetScreenWidth(),
                    m_window->GetScreenHeight());

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDrawBuffer(GL_BACK);
    float depth = 1.0f;
    const GLfloat c[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glClearBufferfv(GL_COLOR, 0, c);
    glClearBufferfv(GL_DEPTH, 0, &depth);

    if (m_program.Use())
    {
      glm::mat4x4 transform = glm::rotate(kPi, glm::vec3(0.0f, 1.0f, 0.0f));
      for (int groupIndex = 0; groupIndex < m_mesh.GetGroupsCount(); ++groupIndex)
      {
        auto const model = m_mesh.GetGroupTransform(groupIndex, transform);
        // GLM requires to multiply in the reverse order.
        auto const mvp = m_camera.GetProjection() * m_camera.GetView() * model;
        m_mesh.GetBonesTransforms(groupIndex, 0, timeSinceStart, true, m_bonesTransforms);

        m_program.SetMatrix("uModelViewProjection", mvp);
        m_program.SetMatrix("uModel", model);
        m_program.SetMatrixArray("uBoneTransforms", m_bonesTransforms.data(),
                                 static_cast<int>(m_bonesTransforms.size()));
        m_program.SetTexture("uDiffuseSampler", m_meshDiffuseTextures[groupIndex].get(), 0);
        m_mesh.RenderGroup(groupIndex);
      }
    }

    glCheckError();
  }

  void OnKeyButton(int key, bool pressed)
  {
    m_camera.OnKeyButton(key, pressed);
  }

  void OnMouseButton(float xpos, float ypos, int button, bool pressed)
  {
    m_camera.OnMouseButton(xpos, ypos, button, pressed);
  }

  void OnMouseMove(float xpos, float ypos)
  {
    m_camera.OnMouseMove(xpos, ypos);
  }

private:
  rf::Window const * m_window = nullptr;
  rf::FreeCamera m_camera;
  GpuProgram m_program;
  Mesh m_mesh;
  std::vector<std::shared_ptr<Texture>> m_meshDiffuseTextures;
  std::map<std::string, std::shared_ptr<Texture>> m_textures;
  std::vector<glm::mat4x4> m_bonesTransforms;
};

int main()
{
  rf::LoggerGuard loggerGuard(rf::Logger::OutputFlags::Console);

  uint8_t constexpr kOpenGLMajor = 4;
  uint8_t constexpr kOpenGLMinor = 1;

  rf::Window window;
  if (!window.InitializeForOpenGL(1024, 768, "OpenGL Skinned Mesh Sample",
                                  kOpenGLMajor, kOpenGLMinor))
  {
    return 1;
  }

  auto sample = std::make_unique<BasicSample>();
  if (!sample->Initialize(&window))
  {
    sample.reset();
    return 1;
  }

  window.SetOnFrameHandler([&sample](double timeSinceStart, double elapsedTime,
                                     double averageFps)
  {
    if (sample)
      sample->Frame(timeSinceStart, elapsedTime, averageFps);
  });

  window.SetOnKeyButtonHandler([&sample](int key, int scancode, bool pressed)
  {
    if (sample)
      sample->OnKeyButton(key, pressed);
  });

  window.SetOnMouseButtonHandler([&sample](float xpos, float ypos, int button,
                                           bool pressed)
  {
    if (sample)
      sample->OnMouseButton(xpos, ypos, button, pressed);
  });

  window.SetOnMouseMoveHandler([&sample](float xpos, float ypos)
  {
    if (sample)
      sample->OnMouseMove(xpos, ypos);
  });

  while(window.Loop());

  sample.reset();
  return 0;
}
