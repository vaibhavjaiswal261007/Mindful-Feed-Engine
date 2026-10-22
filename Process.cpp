#include "Process.h"
#include <cstdio>
#include <sstream>
#include <stdexcept>
#ifdef _WIN32
#include <windows.h>
#include <fstream>
#else
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#endif
namespace mfe
{
#ifndef _WIN32
    ProcessResult runProcess(const std::vector<std::string> &args)
    {
        ProcessResult r;
        if (args.empty())
            return r;
        int pipefd[2];
        if (pipe(pipefd) != 0)
            return r;
        pid_t pid = fork();
        if (pid == 0)
        {
            close(pipefd[0]);
            int nul = open("/dev/null", O_RDONLY);
            if (nul >= 0)
            {
                dup2(nul, STDIN_FILENO);
                close(nul);
            }
            dup2(pipefd[1], STDOUT_FILENO);
            dup2(pipefd[1], STDERR_FILENO);
            close(pipefd[1]);
            std::vector<char *> argv;
            for (const auto &a : args)
                argv.push_back(const_cast<char *>(a.c_str()));
            argv.push_back(nullptr);
            execvp(argv[0], argv.data());
            _exit(127);
        }
        close(pipefd[1]);
        if (pid < 0)
        {
            close(pipefd[0]);
            return r;
        }
        r.launched = true;
        char buf[4096];
        ssize_t n;
        while ((n = read(pipefd[0], buf, sizeof(buf))) > 0)
            r.output.append(buf, static_cast<size_t>(n));
        close(pipefd[0]);
        int status = 0;
        waitpid(pid, &status, 0);
        r.exitCode = WIFEXITED(status) ? WEXITSTATUS(status) : -1;
        return r;
    }
#else
    static std::string quote(const std::string &s)
    {
        // Follow the Windows C runtime command-line quoting rules: backslashes
        // preceding a quote are doubled, and trailing backslashes are doubled
        // before the closing quote.
        std::string out = "\"";
        std::size_t backslashes = 0;
        for (char c : s)
        {
            if (c == '\\')
            {
                ++backslashes;
            }
            else if (c == '"')
            {
                out.append(backslashes * 2 + 1, '\\');
                out += '"';
                backslashes = 0;
            }
            else
            {
                out.append(backslashes, '\\');
                backslashes = 0;
                out += c;
            }
        }
        out.append(backslashes * 2, '\\');
        out += '"';
        return out;
    }
    ProcessResult runProcess(const std::vector<std::string> &args)
    {
        ProcessResult r;
        if (args.empty())
            return r;
        // Windows CreateProcess receives a quoted argument vector; no shell is involved.
        std::string cmd;
        for (const auto &a : args)
        {
            if (!cmd.empty())
                cmd += ' ';
            cmd += quote(a);
        }
        SECURITY_ATTRIBUTES sa{sizeof(SECURITY_ATTRIBUTES), nullptr, TRUE};
        HANDLE rd = nullptr, wr = nullptr;
        if (!CreatePipe(&rd, &wr, &sa, 0))
            return r;
        SetHandleInformation(rd, HANDLE_FLAG_INHERIT, 0);
        STARTUPINFOA si{};
        si.cb = sizeof(si);
        si.dwFlags = STARTF_USESTDHANDLES;
        si.hStdOutput = wr;
        si.hStdError = wr;
        HANDLE nulIn = CreateFileA("NUL", GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, &sa, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
        si.hStdInput = nulIn != INVALID_HANDLE_VALUE ? nulIn : GetStdHandle(STD_INPUT_HANDLE);
        PROCESS_INFORMATION pi{};
        std::vector<char> mutableCmd(cmd.begin(), cmd.end());
        mutableCmd.push_back(0);
        BOOL ok = CreateProcessA(nullptr, mutableCmd.data(), nullptr, nullptr, TRUE, CREATE_NO_WINDOW, nullptr, nullptr, &si, &pi);
        CloseHandle(wr);
        if (nulIn != INVALID_HANDLE_VALUE)
            CloseHandle(nulIn);
        if (!ok)
        {
            CloseHandle(rd);
            return r;
        }
        r.launched = true;
        char b[4096];
        DWORD n;
        while (ReadFile(rd, b, sizeof(b), &n, nullptr) && n)
            r.output.append(b, n);
        WaitForSingleObject(pi.hProcess, INFINITE);
        DWORD code = 0;
        GetExitCodeProcess(pi.hProcess, &code);
        r.exitCode = (int)code;
        CloseHandle(rd);
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
        return r;
    }
#endif
}
