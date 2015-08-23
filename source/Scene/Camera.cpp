#include "Camera.h"

#include "ShinyAssert.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>

namespace Shiny {

namespace {

const glm::vec3 FRONT(0.0f, 0.0f, -1.0f);
const glm::vec3 RIGHT(1.0f, 0.0f, 0.0f);
const glm::vec3 UP(0.0f, 1.0f, 0.0f);

} // namespace

glm::vec3 Camera::getFront() const {
   return FRONT * transform.orientation;
}

glm::vec3 Camera::getRight() const {
   return RIGHT * transform.orientation;
}

glm::vec3 Camera::getUp() const {
   return UP * transform.orientation;
}

glm::mat4 Camera::getViewMatrix() const {
   return glm::lookAt(transform.position, transform.position + getFront(), UP);
}

void Camera::setPosition(const glm::vec3 &pos) {
   transform.position = pos;
}

void Camera::setOrientation(const glm::quat &ori) {
   transform.orientation = glm::normalize(ori);
}

void Camera::fly(float amount) {
   transform.position += getFront() * amount;
}

void Camera::strafe(float amount) {
   transform.position += getRight() * amount;
}

void Camera::rotate(float pitch, float yaw) {
   glm::quat pitchChange = glm::angleAxis(pitch, RIGHT);
   glm::quat yawChange = glm::angleAxis(yaw, UP);
   transform.orientation = glm::normalize(pitchChange * transform.orientation * yawChange);
}

void Camera::lookAt(const glm::vec3 &loc) {
   ASSERT(transform.position != loc, "Camera trying to look at self");

   setOrientation(glm::toQuat(glm::lookAt(transform.position, loc, UP)));
}

} //namespace Shiny
