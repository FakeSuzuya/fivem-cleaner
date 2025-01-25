#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <filesystem>
#include <thread>
#include <chrono>
#include <string>

// Fonction pour configurer le style d'ImGui
void SetupImGuiStyle() {
    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();

    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
    style.Colors[ImGuiCol_Button] = ImVec4(0.5f, 0.0f, 0.5f, 1.0f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.7f, 0.0f, 0.7f, 1.0f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.9f, 0.0f, 0.9f, 1.0f);
}

// Fonction pour obtenir une variable d'environnement de manière sécurisée
std::string GetEnvVar(const char* var) {
    char* buffer;
    size_t size;
    _dupenv_s(&buffer, &size, var);
    std::string result = buffer ? buffer : "";
    free(buffer);
    return result;
}

// Fonction pour supprimer un fichier ou un dossier
void DeletePath(const std::filesystem::path& path) {
    try {
        if (std::filesystem::exists(path)) {
            std::filesystem::remove_all(path);
            std::cout << "Deleted: " << path << std::endl;
        }
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Error deleting " << path << ": " << e.what() << std::endl;
    }
}

// Fonction pour effectuer le nettoyage des fichiers FiveM
void CleanFiveMFiles(bool& cleaning, float& progress, std::string& status_message) {
    cleaning = true;
    status_message = "Cleaning in progress...";
    progress = 0.0f;

    // Chemins des fichiers et dossiers à nettoyer
    std::vector<std::filesystem::path> files_to_clean = {
        std::filesystem::path(GetEnvVar("LOCALAPPDATA")) / "FiveM/FiveM.app/data/cache",
        std::filesystem::path(GetEnvVar("LOCALAPPDATA")) / "FiveM/FiveM.app/data/logs",
        std::filesystem::path(GetEnvVar("LOCALAPPDATA")) / "FiveM/FiveM.app/data/crashes",
        std::filesystem::path(GetEnvVar("LOCALAPPDATA")) / "FiveM/FiveM.app/data/profiles",
        std::filesystem::path(GetEnvVar("LOCALAPPDATA")) / "DigitalEntitlements",
        std::filesystem::path(GetEnvVar("APPDATA")) / "CitizenFX"
    };

    size_t total_files = files_to_clean.size();
    size_t cleaned_files = 0;

    for (const auto& file : files_to_clean) {
        DeletePath(file);
        cleaned_files++;
        progress = static_cast<float>(cleaned_files) / total_files;
        std::this_thread::sleep_for(std::chrono::milliseconds(500)); // Simuler le temps de nettoyage
    }

    cleaning = false;
    status_message = "Cleaning completed successfully!";
}

// Fonction principale pour initialiser et afficher l'interface
int main() {
    if (!glfwInit())
        return -1;

    GLFWwindow* window = glfwCreateWindow(1280, 720, "FiveM Cleaner", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    SetupImGuiStyle();

    bool cleaning = false;
    float progress = 0.0f;
    std::string status_message = "Idle";

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        {
            ImGui::Begin("FiveM Cleaner");

            if (ImGui::Button("Start Cleaning") && !cleaning) {
                std::thread cleaner_thread(CleanFiveMFiles, std::ref(cleaning), std::ref(progress), std::ref(status_message));
                cleaner_thread.detach();
            }

            ImGui::Text("Cleaning status: %s", status_message.c_str());
            ImGui::ProgressBar(progress, ImVec2(0.0f, 0.0f), "");

            ImGui::End();
        }

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}