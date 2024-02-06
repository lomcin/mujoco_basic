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

#ifndef __CSV_WRITER__H_

#include <string>
#include <vector>
#include <cassert>
#include <iostream>
#include <fstream>
#include <iomanip>

namespace csv
{

    // Column Name
    typedef std::string col_name;

    // Column Names
    typedef std::vector<col_name> cols_names;

    // Double values | Column Values
    typedef std::vector<double> double_values;

    typedef std::vector<double_values> cols_double;

    /// @brief CSV file writer
    class csv_writer
    {

        void write_line()
        {
            assert(file.is_open());

            // Setting precision
            file << std::setprecision(5);

            for (size_t i = 0; i < cols.size(); ++i)
            {
                file << cur_line[i] << separator;
            }

            file << std::endl;
            file.flush();
        }

        void write_headers()
        {
            assert(file.is_open());

            for (size_t i = 0; i < cols.size(); ++i)
            {
                file << cols[i] << separator;
            }
            
            file << std::endl;
            file.flush();
        }

    public:
        cols_names cols;
        double_values cur_line;
        std::string file_name;
        std::ofstream file;
        std::string separator = ",";

        csv_writer(std::string file_name) : file_name(file_name)
        {
        }

        ~csv_writer()
        {
            file.close();
        }

        csv_writer &open(std::string file_name)
        {
            file_name = file_name;
            file.open(file_name, std::fstream::out);
            file.close();
            return *this;
        }

        void set_headers(cols_names names)
        {
            if (cols.size() == 0)
            {
                printf("[INFO] writer has %ld names.", names.size());
                cols.assign(names.begin(), names.end());
                file.open(file_name, std::fstream::out);
                write_headers();
                file.close();

                // // Reopen file for appending
                file.open(file_name, std::fstream::out | std::fstream::app);
            }
            else
            {
                printf("[WARNING] tried to reset headers. Failed.");
            }
        }

        void append(double val)
        {
            cur_line.push_back(val);

            if (cur_line.size() == cols.size())
            {
                write_line();
                cur_line.clear();
            }
        }

        void append(double *val, size_t n)
        {
            if (cols.size() == 0)
            {
                printf("[WARNING] tried to write values without headers.");
            }
            else
            {
                for (size_t i = 0; i < n; ++i)
                {
                    append(*val);
                }
            }
        }

        void append(double *val)
        {
           this->append(val, this->cols.size());
        }

    };

}

#endif // __CSV_WRITER__H_
