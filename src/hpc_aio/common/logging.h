#ifndef HPC_AIO_COMMON_HPC_AIO_LOGGING_H
#define HPC_AIO_COMMON_HPC_AIO_LOGGING_H
#if defined(HPC_AIO_HAS_CONFIG)
#include <hpc_aio/hpc_aio_config.hpp>
#else
#error "no config"
#endif

#include <sys/types.h>
#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif

// #define HPC_AIO_NOOP_MACRO do {} while (0)

//=============================================================================
#ifdef HPC_AIO_LOGGER_NO_LOG
//=============================================================================
#define HPC_AIO_LOGGER_INIT() HPC_AIO_NOOP_MACRO
#define HPC_AIO_LOG_STDERR(...) HPC_AIO_NOOP_MACRO
#define HPC_AIO_LOG_STDOUT(...) HPC_AIO_NOOP_MACRO
#define HPC_AIO_LOG_ERROR(...) HPC_AIO_NOOP_MACRO
#define HPC_AIO_LOG_WARN(...) HPC_AIO_NOOP_MACRO
#define HPC_AIO_LOG_INFO(...) HPC_AIO_NOOP_MACRO
#define HPC_AIO_LOG_DEBUG(...) HPC_AIO_NOOP_MACRO
#define HPC_AIO_LOG_TRACE(...) HPC_AIO_NOOP_MACRO
#define HPC_AIO_LOG_STDOUT_REDIRECT(fpath) HPC_AIO_NOOP_MACRO
#define HPC_AIO_LOG_STDERR_REDIRECT(fpath) HPC_AIO_NOOP_MACRO
//=============================================================================
#else
//=============================================================================
#define HPC_AIO_LOG_STDERR(...) fprintf(stderr, __VA_ARGS__);
#define HPC_AIO_LOG_STDOUT(...) fprintf(stdout, __VA_ARGS__);

#if defined(HPC_AIO_LOGGER_CPP_LOGGER)  // CPP_LOGGER
                                        // ---------------------------
#include <cpp-logger/clogger.h>

#define HPC_AIO_LOGGER_NAME "HPC_AIO"

#ifdef HPC_AIO_LOGGER_LEVEL_DEBUG
#define HPC_AIO_LOGGER_INIT() \
  cpp_logger_clog_level(CPP_LOGGER_DEBUG, HPC_AIO_LOGGER_NAME);
#elif defined(HPC_AIO_LOGGER_LEVEL_INFO)
#define HPC_AIO_LOGGER_INIT() \
  cpp_logger_clog_level(CPP_LOGGER_INFO, HPC_AIO_LOGGER_NAME);
#elif defined(HPC_AIO_LOGGER_LEVEL_WARN)
#define HPC_AIO_LOGGER_INIT() \
  cpp_logger_clog_level(CPP_LOGGER_WARN, HPC_AIO_LOGGER_NAME);
#else
#define HPC_AIO_LOGGER_INIT() \
  cpp_logger_clog_level(CPP_LOGGER_ERROR, HPC_AIO_LOGGER_NAME);
#endif

#define HPC_AIO_LOG_STDOUT_REDIRECT(fpath) freopen((fpath), "a+", stdout);
#define HPC_AIO_LOG_STDERR_REDIRECT(fpath) freopen((fpath), "a+", stderr);

#ifdef HPC_AIO_LOGGER_LEVEL_TRACE
#define HPC_AIO_LOG_TRACE(...) \
  cpp_logger_clog(CPP_LOGGER_TRACE, HPC_AIO_LOGGER_NAME, __VA_ARGS__);
#else
#define HPC_AIO_LOG_TRACE(...) HPC_AIO_NOOP_MACRO
#endif

#ifdef HPC_AIO_LOGGER_LEVEL_DEBUG
#define HPC_AIO_LOG_DEBUG(...) \
  cpp_logger_clog(CPP_LOGGER_DEBUG, HPC_AIO_LOGGER_NAME, __VA_ARGS__);
#else
#define HPC_AIO_LOG_DEBUG(...) HPC_AIO_NOOP_MACRO
#endif

#ifdef HPC_AIO_LOGGER_LEVEL_INFO
#define HPC_AIO_LOG_INFO(...) \
  cpp_logger_clog(CPP_LOGGER_INFO, HPC_AIO_LOGGER_NAME, __VA_ARGS__);
#else
#define HPC_AIO_LOG_INFO(...) HPC_AIO_NOOP_MACRO
#endif

#ifdef HPC_AIO_LOGGER_LEVEL_WARN
#define HPC_AIO_LOG_WARN(...) \
  cpp_logger_clog(CPP_LOGGER_WARN, HPC_AIO_LOGGER_NAME, __VA_ARGS__);
#else
#define HPC_AIO_LOG_WARN(...) HPC_AIO_NOOP_MACRO
#endif

#ifdef HPC_AIO_LOGGER_LEVEL_ERROR
#define HPC_AIO_LOG_ERROR(...) \
  cpp_logger_clog(CPP_LOGGER_ERROR, HPC_AIO_LOGGER_NAME, __VA_ARGS__);
#else
#define HPC_AIO_LOG_ERROR(...) HPC_AIO_NOOP_MACRO
#endif
#endif  // HPC_AIO_LOGGER_CPP_LOGGER
        // ----------------------------------------------------
//=============================================================================
#endif  // HPC_AIO_LOGGER_NO_LOG
//=============================================================================

#ifdef __cplusplus
}
#endif

#endif /* HPC_AIO_COMMON_HPC_AIO_LOGGING_H */