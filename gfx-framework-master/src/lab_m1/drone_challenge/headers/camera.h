#ifndef CAMERA_H
#define CAMERA_H

#include "utils/glm_utils.h"
#include "utils/math_utils.h"

namespace camera
{
    class Camera
    {
     public:
        Camera()
        {
            position    = glm::vec3(0, 0, 0);
            forward     = glm::vec3(0, 0, -1);

            up          = glm::vec3(0, 1, 0);
            right       = glm::vec3(1, 0, 0);

            distanceToTarget = 5.0f;
            projectionMatrix = glm::mat4(1);
        }

        Camera(const glm::vec3 &position, const glm::vec3 &center, const glm::vec3 &up)
        {
            Set(position, center, up);
        }

        ~Camera()
        { }

        void Update(float yawAngle) {
            forward.x = -sin(yawAngle);
            forward.z = -cos(yawAngle);

            forward = glm::normalize(forward);
            right = glm::normalize(glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f)));
        }

        void Set(const glm::vec3 &position, const glm::vec3 &center, const glm::vec3 &up)
        {
            this->position = position;
            forward     = glm::normalize(center - position);
            right       = glm::cross(forward, up);
            this->up    = glm::cross(right, forward);
        }

        void SetProjectionMatrix(const glm::mat4& projection)
        {
            projectionMatrix = projection;
        }

        void MoveForward(float distance)
        {
            glm::vec3 dir = glm::normalize(glm::vec3(forward.x, 0, forward.z));
            position += dir * distance;
        }

        void TranslateForward(float distance)
        {
            position += glm::normalize(glm::vec3(forward.x, forward.y, forward.z)) * distance;
        }

        void TranslateUpward(float distance)
        {
            position += glm::normalize(up) * distance;
        }

        void TranslateRight(float distance)
        {
            position += glm::normalize(glm::vec3(right.x, 0, right.z)) * distance;
        }

        void RotateFirstPerson_OX(float angle)
        {
            forward = glm::normalize(glm::vec3(glm::rotate(glm::mat4(1), angle, right) * glm::vec4(forward, 1)));
            up = glm::normalize(glm::cross(right, forward));
        }

        void RotateFirstPerson_OY(float angle)
        {
            forward = glm::normalize(glm::vec3(glm::rotate(glm::mat4(1), angle, glm::vec3(0, 1, 0)) * glm::vec4(forward, 1)));
            right = glm::normalize(glm::vec3(glm::rotate(glm::mat4(1), angle, glm::vec3(0, 1, 0)) * glm::vec4(right, 1)));
            up = glm::normalize(glm::cross(right, forward));
        }

        void RotateFirstPerson_OZ(float angle)
        {
            right = glm::normalize(glm::vec3(glm::rotate(glm::mat4(1), angle, forward) * glm::vec4(right, 1)));
            up = glm::normalize(glm::cross(right, forward));
        }

        void RotateThirdPerson_OX(float angle)
        {
            position += forward * distanceToTarget;
            RotateFirstPerson_OX(angle);
            position -= forward * distanceToTarget;
        }

        void RotateThirdPerson_OY(float angle)
        {
            position += forward * distanceToTarget;
            RotateFirstPerson_OY(angle);
            position -= forward * distanceToTarget;
        }

        void RotateThirdPerson_OZ(float angle)
        {
            position += forward * distanceToTarget;
            RotateFirstPerson_OZ(angle);
            position -= forward * distanceToTarget;
        }

        glm::mat4 GetViewMatrix() const
        {
            return glm::lookAt(position, position + forward, up);
        }

        glm::mat4 GetProjectionMatrix() const
        {
            return projectionMatrix;
        }

        glm::vec3 GetTargetPosition() const
        {
            return position + forward * distanceToTarget;
        }

    public:
        glm::mat4 projectionMatrix;
        float distanceToTarget;

        glm::vec3 position;
        glm::vec3 forward;

        glm::vec3 right;
        glm::vec3 up;
    };
}

#endif // !CAMERA_H
