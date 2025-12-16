#include "utils/logger/stacktrace.h"

#ifdef _DEBUG
    #ifndef __USE_GNU
    #define __USE_GNU
    #endif

    #ifndef _GNU_SOURCE
    #define _GNU_SOURCE
    #endif

    // Linux-specific code
    #include <execinfo.h>
    #include <dlfcn.h>
    #include <stdint.h>
    #include <unistd.h>
    #include <limits.h>

    static char   g_exec_path[PATH_MAX_LENGTH];
    static int    g_exec_path_initialized = 0;

    /* Read /proc/self/exe once so we can call addr2line later */
    int init_symbol_handler(void) {
        if (g_exec_path_initialized) return 1;
        ssize_t len = readlink("/proc/self/exe", g_exec_path, sizeof(g_exec_path)-1);
        if (len > 0 && len < (ssize_t)sizeof(g_exec_path)) {
            g_exec_path[len] = '\0';
        } else {
            g_exec_path[0] = '\0';
        }
        g_exec_path_initialized = 1;

        return g_exec_path_initialized;
    }

    void cleanup_symbol_handler(void) {
        /* No resources to free on Linux version */
        (void)g_exec_path_initialized;
    }

    /**
    * depth: backtrace index.
    *   0 => this function,
    *   1 => the direct caller of this function,
    *   2 => caller’s caller, etc.
    */
    int get_caller_info(char *out, size_t outSize, unsigned int depth) {
        if (!g_exec_path_initialized) {
            printf("symbols handler wasn't init\n");
            init_symbol_handler();
        }

        void *buffer[MAX_FRAMES];
        int nptrs = backtrace(buffer, MAX_FRAMES);
        if (nptrs <= (int)depth) {
            return snprintf(out, outSize, "No stack frame at depth %u", depth) < 0 ? -1 : 0;
        }

        void *addr = buffer[depth];
        Dl_info info;

        const char *func = NULL;

        if (dladdr(addr, &info) && info.dli_sname) {
            func = info.dli_sname;
        } else {
            func = "[unknown]";
        }
        
        /* Write function name */
        int w = snprintf(out, outSize, "Caller function: %s", func);
        if (w < 0 || (size_t)w >= outSize) return -1;
        
        /* Determine module path and relative address for addr2line */
        const char *module = g_exec_path;
        void *rel_addr = addr;
        if (dladdr(addr, &info)) {
            if (info.dli_fname && info.dli_fname[0] != '\0') {
                module = info.dli_fname;
            }
            /* For PIE/ASLR, subtract the module base to get relative offset */
            rel_addr = (void *)((uintptr_t)addr - (uintptr_t)info.dli_fbase);
        } 
        
        /* If we have a module path, ask addr2line for file:line */
        if (module[0] != '\0') {
            char cmd[PATH_MAX_LENGTH + 100];
            /*
            * -f: show function name
            * -C: demangle C++ names (harmless for C)
            * -e: executable or shared object
            * Using a two-stage read: first line = function, second = file:line
            */
            snprintf(cmd, sizeof(cmd),
                    "addr2line -f -C -e \"%s\" %p 2>/dev/null",
                    module, rel_addr);
            FILE *fp = popen(cmd, "r");
            if (fp) {
                char linebuf[PATH_MAX_LENGTH + 50];
                /* Skip the function line (we already have it from dladdr) */
                if (fgets(linebuf, sizeof(linebuf), fp)) {
                    /* Read file:line */
                    if (fgets(linebuf, sizeof(linebuf), fp)) {
                        size_t len = strlen(linebuf);
                        if (len && linebuf[len-1] == '\n') {
                            linebuf[len-1] = '\0';
                        }
                        /* Append to output */
                        size_t cur = strlen(out);
                        snprintf(out + cur, outSize - cur, "\n  at %s", linebuf);
                    }
                }
                pclose(fp);
            }
        }

        return 0;
    }
#else 
    int init_symbol_handler(void) { return 0; }
    void cleanup_symbol_handler(void) {}
    int get_caller_info(char *out, size_t outSize, unsigned int depth) { 
        (void) outSize;
        (void) depth;

        out = ""; 
        (void) out;

        return 0; 
    }

#endif