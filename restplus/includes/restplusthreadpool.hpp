#pragma once
#include <thread>
#include <vector>
#include <iostream>
#include <map>
#ifdef __unix__
using SOCKET = int;
#else
#include <windows.h>
typedef UINT_PTR SOCKET;
#endif

struct ThreadRoutine {
    bool RUNNING = false;
};

class RestPlusThreadPool {
    public:
        RestPlusThreadPool();
        ~RestPlusThreadPool();
        void newjob(SOCKET client_socket, class RestPlus &api, struct RestPlusAPIInfo api_info);
        void clear();
        int size();
    private:
        void startjob(int thread_id);
        std::vector<std::thread> threads;
        std::map<int, ThreadRoutine> thread_routines;
        int MAX_THREADS = 10;
};