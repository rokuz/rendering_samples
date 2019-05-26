#define API_OPENGL
#include "rf.hpp"

using GpuProgram = rf::gl::GpuProgram;

class BasicSample
{
public:
  bool Initialize(rf::Window const * window)
  {
    m_window = window;
    m_freeCamera.Initialize(m_window->GetScreenWidth(), m_window->GetScreenHeight());
    m_freeCamera.Setup(glm::vec3(0.0f, 3.0f, -5.0f), glm::vec3(0.0f, 0.0f, 0.0f));

    if (!m_program.Initialize({"phong.vsh.glsl", "phong.fsh.glsl"}, true /* areFiles */))
      return false;

    glViewport(0, 0, m_window->GetScreenWidth(), m_window->GetScreenHeight());

    return true;
  }

  void Frame(double timeSinceStart, double elapsedTime,
             double averageFps)
  {
    m_freeCamera.Update(elapsedTime, m_window->GetScreenWidth(),
                        m_window->GetScreenHeight());
    float depth = 1.0f;
    const GLfloat c[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glClearBufferfv(GL_COLOR, 0, c);
    glClearBufferfv(GL_DEPTH, 0, &depth);
  }

  void OnKeyButton(int key, bool pressed)
  {
    m_freeCamera.OnKeyButton(key, pressed);
  }

  void OnMouseButton(float xpos, float ypos, int button, bool pressed)
  {
    m_freeCamera.OnMouseButton(xpos, ypos, button, pressed);
  }

  void OnMouseMove(float xpos, float ypos)
  {
    m_freeCamera.OnMouseMove(xpos, ypos);
  }

private:
  rf::Window const * m_window = nullptr;
  rf::FreeCamera m_freeCamera;
  GpuProgram m_program;
};

int main()
{
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
