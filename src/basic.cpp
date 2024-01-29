#include <thread>
#include <mutex>
#include <condition_variable>
#include <cassert>
#include <GLFW/glfw3.h>
#include <mujoco/mujoco.h>
#include <opencv4/opencv2/opencv.hpp>

std::mutex mu;
std::condition_variable condv;
bool ready = false;
#define WIDTH 1280
#define HEIGHT 720
cv::Size video_size(WIDTH, HEIGHT);
cv::Mat video_frame = cv::Mat::zeros(HEIGHT, WIDTH, CV_8UC3);
cv::VideoWriter video("video_out.mp4", cv::CAP_FFMPEG, cv::VideoWriter::fourcc('h', 'e', 'v', '1'), 200.0, video_size);


void runSimulation(mjModel *model, mjData *data)
{
    // Your simulation logic here using model and data
    // This function should handle the simulation steps
    // For example:
    while (true)
    {
        {
            std::unique_lock<std::mutex> lock(mu);
            condv.wait(lock, []
                       { return ready; });
            // Simulation step
            mj_step(model, data);
            ready = false;
        }
        condv.notify_one();
    }
}

void render(mjModel *model, mjData *data)
{
    // init GLFW, create window, make OpenGL context current, request v-sync
    glfwInit();
    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Basic", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    glfwSetWindowAttrib(window, GLFW_RESIZABLE, GLFW_FALSE);


    // initialize visualization data structures
    mjvCamera cam;
    mjvOption opt;
    mjvScene scn;
    mjrContext con;
    mjvPerturb pert;

    mjv_defaultCamera(&cam);
    mjv_defaultPerturb(&pert);
    mjv_defaultOption(&opt);
    mjr_defaultContext(&con);

    cam.trackbodyid = 0;
    cam.type = mjCAMERA_TRACKING;


    // create scene and context using copied model
    mjv_makeScene(model, &scn, 1000);
    mjr_makeContext(model, &con, mjFONTSCALE_100);

    // Your GLFW callbacks and initialization here

    // run main rendering loop
    while (!glfwWindowShouldClose(window))
    {
        {
            std::unique_lock<std::mutex> lock(mu);
            condv.wait(lock, []
                       { return !ready; });
            // Update simulation data in the copied model and data
            mjv_updateScene(model, data, &opt, NULL, &cam, mjCAT_ALL, &scn);
            ready = true;
        }
        condv.notify_one();

        // get framebuffer viewport
        mjrRect viewport = {0, 0, 0, 0};
        glfwGetFramebufferSize(window, &viewport.width, &viewport.height);

        // render the scene
        mjr_render(viewport, &scn, &con);

        // Get rendered OpenGL frame to video frame
        mjr_readPixels(video_frame.data, nullptr, viewport, &con);

        cv::flip(video_frame, video_frame, 0);
        cv::cvtColor(video_frame,video_frame,cv::COLOR_RGB2BGR);
        
        // Save it to video file
        video.write(video_frame);

        // swap OpenGL buffers
        glfwSwapBuffers(window);

        // process GUI events and callbacks
        glfwPollEvents();
    }

    video.release();

    // cleanup GLFW and visualization structures
    glfwTerminate();
    mjv_freeScene(&scn);
    mjr_freeContext(&con);
    exit(0);
}

int main()
{
    // Load original model and data
    mjModel *m = mj_loadXML("model/human.xml", NULL, NULL, 0);
    mjData *d = mj_makeData(m);

    // Start simulation and rendering threads
    std::thread simThread(runSimulation, m, d);
    std::thread renderThread(render, m, d);

    // Join threads
    simThread.join();
    renderThread.join();

    // Cleanup
    mj_deleteData(d);
    mj_deleteModel(m);

    return 0;
}
