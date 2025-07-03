#include "ImageReader.hpp"
#include "ImageWriter.hpp"
#include "VectorTraits_LMUL.hpp"
#include <chrono>
#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>

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

struct BenchmarkResult {
    std::string operation;
    int lmul;
    const char* lmul_str;
    double time_ms;
    double throughput;
    double efficiency;  // Relative to m1
};

class LMULBenchmarkSuite {
private:
    std::vector<BenchmarkResult> results;
    std::vector<std::vector<uint8_t>> testImage;
    const int iterations = 100;
    
public:
    bool initialize(const std::string& imagePath) {
        // Create a simple test image (512x512)
        const int height = 512;
        const int width = 512;
        testImage.resize(height, std::vector<uint8_t>(width));
        
        // Fill with test pattern
        for (int i = 0; i < height; ++i) {
            for (int j = 0; j < width; ++j) {
                testImage[i][j] = static_cast<uint8_t>((i + j) % 256);
            }
        }
        
        std::cout << "=== RISC-V Vector LMUL Performance Analysis ===" << std::endl;
        std::cout << "Image size: " << testImage.size() 
                  << " x " << testImage[0].size() << std::endl;
        std::cout << "Iterations per test: " << iterations << std::endl;
        std::cout << "Testing LMUL values: m1, m2, m4, m8" << std::endl << std::endl;
        
        return true;
    }
    
    template<int LMUL>
    void benchmarkImageAddition() {
        using Traits = VectorTraits_uint8_LMUL<LMUL>;
        
        auto lambda = [&]() {
            simple_add_LMUL<uint8_t, LMUL>(testImage, testImage);
        };
        
        double time_ms = bench_ms(lambda, iterations);
        double pixels = testImage.size() * testImage[0].size();
        double throughput = (pixels / 1e6) / (time_ms / 1000.0);  // Megapixels per second
        
        results.push_back({
            "Image Addition",
            LMUL,
            Traits::lmul_str,
            time_ms,
            throughput,
            0.0  // Will calculate later
        });
    }
    
    template<int LMUL>
    void benchmarkHorizontalFlip() {
        using Traits = VectorTraits_uint8_LMUL<LMUL>;
        
        auto lambda = [&]() {
            simple_flip_horizontal_LMUL<uint8_t, LMUL>(testImage);
        };
        
        double time_ms = bench_ms(lambda, iterations);
        double pixels = testImage.size() * testImage[0].size();
        double throughput = (pixels / 1e6) / (time_ms / 1000.0);
        
        results.push_back({
            "Horizontal Flip",
            LMUL,
            Traits::lmul_str,
            time_ms,
            throughput,
            0.0
        });
    }
    
    template<int LMUL>
    void benchmarkBoxFilter() {
        using Traits = VectorTraits_uint8_LMUL<LMUL>;
        const int kernelSize = 5;
        
        auto lambda = [&]() {
            // Simplified box filter using LMUL traits
            const int height = testImage.size();
            const int width = testImage[0].size();
            std::vector<std::vector<uint8_t>> result(height, std::vector<uint8_t>(width));
            
            for (int i = 1; i < height - 1; ++i) {
                int j = 0;
                while (j < width - kernelSize) {
                    size_t vl = Traits::vsetvl(width - kernelSize - j);
                    
                    // Load center pixels
                    auto center = Traits::vle(&testImage[i][j + 2], vl);
                    
                    // Simple averaging (not full box filter for performance testing)
                    auto neighbors = Traits::vle(&testImage[i-1][j + 2], vl);
                    auto sum = Traits::vsaddu_vv(center, neighbors, vl);
                    
                    neighbors = Traits::vle(&testImage[i+1][j + 2], vl);
                    sum = Traits::vsaddu_vv(sum, neighbors, vl);
                    
                    // Simple divide by 3 using bit shifts
                    auto divided = Traits::vmul_vx(sum, 85, vl);  // 85/256 ≈ 1/3
                    
                    Traits::vse(&result[i][j + 2], divided, vl);
                    j += vl;
                }
            }
        };
        
        double time_ms = bench_ms(lambda, iterations);
        double pixels = testImage.size() * testImage[0].size();
        double throughput = (pixels / 1e6) / (time_ms / 1000.0);
        
        results.push_back({
            "Box Filter (5x5)",
            LMUL,
            Traits::lmul_str,
            time_ms,
            throughput,
            0.0
        });
    }
    
