#include <imgui.h>
// ImGui GLFW binding with OpenGL3 + shaders
// You can copy and use unmodified imgui_impl_* files in your project. See main.cpp for an example of using this.
// If you use this binding you'll need to call 4 functions: ImGui_ImplXXXX_Init(), ImGui_ImplXXXX_NewFrame(), ImGui::Render() and ImGui_ImplXXXX_Shutdown().
// If you are new to ImGui, see examples/README.txt and documentation at the top of imgui.cpp.
// https://github.com/ocornut/imgui

struct GLFWwindow;

IMGUI_API bool glfw_imgui_init(GLFWwindow* window, bool install_callbacks);
IMGUI_API void glfw_imgui_shutdown();
IMGUI_API void glfw_imgui_new_frame();

// Use if you want to reset your rendering device without losing ImGui state.
IMGUI_API void glfw_imgui_invalidate_device_objects();
IMGUI_API bool glfw_imgui_create_device_objects();

// GLFW callbacks (installed by default if you enable 'install_callbacks' during initialization)
// Provided here if you want to chain callbacks.
// You can also handle inputs yourself and use those as a reference.
IMGUI_API void glfw_imgui_mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
IMGUI_API void glfw_imgui_scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
IMGUI_API void glfw_imgui_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
IMGUI_API void glfw_imgui_char_callback(GLFWwindow* window, unsigned int c);
