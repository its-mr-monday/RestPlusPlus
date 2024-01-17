#pragma once
#include "pcf.h"
#include <thread>
#include <vector>
#include <iostream>
#include <map>

//Forward declaration of the RestPlus class
class RestPlus;
//Forward declaration of the RestPlusAPIInfo struct
struct RestPlusAPIInfo;

//ThreadRoutine struct
struct ThreadRoutine {
    bool RUNNING = false;
    bool HAS_EXITED = false;
};

//ThreadArguments struct
struct ThreadArguments {
    RestPlus &api;
    SOCKET client_socket;
    RestPlusAPIInfo api_info;
    ThreadRoutine &thread_routine;
};

//MasterThreadRoutine struct
struct MasterThreadRoutine {
    MasterThreadRoutine() = default;
    MasterThreadRoutine(bool &RUNNING, int &MAX_THREADS, std::vector<Job> &job_queue, std::vector<Job> &running_jobs);
    bool &RUNNING;
    int &MAX_THREADS;
    std::vector<Job> &job_queue;
    std::vector<Job> &running_jobs;
};

//Job struct
struct Job {
    ThreadArguments &args;
    std::thread &thread;
};

//RestPlusThreadPool class
class RestPlusThreadPool {
    public:
        //Constructor of the RestPlusThreadPool class
        RestPlusThreadPool();
        //Destructor of the RestPlusThreadPool class
        ~RestPlusThreadPool();
        //Creates a new job and adds it to the job queue
        void newjob(SOCKET client_socket, RestPlus &api, RestPlusAPIInfo api_info);
        //Sets the maximum number of threads to be used at once
        void set_max_threads(int max_threads);
        //Kills all threads and joins them, anything in job queue will be lost
        void join();
        //Clears the Job queue
        void clear_queue();
        //Returns the size of the job queue
        int size();
    private:
        std::vector<Job> job_queue;
        std::vector<Job> running_jobs;
        MasterThreadRoutine master_thread_routine;
        std::thread master_thread;
        int MAX_THREADS = 10;
        bool RUNNING = false;
};