    void runAllBenchmarks() {
        std::cout << "Running Image Addition benchmarks..." << std::endl;
        benchmarkImageAddition<1>();
        benchmarkImageAddition<2>();
        benchmarkImageAddition<4>();
        benchmarkImageAddition<8>();
        
        std::cout << "Running Horizontal Flip benchmarks..." << std::endl;
        benchmarkHorizontalFlip<1>();
        benchmarkHorizontalFlip<2>();
        benchmarkHorizontalFlip<4>();
        benchmarkHorizontalFlip<8>();
        
        std::cout << "Running Box Filter benchmarks..." << std::endl;
        benchmarkBoxFilter<1>();
        benchmarkBoxFilter<2>();
        benchmarkBoxFilter<4>();
        benchmarkBoxFilter<8>();
        
        calculateEfficiencies();
    }
    
    void calculateEfficiencies() {
        std::vector<std::string> operations = {"Image Addition", "Horizontal Flip", "Box Filter (5x5)"};
        
        for (const auto& op : operations) {
            double m1_time = 0.0;
            
            // Find m1 time for this operation
            for (const auto& result : results) {
                if (result.operation == op && result.lmul == 1) {
                    m1_time = result.time_ms;
                    break;
                }
            }
            
            // Calculate efficiencies relative to m1
            for (auto& result : results) {
                if (result.operation == op && m1_time > 0) {
                    result.efficiency = m1_time / result.time_ms;
                }
            }
        }
    }
    
    void printResults() {
        std::cout << "\n=== LMUL PERFORMANCE COMPARISON ===" << std::endl;
        std::cout << std::setw(20) << "Operation" 
                  << std::setw(8) << "LMUL" 
                  << std::setw(12) << "Time (ms)" 
                  << std::setw(15) << "Throughput"
                  << std::setw(12) << "Speedup"
                  << std::endl;
        std::cout << std::setw(20) << "" 
                  << std::setw(8) << "" 
                  << std::setw(12) << "" 
                  << std::setw(15) << "(MPix/sec)"
                  << std::setw(12) << "vs m1"
                  << std::endl;
        std::cout << std::string(67, '-') << std::endl;
        
        for (const auto& result : results) {
            std::cout << std::setw(20) << result.operation
                      << std::setw(8) << result.lmul_str
                      << std::setw(12) << std::fixed << std::setprecision(3) << result.time_ms
                      << std::setw(15) << std::fixed << std::setprecision(2) << result.throughput
                      << std::setw(12) << std::fixed << std::setprecision(2) << result.efficiency << "x"
                      << std::endl;
        }
    }
    
    void generateCSV(const std::string& filename) {
        std::ofstream file(filename);
        file << "Operation,LMUL,Time_ms,Throughput_MPix_sec,Speedup_vs_m1\n";
        
        for (const auto& result : results) {
            file << result.operation << ","
                 << result.lmul_str << ","
                 << std::fixed << std::setprecision(3) << result.time_ms << ","
                 << std::fixed << std::setprecision(2) << result.throughput << ","
                 << std::fixed << std::setprecision(2) << result.efficiency << "\n";
        }
        
        std::cout << "\nResults saved to: " << filename << std::endl;
        std::cout << "Use this CSV file to generate graphs in your preferred plotting tool." << std::endl;
    }
    
