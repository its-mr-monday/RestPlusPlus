/*
    Copyright 2024 by its-mr-monday.
    All rights reserved
    This file is part of the RestPlusPlus Framework, and is released 
    under the "MIT License Agreement". Please see the LICENSE file that 
    should have been included as part of this package
*/

#include "restplus.hpp"
#include "restplusthreadpool.hpp"
#include "socketfunc.hpp"
#include <sstream>

/*
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
    bool &RUNNING;
    int &MAX_THREADS;
    int &CURRENT_THREADS;
    std::vector<std::thread> &threads;
};

struct Job {
    ThreadArguments &args;
    std::thread thread;
};
*/

MasterThreadRoutine::MasterThreadRoutine(bool &RUNNING, int &MAX_THREADS, std::vector<Job> &job_queue, std::vector<Job> &running_jobs) : RUNNING(RUNNING), MAX_THREADS(MAX_THREADS), job_queue(job_queue), running_jobs(running_jobs) {};

void thread_sleep(int milliseconds) {
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
};

void thread_func(ThreadArguments args) {
    while (!args.thread_routine.RUNNING) {
        thread_sleep(100);
    }
    char buffer[1024];
    std::stringstream requeststream;
    int bytes_read;
    SOCKET client_socket = args.client_socket;
    do {
        bytes_read = multiread(client_socket, buffer, 1024);
        if (bytes_read < 0 || bytes_read == SOCKET_ERROR) {
            multiclose(client_socket);
            args.thread_routine.HAS_EXITED = true;
            printerror("Error reading from socket");
            return;
        }
        requeststream << std::string(buffer, bytes_read);
    } while (bytes_read > 0 || requeststream.str().find("\r\n\r\n") == std::string::npos);

    HTTPRequest request = parse_request(requeststream.str());
    HTTPResponse response = args.api.handle_request(request);
    std::string response_string = response.to_string();
    int bytes_sent = send(client_socket, response_string.c_str(), response_string.length(), 0);
    if (bytes_sent < 0 || bytes_sent == SOCKET_ERROR) {
        printerror("Error sending response:\n");
        printlasterror();
    }
    multiclose(client_socket);
    if (args.api_info.LOGGING) {
        log_request(request, response, args.api_info.LOG_FILE_NAME);
    }
    if (args.api_info.DEBUG_MODE) {
        debug_request(request, response);
    }
    args.thread_routine.HAS_EXITED = true;
    return;
};

void kill_threads(MasterThreadRoutine args) {
    //Clear job queue
    args.job_queue.clear();
    //Set all running jobs to be KILLED
    //Loop through running_jobs
    for (int x = 0;x < args.running_jobs.size(); x++) {
        Job running_job = args.running_jobs[x];
        running_job.args.thread_routine.RUNNING = false;
        //join the thread
        running_job.thread.join();
    }
    args.running_jobs.clear();
    return;
}

void master_thread_func(MasterThreadRoutine args) {
    if (!args.RUNNING) {
        //Wait till we are running
        thread_sleep(100);
    }
    while (args.RUNNING) {
        int current_threads = args.job_queue.size();
        int MAX = args.MAX_THREADS;
        if (current_threads < MAX) {
            //Start new threads if we can
            int remaining = MAX - current_threads;
            for (int i = 0; i < remaining; i++) {
                if (args.job_queue.size() > 0) {
                    Job job = args.job_queue[0];
                    args.job_queue.erase(args.job_queue.begin());
                    //set the thread
                    job.thread = std::thread(thread_func, job.args);
                    args.running_jobs.push_back(job);\
                    //Set status to running
                    job.args.thread_routine.RUNNING = true;
                    //The thread should already exist so just start it
                }
            }
        }
        //Check for finished jobs and remove them
        for (int i = 0; i < args.running_jobs.size(); i++) {
            Job job = args.running_jobs[i];
            if (job.args.thread_routine.HAS_EXITED) {
                //join the thread and remove it
                job.thread.join();
                args.running_jobs.erase(args.running_jobs.begin() + i);
            }
        }
        thread_sleep(100);
    }
    //If we are here RUNNING has be killed and thus all jobs must be set to killed
    kill_threads(args);
    return;
}

RestPlusThreadPool::RestPlusThreadPool() {
    MasterThreadRoutine master_thread_routine = { RUNNING, MAX_THREADS, job_queue, running_jobs };
    RUNNING = false;
}

void RestPlusThreadPool::start() {
    master_thread = std::thread(master_thread_func, master_thread_routine);
    RUNNING = true;
}

void RestPlusThreadPool::newjob(SOCKET client_socket, RestPlus &api, RestPlusAPIInfo api_info) {
    ThreadRoutine thread_routine;
    ThreadArguments args = {api, client_socket, api_info, thread_routine};
    //Create a thread using the thread_func function
    //Don't worry it will not run until the thread_routine.RUNNING variable is set to true
    std::thread job_thread;
    Job job = {args, job_thread};
    //Add the job to the job queue
    job_queue.push_back(job);
}

void RestPlusThreadPool::join() {
    //Stop the master thread
    RUNNING = false;
    //Join the master thread
    master_thread.join();
    //Clear job queue
    this->clear_queue();
    //Join all the running jobs
    for (int i = 0; i < running_jobs.size(); i++) {
        Job job = running_jobs[i];
        //Check if the thread has already join()
        //master_thread should join all threads prior to exit
        if (job.thread.joinable())
            job.thread.join();
    }
    //Clear the running jobs
    running_jobs.clear();
}

void RestPlusThreadPool::clear_queue() {
    job_queue.clear();
}

void RestPlusThreadPool::set_max_threads(int max_threads) {
    MAX_THREADS = max_threads;
}

int RestPlusThreadPool::size() {
    return job_queue.size();
}

RestPlusThreadPool::~RestPlusThreadPool() {
    join();
}