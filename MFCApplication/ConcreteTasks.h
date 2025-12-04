#pragma once
#include "ITask.h"
#include <vector>
#include <random>
#include <chrono>
#include <sstream>
#include <afxwin.h> // 用于 AfxMessageBox

// --- Task B: 矩阵乘法 (CPU密集型任务) ---
class MatrixTask : public ITask {
public:
    std::string GetName() const override { return "Matrix Calculation (Task B)"; }

    void Execute() override {
        // 模拟 200x200 矩阵乘法 [cite: 223]
        int size = 200;
        std::vector<std::vector<int>> A(size, std::vector<int>(size, 1));
        std::vector<std::vector<int>> B(size, std::vector<int>(size, 2));
        std::vector<std::vector<int>> C(size, std::vector<int>(size, 0));

        auto start = std::chrono::high_resolution_clock::now();

        // 简单的 O(N^3) 乘法，足以消耗一些CPU时间
        for (int i = 0; i < size; ++i) {
            for (int j = 0; j < size; ++j) {
                for (int k = 0; k < size; ++k) {
                    C[i][j] += A[i][k] * B[k][j];
                }
            }
        }

        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

        // 注意：实际日志会由 Scheduler 在外部记录，但这里我们可以打印调试信息
        // 在实际 MFC 调试输出窗口可以看到
        std::stringstream ss;
        ss << "Matrix mult finished in " << duration << " ms";
        OutputDebugStringA(ss.str().c_str());
    }
};

// --- Task D: 课堂提醒 (UI交互任务) ---
class ReminderTask : public ITask {
public:
    std::string GetName() const override { return "Class Reminder (Task D)"; }

    void Execute() override {
        // 课件要求：弹出“休息5分钟”对话框 [cite: 227]
        // 注意：AfxMessageBox 会阻塞当前工作线程，直到用户点击确定
        // 在实际复杂系统中通常使用 PostMessage 通知主线程弹窗，但作为作业演示，直接弹窗是最简单的
        AfxMessageBox(_T("休息5分钟！(Take a 5 min break)"), MB_OK | MB_ICONINFORMATION);
    }
};

// --- Task A: 文件备份 (模拟) ---
class BackupTask : public ITask {
public:
    std::string GetName() const override { return "File Backup (Task A)"; }
    void Execute() override {
        // 模拟备份延迟
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        OutputDebugStringA("Backup completed successfully.");
    }
};

#include <afxinet.h> // 必须包含，用于 HTTP 网络请求

// --- Task C: HTTP GET 请求 ---
class HttpTask : public ITask {
public:
    std::string GetName() const override { return "HTTP GET (Task C)"; }

    void Execute() override {
        CInternetSession session(_T("TaskSchedulerSession"));
        CHttpFile* pFile = nullptr;
        try {
            // 请求 GitHub API
            pFile = (CHttpFile*)session.OpenURL(_T("https://api.github.com/zen"));
            if (pFile) {
                CString line;
                std::string content;
                // 读取响应内容
                while (pFile->ReadString(line)) {
                    content += CT2A(line); // CString 转 std::string
                    content += "\n";
                }
                pFile->Close();

                // 写入 zen.txt (这是课件要求的)
                std::ofstream outFile("zen.txt");
                outFile << content;
                outFile.close();

                // 同时也输出到调试日志
                OutputDebugStringA(("HTTP Content: " + content).c_str());
            }
        }
        catch (CInternetException* pEx) {
            // 捕获网络异常
            char szErr[1024];
            pEx->GetErrorMessage((LPTSTR)szErr, 1024);
            pEx->Delete();
            throw std::runtime_error("Network error"); // 抛出异常供 Scheduler 记录
        }

        if (pFile) delete pFile;
        session.Close();
    }
};
#include <numeric> // 用于 accumulate

// --- Task E: 随机数统计 ---
class StatsTask : public ITask {
public:
    std::string GetName() const override { return "Random Stats (Task E)"; }

    void Execute() override {
        // 1. 生成 1000 个随机数 (0-100)
        std::vector<int> numbers;
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 100);

        for (int i = 0; i < 1000; ++i) {
            numbers.push_back(dis(gen));
        }

        // 2. 计算均值 (Mean)
        double sum = std::accumulate(numbers.begin(), numbers.end(), 0.0);
        double mean = sum / numbers.size();

        // 3. 计算方差 (Variance)
        double sq_sum = std::inner_product(numbers.begin(), numbers.end(), numbers.begin(), 0.0);
        double variance = (sq_sum / numbers.size()) - (mean * mean);

        // 4. 格式化输出结果
        std::string result = "Mean: " + std::to_string(mean) + ", Variance: " + std::to_string(variance);

        // 这里可以直接抛出一个带有信息的异常来作为日志输出，或者依靠 Scheduler 的成功日志
        // 为了演示，我们把结果打印到调试输出
        OutputDebugStringA(result.c_str());

        // 也可以写入独立日志文件（如果需要）
    }
};