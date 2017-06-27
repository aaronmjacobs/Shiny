#include "Shiny/Scene/CameraComponent.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Shiny {

namespace {

const glm::vec3 kFront(0.0f, 0.0f, -1.0f);
const glm::vec3 kRight(1.0f, 0.0f, 0.0f);
const glm::vec3 kUp(0.0f, 1.0f, 0.0f);

} // namespace

glm::vec3 CameraComponent::getFront() const {
   return kFront * getAbsoluteTransform().orientation;
}

glm::vec3 CameraComponent::getRight() const {
   return kRight * getAbsoluteTransform().orientation;
}

glm::vec3 CameraComponent::getUp() const {
   return kUp * getAbsoluteTransform().orientation;
}

glm::mat4 CameraComponent::getViewMatrix() const {
   Transform absoluteTransform = getAbsoluteTransform();
   return glm::lookAt(absoluteTransform.position, absoluteTransform.position + getFront(), kUp);
}

void CameraComponent::fly(float amount) {
   relativeTransform.position += getFront() * amount;
}

void CameraComponent::strafe(float amount) {
   relativeTransform.position += getRight() * amount;
}

void CameraComponent::rotate(float pitch, float yaw) {
   glm::quat pitchChange = glm::angleAxis(pitch, kRight);
   glm::quat yawChange = glm::angleAxis(yaw, kUp);
   relativeTransform.orientation = glm::normalize(pitchChange * relativeTransform.orientation * yawChange);
}

void CameraComponent::lookAt(const glm::vec3& loc) {
   ASSERT(relativeTransform.position != loc, "Camera trying to look at self");

   Transform absoluteTransform = getAbsoluteTransform();
   absoluteTransform.orientation = glm::toQuat(glm::lookAt(absoluteTransform.position, loc, kUp));
   setAbsoluteTransform(absoluteTransform);
}

SHINY_REGISTER_COMPONENT(CameraComponent)

} // namespace Shiny
