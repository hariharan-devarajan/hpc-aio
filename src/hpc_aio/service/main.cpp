
#include <csignal>
#include <sstream>
#if defined(HPC_AIO_HAS_CONFIG)
#include <hpc_aio/hpc_aio_config.hpp>
#else
#error "no config"
#endif
#include <hpc_aio/common/logging.h>
#include <signal.h>
#include <uv.h>

uv_timer_t* server_timer;

void handle_sigint(int sig) {
  HPC_AIO_LOG_TRACE("handle_siginit %d", sig);
  switch (sig) {
    case SIGINT: {
      if (server_timer) {
        uv_timer_stop(server_timer);
      }
      break;
    }
  }
}
void run_server(uv_timer_t* timer) {}

int main(int argc, char* argv[]) {
  std::stringstream args;
  for (int i = 0; i < argc; ++i) {
    args << argv[i] << " ";
  }
  HPC_AIO_LOG_TRACE("Starting service %s", args.c_str());
  signal(SIGINT, handle_sigint);
  server_timer = new uv_timer_t();
  uv_timer_init(uv_default_loop(), server_timer);
  uv_timer_start(server_timer, run_server, 0, 1000);
  int rc = uv_run(uv_default_loop(), UV_RUN_DEFAULT);
  if (rc != 0) {
    HPC_AIO_LOG_ERROR("uv_run returned rc %d", rc);
  }
  delete server_timer;
  return 0;
}