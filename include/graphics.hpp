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

#ifndef __GRAPHICS__H_

#include <string>
#include <vector>
#include <mujoco/mujoco.h>

namespace graphics
{
    void set_figure(mjvFigure *figure, mjModel *m, mjData *d)
    {
        assert(m != nullptr && d != nullptr);

        int fi = static_cast<int>(d->time / m->opt.timestep); // naive frame number
        int i = fi % mjMAXLINEPNT;                            // modulo max points per line

        for (int q = 0; q < m->nq; ++q)
        {
            figure->linedata[q][i * 2] = m->opt.timestep * fi;
            figure->linedata[q][i * 2 + 1] = d->qpos[q];
        }

        for (int q = 0; q < m->nq; ++q)
            figure->linepnt[q] = std::min(fi, mjMAXLINEPNT);
    }

    void init_figure(mjvFigure *figure, mjModel *m, std::string title, std::vector<std::string> &headers)
    {
        mjv_defaultFigure(figure);
        strcpy(figure->xlabel, "Time");
        const float scale_factor = 1.0/255.0;

        figure->flg_ticklabel[0] = 1;
        figure->flg_ticklabel[1] = 1;

        figure->figurergba[3] = 0.5;
        figure->gridsize[0] = 10;
        figure->gridsize[1] = 10;

        for (int q = 0; q < m->nq; ++q)
        {
            float *rgb = figure->linergb[q];
            rgb[0] = static_cast<float>(rand() % 256)*scale_factor;
            rgb[1] = static_cast<float>(rand() % 256)*scale_factor;
            rgb[2] = static_cast<float>(rand() % 256)*scale_factor;
        }
        for (int q = 0; q < m->nq; ++q)
        {
            strcpy(figure->linename[q], headers[q].c_str());
        }
    }
}

#endif // __GRAPHICS__H_