    void generatePythonScript(const std::string& scriptName) {
        std::ofstream script(scriptName);
        script << R"(#!/usr/bin/env python3
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# Read the benchmark data
df = pd.read_csv('lmul_benchmark_results.csv')

# Set up the plotting style
plt.style.use('default')
fig, ((ax1, ax2), (ax3, ax4)) = plt.subplots(2, 2, figsize=(15, 12))
fig.suptitle('RISC-V Vector LMUL Performance Analysis', fontsize=16, fontweight='bold')

operations = df['Operation'].unique()
lmul_values = ['m1', 'm2', 'm4', 'm8']
colors = ['#1f77b4', '#ff7f0e', '#2ca02c', '#d62728']

# Plot 1: Execution Time Comparison
ax1.set_title('Execution Time by LMUL', fontweight='bold')
x = np.arange(len(operations))
width = 0.2
for i, lmul in enumerate(lmul_values):
    times = [df[(df['Operation'] == op) & (df['LMUL'] == lmul)]['Time_ms'].values[0] 
             for op in operations]
    ax1.bar(x + i*width, times, width, label=lmul, color=colors[i])
ax1.set_xlabel('Operation')
ax1.set_ylabel('Time (ms)')
ax1.set_xticks(x + width * 1.5)
ax1.set_xticklabels(operations, rotation=45, ha='right')
ax1.legend()
ax1.grid(True, alpha=0.3)

# Plot 2: Throughput Comparison
ax2.set_title('Throughput by LMUL', fontweight='bold')
for i, lmul in enumerate(lmul_values):
    throughputs = [df[(df['Operation'] == op) & (df['LMUL'] == lmul)]['Throughput_MPix_sec'].values[0] 
                   for op in operations]
    ax2.bar(x + i*width, throughputs, width, label=lmul, color=colors[i])
ax2.set_xlabel('Operation')
ax2.set_ylabel('Throughput (MPix/sec)')
ax2.set_xticks(x + width * 1.5)
ax2.set_xticklabels(operations, rotation=45, ha='right')
ax2.legend()
ax2.grid(True, alpha=0.3)

# Plot 3: Speedup vs m1
ax3.set_title('Speedup vs LMUL=1', fontweight='bold')
for i, lmul in enumerate(lmul_values[1:], 1):  # Skip m1
    speedups = [df[(df['Operation'] == op) & (df['LMUL'] == lmul)]['Speedup_vs_m1'].values[0] 
                for op in operations]
    ax3.bar(x + i*width, speedups, width, label=lmul, color=colors[i])
ax3.axhline(y=1.0, color='black', linestyle='--', alpha=0.5, label='m1 baseline')
ax3.set_xlabel('Operation')
ax3.set_ylabel('Speedup Factor')
ax3.set_xticks(x + width * 1.5)
ax3.set_xticklabels(operations, rotation=45, ha='right')
ax3.legend()
ax3.grid(True, alpha=0.3)

# Plot 4: LMUL Efficiency Analysis
ax4.set_title('LMUL Theoretical vs Actual Efficiency', fontweight='bold')
lmul_nums = [1, 2, 4, 8]
theoretical_speedup = lmul_nums  # Perfect scaling
for i, op in enumerate(operations):
    actual_speedups = [df[(df['Operation'] == op) & (df['LMUL'] == f'm{lmul}')]['Speedup_vs_m1'].values[0] 
                       for lmul in lmul_nums]
    ax4.plot(lmul_nums, actual_speedups, 'o-', label=op, linewidth=2, markersize=8)

ax4.plot(lmul_nums, theoretical_speedup, 'k--', label='Theoretical (Perfect)', linewidth=2, alpha=0.7)
ax4.set_xlabel('LMUL Value')
ax4.set_ylabel('Speedup Factor')
ax4.set_xticks(lmul_nums)
ax4.set_xticklabels([f'm{lmul}' for lmul in lmul_nums])
ax4.legend()
ax4.grid(True, alpha=0.3)

plt.tight_layout()
plt.savefig('lmul_performance_analysis.png', dpi=300, bbox_inches='tight')
plt.show()

# Generate summary statistics
print("\n=== LMUL PERFORMANCE SUMMARY ===")
for op in operations:
    print(f"\n{op}:")
    op_data = df[df['Operation'] == op]
    best_lmul = op_data.loc[op_data['Speedup_vs_m1'].idxmax(), 'LMUL']
    best_speedup = op_data['Speedup_vs_m1'].max()
    print(f"  Best LMUL: {best_lmul} ({best_speedup:.2f}x speedup)")
    print(f"  m2 efficiency: {op_data[op_data['LMUL']=='m2']['Speedup_vs_m1'].values[0]:.2f}x")
    print(f"  m4 efficiency: {op_data[op_data['LMUL']=='m4']['Speedup_vs_m1'].values[0]:.2f}x")
    print(f"  m8 efficiency: {op_data[op_data['LMUL']=='m8']['Speedup_vs_m1'].values[0]:.2f}x")
)";
        
        std::cout << "\nPython plotting script generated: " << scriptName << std::endl;
        std::cout << "Run 'python3 " << scriptName << "' to generate performance graphs." << std::endl;
    }
};

int main() {
    LMULBenchmarkSuite suite;
    
    if (!suite.initialize("barb.512.pgm")) {
        return 1;
    }
    
    suite.runAllBenchmarks();
    suite.printResults();
    suite.generateCSV("lmul_benchmark_results.csv");
    suite.generatePythonScript("plot_lmul_results.py");
    
    std::cout << "\n=== ANALYSIS COMPLETE ===" << std::endl;
    std::cout << "1. Review the console output above for immediate results" << std::endl;
    std::cout << "2. Open lmul_benchmark_results.csv for detailed data" << std::endl;
    std::cout << "3. Run 'python3 plot_lmul_results.py' to generate graphs" << std::endl;
    std::cout << "\nThis analysis will help determine optimal LMUL values for different operations." << std::endl;
    
    return 0;
} 