#include <RadonFramework/precompiled.hpp>
#include <RadonFramework/Radon.hpp>
#include <libSpec.h>
#include "MainWindow.hpp"
#include <imgui.h>
#include <RadonFramework/backend/GL/glew.h>

template <typename T, size_t N>
constexpr size_t countof(T const (&)[N])
{
    return N;
}

static RF_Type::Int32 g_ShaderHandle = 0, g_VertHandle = 0, g_FragHandle = 0;
static RF_Type::Int32 g_AttribLocationTex = 0, g_AttribLocationProjMtx = 0;
static RF_Type::UInt32 g_VboHandle = 0, g_VaoHandle = 0, g_ElementsHandle = 0;

class MainLoop: public RF_Pattern::SignalReceiver
{
public:
    void Start()
    {
        window = RF_Mem::AutoPointer<RGLStudio::MainWindow>(new RGLStudio::MainWindow());
        window->Initialize();

        m_FPS.Update();

        ImGuiIO& io = ImGui::GetIO();
        io.RenderDrawListsFn = ImGui_ImplGlfwGL3_RenderDrawLists;

        window->OnIdle += Connector<>(&MainLoop::Run);
        RF_Form::WindowServiceLocator::Default().Application()->Run(window.Get());
    }

    static void ImGui_ImplGlfwGL3_RenderDrawLists(ImDrawData* draw_data)
    {
        // Avoid rendering when minimized, scale coordinates for retina displays (screen coordinates != framebuffer coordinates)
        ImGuiIO& io = ImGui::GetIO();
        int fb_width = (int)(io.DisplaySize.x * io.DisplayFramebufferScale.x);
        int fb_height = (int)(io.DisplaySize.y * io.DisplayFramebufferScale.y);
        if(fb_width == 0 || fb_height == 0)
            return;
        draw_data->ScaleClipRects(io.DisplayFramebufferScale);

        // Backup GL state
        GLint last_program; glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
        GLint last_texture; glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
        GLint last_active_texture; glGetIntegerv(GL_ACTIVE_TEXTURE, &last_active_texture);
        GLint last_array_buffer; glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
        GLint last_element_array_buffer; glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &last_element_array_buffer);
        GLint last_vertex_array; glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vertex_array);
        GLint last_blend_src; glGetIntegerv(GL_BLEND_SRC, &last_blend_src);
        GLint last_blend_dst; glGetIntegerv(GL_BLEND_DST, &last_blend_dst);
        GLint last_blend_equation_rgb; glGetIntegerv(GL_BLEND_EQUATION_RGB, &last_blend_equation_rgb);
        GLint last_blend_equation_alpha; glGetIntegerv(GL_BLEND_EQUATION_ALPHA, &last_blend_equation_alpha);
        GLint last_viewport[4]; glGetIntegerv(GL_VIEWPORT, last_viewport);
        GLboolean last_enable_blend = glIsEnabled(GL_BLEND);
        GLboolean last_enable_cull_face = glIsEnabled(GL_CULL_FACE);
        GLboolean last_enable_depth_test = glIsEnabled(GL_DEPTH_TEST);
        GLboolean last_enable_scissor_test = glIsEnabled(GL_SCISSOR_TEST);

        // Setup render state: alpha-blending enabled, no face culling, no depth testing, scissor enabled
        glEnable(GL_BLEND);
        glBlendEquation(GL_FUNC_ADD);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_SCISSOR_TEST);
        glActiveTexture(GL_TEXTURE0);

        // Setup viewport, orthographic projection matrix
        glViewport(0, 0, (GLsizei)fb_width, (GLsizei)fb_height);
        const float ortho_projection[4][4] =
        {
            {2.0f / io.DisplaySize.x, 0.0f,                   0.0f, 0.0f},
            {0.0f,                  2.0f / -io.DisplaySize.y, 0.0f, 0.0f},
            {0.0f,                  0.0f,                  -1.0f, 0.0f},
            {-1.0f,                  1.0f,                   0.0f, 1.0f},
        };
        glUseProgram(g_ShaderHandle);
        glUniform1i(g_AttribLocationTex, 0);
        glUniformMatrix4fv(g_AttribLocationProjMtx, 1, GL_FALSE, &ortho_projection[0][0]);
        glBindVertexArray(g_VaoHandle);

        for(int n = 0; n < draw_data->CmdListsCount; n++)
        {
            const ImDrawList* cmd_list = draw_data->CmdLists[n];
            const ImDrawIdx* idx_buffer_offset = 0;

            glBindBuffer(GL_ARRAY_BUFFER, g_VboHandle);
            glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)cmd_list->VtxBuffer.size() * sizeof(ImDrawVert), (GLvoid*)&cmd_list->VtxBuffer.front(), GL_STREAM_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_ElementsHandle);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)cmd_list->IdxBuffer.size() * sizeof(ImDrawIdx), (GLvoid*)&cmd_list->IdxBuffer.front(), GL_STREAM_DRAW);

            for(const ImDrawCmd* pcmd = cmd_list->CmdBuffer.begin(); pcmd != cmd_list->CmdBuffer.end(); pcmd++)
            {
                if(pcmd->UserCallback)
                {
                    pcmd->UserCallback(cmd_list, pcmd);
                }
                else
                {
                    glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)pcmd->TextureId);
                    glScissor((int)pcmd->ClipRect.x, (int)(fb_height - pcmd->ClipRect.w), (int)(pcmd->ClipRect.z - pcmd->ClipRect.x), (int)(pcmd->ClipRect.w - pcmd->ClipRect.y));
                    glDrawElements(GL_TRIANGLES, (GLsizei)pcmd->ElemCount, sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, idx_buffer_offset);
                }
                idx_buffer_offset += pcmd->ElemCount;
            }
        }

        // Restore modified GL state
        glUseProgram(last_program);
        glActiveTexture(last_active_texture);
        glBindTexture(GL_TEXTURE_2D, last_texture);
        glBindVertexArray(last_vertex_array);
        glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, last_element_array_buffer);
        glBlendEquationSeparate(last_blend_equation_rgb, last_blend_equation_alpha);
        glBlendFunc(last_blend_src, last_blend_dst);
        if(last_enable_blend) glEnable(GL_BLEND); else glDisable(GL_BLEND);
        if(last_enable_cull_face) glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE);
        if(last_enable_depth_test) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
        if(last_enable_scissor_test) glEnable(GL_SCISSOR_TEST); else glDisable(GL_SCISSOR_TEST);
        glViewport(last_viewport[0], last_viewport[1], (GLsizei)last_viewport[2], (GLsizei)last_viewport[3]);
    }

    void IMGuiCreateFontsTexture()
    {
        // Build texture atlas
        ImGuiIO& io = ImGui::GetIO();
        unsigned char* pixels;
        int width, height;
        io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);   // Load as RGBA 32-bits for OpenGL3 demo because it is more likely to be compatible with user's existing shader.

                                                                  // Upload texture to graphics system
        GLint last_texture;
        glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
        glGenTextures(1, &g_FontTexture);
        glBindTexture(GL_TEXTURE_2D, g_FontTexture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

        // Store our identifier
        io.Fonts->TexID = (void *)(intptr_t)g_FontTexture;

        // Restore state
        glBindTexture(GL_TEXTURE_2D, last_texture);
    }

    void IMGuiCreateDeviceObject()
    {
        // Backup GL state
        GLint last_texture, last_array_buffer, last_vertex_array;
        glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
        glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
        glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vertex_array);

        const GLchar *vertex_shader =
            "#version 330\n"
            "uniform mat4 ProjMtx;\n"
            "in vec2 Position;\n"
            "in vec2 UV;\n"
            "in vec4 Color;\n"
            "out vec2 Frag_UV;\n"
            "out vec4 Frag_Color;\n"
            "void main()\n"
            "{\n"
            "	Frag_UV = UV;\n"
            "	Frag_Color = Color;\n"
            "	gl_Position = ProjMtx * vec4(Position.xy,0,1);\n"
            "}\n";

        const GLchar* fragment_shader =
            "#version 330\n"
            "uniform sampler2D Texture;\n"
            "in vec2 Frag_UV;\n"
            "in vec4 Frag_Color;\n"
            "out vec4 Out_Color;\n"
            "void main()\n"
            "{\n"
            "	Out_Color = Frag_Color * texture( Texture, Frag_UV.st);\n"
            "}\n";

        g_ShaderHandle = glCreateProgram();
        g_VertHandle = glCreateShader(GL_VERTEX_SHADER);
        g_FragHandle = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(g_VertHandle, 1, &vertex_shader, 0);
        glShaderSource(g_FragHandle, 1, &fragment_shader, 0);
        glCompileShader(g_VertHandle);
        glCompileShader(g_FragHandle);
        glAttachShader(g_ShaderHandle, g_VertHandle);
        glAttachShader(g_ShaderHandle, g_FragHandle);
        glLinkProgram(g_ShaderHandle);

        g_AttribLocationTex = glGetUniformLocation(g_ShaderHandle, "Texture");
        g_AttribLocationProjMtx = glGetUniformLocation(g_ShaderHandle, "ProjMtx");
        g_AttribLocationPosition = glGetAttribLocation(g_ShaderHandle, "Position");
        g_AttribLocationUV = glGetAttribLocation(g_ShaderHandle, "UV");
        g_AttribLocationColor = glGetAttribLocation(g_ShaderHandle, "Color");

        glGenBuffers(1, &g_VboHandle);
        glGenBuffers(1, &g_ElementsHandle);

        glGenVertexArrays(1, &g_VaoHandle);
        glBindVertexArray(g_VaoHandle);
        glBindBuffer(GL_ARRAY_BUFFER, g_VboHandle);
        glEnableVertexAttribArray(g_AttribLocationPosition);
        glEnableVertexAttribArray(g_AttribLocationUV);
        glEnableVertexAttribArray(g_AttribLocationColor);

