#include "MarchingSquares.h"

#include <cmath>
#include <iostream>
#include <chrono>

class Timer
{
public:
    Timer(): clock_(std::chrono::high_resolution_clock::now()) {}

    void tic(const std::string& message = "")
    {
        const auto end = std::chrono::high_resolution_clock::now();
        std::cout << message << ": Took " << std::chrono::duration_cast<std::chrono::milliseconds>(end - clock_).count() << "\n";
        clock_ = end;
    }

private:
    std::chrono::steady_clock::time_point clock_;
};

int main()
{
    const auto func = [](const double x, const double y)
    {
        return std::sin(x*x + y*y) - std::cos(x * y);
    };

    const auto mc = marching_squares::MarchingSquares(func, {-10, 10}, {-10, 10}, {25000, 25000});

    Timer clock;
    const auto result = mc.compute(0.5);
    clock.tic("Normal compute");
    const auto result_faster = mc.compute_faster(0.5);
    clock.tic("Improved compute");

    std::cout << "Result size: " << result.size() << std::endl;
    std::cout << "Second result size: " << std::get<1>(result_faster).size() << std::endl;

    return 0;
}
