#pragma once
#define GLEW_STATIC
#include "../../Dependencies/glew-2.1.0/include/GL/glew.h"
#include "../../Dependencies/glfw-3.3.4/include/GLFW/glfw3.h"
#include "../Script.h"
#include "../../Dependencies/imgui-1.76/imgui.h"
#include "../Application.h"

#include <functional>

// used by the Dear ImGui library but for 
//our purposes we will only be using the OpenGL enum
enum GlfwClientApi
{
    GlfwClientApi_Unknown,
    GlfwClientApi_OpenGL,
    GlfwClientApi_Vulkan
};

/* script responsible for setting up the Dear ImGui library, the code in 
   this file has been taken from the example files provided by the library 
   which shows how to set up the library and been modified to fit the framework
*/
class ImGuiManager
{
public:
    ImGuiManager(GLFWwindow* window)
    {
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();

        // Setup Platform/Renderer bindings
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init();
    }

    ~ImGuiManager()
    {
        // Cleanup
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void StartFrame()
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void Render()
    {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

private:
    // Functions
    bool ImGui_ImplOpenGL3_Init()
    {
        // Query for GL version (e.g. 320 for GL 3.2)
        GLint major, minor;
        glGetIntegerv(GL_MAJOR_VERSION, &major);
        glGetIntegerv(GL_MINOR_VERSION, &minor);
        m_glVersion = major * 100 + minor * 10;

        // Setup back-end capabilities flags
        ImGuiIO& io = ImGui::GetIO();
        io.BackendRendererName = "imgui_impl_opengl3";

        // Store GLSL version string so we can refer to it later in case we recreate shaders.
        // Note: GLSL version is NOT the same as GL version. Leave this to NULL if unsure.
        
        const char* glsl_version = "#version 130";

        IM_ASSERT((int)strlen(glsl_version) + 2 < IM_ARRAYSIZE(m_glslVersionString));
        strcpy_s(m_glslVersionString, glsl_version);
        strcat_s(m_glslVersionString, "\n");

        // Dummy construct to make it easily visible in the IDE and debugger which GL loader has been selected.
        // The code actually never uses the 'gl_loader' variable! It is only here so you can read it!
        // If auto-detection fails or doesn't select the same GL loader file as used by your application,
        // you are likely to get a crash below.
        // You can explicitly select a loader by using '#define IMGUI_IMPL_OPENGL_LOADER_XXX' in imconfig.h or compiler command-line.
        const char* gl_loader = "Unknown";
        IM_UNUSED(gl_loader);
        gl_loader = "none";

        // Make a dummy GL call (we don't actually need the result)
        // IF YOU GET A CRASH HERE: it probably means that you haven't initialized the OpenGL function loader used by this code.
        // Desktop OpenGL 3/4 need a function loader. See the IMGUI_IMPL_OPENGL_LOADER_xxx explanation above.
        GLint current_texture;
        glGetIntegerv(GL_TEXTURE_BINDING_2D, &current_texture);

        return true;
    }

    void ImGui_ImplOpenGL3_NewFrame()
    {
        if (!m_shaderHandle)
            ImGui_ImplOpenGL3_CreateDeviceObjects();
    }

    bool ImGui_ImplOpenGL3_CreateDeviceObjects()
    {
        // Backup GL state
        GLint last_texture, last_array_buffer;
        glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
        glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
        GLint last_vertex_array;
        glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vertex_array);

        // Parse GLSL version string
        int glsl_version = 130;
        sscanf_s(m_glslVersionString, "#version %d", &glsl_version);

        const GLchar* vertex_shader_glsl_120 =
            "uniform mat4 ProjMtx;\n"
            "attribute vec2 Position;\n"
            "attribute vec2 UV;\n"
            "attribute vec4 Color;\n"
            "varying vec2 Frag_UV;\n"
            "varying vec4 Frag_Color;\n"
            "void main()\n"
            "{\n"
            "    Frag_UV = UV;\n"
            "    Frag_Color = Color;\n"
            "    gl_Position = ProjMtx * vec4(Position.xy,0,1);\n"
            "}\n";

        const GLchar* vertex_shader_glsl_130 =
            "uniform mat4 ProjMtx;\n"
            "in vec2 Position;\n"
            "in vec2 UV;\n"
            "in vec4 Color;\n"
            "out vec2 Frag_UV;\n"
            "out vec4 Frag_Color;\n"
            "void main()\n"
            "{\n"
            "    Frag_UV = UV;\n"
            "    Frag_Color = Color;\n"
            "    gl_Position = ProjMtx * vec4(Position.xy,0,1);\n"
            "}\n";

        const GLchar* vertex_shader_glsl_300_es =
            "precision mediump float;\n"
            "layout (location = 0) in vec2 Position;\n"
            "layout (location = 1) in vec2 UV;\n"
            "layout (location = 2) in vec4 Color;\n"
            "uniform mat4 ProjMtx;\n"
            "out vec2 Frag_UV;\n"
            "out vec4 Frag_Color;\n"
            "void main()\n"
            "{\n"
            "    Frag_UV = UV;\n"
            "    Frag_Color = Color;\n"
            "    gl_Position = ProjMtx * vec4(Position.xy,0,1);\n"
            "}\n";

        const GLchar* vertex_shader_glsl_410_core =
            "layout (location = 0) in vec2 Position;\n"
            "layout (location = 1) in vec2 UV;\n"
            "layout (location = 2) in vec4 Color;\n"
            "uniform mat4 ProjMtx;\n"
            "out vec2 Frag_UV;\n"
            "out vec4 Frag_Color;\n"
            "void main()\n"
            "{\n"
            "    Frag_UV = UV;\n"
            "    Frag_Color = Color;\n"
            "    gl_Position = ProjMtx * vec4(Position.xy,0,1);\n"
            "}\n";

        const GLchar* fragment_shader_glsl_120 =
            "#ifdef GL_ES\n"
            "    precision mediump float;\n"
            "#endif\n"
            "uniform sampler2D Texture;\n"
            "varying vec2 Frag_UV;\n"
            "varying vec4 Frag_Color;\n"
            "void main()\n"
            "{\n"
            "    gl_FragColor = Frag_Color * texture2D(Texture, Frag_UV.st);\n"
            "}\n";

        const GLchar* fragment_shader_glsl_130 =
            "uniform sampler2D Texture;\n"
            "in vec2 Frag_UV;\n"
            "in vec4 Frag_Color;\n"
            "out vec4 Out_Color;\n"
            "void main()\n"
            "{\n"
            "    Out_Color = Frag_Color * texture(Texture, Frag_UV.st);\n"
            "}\n";

        const GLchar* fragment_shader_glsl_300_es =
            "precision mediump float;\n"
            "uniform sampler2D Texture;\n"
            "in vec2 Frag_UV;\n"
            "in vec4 Frag_Color;\n"
            "layout (location = 0) out vec4 Out_Color;\n"
            "void main()\n"
            "{\n"
            "    Out_Color = Frag_Color * texture(Texture, Frag_UV.st);\n"
            "}\n";

        const GLchar* fragment_shader_glsl_410_core =
            "in vec2 Frag_UV;\n"
            "in vec4 Frag_Color;\n"
            "uniform sampler2D Texture;\n"
            "layout (location = 0) out vec4 Out_Color;\n"
            "void main()\n"
            "{\n"
            "    Out_Color = Frag_Color * texture(Texture, Frag_UV.st);\n"
            "}\n";

        // Select shaders matching our GLSL versions
        const GLchar* vertex_shader = NULL;
        const GLchar* fragment_shader = NULL;
        if (glsl_version < 130)
        {
            vertex_shader = vertex_shader_glsl_120;
            fragment_shader = fragment_shader_glsl_120;
        }
        else if (glsl_version >= 410)
        {
            vertex_shader = vertex_shader_glsl_410_core;
            fragment_shader = fragment_shader_glsl_410_core;
        }
        else if (glsl_version == 300)
        {
            vertex_shader = vertex_shader_glsl_300_es;
            fragment_shader = fragment_shader_glsl_300_es;
        }
        else
        {
            vertex_shader = vertex_shader_glsl_130;
            fragment_shader = fragment_shader_glsl_130;
        }

        // Create shaders
        const GLchar* vertex_shader_with_version[2] = { m_glslVersionString, vertex_shader };
        m_vertHandle = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(m_vertHandle, 2, vertex_shader_with_version, NULL);
        glCompileShader(m_vertHandle);
        CheckShader(m_vertHandle, "vertex shader");

        const GLchar* fragment_shader_with_version[2] = { m_glslVersionString, fragment_shader };
        m_fragHandle = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(m_fragHandle, 2, fragment_shader_with_version, NULL);
        glCompileShader(m_fragHandle);
        CheckShader(m_fragHandle, "fragment shader");

        m_shaderHandle = glCreateProgram();
        glAttachShader(m_shaderHandle, m_vertHandle);
        glAttachShader(m_shaderHandle, m_fragHandle);
        glLinkProgram(m_shaderHandle);
        CheckProgram(m_shaderHandle, "shader program");

        m_attribLocationTex = glGetUniformLocation(m_shaderHandle, "Texture");
        m_attribLocationProjMtx = glGetUniformLocation(m_shaderHandle, "ProjMtx");
        m_attribLocationVtxPos = glGetAttribLocation(m_shaderHandle, "Position");
        m_attribLocationVtxUV = glGetAttribLocation(m_shaderHandle, "UV");
        m_attribLocationVtxColor = glGetAttribLocation(m_shaderHandle, "Color");

        // Create buffers
        glGenBuffers(1, &m_vboHandle);
        glGenBuffers(1, &m_elementsHandle);

        ImGui_ImplOpenGL3_CreateFontsTexture();

        // Restore modified GL state
        glBindTexture(GL_TEXTURE_2D, last_texture);
        glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
#ifndef IMGUI_IMPL_OPENGL_ES2
        glBindVertexArray(last_vertex_array);
#endif

        return true;
    }

    bool CheckShader(GLuint handle, const char* desc)
    {
        GLint status = 0, log_length = 0;
        glGetShaderiv(handle, GL_COMPILE_STATUS, &status);
        glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &log_length);
        if ((GLboolean)status == GL_FALSE)
            fprintf(stderr, "ERROR: ImGui_ImplOpenGL3_CreateDeviceObjects: failed to compile %s!\n", desc);
        if (log_length > 1)
        {
            ImVector<char> buf;
            buf.resize((int)(log_length + 1));
            glGetShaderInfoLog(handle, log_length, NULL, (GLchar*)buf.begin());
            fprintf(stderr, "%s\n", buf.begin());
        }
        return (GLboolean)status == GL_TRUE;
    }

