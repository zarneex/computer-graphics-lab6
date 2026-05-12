#define GLFW_DLL
#define GLEW_DLL

#include <iostream>
#include <cmath>

#include "glew-2.1.0/include/GL/glew.h"
#include "glfw-3.4.bin.WIN64/include/GLFW/glfw3.h"
#include "Shader.h"
#include "Model.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 200.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float cameraSpeed = 0.5f;
float yaw = -90.0f;
float pitch = 0.0f;
float lastX = 512, lastY = 512;
bool firstMouse = true;

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos; lastY = ypos;
        firstMouse = false;
    }
    float xoffset = (xpos - lastX) * 0.02f;
    float yoffset = (lastY - ypos) * 0.02f;
    lastX = xpos; lastY = ypos;

    yaw += xoffset;
    pitch += yoffset;
    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

int main() {
    if (!glfwInit()) return 1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* Okno = glfwCreateWindow(1024, 1024, "Lab6", NULL, NULL);
    if (!Okno) { glfwTerminate(); return 1; }
    glfwMakeContextCurrent(Okno);

    glewExperimental = GL_TRUE;
    glewInit();

    glfwSetCursorPosCallback(Okno, mouse_callback);
    glfwSetInputMode(Okno, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glEnable(GL_DEPTH_TEST);

    Shader lightShader("triangle.vert", "triangle.frag");
    Model  ourModel("model.obj");

    glm::vec3 lightPos(2.0f, 3.0f, 4.0f);

    while (!glfwWindowShouldClose(Okno)) {
        processInput(Okno);

        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        lightShader.use();

        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 1000.0f);

        lightShader.setMat4("model", model);
        lightShader.setMat4("view", view);
        lightShader.setMat4("projection", projection);

        glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(model)));
        lightShader.setMat3("normalMatrix", normalMatrix);

        lightShader.setVec3("material.ambient", 1.0f, 0.5f, 0.31f);
        lightShader.setVec3("material.diffuse", 1.0f, 0.5f, 0.31f);
        lightShader.setVec3("material.specular", 0.5f, 0.5f, 0.5f);
        lightShader.setFloat("material.shininess", 32.0f);

        lightShader.setVec3("light.position", lightPos.x, lightPos.y, lightPos.z);
        lightShader.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
        lightShader.setVec3("light.diffuse", 0.8f, 0.8f, 0.8f);
        lightShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);

        lightShader.setVec3("viewPos", cameraPos.x, cameraPos.y, cameraPos.z);

        ourModel.Draw();

        glfwSwapBuffers(Okno);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}