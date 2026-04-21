#ifndef RTEMSG_PORTABILITY_H
#define RTEMSG_PORTABILITY_H

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <direct.h>
#include <windows.h>
#include <sys/stat.h>

#define RTE_NORETURN __declspec(noreturn)
#define RTE_NOINLINE __declspec(noinline)
#define RTE_MAX_PATH _MAX_PATH
#else
#include <limits.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#define RTE_NORETURN __attribute__((noreturn))
#define RTE_NOINLINE __attribute__((noinline))

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

#define RTE_MAX_PATH PATH_MAX
#endif

static inline void rte_set_console_utf8(void)
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
}

static inline void rte_sleep_ms(unsigned ms)
{
#ifdef _WIN32
    Sleep(ms);
#else
    struct timespec request;
    request.tv_sec = (time_t)(ms / 1000U);
    request.tv_nsec = (long)((ms % 1000U) * 1000000UL);
    nanosleep(&request, NULL);
#endif
}

static inline void rte_clear_errno(void)
{
    errno = 0;
}

static inline void rte_set_errno_value(int value)
{
    errno = value;
}

static inline int rte_change_directory(const char *path)
{
#ifdef _WIN32
    return _chdir(path);
#else
    return chdir(path);
#endif
}

static inline char *rte_get_current_directory(void)
{
#ifdef _WIN32
    return _getcwd(NULL, 0);
#else
    return getcwd(NULL, 0);
#endif
}

static inline int rte_get_program_path(char *buffer, size_t size)
{
    if ((buffer == NULL) || (size == 0))
    {
        return -1;
    }

#ifdef _WIN32
    DWORD length = GetModuleFileNameA(NULL, buffer, (DWORD)size);
    if ((length == 0) || (length >= size))
    {
        return -1;
    }

    buffer[length] = '\0';
    return 0;
#else
    ssize_t length = readlink("/proc/self/exe", buffer, size - 1);
    if ((length < 0) || ((size_t)length >= size))
    {
        return -1;
    }

    buffer[length] = '\0';
    return 0;
#endif
}

static inline int rte_seek_file_end(FILE *fp)
{
#ifdef _WIN32
    return _fseeki64(fp, 0, SEEK_END);
#else
    return fseeko(fp, 0, SEEK_END);
#endif
}

static inline int64_t rte_tell_file(FILE *fp)
{
#ifdef _WIN32
    return _ftelli64(fp);
#else
    return (int64_t)ftello(fp);
#endif
}

static inline int rte_get_full_path(const char *path, char *resolved_path, size_t resolved_path_size)
{
    if ((path == NULL) || (resolved_path == NULL) || (resolved_path_size == 0))
    {
        return -1;
    }

#ifdef _WIN32
    return (_fullpath(resolved_path, path, resolved_path_size) == NULL) ? -1 : 0;
#else
    return (realpath(path, resolved_path) == NULL) ? -1 : 0;
#endif
}

static inline void rte_get_desktop_work_area(unsigned *screen_width, unsigned *screen_height)
{
#ifdef _WIN32
    RECT work_area;
    if (SystemParametersInfo(SPI_GETWORKAREA, 0, &work_area, 0))
    {
        *screen_width = (unsigned)(work_area.right - work_area.left);
        *screen_height = (unsigned)(work_area.bottom - work_area.top);
    }
#else
    (void)screen_width;
    (void)screen_height;
#endif
}

#endif