    bool CheckProgram(GLuint handle, const char* desc)
    {
        GLint status = 0, log_length = 0;
        glGetProgramiv(handle, GL_LINK_STATUS, &status);
        glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &log_length);
        if ((GLboolean)status == GL_FALSE)
            fprintf(stderr, "ERROR: ImGui_ImplOpenGL3_CreateDeviceObjects: failed to link %s! (with GLSL '%s')\n", 
                desc, m_glslVersionString);
        if (log_length > 1)
        {
            ImVector<char> buf;
            buf.resize((int)(log_length + 1));
            glGetProgramInfoLog(handle, log_length, NULL, (GLchar*)buf.begin());
            fprintf(stderr, "%s\n", buf.begin());
        }
        return (GLboolean)status == GL_TRUE;
    }

    bool ImGui_ImplOpenGL3_CreateFontsTexture()
    {
        // Build texture atlas
        ImGuiIO& io = ImGui::GetIO();
        unsigned char* pixels;
        int width, height;
        io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);   // Load as RGBA 32-bit (75% of the memory is wasted, but default font is so small) because it is more likely to be compatible with user's existing shaders. If your ImTextureId represent a higher-level concept than just a GL texture id, consider calling GetTexDataAsAlpha8() instead to save on GPU memory.

        // Upload texture to graphics system
        GLint last_texture;
        glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
        glGenTextures(1, &m_fontTexture);
        glBindTexture(GL_TEXTURE_2D, m_fontTexture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

        // Store our identifier
        io.Fonts->TexID = (ImTextureID)(intptr_t)m_fontTexture;

        // Restore state
        glBindTexture(GL_TEXTURE_2D, last_texture);

        return true;
    }

    bool ImGui_ImplGlfw_InitForOpenGL(GLFWwindow* window, bool install_callbacks)
    {
        return ImGui_ImplGlfw_Init(window, install_callbacks, GlfwClientApi_OpenGL);
    }

    static const char* ImGui_ImplGlfw_GetClipboardText(void* user_data)
    {
        return glfwGetClipboardString((GLFWwindow*)user_data);
    }

    static void ImGui_ImplGlfw_SetClipboardText(void* user_data, const char* text)
    {
        glfwSetClipboardString((GLFWwindow*)user_data, text);
    }

    bool ImGui_ImplGlfw_Init(GLFWwindow* window, bool install_callbacks, GlfwClientApi client_api)
    {
        m_window = window;
        m_time = 0.0;

        // Setup back-end capabilities flags
        ImGuiIO& io = ImGui::GetIO();
        io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;         // We can honor GetMouseCursor() values (optional)
        io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;          // We can honor io.WantSetMousePos requests (optional, rarely used)
        io.BackendPlatformName = "imgui_impl_glfw";

        // Keyboard mapping. ImGui will use those indices to peek into the io.KeysDown[] array.
        io.KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB;
        io.KeyMap[ImGuiKey_LeftArrow] = GLFW_KEY_LEFT;
        io.KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
        io.KeyMap[ImGuiKey_UpArrow] = GLFW_KEY_UP;
        io.KeyMap[ImGuiKey_DownArrow] = GLFW_KEY_DOWN;
        io.KeyMap[ImGuiKey_PageUp] = GLFW_KEY_PAGE_UP;
        io.KeyMap[ImGuiKey_PageDown] = GLFW_KEY_PAGE_DOWN;
        io.KeyMap[ImGuiKey_Home] = GLFW_KEY_HOME;
        io.KeyMap[ImGuiKey_End] = GLFW_KEY_END;
        io.KeyMap[ImGuiKey_Insert] = GLFW_KEY_INSERT;
        io.KeyMap[ImGuiKey_Delete] = GLFW_KEY_DELETE;
        io.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
        io.KeyMap[ImGuiKey_Space] = GLFW_KEY_SPACE;
        io.KeyMap[ImGuiKey_Enter] = GLFW_KEY_ENTER;
        io.KeyMap[ImGuiKey_Escape] = GLFW_KEY_ESCAPE;
        io.KeyMap[ImGuiKey_KeyPadEnter] = GLFW_KEY_KP_ENTER;
        io.KeyMap[ImGuiKey_A] = GLFW_KEY_A;
        io.KeyMap[ImGuiKey_C] = GLFW_KEY_C;
        io.KeyMap[ImGuiKey_V] = GLFW_KEY_V;
        io.KeyMap[ImGuiKey_X] = GLFW_KEY_X;
        io.KeyMap[ImGuiKey_Y] = GLFW_KEY_Y;
        io.KeyMap[ImGuiKey_Z] = GLFW_KEY_Z;

        io.SetClipboardTextFn = ImGui_ImplGlfw_SetClipboardText;
        io.GetClipboardTextFn = ImGui_ImplGlfw_GetClipboardText;
        io.ClipboardUserData = m_window;

        /*
        if defined(_WIN32)
            io.ImeWindowHandle = (void*)glfwGetWin32Window(g_Window);
        endif
        */

        // Create mouse cursors
        // (By design, on X11 cursors are user configurable and some cursors may be missing. When a cursor doesn't exist,
        // GLFW will emit an error which will often be printed by the app, so we temporarily disable error reporting.
        // Missing cursors will return NULL and our _UpdateMouseCursor() function will use the Arrow cursor instead.)
        GLFWerrorfun prev_error_callback = glfwSetErrorCallback(NULL);
        m_mouseCursors[ImGuiMouseCursor_Arrow] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
        m_mouseCursors[ImGuiMouseCursor_TextInput] = glfwCreateStandardCursor(GLFW_IBEAM_CURSOR);
        m_mouseCursors[ImGuiMouseCursor_ResizeNS] = glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR);
        m_mouseCursors[ImGuiMouseCursor_ResizeEW] = glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
        m_mouseCursors[ImGuiMouseCursor_Hand] = glfwCreateStandardCursor(GLFW_HAND_CURSOR);

        m_mouseCursors[ImGuiMouseCursor_ResizeAll] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
        m_mouseCursors[ImGuiMouseCursor_ResizeNESW] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
        m_mouseCursors[ImGuiMouseCursor_ResizeNWSE] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
        m_mouseCursors[ImGuiMouseCursor_NotAllowed] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);

        glfwSetErrorCallback(prev_error_callback);

        // Chain GLFW callbacks: our callbacks will call the user's previously installed callbacks, if any.
        m_prevUserCallbackMousebutton = NULL;
        m_prevUserCallbackScroll = NULL;
        m_prevUserCallbackKey = NULL;
        m_prevUserCallbackChar = NULL;
        if (install_callbacks)
        {
            m_installedCallbacks = true;
            auto mouseFunc = (std::function<void(GLFWwindow*, int, int, int)>)
                std::bind(&ImGuiManager::ImGui_ImplGlfw_MouseButtonCallback, this,
                    std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
            m_prevUserCallbackMousebutton = glfwSetMouseButtonCallback(window, 
                (GLFWmousebuttonfun)(void*)&mouseFunc);

            auto scrollFunc = (std::function<void(GLFWwindow*, double, double)>)
                std::bind(&ImGuiManager::ImGui_ImplGlfw_ScrollCallback, this,
                    std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
            m_prevUserCallbackScroll = glfwSetScrollCallback(window, (GLFWscrollfun)(void*)&scrollFunc);

            auto keyFunc = (std::function<void(GLFWwindow*, int, int, int, int)>)
                std::bind(&ImGuiManager::ImGui_ImplGlfw_KeyCallback, this,
                    std::placeholders::_1, std::placeholders::_2, std::placeholders::_3,
                    std::placeholders::_4, std::placeholders::_5);
            m_prevUserCallbackKey = glfwSetKeyCallback(window, (GLFWkeyfun)(void*)&keyFunc);

            auto charFunc = (std::function<void(GLFWwindow*, unsigned int)>)
                std::bind(&ImGuiManager::ImGui_ImplGlfw_CharCallback, this,
                    std::placeholders::_1, std::placeholders::_2);
            m_prevUserCallbackChar = glfwSetCharCallback(window, (GLFWcharfun)(void*)&charFunc);
        }

        m_clientApi = client_api;
        return true;
    }

    void ImGui_ImplGlfw_MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
    {
        if (m_prevUserCallbackMousebutton != NULL)
            m_prevUserCallbackMousebutton(window, button, action, mods);

        if (action == GLFW_PRESS && button >= 0 && button < IM_ARRAYSIZE(m_mouseJustPressed))
            m_mouseJustPressed[button] = true;
    }

    void ImGui_ImplGlfw_ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
    {
        if (m_prevUserCallbackScroll != NULL)
            m_prevUserCallbackScroll(window, xoffset, yoffset);

        ImGuiIO& io = ImGui::GetIO();
        io.MouseWheelH += (float)xoffset;
        io.MouseWheel += (float)yoffset;
    }

    void ImGui_ImplGlfw_KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        if (m_prevUserCallbackKey != NULL)
            m_prevUserCallbackKey(window, key, scancode, action, mods);

        ImGuiIO& io = ImGui::GetIO();
        if (action == GLFW_PRESS)
            io.KeysDown[key] = true;
        if (action == GLFW_RELEASE)
            io.KeysDown[key] = false;

        // Modifiers are not reliable across systems
        io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
        io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
        io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
        io.KeySuper = io.KeysDown[GLFW_KEY_LEFT_SUPER] || io.KeysDown[GLFW_KEY_RIGHT_SUPER];
    }

    void ImGui_ImplGlfw_CharCallback(GLFWwindow* window, unsigned int c)
    {
        if (m_prevUserCallbackChar != NULL)
            m_prevUserCallbackChar(window, c);

        ImGuiIO& io = ImGui::GetIO();
        io.AddInputCharacter(c);
    }

    void ImGui_ImplGlfw_NewFrame()
    {
        ImGuiIO& io = ImGui::GetIO();
        IM_ASSERT(io.Fonts->IsBuilt() && "Font atlas not built! It is generally built by the renderer back-end. Missing call to renderer _NewFrame() function? e.g. ImGui_ImplOpenGL3_NewFrame().");

        // Setup display size (every frame to accommodate for window resizing)
        int w, h;
        int display_w, display_h;
        glfwGetWindowSize(m_window, &w, &h);
        glfwGetFramebufferSize(m_window, &display_w, &display_h);
        io.DisplaySize = ImVec2((float)w, (float)h);
        if (w > 0 && h > 0)
            io.DisplayFramebufferScale = ImVec2((float)display_w / w, (float)display_h / h);

        // Setup time step
        double current_time = glfwGetTime();
        io.DeltaTime = m_time > 0.0 ? (float)(current_time - m_time) : (float)(1.0f / 60.0f);
        m_time = current_time;

        ImGui_ImplGlfw_UpdateMousePosAndButtons();
        ImGui_ImplGlfw_UpdateMouseCursor();
    }

    void ImGui_ImplGlfw_UpdateMousePosAndButtons()
    {
        // Update buttons
        ImGuiIO& io = ImGui::GetIO();
        for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++)
        {
            // If a mouse press event came, always pass it as "mouse held this frame", so we don't 
            //miss click-release events that are shorter than 1 frame.
            io.MouseDown[i] = m_mouseJustPressed[i] || glfwGetMouseButton(m_window, i) != 0;
            m_mouseJustPressed[i] = false;
        }

        // Update mouse position
        const ImVec2 mouse_pos_backup = io.MousePos;
        io.MousePos = ImVec2(-FLT_MAX, -FLT_MAX);
        const bool focused = glfwGetWindowAttrib(m_window, GLFW_FOCUSED) != 0;
        if (focused)
        {
            if (io.WantSetMousePos)
            {
                glfwSetCursorPos(m_window, (double)mouse_pos_backup.x, (double)mouse_pos_backup.y);
            }
            else
            {
                double mouse_x, mouse_y;
                glfwGetCursorPos(m_window, &mouse_x, &mouse_y);
                io.MousePos = ImVec2((float)mouse_x, (float)mouse_y);
            }
        }
    }

    void ImGui_ImplGlfw_UpdateMouseCursor()
    {
        ImGuiIO& io = ImGui::GetIO();
        if ((io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange) 
            || glfwGetInputMode(m_window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED)
            return;

        ImGuiMouseCursor imgui_cursor = ImGui::GetMouseCursor();
        if (imgui_cursor == ImGuiMouseCursor_None || io.MouseDrawCursor)
        {
            // Hide OS mouse cursor if imgui is drawing it or if it wants no cursor
            glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
        }
        else
        {
            // Show OS mouse cursor
            // FIXME-PLATFORM: Unfocused windows seems to fail changing the mouse cursor with GLFW 3.2, but 3.3 works here.
            glfwSetCursor(m_window, m_mouseCursors[imgui_cursor] ? m_mouseCursors[imgui_cursor] 
                : m_mouseCursors[ImGuiMouseCursor_Arrow]);
            glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    }

    // OpenGL3 Render function.
    // (this used to be set in io.RenderDrawListsFn and called by ImGui::Render(), but you can now call this directly from your main loop)
    // Note that this implementation is little overcomplicated because we are saving/setting up/restoring every OpenGL state explicitly, in order to be able to run within any OpenGL engine that doesn't do so.
    void ImGui_ImplOpenGL3_RenderDrawData(ImDrawData* draw_data)
    {
        // Avoid rendering when minimized, scale coordinates for retina displays (screen coordinates != framebuffer coordinates)
        int fb_width = (int)(draw_data->DisplaySize.x * draw_data->FramebufferScale.x);
        int fb_height = (int)(draw_data->DisplaySize.y * draw_data->FramebufferScale.y);
        if (fb_width <= 0 || fb_height <= 0)
            return;

        // Backup GL state
        GLenum last_active_texture; glGetIntegerv(GL_ACTIVE_TEXTURE, (GLint*)&last_active_texture);
        glActiveTexture(GL_TEXTURE0);
        GLint last_program; glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
        GLint last_texture; glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
#ifdef GL_SAMPLER_BINDING
        GLint last_sampler; glGetIntegerv(GL_SAMPLER_BINDING, &last_sampler);
#endif
        GLint last_array_buffer; glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
#ifndef IMGUI_IMPL_OPENGL_ES2
        GLint last_vertex_array_object; glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vertex_array_object);
#endif
#ifdef GL_POLYGON_MODE
        GLint last_polygon_mode[2]; glGetIntegerv(GL_POLYGON_MODE, last_polygon_mode);
#endif
        GLint last_viewport[4]; glGetIntegerv(GL_VIEWPORT, last_viewport);
        GLint last_scissor_box[4]; glGetIntegerv(GL_SCISSOR_BOX, last_scissor_box);
        GLenum last_blend_src_rgb; glGetIntegerv(GL_BLEND_SRC_RGB, (GLint*)&last_blend_src_rgb);
        GLenum last_blend_dst_rgb; glGetIntegerv(GL_BLEND_DST_RGB, (GLint*)&last_blend_dst_rgb);
        GLenum last_blend_src_alpha; glGetIntegerv(GL_BLEND_SRC_ALPHA, (GLint*)&last_blend_src_alpha);
        GLenum last_blend_dst_alpha; glGetIntegerv(GL_BLEND_DST_ALPHA, (GLint*)&last_blend_dst_alpha);
        GLenum last_blend_equation_rgb; glGetIntegerv(GL_BLEND_EQUATION_RGB, (GLint*)&last_blend_equation_rgb);
        GLenum last_blend_equation_alpha; glGetIntegerv(GL_BLEND_EQUATION_ALPHA, (GLint*)&last_blend_equation_alpha);
        GLboolean last_enable_blend = glIsEnabled(GL_BLEND);
        GLboolean last_enable_cull_face = glIsEnabled(GL_CULL_FACE);
        GLboolean last_enable_depth_test = glIsEnabled(GL_DEPTH_TEST);
        GLboolean last_enable_scissor_test = glIsEnabled(GL_SCISSOR_TEST);
        bool clip_origin_lower_left = true;
#if defined(GL_CLIP_ORIGIN) && !defined(__APPLE__)
        GLenum last_clip_origin = 0; glGetIntegerv(GL_CLIP_ORIGIN, (GLint*)&last_clip_origin); // Support for GL 4.5's glClipControl(GL_UPPER_LEFT)
        if (last_clip_origin == GL_UPPER_LEFT)
            clip_origin_lower_left = false;
#endif

        // Setup desired GL state
        // Recreate the VAO every time (this is to easily allow multiple GL contexts to be rendered to. VAO are not shared among GL contexts)
        // The renderer would actually work without any VAO bound, but then our VertexAttrib calls would overwrite the default one currently bound.
        GLuint vertex_array_object = 0;
#ifndef IMGUI_IMPL_OPENGL_ES2
        glGenVertexArrays(1, &vertex_array_object);
#endif
        ImGui_ImplOpenGL3_SetupRenderState(draw_data, fb_width, fb_height, vertex_array_object);

        // Will project scissor/clipping rectangles into framebuffer space
        ImVec2 clip_off = draw_data->DisplayPos;         // (0,0) unless using multi-viewports
        ImVec2 clip_scale = draw_data->FramebufferScale; // (1,1) unless using retina display which are often (2,2)

        // Render command lists
        for (int n = 0; n < draw_data->CmdListsCount; n++)
        {
            const ImDrawList* cmd_list = draw_data->CmdLists[n];

            // Upload vertex/index buffers
            glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)cmd_list->VtxBuffer.Size * sizeof(ImDrawVert), (const GLvoid*)cmd_list->VtxBuffer.Data, GL_STREAM_DRAW);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx), (const GLvoid*)cmd_list->IdxBuffer.Data, GL_STREAM_DRAW);

            for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
            {
                const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
                if (pcmd->UserCallback != NULL)
                {
                    // User callback, registered via ImDrawList::AddCallback()
                    // (ImDrawCallback_ResetRenderState is a special callback value used by the user to request the renderer to reset render state.)
                    if (pcmd->UserCallback == ImDrawCallback_ResetRenderState)
                        ImGui_ImplOpenGL3_SetupRenderState(draw_data, fb_width, fb_height, vertex_array_object);
                    else
                        pcmd->UserCallback(cmd_list, pcmd);
                }
                else
                {
                    // Project scissor/clipping rectangles into framebuffer space
                    ImVec4 clip_rect;
                    clip_rect.x = (pcmd->ClipRect.x - clip_off.x) * clip_scale.x;
                    clip_rect.y = (pcmd->ClipRect.y - clip_off.y) * clip_scale.y;
                    clip_rect.z = (pcmd->ClipRect.z - clip_off.x) * clip_scale.x;
                    clip_rect.w = (pcmd->ClipRect.w - clip_off.y) * clip_scale.y;

                    if (clip_rect.x < fb_width && clip_rect.y < fb_height && clip_rect.z >= 0.0f && clip_rect.w >= 0.0f)
                    {
                        // Apply scissor/clipping rectangle
                        if (clip_origin_lower_left)
                            glScissor((int)clip_rect.x, (int)(fb_height - clip_rect.w), (int)(clip_rect.z - clip_rect.x), (int)(clip_rect.w - clip_rect.y));
                        else
                            glScissor((int)clip_rect.x, (int)clip_rect.y, (int)clip_rect.z, (int)clip_rect.w); // Support for GL 4.5 rarely used glClipControl(GL_UPPER_LEFT)

                        // Bind texture, Draw
                        glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)pcmd->TextureId);
#if IMGUI_IMPL_OPENGL_MAY_HAVE_VTX_OFFSET
                        if (g_GlVersion >= 320)
                            glDrawElementsBaseVertex(GL_TRIANGLES, (GLsizei)pcmd->ElemCount, sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, (void*)(intptr_t)(pcmd->IdxOffset * sizeof(ImDrawIdx)), (GLint)pcmd->VtxOffset);
                        else
#endif
                            glDrawElements(GL_TRIANGLES, (GLsizei)pcmd->ElemCount, sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, (void*)(intptr_t)(pcmd->IdxOffset * sizeof(ImDrawIdx)));
                    }
                }
            }
        }

        // Destroy the temporary VAO
