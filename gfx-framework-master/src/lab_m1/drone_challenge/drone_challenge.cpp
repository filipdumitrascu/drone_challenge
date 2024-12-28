#include "headers/drone_challenge.h"
#include "headers/literals.h"
#include "headers/objects3D.h"
#include "headers/obstacles.h"
#include "headers/drone.h"

#include <vector>
#include <string>
#include <iostream>

using namespace m1;

DroneChallenge::DroneChallenge()
{
    droneCamera = nullptr;
    miniMapCamera = nullptr;

    leftFrontPropellerAngle = RADIANS(0.0f);
    leftRearPropellerAngle = RADIANS(0.0f);
    rightFrontPropellerAngle = RADIANS(0.0f);
    rightRearPropellerAngle = RADIANS(0.0f);

    dronePos = glm::vec3(0.0f, lit::maxObsHeight, lit::fieldZ / 2.0f - 5.0f);
    fieldSeed = 0.0f;

    yawAngle = RADIANS(0.0f);
    pitchAngle = RADIANS(0.0f);
    rollAngle = RADIANS(0.0f);

    xoyTiltLvl = 0;
    zoneIndex = 0;

    packageIndex = lit::numOfZones;
    arrowIndex = lit::numOfZones;

    packageStatus = PackageStatus::FREE;
    pickupTime = true;
}

DroneChallenge::~DroneChallenge()
{
}

void DroneChallenge::Init()
{
    droneCamera = new camera::Camera();
    glm::vec3 cameraPos = dronePos - droneCamera->forward * droneCamera->distanceToTarget + glm::vec3(0.0f, 1.0f, 0.0f);

    droneCamera->Set(cameraPos, dronePos, glm::vec3(0.0f, 1.0f, 0.0f));
    droneCamera->SetProjectionMatrix(glm::perspective(lit::fov, window->props.aspectRatio, lit::near, lit::far));

    miniMapCamera = new camera::Camera();
    miniMapCamera->Set(glm::vec3(0.0f, 50.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));

    miniMapCamera->SetProjectionMatrix(glm::ortho(
        -lit::fieldX / 2.0f, lit::fieldX / 2.0f,
        -lit::fieldZ / 2.0f, lit::fieldZ / 2.0f,
        lit::zNear, lit::zFar));

    Shader* shader = new Shader("FieldShader");
    shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "drone_challenge", "shaders", "VertexShader.glsl"), GL_VERTEX_SHADER);
    shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "drone_challenge", "shaders", "FragmentShader.glsl"), GL_FRAGMENT_SHADER);
    shader->CreateAndLink();
    shaders[shader->GetName()] = shader;

    treesAndHouses = obstacle::GeneratePositionsAndSizes(lit::numOfObstacles, packagesAndZone);

    Mesh* field = objects3D::CreateField("field", lit::origin);
    AddMeshToList(field);

    Mesh* treeTrunk = objects3D::CreateTreeTrunk("treeTrunk", lit::origin, lit::darkBrown);
    AddMeshToList(treeTrunk);

    Mesh* treeCrown = objects3D::CreateTreeCrown("treeCrown", lit::origin, lit::green);
    AddMeshToList(treeCrown);

    Mesh* droneBody = objects3D::CreateDroneBody("droneBody", lit::origin, lit::gray);
    AddMeshToList(droneBody);

    Mesh* dronePropeller = objects3D::CreateDronePropeller("dronePropeller", lit::origin, lit::black);
    AddMeshToList(dronePropeller);

    Mesh* houseBody = objects3D::CreateCube("houseBody", lit::houseSide, lit::cream);
    AddMeshToList(houseBody);

    Mesh* houseRoof = objects3D::CreateHouseRoof("houseRoof", lit::scarletRed);
    AddMeshToList(houseRoof);

    Mesh* package = objects3D::CreateCube("package", lit::packageSide, lit::lightBrown);
    AddMeshToList(package);

    Mesh* deliveryZone = objects3D::CreateSquare("deliveryZone", lit::origin, lit::squareSide, lit::lightBrown, false);
    AddMeshToList(deliveryZone);

    Mesh* arrow = objects3D::CreateArrow("arrow", lit::origin, lit::scarletRed);
    AddMeshToList(arrow);

    Mesh* indicator = objects3D::CreateTriangle("indicator", lit::origin, lit::lightBrown);
    AddMeshToList(indicator);
}