#define OFFSETOF(TYPE, ELEMENT) ((size_t)&(((TYPE *)0)->ELEMENT))
        glVertexAttribPointer(g_AttribLocationPosition, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)OFFSETOF(ImDrawVert, pos));
        glVertexAttribPointer(g_AttribLocationUV, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)OFFSETOF(ImDrawVert, uv));
        glVertexAttribPointer(g_AttribLocationColor, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(ImDrawVert), (GLvoid*)OFFSETOF(ImDrawVert, col));
#undef OFFSETOF

        IMGuiCreateFontsTexture();

        // Restore modified GL state
        glBindTexture(GL_TEXTURE_2D, last_texture);
        glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
        glBindVertexArray(last_vertex_array);
    }

    void IMGuiNewFrame()
    {
        if(!g_FontTexture)
            IMGuiCreateDeviceObject();

        ImGuiIO& io = ImGui::GetIO();

        // Setup display size (every frame to accommodate for window resizing)
        auto size = window->GetSize();
        io.DisplaySize = ImVec2((float)size.Width, (float)size.Height);
        io.DisplayFramebufferScale = ImVec2(size.Width > 0 ? 1 : 0, size.Height > 0 ? 1 : 0);

        // Setup time step
        RF_Type::Float32 current_time = m_FPS.Delta();
        io.DeltaTime = m_FPS.Delta();
        g_Time = current_time;

        // Setup inputs
        // (we already got mouse wheel, keyboard keys & characters from glfw callbacks polled in glfwPollEvents())
        
        if(window->HasFocus())
        {
            auto cursorposition = window->GetCursorPosition();
            io.MousePos = ImVec2((float)cursorposition.X, (float)cursorposition.Y);   // Mouse position in screen coordinates (set to -1,-1 if no mouse / on another screen, etc.)
        }
        else
        {
            io.MousePos = ImVec2(-1, -1);
        }

        RF_IO::VirtualMouseButton::Type mouseButtons[RF_IO::VirtualMouseButton::MAX] = {RF_IO::VirtualMouseButton::Left,
            RF_IO::VirtualMouseButton::Right, RF_IO::VirtualMouseButton::Middle};
        for(int i = 0; i < 3; i++)
        {
            io.MouseDown[i] = g_MousePressed[i] || window->Backend()->GetService()->IsMouseButtonPressed(mouseButtons[i]);    // If a mouse press event came, always pass it as "mouse held this frame", so we don't miss click-release events that are shorter than 1 frame.
            g_MousePressed[i] = false;
        }

        io.MouseWheel = g_MouseWheel;
        g_MouseWheel = 0.0f;

        // Hide OS mouse cursor if ImGui is drawing it
        window->CursorVisible(io.MouseDrawCursor);

        // Start the frame
        ImGui::NewFrame();
    }

    void Run()
    {
        m_FPS.Update();

        glBegin(GL_TRIANGLES);
        glColor3f(1, 0, 0);
        glVertex3f(-1, 0, 0);
        glVertex3f(0, 1, 0);
        glVertex3f(1, 0, 0);
        glEnd();

        IMGuiNewFrame();
        if(ImGui::BeginMainMenuBar())
        {
            if(ImGui::BeginMenu("File"))
            {
                ImGui::MenuItem("New");
                ImGui::MenuItem("Open");
                ImGui::MenuItem("Save");
                ImGui::Separator();
                ImGui::MenuItem("Exit");
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

        const char* listbox_items[] = {"Apple", "Banana", "Cherry", "Kiwi", "Mango", "Orange", "Pineapple", "Strawberry", "Watermelon"};
        const char* states[] = {"ZPass", "Shadow", "UI", "Albedo", "Normal"};
        static int listbox_item_current = 1;
        static int selectedState = 1;
        static int selectedFunction = 1;
        static char str0[128] = "Hello, world!";

        ImGui::Begin("GLVM-Codegen", 0, ImGuiWindowFlags_AlwaysAutoResize);

        ImGui::BeginGroup();
        ImGui::InputText("", str0, countof(str0));
        ImGui::PushID(1);
        ImGui::ListBox("", &listbox_item_current, listbox_items, countof(listbox_items));
        ImGui::PopID();
        ImGui::EndGroup();

        ImGui::SameLine(); ImGui::BeginGroup();
        ImGui::Combo("State", &selectedState, states, countof(states));
        ImGui::SameLine(); ImGui::Button("+");
        ImGui::SameLine(); ImGui::Button("-");
        ImGui::PushID(2);
        ImGui::ListBox("", &selectedFunction, listbox_items, countof(listbox_items));
        ImGui::PopID();
        ImGui::EndGroup();

        ImGui::SameLine();
        ImGui::BeginChild(3, ImVec2(ImGui::GetWindowContentRegionWidth() * 0.3f, 0), false, ImGuiWindowFlags_HorizontalScrollbar);
        // header
        ImGui::Columns(2);
        ImGui::Text("Name");
        ImGui::NextColumn();
        ImGui::Text("Value");
        ImGui::Separator();
        ImGui::NextColumn();
        // data
        ImGui::Text("1");
        ImGui::Text("2");
        ImGui::Text("3");
        ImGui::Text("4");
        ImGui::Text("5");
        ImGui::Text("6");
        ImGui::Text("7");
        ImGui::Text("8");
        ImGui::Text("9");
        ImGui::Text("10");
        ImGui::Text("11");
        ImGui::Text("12");
        ImGui::Text("13");
        ImGui::Text("14");
        ImGui::Text("15");
        ImGui::Text("16");

        ImGui::NextColumn();

        ImGui::Text("a");
        ImGui::Text("b");
        ImGui::Text("c");
        ImGui::Text("d");
        ImGui::Text("e");
        ImGui::Text("f");
        ImGui::Text("g");
        ImGui::Text("h");
        ImGui::Text("i");
        ImGui::Text("j");
        ImGui::Text("k");
        ImGui::Text("l");
        ImGui::Text("m");
        ImGui::Text("n");
        ImGui::Text("o");
        ImGui::Text("p");

        ImGui::EndChild();

        ImGui::End();

        bool debug = true;
        ImGui::ShowMetricsWindow(&debug);

        ImGui::Render();
    }
private:
    RF_Type::UInt32 g_FontTexture = 0;    
    RF_Type::Int32 g_AttribLocationPosition = 0, g_AttribLocationUV = 0, g_AttribLocationColor = 0;    
    RF_Mem::AutoPointer<RGLStudio::MainWindow> window;
    RF_Time::Framecounter m_FPS;
    RF_Type::Float32 g_Time = 0.0f;
    RF_Type::Float32 g_MouseWheel = 0.0f;
    RF_Type::Bool g_MousePressed[3] = {false, false, false};
};

int main()
{
    RadonFramework::Radon framework;
    RF_Mem::AutoPointer<RF_Diag::Appender> console(new RF_IO::LogConsole);
    RF_IO::Log::AddAppender(console);

    if(RF_Form::WindowServiceLocator::Default().Application()->IsRunningOnDesktop())
        RF_Form::WindowServiceLocator::Default().Application()->ShowConsole(false);

    MainLoop loop;
    loop.Start();

    RF_Pattern::Singleton<RF_Thread::ThreadPool>::GetInstance().WaitTillDoneWithInactiveQueue();
    return 0;
}