#ifndef IMGUI_IMPL_OPENGL_ES2
        glDeleteVertexArrays(1, &vertex_array_object);
#endif

        // Restore modified GL state
        glUseProgram(last_program);
        glBindTexture(GL_TEXTURE_2D, last_texture);
#ifdef GL_SAMPLER_BINDING
        glBindSampler(0, last_sampler);
#endif
        glActiveTexture(last_active_texture);
#ifndef IMGUI_IMPL_OPENGL_ES2
        glBindVertexArray(last_vertex_array_object);
#endif
        glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
        glBlendEquationSeparate(last_blend_equation_rgb, last_blend_equation_alpha);
        glBlendFuncSeparate(last_blend_src_rgb, last_blend_dst_rgb, last_blend_src_alpha, last_blend_dst_alpha);
        if (last_enable_blend) glEnable(GL_BLEND); else glDisable(GL_BLEND);
        if (last_enable_cull_face) glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE);
        if (last_enable_depth_test) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
        if (last_enable_scissor_test) glEnable(GL_SCISSOR_TEST); else glDisable(GL_SCISSOR_TEST);
#ifdef GL_POLYGON_MODE
        glPolygonMode(GL_FRONT_AND_BACK, (GLenum)last_polygon_mode[0]);
#endif
        glViewport(last_viewport[0], last_viewport[1], (GLsizei)last_viewport[2], (GLsizei)last_viewport[3]);
        glScissor(last_scissor_box[0], last_scissor_box[1], (GLsizei)last_scissor_box[2], (GLsizei)last_scissor_box[3]);
    }

    void ImGui_ImplOpenGL3_SetupRenderState(ImDrawData* draw_data, int fb_width, int fb_height, GLuint vertex_array_object)
    {
        // Setup render state: alpha-blending enabled, no face culling, no depth testing, scissor enabled, polygon fill
        glEnable(GL_BLEND);
        glBlendEquation(GL_FUNC_ADD);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_SCISSOR_TEST);
