#define API_OPENGL
#include "rf.hpp"

using GpuProgram = rf::gl::GpuProgram;
using Mesh = rf::gl::Mesh;
using SinglePointMesh = rf::gl::SinglePointMesh;

class BasicSample
{
public:
  bool Initialize(rf::Window const * window)
  {
    m_window = window;
    m_camera.Initialize(m_window->GetScreenWidth(), m_window->GetScreenHeight());
    m_camera.Setup(glm::vec3(0.0f, 30.0f, -70.0f), glm::vec3(0.0f, 0.0f, 0.0f));

    if (!m_program.Initialize({"terrain.vsh.glsl", "terrain.gsh.glsl", "terrain.fsh.glsl"}, true /* areFiles */))
      return false;

    if (!m_rainProgram.Initialize({"rain.vsh.glsl", "rain.gsh.glsl", "rain.fsh.glsl"}, true /* areFiles */))
      return false;

    if (!m_wireframeProgram.Initialize({"wireframe.vsh.glsl", "wireframe.gsh.glsl", "wireframe.fsh.glsl"}, true /* areFiles */))
      return false;

    if (!m_normalsProgram.Initialize({"normals.vsh.glsl", "normals.gsh.glsl", "normals.fsh.glsl"}, true /* areFiles */))
      return false;

    rf::BaseTexture hmTex;
    auto hmData = hmTex.LoadHeightmap("heightmap.png");
    if (hmData.empty() || !m_mesh.InitializeAsTerrain(hmData, hmTex.GetWidth(), hmTex.GetHeight(),
                                                      m_minAltitude, m_maxAltitude, 100.0f, 100.0f))
    {
      return false;
    }
    rf::Logger::ToLogWithFormat(rf::Logger::Info, "Triangles count = %d", m_mesh.GetTrianglesCount());

    m_groupData.resize(m_mesh.GetGroupsCount());

    m_pointMesh.Initialize();

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

    m_normalHardness = glm::clamp(m_normalHardness + m_normalHardnessDir * static_cast<float>(elapsedTime) * 0.1f,
                                  0.0f, 1.0f);

    auto const viewProjection = m_camera.GetProjection() * m_camera.GetView();
    for (int groupIndex = 0; groupIndex < m_mesh.GetGroupsCount(); ++groupIndex)
    {
      m_groupData[groupIndex].m_model = m_mesh.GetGroupTransform(groupIndex, glm::mat4x4());
      // GLM requires to multiply in the reverse order.
      m_groupData[groupIndex].m_mvp = viewProjection * m_groupData[groupIndex].m_model;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDrawBuffer(GL_BACK);
    float depth = 1.0f;
    const GLfloat c[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glClearBufferfv(GL_COLOR, 0, c);
    glClearBufferfv(GL_DEPTH, 0, &depth);

    if (m_program.Use())
    {
      static glm::mat4x4 const kColors = glm::mat4x4(19.0 / 255.0, 89.0 / 255.0, 20.0 / 255.0, 1.0,
                                                     92.0 / 255.0, 78.0 / 255.0, 64.0 / 255.0, 1.0,
                                                     90.0 / 255.0, 99.0 / 255.0, 91.0 / 255.0, 1.0,
                                                     1.0, 1.0, 1.0, 1.0);
      static glm::vec4 const kWeights = glm::vec4(0.0, 0.15, 0.75, 1.0);
      m_program.SetVector("uMinMaxAltitudes", glm::vec2(m_minAltitude, m_maxAltitude));
      m_program.SetMatrix("uColors", kColors);
      m_program.SetVector("uWeights", kWeights);
      m_program.SetFloat("uNormalHardness", m_normalHardness);
      m_program.SetVector("uCameraPos", m_camera.GetPosition());
      for (int groupIndex = 0; groupIndex < m_mesh.GetGroupsCount(); ++groupIndex)
      {
        m_program.SetMatrix("uModelViewProjection", m_groupData[groupIndex].m_mvp);
        m_program.SetMatrix("uModel", m_groupData[groupIndex].m_model);

        m_mesh.RenderGroup(groupIndex);
      }
    }

    if (m_rainMode)
    {
      glEnable(GL_BLEND);
      if (m_rainProgram.Use())
      {
        int constexpr kRainDropPerSideCount = 120;
        auto const & bb = m_mesh.GetBoundingBox();
        m_rainProgram.SetMatrix("uViewProjection", viewProjection);
        m_rainProgram.SetInt("uRainDropPerSideCount", kRainDropPerSideCount);
        m_rainProgram.SetFloat("uTime", static_cast<float>(timeSinceStart));
        m_rainProgram.SetVector("uBounds", glm::vec4(bb.getMin().x, bb.getMax().x,
                                                     bb.getMin().z, bb.getMax().z));
        m_pointMesh.RenderInstanced(kRainDropPerSideCount * kRainDropPerSideCount);
      }
      glDisable(GL_BLEND);
    }

    if (m_wireframeMode && m_wireframeProgram.Use())
    {
      for (int groupIndex = 0; groupIndex < m_mesh.GetGroupsCount(); ++groupIndex)
      {
        m_wireframeProgram.SetMatrix("uModelViewProjection", m_groupData[groupIndex].m_mvp);

        m_mesh.RenderGroup(groupIndex);
      }
    }

    if (m_normalsMode && m_normalsProgram.Use())
    {
      for (int groupIndex = 0; groupIndex < m_mesh.GetGroupsCount(); ++groupIndex)
      {
        m_normalsProgram.SetMatrix("uModelViewProjection", m_groupData[groupIndex].m_mvp);

        m_mesh.RenderGroup(groupIndex);
      }
    }

    glCheckError();
  }

  void OnKeyButton(int key, bool pressed)
  {
    if (pressed && key == GLFW_KEY_M)
      m_wireframeMode = !m_wireframeMode;
    if (pressed && key == GLFW_KEY_N)
      m_normalsMode = !m_normalsMode;
    if (pressed && key == GLFW_KEY_R)
      m_rainMode = !m_rainMode;

    if (key == GLFW_KEY_MINUS)
      m_normalHardnessDir = pressed ? -1.0f : 0.0f;
    if (key == GLFW_KEY_EQUAL)
      m_normalHardnessDir = pressed ? 1.0f : 0.0f;

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
  struct GroupData
  {
    glm::mat4x4 m_model;
    glm::mat4x4 m_mvp;
  };
  rf::Window const * m_window = nullptr;
  rf::FreeCamera m_camera;
  GpuProgram m_program;
  GpuProgram m_rainProgram;
  GpuProgram m_wireframeProgram;
  GpuProgram m_normalsProgram;
  Mesh m_mesh;
  SinglePointMesh m_pointMesh;
  float m_minAltitude = 0.0f;
  float m_maxAltitude = 10.0f;
  bool m_wireframeMode = false;
  bool m_normalsMode = false;
  float m_normalHardness = 0.1f;
  float m_normalHardnessDir = 0.0f;
  bool m_rainMode = false;
  std::vector<GroupData> m_groupData;
};

int main()
{
  rf::LoggerGuard loggerGuard(rf::Logger::OutputFlags::Console);

  rf::Utils::RandomizeSeed();

  uint8_t constexpr kOpenGLMajor = 4;
  uint8_t constexpr kOpenGLMinor = 1;

  rf::Window window;
  if (!window.InitializeForOpenGL(1024, 768, "OpenGL Terrain Sample",
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
