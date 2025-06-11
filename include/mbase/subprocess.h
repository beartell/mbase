#ifndef MBASE_SUBPROCESS_H
#define MBASE_SUBPROCESS_H

#include <mbase/common.h>
#include <mbase/string.h>
#include <mbase/vector.h>
#include <mbase/unordered_map.h>
#include <mbase/io_file.h>

#ifdef MBASE_PLATFORM_UNIX
    #include <unistd.h> // pipe, fork, exec family procedures
    #include <sys/wait.h>
#endif

#ifdef MBASE_PLATFORM_WINDOWS
    #include <Windows.h>
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

    bool mIsChild = false;
    mbase::string mProcessName;
    mbase::io_file mReadPipe;
    mbase::io_file mWritePipe;
    mbase::io_file mReadPipe1;
    mbase::io_file mWritePipe1;
    #ifdef MBASE_PLATFORM_UNIX
    int mChildPid = 0;
    #endif
    #ifdef MBASE_PLATFORM_WINDOWS
    STARTUPINFO procStartInformation;
    PROCESS_INFORMATION procInformation;
    mbase::wstring commandLineString;
    #endif
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
    this->join();
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
    #ifdef MBASE_PLATFORM_UNIX
    if(mChildPid)
    {
        mWritePipe.close_file();
        mReadPipe1.close_file();
        int status = 0;
        waitpid(mChildPid, &status, 0);
    }
    #endif 

    #ifdef MBASE_PLATFORM_WINDOWS
    if(mIsChild)
    {
        mWritePipe.close_file();
        mReadPipe1.close_file();
        WaitForSingleObject(procInformation.hProcess, INFINITE);
        CloseHandle(procInformation.hProcess);
        CloseHandle(procInformation.hThread);
    }
    #endif

    mIsChild = false;
}

MBASE_INLINE GENERIC subprocess::create_new_process(bool in_stdio, mbase::string in_process_name, process_arguments in_arguments, process_environment_vars in_environment_variables)
{
    this->join();
    if(!in_process_name.size())
    {
        mIsChild = false;
        return;
    }

    mProcessName = in_process_name;
    #ifdef MBASE_PLATFORM_UNIX
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
    
    mbase::vector<char*> argumentsArray(in_arguments.size() + 2); // + 2 stands for two things. First is the program name, last is the null terminator
    argumentsArray.push_back(in_process_name.data());

    for(mbase::string& tmpArgument : in_arguments)
    {
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
    #endif

    #ifdef MBASE_PLATFORM_WINDOWS
    ZeroMemory(&procStartInformation, sizeof(procStartInformation));
    procStartInformation.cb = sizeof(procStartInformation);
    ZeroMemory(&procInformation, sizeof(procInformation));
    //commandLineString.clear();
    commandLineString = mbase::from_utf8(mProcessName);

    for(mbase::string& tmpArgument : in_arguments)
    {
        commandLineString += ' ' + mbase::from_utf8(tmpArgument);
    }
    commandLineString.push_back('\0');

    SECURITY_ATTRIBUTES securityAttrs;
    securityAttrs.nLength = sizeof(SECURITY_ATTRIBUTES);
    securityAttrs.bInheritHandle = TRUE;
    securityAttrs.lpSecurityDescriptor = NULL;

    HANDLE parentReadEnd = NULL;
    HANDLE childWriteEnd = NULL;
    HANDLE childReadEnd = NULL;
    HANDLE parentWriteEnd = NULL;

    if(!CreatePipe(&parentReadEnd, &childWriteEnd, &securityAttrs, 0))
    { 
        return; 
    }
    if(!CreatePipe(&childReadEnd, &parentWriteEnd, &securityAttrs, 0))
    { 
        CloseHandle(parentReadEnd);
        CloseHandle(childWriteEnd);
        return; 
    }
    if(!SetHandleInformation(parentReadEnd, HANDLE_FLAG_INHERIT, 0))
    {
        CloseHandle(parentReadEnd);
        CloseHandle(childWriteEnd);
        CloseHandle(childReadEnd);
        CloseHandle(parentWriteEnd);
        return;
    }
    if(!SetHandleInformation(parentWriteEnd, HANDLE_FLAG_INHERIT, 0))
    {
        CloseHandle(parentReadEnd);
        CloseHandle(childWriteEnd);
        CloseHandle(childReadEnd);
        CloseHandle(parentWriteEnd);
        return;
    }

    if(in_stdio)
    {
        procStartInformation.hStdOutput = childWriteEnd;
        procStartInformation.hStdInput = childReadEnd;
        procStartInformation.dwFlags |= STARTF_USESTDHANDLES;
    }

    mbase::vector<wchar_t> environmentBlockData;

    for(auto& envKval : in_environment_variables)
    {
        mbase::wstring wEnvKey = mbase::from_utf8(envKval.first);
        mbase::wstring wEnvVal = mbase::from_utf8(envKval.second);

        for(const wchar_t& tmpWch : wEnvKey)
        {
            environmentBlockData.push_back(tmpWch);
        }
        environmentBlockData.push_back(L'=');
        for(const wchar_t& tmpWch : wEnvVal)
        {
            environmentBlockData.push_back(tmpWch);
        }
        environmentBlockData.push_back(L'\0');
    }

    wchar_t* envStrings = GetEnvironmentStrings();
    wchar_t* current = envStrings;
    while (*current) {
        mbase::SIZE_T currentRowSize = wcslen(current) + 1;
        for(mbase::SIZE_T i = 0; i < currentRowSize; i++)
        {
            environmentBlockData.push_back(*current);
            current++;
        }
    }

    environmentBlockData.push_back('\0');
    
    if(!CreateProcess(
        NULL,
        commandLineString.data(),
        NULL,
        NULL,
        TRUE,
        CREATE_UNICODE_ENVIRONMENT,
        environmentBlockData.data(),
        NULL,
        &procStartInformation,
        &procInformation
    ))
    {
        // subprocess creation failed
        FreeEnvironmentStrings(envStrings);
        return;
    }
    FreeEnvironmentStrings(envStrings);
    CloseHandle(childReadEnd);
    CloseHandle(childWriteEnd);
    mWritePipe.set_raw_handle(parentWriteEnd);
    mReadPipe1.set_raw_handle(parentReadEnd);
    mIsChild = true;
    #endif
}

MBASE_END

#endif // MBASE_SUBPROCESS_H
