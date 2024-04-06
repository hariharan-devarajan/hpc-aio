#include <cstddef>
#include <sys/types.h>
#include <cstring>
#include <uv.h>
#include <iostream>
#include <sys/inotify.h>
#include <cassert>

struct InotifyData {
  int inotify_fd;
};

void inotify_callback(uv_poll_t *handle, int status, int events) {
  InotifyData *inotify_data = (InotifyData*) handle->data;
  // std::cout << "inotify Event occured from thread: " << tid << std::endl;
  char buffer[16384];
  ssize_t buffer_i;
  struct inotify_event* pevent;
  ssize_t r;
  size_t event_size;
  int count = 0;

  r = read(inotify_data->inotify_fd, buffer, 16384);

  if (r <= 0) {
    std::cerr << "ERROR READING INOTIFY FD" << std::endl;
    exit(-1);
  }
  buffer_i = 0;
  while (buffer_i < r) {
    // Parse events and queue them
    pevent = (struct inotify_event*)&buffer[buffer_i];
    event_size = offsetof(struct inotify_event, name) + pevent->len;
    struct inotify_event *event = (struct inotify_event*)malloc(event_size);
    memmove(event, pevent, event_size);

    /* Parse Event*/
    /* If the event was associated with a filename, we will store it here */
    std::string cur_event_filename = "";
    std::string cur_event_file_or_dir = "";

    if (event->len) {
      cur_event_filename = event->name;
    }
    if (event->mask & IN_ISDIR) {
      cur_event_file_or_dir = "Dir";
    } else {
      cur_event_file_or_dir = "File";
    }
    auto event_id = event->mask & IN_ALL_EVENTS;
    std::cout << "recieved event " << event_id << " from " << event->name << " on watch fd " << event->wd << std::endl;
    buffer_i += event_size;
    count++;
  }
}

int main(int argc, char* argv[]) {
  char* watched_dir = argv[1];
  int inotify_fd = inotify_init();
  if (inotify_fd < 0) {
    std::cout << "Error initializing inotify" << std::endl;
    // handle error
    exit(-1);
  }
  std::cout << "Adding inotify watch" << std::endl;
  int wd = inotify_add_watch(inotify_fd, watched_dir, IN_ALL_EVENTS);
  if (wd < 0) {
    std::cout << "Error Adding Watch" << std::endl;
    // handle error
    exit(-1);
  }
  InotifyData data;
  data.inotify_fd = inotify_fd;
  uv_poll_t* uv_poll_handler = (uv_poll_t*)malloc(sizeof(uv_poll_t));
  uv_poll_handler->data = &data;

  int init_status =
      uv_poll_init(uv_default_loop(), uv_poll_handler,
                   inotify_fd);  // UV_READABLE | UV_WRITABLE | UV_DISCONNECT
  assert(init_status == 0);
  int start_status =
      uv_poll_start(uv_poll_handler,
                    UV_READABLE | UV_WRITABLE | UV_DISCONNECT | UV_PRIORITIZED,
                    inotify_callback);
  assert(start_status == 0);

  uv_run(uv_default_loop(), UV_RUN_DEFAULT);
  return 0;
}