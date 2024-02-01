#include <thread>
#include <mutex>
#include <condition_variable>
#include <cassert>
#include <GLFW/glfw3.h>
#include <mujoco/mujoco.h>
#include <opencv4/opencv2/opencv.hpp>

std::mutex mu, muvideo;
std::mutex video_frame_mtx;
std::condition_variable condv, condvideo;
bool ready = false;
bool Exit = false;

// SD
#define WIDTH 640
#define HEIGHT 480

// HD
// #define WIDTH 1280
// #define HEIGHT 720

// FHD
// #define WIDTH 1920
// #define HEIGHT 1080

// UHD 4K
// #define WIDTH 1920*2
// #define HEIGHT 1080*2
cv::Size video_size(WIDTH, HEIGHT);
cv::Mat video_frame = cv::Mat::zeros(HEIGHT, WIDTH, CV_8UC3);
cv::VideoWriter video("video_out.mp4", cv::CAP_FFMPEG, cv::VideoWriter::fourcc('a', 'v', 'c', '1'), 200.0, video_size);

void runSimulation(mjModel *model, mjData *data)
{
    while (!Exit)
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

    mjr_setBuffer(mjFB_OFFSCREEN, &con);
    mjr_resizeOffscreen(WIDTH, HEIGHT, &con);

    mjrRect render_viewport = {0, 0, WIDTH, HEIGHT};
    mjrRect viewport = {0, 0, 0, 0};

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

        // Get GLFW framebuffer viewport size
        glfwGetFramebufferSize(window, &viewport.width, &viewport.height);

        // render the scene
        mjr_render(render_viewport, &scn, &con);
        mjr_blitBuffer(render_viewport, viewport, 1, 0, &con);

        // Get rendered OpenGL frame to video frame
        video_frame_mtx.lock();
        mjr_readPixels(video_frame.data, nullptr, render_viewport, &con);
        video_frame_mtx.unlock();

        condvideo.notify_one();

        // swap OpenGL buffers
        glfwSwapBuffers(window);

        // process GUI events and callbacks
        glfwPollEvents();
    }
    mjr_restoreBuffer(&con);
    Exit = true;
    video.release();

    // cleanup GLFW and visualization structures
    glfwTerminate();
    mjv_freeScene(&scn);
    mjr_freeContext(&con);
    exit(0);
}

void video_thread()
{

    while (!Exit)
    {
        // Wait for new frame to be available from OpenGL
        std::unique_lock<std::mutex> lock(muvideo);
        condv.wait(lock);

        video_frame_mtx.lock();
        // Convert data | This should be better done in GPU... Maybe using FFMPEG/libavfilter
        cv::flip(video_frame, video_frame, 0);
        cv::cvtColor(video_frame, video_frame, cv::COLOR_RGB2BGR);

        // Save it to video file | This step could take advantage from GPU Encoding...  Maybe using FFMPEG/libavutil*
        video.write(video_frame);
        video_frame_mtx.unlock();
    }
}

int main()
{
    // Load original model and data
    mjModel *m = mj_loadXML("model/human.xml", NULL, NULL, 0);
    mjData *d = mj_makeData(m);

    // Start simulation and rendering threads
    std::thread simThread(runSimulation, m, d);
    std::thread renderThread(render, m, d);
    std::thread videoThread(video_thread);

    // Join threads
    simThread.join();
    renderThread.join();
    videoThread.join();

    // Cleanup
    mj_deleteData(d);
    mj_deleteModel(m);

    return 0;
}
