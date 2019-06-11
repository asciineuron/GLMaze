#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include "stb_image.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "Shader.h"
#include "Camera.h"
#include <iostream>
#include "Maze.h"
#include <chrono>
#include <thread>


#define CLEAR_COLOR 0.2f, 0.3f, 0.3f, 1.0f

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
GLFWwindow* setup();
void mazeInit();
unsigned int loadCubemap(std::vector<std::string> faces);
bool inCorner();

const int HEIGHT = 600;
const int WIDTH = 800;


Camera camera(glm::vec3(1.0f, 0.0f, -1.0f));
float lastX = WIDTH / 2.0f;
float lastY = HEIGHT / 2.0f;
bool firstMouse = true;
// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

const int MAZE_SIZE = 31;
// will shift cube around for maze
const float CUBE_VERTICES[] =
{
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
	0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
};

const float FLOOR[] =
  { // vertex - texture
   0.5f, -0.5f, 0.5f, 1.0f, 1.0f, // forward right
   0.5f, -0.5f, -0.5f, 1.0f, 0.0f, // back right
   -0.5f, -0.5f, 0.5f, 0.0f, 1.0f, // forward left
   0.5f, -0.5f, -0.5f, 1.0f, 0.0f, // back right
   -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, // back left
   -0.5f, -0.5f, 0.5f, 0.0f, 1.0f // forward left -> describes a cube laying down relative camera
   };

std::vector<float> cubes_shifted{};
std::vector<float> floor_shifted{};

Maze m{MAZE_SIZE};

int main()
{
	GLFWwindow* window = setup();
	mazeInit();
	// skybox file locations
	std::vector<std::string> faces =
	  {

	   "./ame_redplanet/redplanet_rt.tga",
	   	   "./ame_redplanet/redplanet_lf.tga",
	   "./ame_redplanet/redplanet_up.tga",
	   "./ame_redplanet/redplanet_dn.tga",

	   "./ame_redplanet/redplanet_bk.tga",
	   	   "./ame_redplanet/redplanet_ft.tga"
	  };
	unsigned int cubeMapTexture = loadCubemap(faces);
	float skyboxVertices[] =
	  {
	   // positions          
	   -1.0f,  1.0f, -1.0f,
	   -1.0f, -1.0f, -1.0f,
	   1.0f, -1.0f, -1.0f,
	   1.0f, -1.0f, -1.0f,
	   1.0f,  1.0f, -1.0f,
	   -1.0f,  1.0f, -1.0f,
	   
	   -1.0f, -1.0f,  1.0f,
	   -1.0f, -1.0f, -1.0f,
	   -1.0f,  1.0f, -1.0f,
	   -1.0f,  1.0f, -1.0f,
	   -1.0f,  1.0f,  1.0f,
	   -1.0f, -1.0f,  1.0f,
	   
	   1.0f, -1.0f, -1.0f,
	   1.0f, -1.0f,  1.0f,
	   1.0f,  1.0f,  1.0f,
	   1.0f,  1.0f,  1.0f,
	   1.0f,  1.0f, -1.0f,
	   1.0f, -1.0f, -1.0f,
	   
	   -1.0f, -1.0f,  1.0f,
	   -1.0f,  1.0f,  1.0f,
	   1.0f,  1.0f,  1.0f,
	   1.0f,  1.0f,  1.0f,
	   1.0f, -1.0f,  1.0f,
	   -1.0f, -1.0f,  1.0f,
	   
	   -1.0f,  1.0f, -1.0f,
	   1.0f,  1.0f, -1.0f,
	   1.0f,  1.0f,  1.0f,
	   1.0f,  1.0f,  1.0f,
	   -1.0f,  1.0f,  1.0f,
	   -1.0f,  1.0f, -1.0f,
	   
	   -1.0f, -1.0f, -1.0f,
	   -1.0f, -1.0f,  1.0f,
	   1.0f, -1.0f, -1.0f,
	   1.0f, -1.0f, -1.0f,
	   -1.0f, -1.0f,  1.0f,
	   1.0f, -1.0f,  1.0f
	  };
	Shader shaderSky("vertexSky.vs", "fragmentSky.fs");
	// vao vbo for skybox
	unsigned int VAOSKY, VBOSKY;
	glGenVertexArrays(1, &VAOSKY);
	glGenBuffers(1, &VBOSKY);
	glBindVertexArray(VAOSKY);
	glBindBuffer(GL_ARRAY_BUFFER, VBOSKY);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);
	shaderSky.setInt("skybox", 0);
	Shader shader("vertex.vs", "fragment.fs");
	// set up vao, vbo
	unsigned int VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*cubes_shifted.size(), &cubes_shifted[0], GL_STATIC_DRAW);
	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// load texture
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
	unsigned char* data = stbi_load("doomwall.png", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);
	shader.use();
	shader.setInt("texture1", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glBindVertexArray(0);
	// now for floor
	unsigned int VAOFLOOR, VBOFLOOR;
	glGenVertexArrays(1, &VAOFLOOR);
	glGenBuffers(1, &VBOFLOOR);
	glBindVertexArray(VAOFLOOR);
	glBindBuffer(GL_ARRAY_BUFFER, VBOFLOOR);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*floor_shifted.size(), &floor_shifted[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(3*sizeof(float)));
	glEnableVertexAttribArray(1);

	unsigned int texturefloor;
	glGenTextures(1, &texturefloor);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texturefloor);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	data = stbi_load("floor.png", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);
	glBindVertexArray(0);

	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		// --------------------
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		if (inCorner())
		  {
		    std::cout << "you won!, total time taken: " << glfwGetTime() << std::endl;
		    glDeleteVertexArrays(1, &VAO);
		    glDeleteBuffers(1, &VBO);
		    glfwTerminate();
		    return 0;
		  }
		// input
		// -----
		processInput(window);
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// first skybox
		glDepthMask(GL_FALSE);
		shaderSky.use();
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
		shaderSky.setMat4("projection", projection);
		glm::mat4 view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
		shaderSky.setMat4("view", view);
		glBindVertexArray(VAOSKY);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glDepthMask(GL_TRUE);
		// then rest of scene, first walls
		shader.use();
		shader.setMat4("projection", projection);
		glm::mat4 view2 = camera.GetViewMatrix();
		//view = camera.GetViewMatrix();
		shader.setMat4("view", view2);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glBindVertexArray(VAO);
		shader.setInt("texture1", 0);
		glm::mat4 model = glm::mat4(1.0f);
		shader.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, cubes_shifted.size()/5); // five elements per vertex
		// floor
		glBindVertexArray(VAOFLOOR);
		shader.setInt("texture1", 1);
		glDrawArrays(GL_TRIANGLES, 0, floor_shifted.size()/5);
		
		glfwSwapBuffers(window);
		glfwPollEvents();
		  
	}
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	  camera.ProcessKeyboard(FORWARD, deltaTime, m);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	  camera.ProcessKeyboard(BACKWARD, deltaTime, m);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	  camera.ProcessKeyboard(LEFT, deltaTime, m);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	  camera.ProcessKeyboard(RIGHT, deltaTime, m);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

