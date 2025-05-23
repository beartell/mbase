#ifndef MBASE_SUBPROCESS_H
#define MBASE_SUBPROCESS_H

#include <mbase/common.h>
#include <mbase/string.h>
#include <mbase/vector.h>
#include <mbase/unordered_map.h>
#include <mbase/io_file.h>

#ifdef MBASE_PLATFORM_UNIX
#include <unistd.h> // pipe, fork, exec family procedures
#endif

MBASE_BEGIN

// creates a subprocess with bidirectional pipe as a communication channel
class subprocess {
public:
    using process_arguments = mbase::vector<mbase::string>;
    using process_environment_vars = mbase::unordered_map<mbase::string, mbase::string>;

    // if in stdio is true, the subprocess will communicate with the parent using STDIO
    MBASE_INLINE subprocess(bool in_stdio = false); // forks the main process
    MBASE_INLINE subprocess(bool in_stdio, const mbase::string& in_process_name); // creates a new process
    MBASE_INLINE subprocess(bool in_stdio, const mbase::string& in_process_name, const process_arguments& in_arguments); // creates a new process with arguments
    MBASE_INLINE subprocess(bool in_stdio, const mbase::string& in_process_name, const process_arguments& in_arguments, const process_environment_vars& in_environment_variables); // creates a new process with arguments and environment variables
    MBASE_INLINE subprocess(const subprocess&) = delete;
    MBASE_INLINE subprocess(subprocess&&) = delete;
    MBASE_INLINE ~subprocess();

    MBASE_INLINE subprocess& operator=(const subprocess&) = delete;
    MBASE_INLINE subprocess& operator=(subprocess&&) = delete;

    MBASE_INLINE bool is_child() const noexcept;
    MBASE_INLINE io_file& get_read_pipe();
    MBASE_INLINE io_file& get_write_pipe();
    MBASE_INLINE io_file& get_read_pipe1();
    MBASE_INLINE io_file& get_write_pipe1();
    MBASE_INLINE const mbase::string& get_process_name() const noexcept;
    MBASE_INLINE GENERIC join();

private:
    MBASE_INLINE GENERIC create_new_process(bool in_stdio, mbase::string in_process_name = mbase::string(), process_arguments in_arguments = process_arguments(), process_environment_vars in_environment_variables = process_environment_vars());

    bool mIsChild = true;
    mbase::string mProcessName;
    mbase::io_file mReadPipe;
    mbase::io_file mWritePipe;
    mbase::io_file mReadPipe1;
    mbase::io_file mWritePipe1;
    int mChildPid = 0;
};

MBASE_INLINE subprocess::subprocess(bool in_stdio)
{
    create_new_process(in_stdio);
}

MBASE_INLINE subprocess::subprocess(bool in_stdio, const mbase::string& in_process_name)
{
    create_new_process(in_stdio, in_process_name);
}

MBASE_INLINE subprocess::subprocess(bool in_stdio, const mbase::string& in_process_name, const process_arguments& in_arguments)
{
    create_new_process(in_stdio, in_process_name, in_arguments);
}

MBASE_INLINE subprocess::subprocess(bool in_stdio, const mbase::string& in_process_name, const process_arguments& in_arguments, const process_environment_vars& in_environment_variables)
{
    create_new_process(in_stdio, in_process_name, in_arguments, in_environment_variables);
}

MBASE_INLINE subprocess::~subprocess()
{
    if(mChildPid)
    {
        int status = 0;
        waitpid(mChildPid, &status, 0);
    }
}

bool subprocess::is_child() const noexcept
{
    return mIsChild;
}

MBASE_INLINE io_file& subprocess::get_read_pipe()
{
    return mReadPipe;
}

MBASE_INLINE io_file& subprocess::get_write_pipe()
{
    return mWritePipe;
}

MBASE_INLINE io_file& subprocess::get_read_pipe1()
{
    return mReadPipe1;
}

MBASE_INLINE io_file& subprocess::get_write_pipe1()
{
    return mWritePipe1;
}

MBASE_INLINE const mbase::string& subprocess::get_process_name() const noexcept
{
    return mProcessName;
}

MBASE_INLINE GENERIC subprocess::join()
{
    if(mChildPid)
    {
        mWritePipe.close_file();
        mReadPipe1.close_file();
        int status = 0;
        waitpid(mChildPid, &status, 0);
    }
}

MBASE_INLINE GENERIC subprocess::create_new_process(bool in_stdio, mbase::string in_process_name, process_arguments in_arguments, process_environment_vars in_environment_variables)
{
    mIsChild = true;
    this->join();
    int pipeFd[2] = {0};
    int pipeFd1[2] = {0};
    pipe(pipeFd);
    pipe(pipeFd1);
    mReadPipe.set_raw_handle(pipeFd[0]);
    mWritePipe.set_raw_handle(pipeFd[1]);
    mReadPipe1.set_raw_handle(pipeFd1[0]);
    mWritePipe1.set_raw_handle(pipeFd1[1]);

    mChildPid = fork();

    if(mChildPid)
    {
        // the parent process
        mReadPipe.close_file();
        mWritePipe1.close_file();
        return;
    }
    mIsChild = false;
    mWritePipe.close_file();
    mReadPipe1.close_file();

    if(in_stdio)
    {
        dup2(pipeFd[0], STDIN_FILENO);
        dup2(pipeFd1[1], STDOUT_FILENO);
        mReadPipe.set_raw_handle(STDIN_FILENO);
        mWritePipe1.set_raw_handle(STDOUT_FILENO);
        close(pipeFd[0]);
        close(pipeFd1[1]);
    }

    if(!in_process_name.size())
    {
        mIsChild = false;
        return;
    }

    mProcessName = in_process_name;

    mbase::vector<char*> argumentsArray(in_arguments.size() + 2); // + 2 stands for two things. First is the program name, last is the null terminator
    argumentsArray.push_back(in_process_name.data());

    for(mbase::string& tmpArgument : in_arguments)
    {
        if(tmpArgument.size() > ARG_MAX)
        {
            // argument data length is too long
            // handle this case
            return;
        }
        argumentsArray.push_back(tmpArgument.data());
    }
    argumentsArray.push_back(NULL);

    if(!in_environment_variables.size())
    {
        execvp(in_process_name.c_str(), argumentsArray.data());
        exit(1);
        return;
    }

    else
    {
        mbase::vector<mbase::string> envVarString;
        mbase::vector<char*> environmentVariables(in_environment_variables.size());
        for(auto& envVar : in_environment_variables)
        {
            envVarString.push_back(mbase::string(envVar.first + '=' + envVar.second));
            environmentVariables.push_back(envVarString.back().data());
        }
        
        execve(in_process_name.c_str(), argumentsArray.data(), environmentVariables.data());
        exit(1);
        return;
    }
}

MBASE_END

#endif MBASE_SUBPROCESS_H