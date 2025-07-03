#include "ImageReader.hpp"
#include "ImageWriter.hpp"
#include "BoxFilter.hpp"
#include "VectorTraits_LMUL.hpp"
#include <chrono>
#include <iostream>
#include <iomanip>
#include <vector>
#include <cmath>

// Generic benchmark that runs `fn` once for warm-up and `iterations` times for timing
template <typename F>
double bench_ms(F fn, int iterations = 100) {
    fn();  // warm-up (cache, etc.)
    auto t0 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; ++i) fn();
    auto t1 = std::chrono::high_resolution_clock::now();
    double total_ms = std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(t1 - t0).count();
    return total_ms / iterations;
}

// Simplified box filter using LMUL traits with basic operations
template<int LMUL>
std::vector<std::vector<uint8_t>> boxFilter_LMUL_simple(const std::vector<std::vector<uint8_t>>& img) {
    const int height = img.size();
    const int width = img[0].size();
    std::vector<std::vector<uint8_t>> result(height, std::vector<uint8_t>(width));
    
    using Traits = VectorTraits_LMUL<uint8_t, LMUL>;
    
    // Simple 3x3 box filter for performance testing
    for (int i = 1; i < height - 1; ++i) {
        int j = 0;
        while (j < width - 2) {
            size_t vl = Traits::vsetvl(width - 2 - j);
            
            // Load center and surrounding pixels
            auto sum = Traits::vle(&img[i][j + 1], vl);  // Center pixel
            
            // Add 8 neighboring pixels with saturation
            auto neighbor = Traits::vle(&img[i-1][j], vl);
            sum = Traits::vsaddu_vv(sum, neighbor, vl);
            
            neighbor = Traits::vle(&img[i-1][j + 1], vl);
            sum = Traits::vsaddu_vv(sum, neighbor, vl);
            
            neighbor = Traits::vle(&img[i-1][j + 2], vl);
            sum = Traits::vsaddu_vv(sum, neighbor, vl);
            
            neighbor = Traits::vle(&img[i][j], vl);
            sum = Traits::vsaddu_vv(sum, neighbor, vl);
            
            neighbor = Traits::vle(&img[i][j + 2], vl);
            sum = Traits::vsaddu_vv(sum, neighbor, vl);
            
            neighbor = Traits::vle(&img[i+1][j], vl);
            sum = Traits::vsaddu_vv(sum, neighbor, vl);
            
            neighbor = Traits::vle(&img[i+1][j + 1], vl);
            sum = Traits::vsaddu_vv(sum, neighbor, vl);
            
            neighbor = Traits::vle(&img[i+1][j + 2], vl);
            sum = Traits::vsaddu_vv(sum, neighbor, vl);
            
            // Approximate division by 9 using multiplication (28/256 ≈ 1/9)
            auto filtered = Traits::vmul_vx(sum, 28, vl);
            
            Traits::vse(&result[i][j + 1], filtered, vl);
            j += vl;
        }
    }
    
    return result;
}

int main() {
    // Create a simple test image (512x512)
    const int height = 512;
    const int width = 512;
    std::vector<std::vector<uint8_t>> image(height, std::vector<uint8_t>(width));
    
    // Fill with test pattern
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            image[i][j] = static_cast<uint8_t>((i + j) % 256);
        }
    }

    const int iterations = 100;

    std::cout << "=== Box Filter LMUL Comparison ===" << std::endl;
    std::cout << "Image size: " << image.size() << " x " << image[0].size() << std::endl;
    std::cout << "Iterations: " << iterations << std::endl << std::endl;

    // Benchmark LMUL variants
    std::vector<std::pair<std::string, double>> lmul_results;
    
    // LMUL=1
    {
        auto lmul1_fn = [&]() {
            auto result = boxFilter_LMUL_simple<1>(image);
        };
        double time_m1 = bench_ms(lmul1_fn, iterations);
        lmul_results.push_back({"m1", time_m1});
    }
    
    // LMUL=2
    {
        auto lmul2_fn = [&]() {
            auto result = boxFilter_LMUL_simple<2>(image);
        };
        double time_m2 = bench_ms(lmul2_fn, iterations);
        lmul_results.push_back({"m2", time_m2});
    }
    
    // LMUL=4
    {
        auto lmul4_fn = [&]() {
            auto result = boxFilter_LMUL_simple<4>(image);
        };
        double time_m4 = bench_ms(lmul4_fn, iterations);
        lmul_results.push_back({"m4", time_m4});
    }
    
    // LMUL=8
    {
        auto lmul8_fn = [&]() {
            auto result = boxFilter_LMUL_simple<8>(image);
        };
        double time_m8 = bench_ms(lmul8_fn, iterations);
        lmul_results.push_back({"m8", time_m8});
    }

    // Print results
    std::cout << std::setw(15) << "Implementation" 
              << std::setw(12) << "Time (ms)" 
              << std::setw(12) << "Speedup"
              << std::setw(15) << "Throughput"
              << std::endl;
    std::cout << std::setw(15) << "" 
              << std::setw(12) << "" 
              << std::setw(12) << "vs m1"
              << std::setw(15) << "(MPix/sec)"
              << std::endl;
    std::cout << std::string(54, '-') << std::endl;

    for (const auto& result : lmul_results) {
        double speedup_vs_m1 = lmul_results[0].second / result.second;
        double pixels = image.size() * image[0].size();
        double throughput = (pixels / 1e6) / (result.second / 1000.0);
        
        std::cout << std::setw(15) << ("LMUL " + result.first)
                  << std::setw(12) << std::fixed << std::setprecision(3) << result.second
                  << std::setw(12) << std::fixed << std::setprecision(2) << speedup_vs_m1 << "x"
                  << std::setw(15) << std::fixed << std::setprecision(1) << throughput
                  << std::endl;
    }

    std::cout << std::endl;
    std::cout << "=== LMUL Analysis ===" << std::endl;
    std::cout << "Best LMUL for box filter: ";
    
    auto best_lmul = std::min_element(lmul_results.begin(), lmul_results.end(),
                                      [](const auto& a, const auto& b) { return a.second < b.second; });
    
    std::cout << best_lmul->first << " (" << std::fixed << std::setprecision(3) 
              << best_lmul->second << " ms)" << std::endl;
              
    std::cout << "LMUL efficiency progression:" << std::endl;
    for (size_t i = 0; i < lmul_results.size(); ++i) {
        double theoretical_speedup = (i == 0) ? 1.0 : std::pow(2, i);
        double actual_speedup = lmul_results[0].second / lmul_results[i].second;
        double efficiency = (actual_speedup / theoretical_speedup) * 100.0;
        
        std::cout << "  " << lmul_results[i].first << ": " 
                  << std::fixed << std::setprecision(1) << efficiency << "% efficiency" 
                  << " (actual: " << std::setprecision(2) << actual_speedup 
                  << "x, theoretical: " << std::setprecision(1) << theoretical_speedup << "x)" << std::endl;
    }

    return 0;
} 