GLFWwindow* setup()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "test01", glfwGetPrimaryMonitor(), NULL);
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "test01", NULL, NULL);
	if (window == NULL)
	{
		printf("Failure to create window\n");
		glfwTerminate();
		return NULL;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		printf("Failure to initialize GLAD\n");
		return NULL;
	}

	glViewport(0, 0, WIDTH, HEIGHT);
	glClearColor(CLEAR_COLOR);
	glEnable(GL_DEPTH_TEST);

	return window;
}

void mazeInit()
{
	// generates maze, adds cube positions based on maze data
	//m = Maze(MAZE_SIZE);
	for (int row = 0; row < MAZE_SIZE; row++)
	{
		for (int col = 0; col < MAZE_SIZE; col++)
		{
			if (!m.data[row][col]) // if wall generate new cube
			{
				// x is row, depth is col, all at same height y
				//cube_positions.push_back(glm::vec3((float)row, 0.0f, -(float)col));
				glm::vec3 shift = glm::vec3((float)row, 0.0f, -(float)col);
				glm::mat4 model = glm::mat4(1.0f);
				model = glm::translate(model, shift);
				for (int i = 0; i < 180; i += 5) // 48 is num vertices + tex for cube
				  {
				    glm::vec4 newVert = glm::vec4(CUBE_VERTICES[i], CUBE_VERTICES[i+1], CUBE_VERTICES[i+2], 1.0);
				    newVert = model*newVert;
				    cubes_shifted.push_back(newVert.x);
				    cubes_shifted.push_back(newVert.y);
				    cubes_shifted.push_back(newVert.z);
				    cubes_shifted.push_back(CUBE_VERTICES[i+3]);
				    cubes_shifted.push_back(CUBE_VERTICES[i+4]); // texcoords
				  }
			}
			else
			  {
			    glm::vec3 shift = glm::vec3((float)row, 0.0f, -(float)col);
			    glm::mat4 model = glm::mat4(1.0f);
			    model = glm::translate(model, shift);
			    for (int i = 0; i < 30; i += 5) // 30 is num vertices + tex for cube
			      {
				glm::vec4 newVert = glm::vec4(FLOOR[i], FLOOR[i+1], FLOOR[i+2], 1.0);
				newVert = model*newVert;
				floor_shifted.push_back(newVert.x);
				floor_shifted.push_back(newVert.y);
				floor_shifted.push_back(newVert.z);
				floor_shifted.push_back(FLOOR[i+3]);
				floor_shifted.push_back(FLOOR[i+4]); // texcoords
			      }
			  }
		}
	}

}

bool inCorner()
{
  return ((int)camera.Position.x == MAZE_SIZE-2 && (int)camera.Position.z == -MAZE_SIZE+2);
}



unsigned int loadCubemap(std::vector<std::string> faces)
{
  unsigned int textureID;
  glGenTextures(1, &textureID);
  glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

  int width, height, nrChannels;
  for (unsigned int i = 0; i < faces.size(); i++)
    {
      unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
      if (data)
	{
	  glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 
		       0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
		       );
	}
      else
	{
	  std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
	}
      stbi_image_free(data); 
    }
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  
  return textureID;
}
