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
#include <cstring>
#include <vector>
#include <mujoco/mujoco.h>

namespace graphics
{
    void set_figure(mjvFigure *figure, mjModel *m, mjData *d)
    {
        assert(m != nullptr && d != nullptr);

        int fi = static_cast<int>(d->time / m->opt.timestep); // naive frame number
        int i = std::min(fi, mjMAXLINEPNT - 1);               // points per line
        double timestamp = m->opt.timestep * static_cast<double>(fi);

        figure->range[0][0] = timestamp - (m->opt.timestep * i);
        figure->range[0][1] = timestamp;

        for (int q = 0; q < m->nq; ++q)
            figure->linepnt[q] = i;

        for (int q = 0; q < m->nq; ++q)
        {
            figure->linedata[q][i * 2] = timestamp;
            figure->linedata[q][i * 2 + 1] = d->qpos[q];
        }

        if (i == mjMAXLINEPNT - 1)
            for (int q = 0; q < m->nq; ++q)
                for (int j = 0; j < i; ++j)
                {
                    int jj = j * 2;
                    figure->linedata[q][jj] = figure->linedata[q][jj + 2];
                    figure->linedata[q][jj + 1] = figure->linedata[q][jj + 3];
                }
    }

    void init_figure(mjvFigure *figure, mjModel *m, std::string title, std::vector<std::string> &headers)
    {
        mjv_defaultFigure(figure);
        strcpy(figure->title, title.c_str());
        strcpy(figure->xlabel, "Time");
        const float scale_factor = 1.0 / 255.0;

        strcpy(figure->xformat, "%.2lf");

        figure->flg_ticklabel[0] = 1;
        figure->flg_ticklabel[1] = 1;

        figure->figurergba[3] = 0.5;
        figure->gridsize[0] = 10;
        figure->gridsize[1] = 10;

        for (int q = 0; q < m->nq; ++q)
        {
            float *rgb = figure->linergb[q];
            rgb[0] = static_cast<float>(rand() % 256) * scale_factor;
            rgb[1] = static_cast<float>(rand() % 256) * scale_factor;
            rgb[2] = static_cast<float>(rand() % 256) * scale_factor;
        }
        for (int q = 0; q < m->nq; ++q)
        {
            strcpy(figure->linename[q], headers[q].c_str());

            // fill with zeros
            memset(figure->linedata[q], 0, mjMAXLINEPNT);
        }
    }
}

#endif // __GRAPHICS__H_