void DroneChallenge::FrameStart()
{
    glClearColor(lit::backgroundRed, lit::backgroundGreen, lit::backgroundBlue, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    glViewport(0, 0, resolution.x, resolution.y);
}

void DroneChallenge::Restart()
{
    packagesAndZone.clear();
    treesAndHouses = obstacle::GeneratePositionsAndSizes(lit::numOfObstacles, packagesAndZone);
    fieldSeed = obstacle::RandomFloat(0.25f, 2.0f);

    dronePos = glm::vec3(0.0f, lit::maxObsHeight, lit::fieldZ / 2.0f - 5.0f);
    yawAngle = RADIANS(0.0f);
    droneCamera->Update(yawAngle);

    zoneIndex = 0;
    arrowIndex = lit::numOfZones;
    packageIndex = lit::numOfZones;
}

void DroneChallenge::RenderMesh(Mesh* mesh, Shader* shader, camera::Camera* cam, const glm::mat4& modelMatrix) const
{
    if (!mesh || !shader || !shader->GetProgramID())
        return;

    // Render an object using the specified shader and the specified position
    glUseProgram(shader->program);

    // Bind model matrix
    GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
    glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    // Bind view matrix
    glm::mat4 viewMatrix = cam->GetViewMatrix();
    int loc_view_matrix = glGetUniformLocation(shader->program, "View");
    glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

    // Bind projection matrix
    glm::mat4 projectionMatrix = cam->GetProjectionMatrix();
    int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
    glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    glUniform1f(glGetUniformLocation(shader->program, "seed"), fieldSeed);

    // Draw the object
    glBindVertexArray(mesh->GetBuffers()->m_VAO);
    glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}

void DroneChallenge::RenderDrone(float deltaTimeSeconds, camera::Camera* cam)
{
    glm::mat4 droneBodyMatrix = drone::GenerateDrone(dronePos, pitchAngle, yawAngle, rollAngle);
    RenderMesh(meshes["droneBody"], shaders["VertexColor"], cam, droneBodyMatrix);

    const glm::vec3 front{ glm::vec3(0.0f, lit::droneBodyOY + lit::droneBodyOZ, -lit::droneBodyOX / 2.0f + lit::droneBodyOZ / 2.0f) };
    const glm::vec3 leftFrontPropellerCenter{ objects3D::RotateOY(front, lit::droneAngle) };
    glm::mat4 modelMatrixLeftFrontPropeller = droneBodyMatrix * drone::GeneratePropeller(leftFrontPropellerCenter, leftFrontPropellerAngle);
    RenderMesh(meshes["dronePropeller"], shaders["VertexColor"], droneCamera, modelMatrixLeftFrontPropeller);

    const glm::vec3 left{ glm::vec3(-lit::droneBodyOX / 2.0f + lit::droneBodyOZ / 2.0f, lit::droneBodyOY + lit::droneBodyOZ, 0.0f) };
    const glm::vec3 leftRearPropellerCenter{ objects3D::RotateOY(left, lit::droneAngle) };
    glm::mat4 modelMatrixLeftRearPropeller = droneBodyMatrix * drone::GeneratePropeller(leftRearPropellerCenter, leftRearPropellerAngle);
    RenderMesh(meshes["dronePropeller"], shaders["VertexColor"], droneCamera, modelMatrixLeftRearPropeller);

    const glm::vec3 right{ glm::vec3(lit::droneBodyOX / 2.0f - lit::droneBodyOZ / 2.0f, lit::droneBodyOY + lit::droneBodyOZ, 0.0f) };
    const glm::vec3 rightFrontPropellerCenter{ objects3D::RotateOY(right, lit::droneAngle) };
    glm::mat4 modelMatrixFrontRearPropeller = droneBodyMatrix * drone::GeneratePropeller(rightFrontPropellerCenter, rightFrontPropellerAngle);
    RenderMesh(meshes["dronePropeller"], shaders["VertexColor"], droneCamera, modelMatrixFrontRearPropeller);

    const glm::vec3 back{ glm::vec3(0.0f, lit::droneBodyOY + lit::droneBodyOZ, lit::droneBodyOX / 2.0f - lit::droneBodyOZ / 2.0f) };
    const glm::vec3 rightRearPropellerCenter{ objects3D::RotateOY(back, lit::droneAngle) };
    glm::mat4 modelMatrixRightRearPropeller = droneBodyMatrix * drone::GeneratePropeller(rightRearPropellerCenter, rightRearPropellerAngle);
    RenderMesh(meshes["dronePropeller"], shaders["VertexColor"], droneCamera, modelMatrixRightRearPropeller);
}

void DroneChallenge::RenderObstacles(camera::Camera* cam)
{
    for (const auto& obstacleInfo : treesAndHouses) {
        switch (obstacleInfo.type) {

        case ObstacleType::TREE:
            RenderMesh(meshes["treeTrunk"], shaders["VertexColor"], cam, obstacle::GenerateTree(obstacleInfo).first);
            RenderMesh(meshes["treeCrown"], shaders["VertexColor"], cam, obstacle::GenerateTree(obstacleInfo).second);
            break;

        case ObstacleType::HOUSE:
            RenderMesh(meshes["houseBody"], shaders["VertexColor"], cam, obstacle::GenerateHouse(obstacleInfo));
            RenderMesh(meshes["houseRoof"], shaders["VertexColor"], cam, obstacle::GenerateHouse(obstacleInfo));
            break;

        default:
            break;
        }
    }
}

void DroneChallenge::RenderPackages(camera::Camera* cam)
{
    if (!pickupTime) {
        RenderMesh(meshes["package"], shaders["VertexColor"], cam, drone::GenerateDrone(dronePos, pitchAngle, yawAngle, rollAngle) *
            transforms3D::Translate(0.0f, -lit::packageSide, 0.0f));

        RenderMesh(meshes["deliveryZone"], shaders["VertexColor"], cam, obstacle::GenerateZone(packagesAndZone[zoneIndex]));
        RenderMesh(meshes["indicator"], shaders["VertexColor"], droneCamera,
            drone::GenerateIndicator(dronePos, packagesAndZone[zoneIndex].position));

    } else {
        RenderMesh(meshes["package"], shaders["VertexColor"], cam, obstacle::GeneratePackage(packagesAndZone[packageIndex]));
    }
}

void DroneChallenge::RenderScene(float deltaTimeSeconds, camera::Camera* cam)
{   
    RenderMesh(meshes["field"], shaders["FieldShader"], cam, glm::mat4(1));
    RenderObstacles(cam);

    RenderDrone(deltaTimeSeconds, cam);
    glm::vec3 targetPos{ glm::vec3(packagesAndZone[arrowIndex].position.x, 1.0f, packagesAndZone[arrowIndex].position.y) };

    RenderMesh(meshes["arrow"], shaders["VertexColor"], droneCamera, drone::GenerateArrow(dronePos, targetPos));
    RenderPackages(cam);
}

void DroneChallenge::RenderMinimap(float deltaTimeSeconds, camera::Camera* cam)
{
    glClear(GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    float miniMapSize = resolution.y * 0.3f;

    glViewport(
        (GLint) (resolution.x - miniMapSize),
        (GLint) (resolution.y - miniMapSize),
        (GLsizei) miniMapSize,
        (GLsizei) miniMapSize
    );

    RenderScene(deltaTimeSeconds, cam);
}

void DroneChallenge::Update(float deltaTimeSeconds)
{
    leftFrontPropellerAngle -= RADIANS(2880.0f) * deltaTimeSeconds;
    leftRearPropellerAngle += RADIANS(2880.0f) * deltaTimeSeconds;
    rightFrontPropellerAngle += RADIANS(2880.0f) * deltaTimeSeconds;
    rightRearPropellerAngle -= RADIANS(2880.0f) * deltaTimeSeconds;

    RenderScene(deltaTimeSeconds, droneCamera);
    RenderMinimap(deltaTimeSeconds, miniMapCamera);
}

void DroneChallenge::FrameEnd()
{
}

void DroneChallenge::AngleMovement(glm::vec3& newPos, float deltaTime)
{
    float movement = 5.0f * deltaTime;
    float thrust = 3.0f * deltaTime;

    float yawDisplacement = RADIANS(45.0f) * deltaTime;
    float rollDisplacement = RADIANS(45.0f) * deltaTime;
    float pitchDisplacement = RADIANS(45.0f) * deltaTime;

    // Movement
    if (window->KeyHold(GLFW_KEY_W)) {
        if (window->KeyHold(GLFW_KEY_LEFT)) {
            newPos -= droneCamera->right * movement;
        }
        
        if (window->KeyHold(GLFW_KEY_RIGHT)) {
            newPos += droneCamera->right * movement;
        }
        
        if (window->KeyHold(GLFW_KEY_UP)) {
            newPos += glm::vec3(droneCamera->forward.x, 0.0f, droneCamera->forward.z) * movement;
        }
        
        if (window->KeyHold(GLFW_KEY_DOWN)) {
            newPos -= glm::vec3(droneCamera->forward.x, 0.0f, droneCamera->forward.z) * movement;

        } 
        
        if (!window->KeyHold(GLFW_KEY_UP) && !window->KeyHold(GLFW_KEY_DOWN) &&
            !window->KeyHold(GLFW_KEY_LEFT) && !window->KeyHold(GLFW_KEY_RIGHT)) {
            newPos.y += thrust;
        }
    }

    if (window->KeyHold(GLFW_KEY_S)) {
        newPos.y -= thrust;
    }

    // Yaw angle
    if (window->KeyHold(GLFW_KEY_A)) {
        droneCamera->Update(yawAngle);

        yawAngle += yawDisplacement;
        droneCamera->RotateThirdPerson_OY(yawDisplacement);
    }

    if (window->KeyHold(GLFW_KEY_D)) {
        droneCamera->Update(yawAngle);

        yawAngle -= yawDisplacement;
        droneCamera->RotateThirdPerson_OY(-yawDisplacement);
    }

    // Roll angle
    if (window->KeyHold(GLFW_KEY_LEFT)) {
        droneCamera->Update(yawAngle);
        xoyTiltLvl = 1;

        if (rollAngle < RADIANS(15.0f)) {
            rollAngle += rollDisplacement;
        }

        leftFrontPropellerAngle += RADIANS(1080.0f) * deltaTime;
        leftRearPropellerAngle -= RADIANS(1080.0f) * deltaTime;

        rightFrontPropellerAngle += RADIANS(2880.0f) * deltaTime;
        rightRearPropellerAngle -= RADIANS(2880.0f) * deltaTime;
    }

    if (window->KeyHold(GLFW_KEY_RIGHT)) {
        droneCamera->Update(yawAngle);
        xoyTiltLvl = 1;

        if (rollAngle > RADIANS(-15.0f)) {
            rollAngle -= rollDisplacement;
        }

        leftFrontPropellerAngle -= RADIANS(2880.0f) * deltaTime;
        leftRearPropellerAngle += RADIANS(2880.0f) * deltaTime;

        rightFrontPropellerAngle -= RADIANS(1080.0f) * deltaTime;
        rightRearPropellerAngle += RADIANS(1080.0f) * deltaTime;
    }

    // Pitch angle
    if (window->KeyHold(GLFW_KEY_UP)) {
        droneCamera->Update(yawAngle);
        xoyTiltLvl = 1;

        if (pitchAngle > RADIANS(-15.0f)) {
            pitchAngle -= pitchDisplacement;
        }

        leftFrontPropellerAngle += RADIANS(1080.0f) * deltaTime;
        leftRearPropellerAngle += RADIANS(2880.0f) * deltaTime;

        rightFrontPropellerAngle -= RADIANS(1080.0f) * deltaTime;
        rightRearPropellerAngle -= RADIANS(2880.0f) * deltaTime;
    }

    if (window->KeyHold(GLFW_KEY_DOWN)) {
        droneCamera->Update(yawAngle);
        xoyTiltLvl = 1;

        if (pitchAngle < RADIANS(15.0f)) {
            pitchAngle += pitchDisplacement;
        }

        leftFrontPropellerAngle -= RADIANS(2880.0f) * deltaTime;
        leftRearPropellerAngle -= RADIANS(1080.0f) * deltaTime;

        rightFrontPropellerAngle += RADIANS(2880.0f) * deltaTime;
        rightRearPropellerAngle += RADIANS(1080.0f) * deltaTime;
    }

    // Angle stabilization
    if (!window->KeyHold(GLFW_KEY_LEFT) && !window->KeyHold(GLFW_KEY_RIGHT)) {
        rollAngle > 0.0f ? rollAngle -= rollDisplacement : rollAngle += rollDisplacement;
    }

    if (!window->KeyHold(GLFW_KEY_UP) && !window->KeyHold(GLFW_KEY_DOWN)) {
        pitchAngle > 0.0f ? pitchAngle -= pitchDisplacement : pitchAngle += pitchDisplacement;
    }

    // Level of tiltness
    if ((window->KeyHold(GLFW_KEY_UP) && window->KeyHold(GLFW_KEY_RIGHT)) ||
        (window->KeyHold(GLFW_KEY_UP) && window->KeyHold(GLFW_KEY_LEFT)) ||
        (window->KeyHold(GLFW_KEY_DOWN) && window->KeyHold(GLFW_KEY_RIGHT)) ||
        (window->KeyHold(GLFW_KEY_DOWN) && window->KeyHold(GLFW_KEY_LEFT))) {
        xoyTiltLvl = 2;
    }

    if (!window->KeyHold(GLFW_KEY_UP) && !window->KeyHold(GLFW_KEY_DOWN) &&
        !window->KeyHold(GLFW_KEY_LEFT) && !window->KeyHold(GLFW_KEY_RIGHT)) {
        xoyTiltLvl = 0;
    }
}

void DroneChallenge::OnInputUpdate(float deltaTime, int mods)
{
    glm::vec3 newPos = dronePos;
    AngleMovement(newPos, deltaTime);

    // Package
    glm::vec3 packagePos{glm::vec3(packagesAndZone[packageIndex].position.x, 1.0f, packagesAndZone[packageIndex].position.y)};
    if (packageStatus != PackageStatus::ATTACHED && drone::isDroneCollidingWithCube(newPos, packagePos, xoyTiltLvl, lit::packageSide,
        1.0f, packageStatus)) {
        packageStatus = PackageStatus::COLLIDING;
        return;
    }

    if (pickupTime) {
        packageStatus = PackageStatus::FREE;
    }

    // Field
    if (drone::isDroneCollidingWithField(newPos, xoyTiltLvl, fieldSeed, packageStatus)) {
        return;
    }

    for (const auto& obs : treesAndHouses) {
        if (obs.type == ObstacleType::TREE) {
            // TreeTrunk
            if (drone::isDroneCollidingWithCones(newPos, glm::vec3(obs.position.x, lit::treeTrunkHeight * obs.scaleFactor, obs.position.y), xoyTiltLvl, packageStatus)) {
                return;
            }

            // TreeCrown
            if (drone::isDroneCollidingWithCylinder(newPos, glm::vec3(obs.position.x, 0.0f, obs.position.y), xoyTiltLvl, obs.scaleFactor, packageStatus)) {
                return;
            }

        } else if (obs.type == ObstacleType::HOUSE) {
            // HouseBody
            if (drone::isDroneCollidingWithCube(newPos, glm::vec3(obs.position.x, 0.0f, obs.position.y), xoyTiltLvl, lit::houseSide, obs.scaleFactor, packageStatus)) {
                return;
            }

            // HouseRoof
            if (drone::isDroneCollidingWithPrism(newPos, glm::vec3(obs.position.x, lit::houseSide * obs.scaleFactor, obs.position.y), 
                xoyTiltLvl, obs.scaleFactor, packageStatus)) {
                return;
            }
        }
    }

    dronePos = newPos;
    droneCamera->position = dronePos - droneCamera->forward * droneCamera->distanceToTarget
        + glm::vec3(0.0f, 1.0f, 0.0f);
}

void DroneChallenge::OnKeyPress(int key, int mods)
{
    if (key == GLFW_KEY_R) {
        Restart();
    }

    if (key == GLFW_KEY_SPACE && packageStatus == PackageStatus::COLLIDING) {
        pickupTime = false;
        packageStatus = PackageStatus::ATTACHED;
        arrowIndex -= lit::numOfZones;
    }
    
    glm::vec3 zonePos{ glm::vec3(packagesAndZone[zoneIndex].position.x, 1.0f, packagesAndZone[zoneIndex].position.y) };
    if (key == GLFW_KEY_SPACE && packageStatus == PackageStatus::ATTACHED && drone::isDroneInTheZone(dronePos, zonePos)) {
        pickupTime = true;
        packageStatus = PackageStatus::FREE;

        packageIndex++;
        zoneIndex++;

        arrowIndex += (lit::numOfZones + 1);

        std::cout << "You delivered " << zoneIndex << " package(s) out of " << lit::numOfZones << "!\n";

        if (zoneIndex == lit::numOfZones) {
            std::cout << "Good Job! Game is restarting...\n";
            Restart();
        }
    }
}

void DroneChallenge::OnKeyRelease(int key, int mods)
{
}

void DroneChallenge::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
    {
        float sensivityOX = 0.001f;
        float sensivityOY = 0.001f;

        droneCamera->RotateThirdPerson_OX(-sensivityOY * deltaY);
        droneCamera->RotateThirdPerson_OY(-sensivityOX * deltaX);
    }
}

void DroneChallenge::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
}

void DroneChallenge::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
}

void DroneChallenge::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}

void DroneChallenge::OnWindowResize(int width, int height)
{
}
