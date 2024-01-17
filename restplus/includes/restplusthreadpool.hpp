#pragma once
#include "restplus.hpp"
#include "pcf.h"
#include <thread>
#include <vector>
#include <iostream>
#include <map>

struct ThreadRoutine {
    bool RUNNING = false;
    bool HAS_EXITED = false;
};

struct ThreadArguments {
    RestPlus &api;
    SOCKET client_socket;
    RestPlusAPIInfo api_info;
    ThreadRoutine &thread_routine;
};

struct MasterThreadRoutine {
    MasterThreadRoutine() = default;
    MasterThreadRoutine(bool &RUNNING, int &MAX_THREADS, std::vector<Job> &job_queue, std::vector<Job> &running_jobs);
    bool &RUNNING;
    int &MAX_THREADS;
    std::vector<Job> &job_queue;
    std::vector<Job> &running_jobs;
};

struct Job {
    ThreadArguments &args;
    std::thread &thread;
};

class RestPlusThreadPool {
    public:
        RestPlusThreadPool();
        ~RestPlusThreadPool();
        //Creates a new job and adds it to the job queue
        void newjob(SOCKET client_socket, RestPlus &api, RestPlusAPIInfo api_info);
        //Kills all threads and joins them, anything in job queue will be lost
        void join();
        //Clears the Job queue
        void clear_queue();
        void clear();
        int size();
    private:
        void startjob(int thread_id);
        std::vector<Job> job_queue;
        std::vector<Job> running_jobs;
        MasterThreadRoutine master_thread_routine;
        std::thread master_thread;
        int MAX_THREADS = 10;
        bool RUNNING = false;
};