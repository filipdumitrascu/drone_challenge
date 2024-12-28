#include "core/world.h"

#include "core/engine.h"
#include "components/camera_input.h"
#include "components/transform.h"


World::World()
{
    previousTime = 0;
    elapsedTime = 0;
    deltaTime = 0;
    paused = false;
    shouldClose = false;

    window = Engine::GetWindow();
}


void World::Run()
{
    if (!window)
        return;

    while (!window->ShouldClose())
    {
        LoopUpdate();
    }
}


void World::Pause()
{
    paused = !paused;
}


void World::Exit()
{
    shouldClose = true;
    window->Close();
}


double World::GetLastFrameTime()
{
    return deltaTime;
}


void World::ComputeFrameDeltaTime()
{
    elapsedTime = Engine::GetElapsedTime();
    deltaTime = elapsedTime - previousTime;
    previousTime = elapsedTime;
}

void World::LoopUpdate()
{
    // Polls and buffers the events
    window->PollEvents();               // all the events from the window are saved in order to be prelucrated after 

    // Computes frame deltaTime in seconds
    ComputeFrameDeltaTime();                // calculates fps (framerate on this iteration) from the last iteration   

    // Calls the methods of the instance of InputController in the following order
    // OnWindowResize, OnMouseMove, OnMouseBtnPress, OnMouseBtnRelease, OnMouseScroll, OnKeyPress, OnMouseScroll, OnInputUpdate
    // OnInputUpdate will be called each frame, the other functions are called only if an event is registered
    window->UpdateObservers();          // prelucrates the events

    // Frame processing
    FrameStart();                               // updates the variables
    Update(static_cast<float>(deltaTime));      // prelucrates the frame, drawing commands are executed
    FrameEnd();                                 // the grid, xoy system

    // Swap front and back buffers - image will be displayed to the screen
    window->SwapBuffers();                      // one buffer? flickering, the pixels are modified while the image is shown
}                                               // at least 2 buffers:  one already complete on the screen, one prelucrated
                                                // after the prelucr for the 2nd one is done, swap them 
