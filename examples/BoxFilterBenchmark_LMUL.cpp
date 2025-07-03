#include "ImageReader.hpp"
#include "ImageWriter.hpp"
#include "BoxFilter.hpp"
#include "BoxFilter_Vector_Template.hpp"
#include "VectorTraits_LMUL.hpp"
#include <chrono>
#include <iostream>
#include <iomanip>
#include <vector>

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

template<int LMUL>
std::vector<std::vector<uint8_t>> boxFilter_LMUL_impl(const std::vector<std::vector<uint8_t>>& img, int kernelSize) {
    const int height = img.size();
    const int width = img[0].size();
    const int half = kernelSize / 2;
    
    // Pad the image for border handling
    std::vector<std::vector<uint8_t>> padded(height + 2 * half, std::vector<uint8_t>(width + 2 * half));
    
    // Copy original image to center of padded image
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            padded[i + half][j + half] = img[i][j];
        }
    }
    
    // Pad borders by replication
    for (int i = 0; i < half; ++i) {
        for (int j = 0; j < width + 2 * half; ++j) {
            padded[i][j] = padded[half][j];  // Top border
            padded[height + half + i][j] = padded[height + half - 1][j];  // Bottom border
        }
    }
    for (int i = 0; i < height + 2 * half; ++i) {
        for (int j = 0; j < half; ++j) {
            padded[i][j] = padded[i][half];  // Left border
            padded[i][width + half + j] = padded[i][width + half - 1];  // Right border
        }
    }
    
    std::vector<std::vector<uint8_t>> result(height, std::vector<uint8_t>(width));
    
    using Traits = VectorTraits_LMUL<uint8_t, LMUL>;
    using wide_vec_type = typename Traits::wide_vec_type;
    
    for (int i = 0; i < height; ++i) {
        int y = i + half;
        int j = 0;
        while (j < width) {
            size_t vl = Traits::vsetvl(width - j);
            
            // Initialize accumulator using widening operations
            auto vsum = Traits::vmv_v_x_wide(0, vl);
            
            // Accumulate over the kernel
            for (int ky = -half; ky <= half; ++ky) {
                for (int kx = -half; kx <= half; ++kx) {
                    const uint8_t* ptr = &padded[y + ky][j + half + kx];
                    auto vpixel = Traits::vle(ptr, vl);
                    auto vpixel_wide = Traits::wadd_vv(vpixel, vpixel, vl);  // Use wadd_vv for widening
                    
                    if constexpr (std::is_same_v<wide_vec_type, vuint16m2_t>) {
                        vsum = __riscv_vadd_vv_u16m2(vsum, vpixel_wide, vl);
                    } else if constexpr (std::is_same_v<wide_vec_type, vuint16m4_t>) {
                        vsum = __riscv_vadd_vv_u16m4(vsum, vpixel_wide, vl);
                    } else if constexpr (std::is_same_v<wide_vec_type, vuint16m8_t>) {
                        vsum = __riscv_vadd_vv_u16m8(vsum, vpixel_wide, vl);
                    }
                }
            }
            
            // Divide by kernel area and narrow back to uint8_t
            int kernel_area = kernelSize * kernelSize;
            
            if constexpr (std::is_same_v<wide_vec_type, vuint16m2_t>) {
                auto vavg = __riscv_vdivu_vx_u16m2(vsum, kernel_area, vl);
                auto vavg_narrow = __riscv_vnclipu_wx_u8m1(vavg, 0, 0, vl);
                Traits::vse(&result[i][j], vavg_narrow, vl);
            } else if constexpr (std::is_same_v<wide_vec_type, vuint16m4_t>) {
                auto vavg = __riscv_vdivu_vx_u16m4(vsum, kernel_area, vl);
                auto vavg_narrow = __riscv_vnclipu_wx_u8m2(vavg, 0, 0, vl);
                Traits::vse(&result[i][j], vavg_narrow, vl);
            } else if constexpr (std::is_same_v<wide_vec_type, vuint16m8_t>) {
                auto vavg = __riscv_vdivu_vx_u16m8(vsum, kernel_area, vl);
                auto vavg_narrow = __riscv_vnclipu_wx_u8m8(vavg, 0, 0, vl);
                Traits::vse(&result[i][j], vavg_narrow, vl);
            }
            
            j += vl;
        }
    }
    
    return result;
}