#ifdef GL_POLYGON_MODE
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif

        // Setup viewport, orthographic projection matrix
        // Our visible imgui space lies from draw_data->DisplayPos (top left) to draw_data->DisplayPos+data_data->DisplaySize (bottom right). DisplayPos is (0,0) for single viewport apps.
        glViewport(0, 0, (GLsizei)fb_width, (GLsizei)fb_height);
        float L = draw_data->DisplayPos.x;
        float R = draw_data->DisplayPos.x + draw_data->DisplaySize.x;
        float T = draw_data->DisplayPos.y;
        float B = draw_data->DisplayPos.y + draw_data->DisplaySize.y;
        const float ortho_projection[4][4] =
        {
            { 2.0f / (R - L),   0.0f,         0.0f,   0.0f },
            { 0.0f,         2.0f / (T - B),   0.0f,   0.0f },
            { 0.0f,         0.0f,        -1.0f,   0.0f },
            { (R + L) / (L - R),  (T + B) / (B - T),  0.0f,   1.0f },
        };
        glUseProgram(m_shaderHandle);
        glUniform1i(m_attribLocationTex, 0);
        glUniformMatrix4fv(m_attribLocationProjMtx, 1, GL_FALSE, &ortho_projection[0][0]);
#ifdef GL_SAMPLER_BINDING
        glBindSampler(0, 0); // We use combined texture/sampler state. Applications using GL 3.3 may set that otherwise.
