#ifndef DRONE_CHALLENGE_H
#define DRONE_CHALLENGE_H

#include "camera.h"
#include "components/simple_scene.h"

namespace m1
{
    enum ObstacleType {
        TREE,
        HOUSE,
        PACKAGE,
        ZONE
    };

    enum PackageStatus {
        FREE,
        COLLIDING,
        ATTACHED
    };

    struct Obstacle {
        glm::vec2 position;
        float scaleFactor;
        ObstacleType type;

        Obstacle(const glm::vec2& pos, float s, ObstacleType t)
            : position(pos), scaleFactor(s), type(t) {
        }
    };

    class DroneChallenge : public gfxc::SimpleScene
    {
     public:
         DroneChallenge();
        ~DroneChallenge();

        void Init() override;

     private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;

        void Restart();
        void AngleMovement(glm::vec3& newPos, float deltaTime);

        void RenderMesh(Mesh* mesh, Shader* shader, camera::Camera* cam, const glm::mat4& modelMatrix) const;
        void RenderDrone(float deltaTimeSeconds, camera::Camera* cam);

        void RenderObstacles(camera::Camera* cam);
        void RenderPackages(camera::Camera* cam);

        void RenderScene(float deltaTimeSeconds, camera::Camera* cam);
        void RenderMinimap(float deltaTimeSeconds, camera::Camera* cam);

     protected:
        camera::Camera *droneCamera;
        camera::Camera *miniMapCamera;

        std::vector<Obstacle> treesAndHouses;
        std::vector<Obstacle> packagesAndZone;

        float rightFrontPropellerAngle;
        float rightRearPropellerAngle;
        float leftFrontPropellerAngle;
        float leftRearPropellerAngle;

        glm::vec3 dronePos;
        int xoyTiltLvl;

        float fieldSeed;
        PackageStatus packageStatus;

        float pitchAngle;
        float yawAngle;
        float rollAngle;

        bool pickupTime;

        int zoneIndex;
        int packageIndex;

        int arrowIndex;
    };
}

#endif // !DRONE_CHALLENGE_H
