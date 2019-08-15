#ifndef CAMERA_H
#define CAMERA_H

#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "Maze.h"
#include <vector>
#include <cmath>
#include <algorithm>
// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

// Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;


// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
	// Camera Attributes
	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;
	// Euler Angles
	float Yaw;
	float Pitch;
	// Camera options
	float MovementSpeed;
	float MouseSensitivity;
	float Zoom;

	// Constructor with vectors
	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
	{
		Position = position;
		WorldUp = up;
		Yaw = yaw;
		Pitch = pitch;
		updateCameraVectors();
	}
	// Constructor with scalar values
	Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
	{
		Position = glm::vec3(posX, posY, posZ);
		WorldUp = glm::vec3(upX, upY, upZ);
		Yaw = yaw;
		Pitch = pitch;
		updateCameraVectors();
	}

	// Returns the view matrix calculated using Euler Angles and the LookAt Matrix
	glm::mat4 GetViewMatrix()
	{
		return glm::lookAt(Position, Position + Front, Up);
	}

  std::vector<int> hitMaze(Maze& m)
  {
    int limit = 0.05;
    std::vector<int> test;
    for (int row = 0; row < m.data.size(); row++)
      {
	for (int col = 0; col < m.data[row].size(); col++)
	  {
	    if (!m.data[row][col] && this->Position.x > row - 0.5 - limit /*+x*/
		&& this->Position.x < row + 0.5 + limit /*-x*/
		&& this->Position.z < -1.0*col + 0.5 + limit /*-z,max*/
		&& this->Position.z > -1.0*col - 0.5 - limit/*+z, min*/) // in vicinity of wall
	      {
		// close to wall, now check particular direction
		if (-col < this->Position.z) // N block
		  {
		    test.push_back(0);
		  }
		if (-col > this->Position.z)
		  {
		    test.push_back(2);
		  }
		if (row > this->Position.x)
		  {
		    test.push_back(1);
		  }
		if (row < this->Position.x)
		  {
		    test.push_back(3);
		  }
	      }
	  }
      }
    for (int i : test)
      {
	//std::cout << i << " ";
      }
    return test;
  }

  
	// Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
  void ProcessKeyboard(Camera_Movement direction, float deltaTime, Maze& m)
	{
		float velocity = MovementSpeed * deltaTime;
		std::vector<int> constrain = hitMaze(m);
		
		if (std::find(constrain.begin(), constrain.end(), 0) == constrain.end())
		  { // n free , flip signs since z negative
		    if (direction == FORWARD && glm::dot(Front, glm::vec3(0.0f,0.0f,-1.0f)) > 0)
		      {
			Position.z -= velocity*glm::dot(Front, glm::vec3(0.0f,0.0f,-1.0f));
		      }
		    if (direction == BACKWARD && glm::dot(Front, glm::vec3(0.0f,0.0f,-1.0f)) < 0)
		      {
			Position.z += velocity*glm::dot(Front, glm::vec3(0.0f,0.0f,-1.0f));
		      }
		    if (direction == LEFT && glm::dot(Right, glm::vec3(0.0f,0.0f,-1.0f)) < 0)
		      {
			Position.z += velocity*glm::dot(Right, glm::vec3(0.0f,0.0f,-1.0f));
		      }
		    if (direction == RIGHT && glm::dot(Right, glm::vec3(0.0f,0.0f,-1.0f)) > 0)
		      {
			Position.z -= velocity*glm::dot(Right, glm::vec3(0.0f,0.0f,-1.0f));
		      }
		  }

		if (std::find(constrain.begin(), constrain.end(), 1) == constrain.end())
		  { // e free no change x positive
		    if (direction == FORWARD && glm::dot(Front, glm::vec3(1.0f,0.0f,0.0f)) > 0)
		      {
			Position.x += velocity*glm::dot(Front, glm::vec3(1.0f,0.0f,0.0f));
		      }
		    if (direction == BACKWARD && glm::dot(Front, glm::vec3(0.0f,0.0f,-1.0f)) < 0)
		      {
			Position.x -= velocity*glm::dot(Front, glm::vec3(1.0f,0.0f,0.0f));
		      }
		    if (direction == LEFT && glm::dot(Right, glm::vec3(1.0f,0.0f,0.0f)) < 0)
		      {
			Position.x -= velocity*glm::dot(Right, glm::vec3(1.0f,0.0f,0.0f));
		      }
		    if (direction == RIGHT && glm::dot(Right, glm::vec3(1.0f,0.0f,0.0f)) > 0)
		      {
			Position.x += velocity*glm::dot(Right, glm::vec3(1.0f,0.0f,0.0f));
		      }
		  }
		if (std::find(constrain.begin(), constrain.end(), 2) == constrain.end())
		  { // s free, z positive
		    if (direction == FORWARD && glm::dot(Front, glm::vec3(0.0f,0.0f,1.0f)) > 0)
		      {
			Position.z += velocity*glm::dot(Front, glm::vec3(0.0f,0.0f,1.0f));
		      }
		    if (direction == BACKWARD && glm::dot(Front, glm::vec3(0.0f,0.0f,1.0f)) < 0)
		      {
			Position.z -= velocity*glm::dot(Front, glm::vec3(0.0f,0.0f,1.0f));
		      }
		    if (direction == LEFT && glm::dot(Right, glm::vec3(0.0f,0.0f,1.0f)) < 0)
		      {
			Position.z -= velocity*glm::dot(Right, glm::vec3(0.0f,0.0f,1.0f));
		      }
		    if (direction == RIGHT && glm::dot(Right, glm::vec3(0.0f,0.0f,1.0f)) > 0)
		      {
			Position.z += velocity*glm::dot(Right, glm::vec3(0.0f,0.0f,1.0f));
		      }
		  }
		if (std::find(constrain.begin(), constrain.end(), 0) == constrain.end())
		  { // w free, x negative
		    if (direction == FORWARD && glm::dot(Front, glm::vec3(-1.0f,0.0f,0.0f)) > 0)
		      {
			Position.x -= velocity*glm::dot(Front, glm::vec3(-1.0f,0.0f,0.0f));
		      }
		    if (direction == BACKWARD && glm::dot(Front, glm::vec3(-1.0f,0.0f,0.0f)) < 0)
		      {
			Position.x += velocity*glm::dot(Front, glm::vec3(-1.0f,0.0f,0.0f));
		      }
		    if (direction == LEFT && glm::dot(Right, glm::vec3(-1.0f,0.0f,0.0f)) < 0)
		      {
			Position.x += velocity*glm::dot(Right, glm::vec3(-1.0f,0.0f,0.0f));
		      }
		    if (direction == RIGHT && glm::dot(Right, glm::vec3(-1.0f,0.0f,0.0f)) > 0)
		      {
			Position.x -= velocity*glm::dot(Right, glm::vec3(-1.0f,0.0f,0.0f));
		      }
		  }

		
		Position.y = 0;

		
	}

	// Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
	void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
	{
		xoffset *= MouseSensitivity;
		yoffset *= MouseSensitivity;

		Yaw += xoffset;
		Pitch += yoffset;

		// Make sure that when pitch is out of bounds, screen doesn't get flipped
		if (constrainPitch)
		{
			if (Pitch > 89.0f)
				Pitch = 89.0f;
			if (Pitch < -89.0f)
				Pitch = -89.0f;
		}

		// Update Front, Right and Up Vectors using the updated Euler angles
		updateCameraVectors();
	}

	// Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
	void ProcessMouseScroll(float yoffset)
	{
		if (Zoom >= 1.0f && Zoom <= 45.0f)
			Zoom -= yoffset;
		if (Zoom <= 1.0f)
			Zoom = 1.0f;
		if (Zoom >= 45.0f)
			Zoom = 45.0f;
	}

private:
	// Calculates the front vector from the Camera's (updated) Euler Angles
	void updateCameraVectors()
	{
		// Calculate the new Front vector
		glm::vec3 front;
		front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		front.y = sin(glm::radians(Pitch));
		front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		Front = glm::normalize(front);
		// Also re-calculate the Right and Up vector
		Right = glm::normalize(glm::cross(Front, WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
		Up = glm::normalize(glm::cross(Right, Front));
	}
};
#endif
