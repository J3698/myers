#include <vector>
#include <string>
#include <tuple>
#include <cassert>
#include <algorithm>
#include <stdexcept>
#include <iostream>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

using namespace std;

enum DiffType { EQUAL, INSERT, DELETE };



vector<tuple<DiffType, string>> myers_diff2(const vector<string>& a_lines, const vector<string>& b_lines) {
    try {
        int a_max = a_lines.size();
        int b_max = b_lines.size();
        int max_size = a_max + b_max;

        // Create a large array to store x, y, and d0 for every (d, k) combination
        vector<array<int16_t, 2>> dp_array((max_size + 1) * (2 * max_size + 1));

        vector<int16_t> frontier(2 * max_size + 3);
        frontier[max_size + 2] = 0;


        for (int d = 0; d <= a_max + b_max; ++d) {
            for (int k = -d; k <= d; k += 2) {
                int frontier_index = k + max_size + 1;
                bool go_down = (k == -d || (k != d && frontier[frontier_index - 1] < frontier[frontier_index + 1]));

                int x, old_x;

                if (go_down) {
                    old_x = frontier[frontier_index + 1];
                    x = old_x;
                } else {
                    old_x = frontier[frontier_index - 1];
                    x = old_x + 1;
                }

                int y = x - k;
                while (x < a_max && y < b_max && a_lines[x] == b_lines[y]) {
                    ++x;
                    ++y;
                }

                // set history
                int dp_index = d * (2 * max_size + 1) + (k + max_size);
                // stores x, y, and the the k to backtrack to
                //cout << d << " " << k << " " << x << " " << y << " " << ((go_down ? frontier_index + 1 : frontier_index - 1) - max_size-1) << endl;
                dp_array[dp_index] = {x, (go_down ? frontier_index + 1 : frontier_index - 1) - max_size - 1};

                if (x >= a_max && y >= b_max) {
                    vector<tuple<DiffType, string>> diff;
                    int final_d = d;
                    //cout << endl;
                    for (d = final_d; d > 0; --d) {
                        int dp_index = d * (2 * max_size + 1) + (k + max_size);
                        auto [curr_x, prev_k] = dp_array[dp_index];
                        auto curr_y = curr_x - k;

                        int prev_dp_index = (d-1) * (2 * max_size + 1) + (prev_k + max_size);
                        auto [prev_x, _] = dp_array[prev_dp_index];
                        auto prev_y = prev_x - prev_k;

                        //cout << d << " " << curr_x << " " << curr_y << " " << prev_x << " " << prev_y << " " << endl;

                        while (curr_x > prev_x && curr_y > prev_y) {
                            //cout << curr_x << " " << curr_y << endl;
                            //cout << a_lines[curr_x-1] << endl;
                            diff.emplace_back(EQUAL, a_lines[curr_x-1]);
                            curr_x--;
                            curr_y--;
                        }
                        if (curr_y > prev_y) {
                            //cout << curr_x << " " << curr_y << endl;
                            //cout << b_lines[curr_y-1] << endl;
                            diff.emplace_back(INSERT, b_lines[curr_y-1]);
                        } else if (curr_x > prev_x) {
                            //cout << curr_x << " " << curr_y << endl;
                            //cout << a_lines[curr_x-1] << endl;
                            diff.emplace_back(DELETE, a_lines[curr_x-1]);
                        } else {
                            throw runtime_error("expected to go up or down");
                        }
                        //cout << "did iter" << endl;
                        k = prev_k;
                    }
                    d = 0;
                    k = 0;
                    int dp_index = d * (2 * max_size + 1) + (k + max_size);
                    auto [curr_x, prev_k] = dp_array[dp_index];
                    auto curr_y = curr_x;
                    while (curr_x != 0) {
                        //cout << curr_x << " " << curr_y << endl;
                        //cout << a_lines[curr_x-1] << endl;
                        diff.emplace_back(EQUAL, a_lines[curr_x-1]);
                        curr_x--;
                        curr_y--;
                    }

                    reverse(diff.begin(), diff.end());
                    return diff;
                } else {
                    frontier[frontier_index] = x;
                }
            }
        }

        throw runtime_error("Could not find edit script");
    } catch (const exception& e) {
        cerr << "Error in myers_diff2: " << e.what() << endl;
        throw;
    }
}

// Wrapper function for Python
py::list py_myers_diff2(const py::list& a_lines, const py::list& b_lines) {
    try {
        vector<string> cpp_a_lines = a_lines.cast<vector<string>>();
        vector<string> cpp_b_lines = b_lines.cast<vector<string>>();

        auto result = myers_diff2(cpp_a_lines, cpp_b_lines);

        py::list py_result;
        for (const auto& [diff_type, line] : result) {
            py::tuple py_tuple = py::make_tuple(static_cast<int>(diff_type), line);
            py_result.append(py_tuple);
        }

        return py_result;
    } catch (const exception& e) {
        cerr << "Error in py_myers_diff2: " << e.what() << endl;
        throw;
    }
}

PYBIND11_MODULE(myers_diff, m) {
    m.doc() = "Python bindings for Myers diff algorithm";
    m.def("myers_diff2", &py_myers_diff2, "Compute the diff between two sequences of lines",
          py::arg("a_lines"), py::arg("b_lines"));

    py::enum_<DiffType>(m, "DiffType")
        .value("EQUAL", DiffType::EQUAL)
        .value("INSERT", DiffType::INSERT)
        .value("DELETE", DiffType::DELETE)
        .export_values();
}
