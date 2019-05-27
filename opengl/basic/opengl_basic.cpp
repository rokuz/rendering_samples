#define API_OPENGL
#include "rf.hpp"

using GpuProgram = rf::gl::GpuProgram;
using Mesh = rf::gl::Mesh;

class BasicSample
{
public:
  bool Initialize(rf::Window const * window)
  {
    m_window = window;
    m_camera.Initialize(m_window->GetScreenWidth(), m_window->GetScreenHeight());
    m_camera.Setup(glm::vec3(0.0f, 0.0f, -5.0f), glm::vec3(0.0f, 0.0f, 0.0f));

    if (!m_program.Initialize({"lambert.vsh.glsl", "lambert.fsh.glsl"}, true /* areFiles */))
      return false;

    if (!m_mesh.InitializeAsSphere(1.0f))
      return false;

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
      m_program.SetVector("uDiffuseColor", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
      for (int groupIndex = 0; groupIndex < m_mesh.GetGroupsCount(); ++groupIndex)
      {
        auto const model = m_mesh.GetGroupTransform(groupIndex, glm::mat4x4());
        // GLM requires to multiply in the reverse order.
        auto const mvp = m_camera.GetProjection() * m_camera.GetView() * model;
        m_program.SetMatrix("uModelViewProjection", mvp);
        m_program.SetMatrix("uModel", model);

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
};

int main()
{
  rf::LoggerGuard loggerGuard(rf::Logger::OutputFlags::Console);

  uint8_t constexpr kOpenGLMajor = 4;
  uint8_t constexpr kOpenGLMinor = 1;

  rf::Window window;
  if (!window.InitializeForOpenGL(1024, 768, "OpenGL Basic Sample",
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
