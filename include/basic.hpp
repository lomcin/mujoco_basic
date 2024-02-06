/*
MIT License

Copyright (c) 2024 Lucas Maggi

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef __BASIC__H_

#include <thread>
#include <mutex>
#include <condition_variable>
#include <cassert>
#include <GLFW/glfw3.h>
#include <mujoco/mujoco.h>
#include <csv/csv.hpp>
#ifdef USE_OPENCV
#include <opencv4/opencv2/opencv.hpp>
#endif

namespace basic
{
    std::vector<std::string> joint_names(mjModel *m, mjData *d)
    {
        assert(m != nullptr && d != nullptr);

        std::vector<std::string> v;
        v.resize(m->nq);
        int i = 0;

        for (int j = 0; j < m->njnt; ++j)
        {
            if (m->jnt_type[j] == mjJNT_FREE)
            {
                const char *name = mj_id2name(m, mjOBJ_JOINT, j);
                // Cartesian position
                v[i++].assign(std::string(name) + "_x");
                v[i++].assign(std::string(name) + "_y");
                v[i++].assign(std::string(name) + "_z");

                // Quaternion orientation
                v[i++].assign(std::string(name) + "_qw");
                v[i++].assign(std::string(name) + "_qx");
                v[i++].assign(std::string(name) + "_qy");
                v[i++].assign(std::string(name) + "_qz");
            }
            else if (m->jnt_type[j] == mjJNT_BALL)
            {
                const char *name = mj_id2name(m, mjOBJ_JOINT, j);
                // Quaternion orientation
                v[i++].assign(std::string(name) + "_qw");
                v[i++].assign(std::string(name) + "_qx");
                v[i++].assign(std::string(name) + "_qy");
                v[i++].assign(std::string(name) + "_qz");
            }
            else if (m->jnt_type[j] == mjJNT_SLIDE)
            {
                const char *name = mj_id2name(m, mjOBJ_JOINT, j);
                // Sliding position in fixed axis
                v[i++].assign(std::string(name));
            }
            else if (m->jnt_type[j] == mjJNT_HINGE)
            {
                const char *name = mj_id2name(m, mjOBJ_JOINT, j);
                v[i++].assign(std::string(name));
            }
        }

        return v;
    }
}

#endif // __BASIC__H_