int main() {
    ImageReader reader;
    ImageWriter writer;
    Image image;

    const int kernelSize = 5;
    const int iterations = 100;

    // Read input image once
    ImageStatus status = reader.readImage("barb.512.pgm", image);
    if (status != ImageStatus::SUCCESS) {
        std::cerr << "Failed to read image: " << static_cast<int>(status) << std::endl;
        return 1;
    }

    std::cout << "=== Box Filter LMUL Comparison ===" << std::endl;
    std::cout << "Image size: " << image.pixelMatrix.size() << " x " << image.pixelMatrix[0].size() << std::endl;
    std::cout << "Kernel size: " << kernelSize << "x" << kernelSize << std::endl;
    std::cout << "Iterations: " << iterations << std::endl << std::endl;

    // Benchmark scalar reference
    double time_scalar;
    {
        auto scalar_fn = [&]() {
            auto result = BoxFilter().applyBoxFilterSlidingGrey(image.pixelMatrix, kernelSize);
        };
        time_scalar = bench_ms(scalar_fn, iterations);
    }

    // Benchmark original vector implementation
    double time_original;
    {
        auto vector_fn = [&]() {
            auto result = __riscv_BoxFilter<uint8_t>(image.pixelMatrix, kernelSize);
        };
        time_original = bench_ms(vector_fn, iterations);
    }

    // Benchmark LMUL variants
    std::vector<std::pair<std::string, double>> lmul_results;
    
    // LMUL=1
    {
        auto lmul1_fn = [&]() {
            auto result = boxFilter_LMUL_impl<1>(image.pixelMatrix, kernelSize);
        };
        double time_m1 = bench_ms(lmul1_fn, iterations);
        lmul_results.push_back({"m1", time_m1});
    }
    
    // LMUL=2
    {
        auto lmul2_fn = [&]() {
            auto result = boxFilter_LMUL_impl<2>(image.pixelMatrix, kernelSize);
        };
        double time_m2 = bench_ms(lmul2_fn, iterations);
        lmul_results.push_back({"m2", time_m2});
    }
    
    // LMUL=4
    {
        auto lmul4_fn = [&]() {
            auto result = boxFilter_LMUL_impl<4>(image.pixelMatrix, kernelSize);
        };
        double time_m4 = bench_ms(lmul4_fn, iterations);
        lmul_results.push_back({"m4", time_m4});
    }
    
    // LMUL=8
    {
        auto lmul8_fn = [&]() {
            auto result = boxFilter_LMUL_impl<8>(image.pixelMatrix, kernelSize);
        };
        double time_m8 = bench_ms(lmul8_fn, iterations);
        lmul_results.push_back({"m8", time_m8});
    }

    // Print results
    std::cout << std::setw(15) << "Implementation" 
              << std::setw(12) << "Time (ms)" 
              << std::setw(12) << "Speedup"
              << std::setw(15) << "vs Scalar"
              << std::endl;
    std::cout << std::string(54, '-') << std::endl;
    
    std::cout << std::setw(15) << "Scalar" 
              << std::setw(12) << std::fixed << std::setprecision(3) << time_scalar
              << std::setw(12) << "1.00x"
              << std::setw(15) << "baseline"
              << std::endl;
              
    std::cout << std::setw(15) << "Original Vector" 
              << std::setw(12) << std::fixed << std::setprecision(3) << time_original
              << std::setw(12) << std::fixed << std::setprecision(2) << (time_scalar / time_original) << "x"
              << std::setw(15) << std::fixed << std::setprecision(2) << (time_scalar / time_original) << "x"
              << std::endl;

    for (const auto& result : lmul_results) {
        double speedup_vs_scalar = time_scalar / result.second;
        double speedup_vs_m1 = lmul_results[0].second / result.second;
        
        std::cout << std::setw(15) << ("LMUL " + result.first)
                  << std::setw(12) << std::fixed << std::setprecision(3) << result.second
                  << std::setw(12) << std::fixed << std::setprecision(2) << speedup_vs_m1 << "x"
                  << std::setw(15) << std::fixed << std::setprecision(2) << speedup_vs_scalar << "x"
                  << std::endl;
    }

    std::cout << std::endl;
    std::cout << "=== LMUL Analysis ===" << std::endl;
    std::cout << "Best LMUL for box filter: ";
    
    auto best_lmul = std::min_element(lmul_results.begin(), lmul_results.end(),
                                      [](const auto& a, const auto& b) { return a.second < b.second; });
    
    std::cout << best_lmul->first << " (" << std::fixed << std::setprecision(2) 
              << (time_scalar / best_lmul->second) << "x vs scalar)" << std::endl;
              
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

    std::cout << std::endl;
    std::cout << "=== Performance Insights ===" << std::endl;
    std::cout << "Box filter performance characteristics:" << std::endl;
    std::cout << "- Memory-intensive operation with " << kernelSize*kernelSize << " loads per output pixel" << std::endl;
    std::cout << "- Higher LMUL may show diminishing returns due to memory bandwidth limits" << std::endl;
    std::cout << "- Widening operations prevent integer overflow during accumulation" << std::endl;

    return 0;
} 