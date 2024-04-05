#include <cassert>
#include <cstdint>
#include <cstring>
#include <uv.h>
#include <stdio.h>
typedef struct {
  int index;
  uv_async_t* async;
  uv_mutex_t* mutex;
  uv_timer_t* timer;
  uint64_t timer_val;
  int processed;
} thread_comm_t;
int total_processed = 0;

void change_timer_cb(uv_async_s* async) {
  thread_comm_t *comm = (thread_comm_t *)async->data;
  printf("change_timer_cb for %d to %ld\n", comm->index, comm->timer_val);
  uv_timer_set_repeat(comm->timer, comm->timer_val);
  // int rc = uv_timer_again(&comm->timer);
  // assert(rc == 0);
}

void timer(uv_timer_t *handle) {
  thread_comm_t *comm = (thread_comm_t *)handle->data;
  total_processed++;
  comm->processed++;
  printf("timer %d triggered %d of %d\n", comm->index, comm->processed, total_processed);
  float rate = comm->processed * 1.0 / total_processed;
  uint64_t new_timer_val = comm->timer_val;
  if (rate < .5) {
    new_timer_val = new_timer_val - new_timer_val * (.5 - rate);
  } else if (rate > .5) {
    new_timer_val = new_timer_val + new_timer_val * (rate - .5);
  }
  if (new_timer_val !=  comm->timer_val) {
    comm->timer_val = new_timer_val;
    comm->timer = handle;
    printf("changing timer %d to %ld as rate is %f\n", comm->index, new_timer_val, rate);
    comm->async->data = comm;
    uv_async_send(comm->async);
  }
  if (total_processed > 20) {
    uv_timer_stop(handle);
  }
}

int main() {
  thread_comm_t comm1, comm2;
  int r;
  memset(&comm1, 0, sizeof(comm1));
  memset(&comm2, 0, sizeof(comm2));
  comm1.mutex = new uv_mutex_t();
  comm1.async = new uv_async_t();
  comm1.timer = new uv_timer_t();
  comm1.index = 0;
  comm1.processed = 0;
  comm1.timer_val = 600;

  comm2.mutex = new uv_mutex_t();
  comm2.async = new uv_async_t();
  comm2.timer = new uv_timer_t();
  comm2.index = 1;
  comm2.processed = 0;
  comm2.timer_val = 300;

  r = uv_mutex_init(comm1.mutex);
  assert(r == 0);

  r = uv_mutex_init(comm2.mutex);
  assert(r == 0);

  

  r = uv_async_init(uv_default_loop(), comm1.async, change_timer_cb);
  r = uv_async_init(uv_default_loop(), comm2.async, change_timer_cb);

  uv_timer_t timer1_req, timer2_req;
  timer1_req.data = &comm1;
  timer2_req.data = &comm2;
  uv_timer_init(uv_default_loop(), &timer1_req);
  uv_timer_init(uv_default_loop(), &timer2_req);
  uv_timer_start(&timer1_req, timer, comm1.timer_val, comm1.timer_val);
  uv_timer_start(&timer2_req, timer, comm2.timer_val, comm2.timer_val);

  uv_run(uv_default_loop(), UV_RUN_DEFAULT);
  uv_loop_close(uv_default_loop());
  delete comm1.mutex;
  delete comm1.async;
  delete comm1.timer;
  delete comm2.mutex;
  delete comm2.async;
  delete comm2.timer;
  return 0;
}