#endif

        (void)vertex_array_object;
#ifndef IMGUI_IMPL_OPENGL_ES2
        glBindVertexArray(vertex_array_object);
#endif

        // Bind vertex/index buffers and setup attributes for ImDrawVert
        glBindBuffer(GL_ARRAY_BUFFER, m_vboHandle);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_elementsHandle);
        glEnableVertexAttribArray(m_attribLocationVtxPos);
        glEnableVertexAttribArray(m_attribLocationVtxUV);
        glEnableVertexAttribArray(m_attribLocationVtxColor);
        glVertexAttribPointer(m_attribLocationVtxPos, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)IM_OFFSETOF(ImDrawVert, pos));
        glVertexAttribPointer(m_attribLocationVtxUV, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)IM_OFFSETOF(ImDrawVert, uv));
        glVertexAttribPointer(m_attribLocationVtxColor, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(ImDrawVert), (GLvoid*)IM_OFFSETOF(ImDrawVert, col));
    }

    void ImGui_ImplOpenGL3_Shutdown()
    {
        ImGui_ImplOpenGL3_DestroyDeviceObjects();
    }

    void ImGui_ImplOpenGL3_DestroyDeviceObjects()
    {
        if (m_vboHandle) { glDeleteBuffers(1, &m_vboHandle); m_vboHandle = 0; }
        if (m_elementsHandle) { glDeleteBuffers(1, &m_elementsHandle); m_elementsHandle = 0; }
        if (m_shaderHandle && m_vertHandle) { glDetachShader(m_shaderHandle, m_vertHandle); }
        if (m_shaderHandle && m_fragHandle) { glDetachShader(m_shaderHandle, m_fragHandle); }
        if (m_vertHandle) { glDeleteShader(m_vertHandle); m_vertHandle = 0; }
        if (m_fragHandle) { glDeleteShader(m_fragHandle); m_fragHandle = 0; }
        if (m_shaderHandle) { glDeleteProgram(m_shaderHandle); m_shaderHandle = 0; }

        ImGui_ImplOpenGL3_DestroyFontsTexture();
    }

    void ImGui_ImplOpenGL3_DestroyFontsTexture()
    {
        if (m_fontTexture)
        {
            ImGuiIO& io = ImGui::GetIO();
            glDeleteTextures(1, &m_fontTexture);
            io.Fonts->TexID = 0;
            m_fontTexture = 0;
        }
    }

    void ImGui_ImplGlfw_Shutdown()
    {
        if (m_installedCallbacks)
        {
            glfwSetMouseButtonCallback(m_window, m_prevUserCallbackMousebutton);
            glfwSetScrollCallback(m_window, m_prevUserCallbackScroll);
            glfwSetKeyCallback(m_window, m_prevUserCallbackKey);
            glfwSetCharCallback(m_window, m_prevUserCallbackChar);
            m_installedCallbacks = false;
        }

        for (ImGuiMouseCursor cursor_n = 0; cursor_n < ImGuiMouseCursor_COUNT; cursor_n++)
        {
            glfwDestroyCursor(m_mouseCursors[cursor_n]);
            m_mouseCursors[cursor_n] = NULL;
        }
        m_clientApi = GlfwClientApi_Unknown;
    }


    // OpenGL Data
    unsigned int m_glVersion = 0;                // Extracted at runtime using GL_MAJOR_VERSION, GL_MINOR_VERSION queries (e.g. 320 for GL 3.2)
    char m_glslVersionString[32] = "";   // Specified by user or detected based on compile time GL settings.
    unsigned int m_fontTexture = 0;
    unsigned int m_shaderHandle = 0, m_vertHandle = 0, m_fragHandle = 0;
    int m_attribLocationTex = 0, m_attribLocationProjMtx = 0;                                // Uniforms location
    int m_attribLocationVtxPos = 0, m_attribLocationVtxUV = 0, m_attribLocationVtxColor = 0; // Vertex attributes location
    unsigned int m_vboHandle = 0, m_elementsHandle = 0;

    //GLFW data
    GLFWwindow* m_window = nullptr;
    GlfwClientApi m_clientApi = GlfwClientApi_Unknown;
    double m_time = 0.0;
    bool m_mouseJustPressed[5] = { false, false, false, false, false };
    GLFWcursor* m_mouseCursors[ImGuiMouseCursor_COUNT] = {};
    bool m_installedCallbacks = false;

    GLFWmousebuttonfun m_prevUserCallbackMousebutton = NULL;
    GLFWscrollfun m_prevUserCallbackScroll = NULL;
    GLFWkeyfun m_prevUserCallbackKey = NULL;
    GLFWcharfun m_prevUserCallbackChar = NULL;
};