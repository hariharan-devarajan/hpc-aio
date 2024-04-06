void inotify_callback_() {
  std::thread::id tid = std::this_thread::get_id();
  // std::cout << "inotify Event occured from thread: " << tid << std::endl;
  char buffer[16384];
  size_t buffer_i;
  struct inotify_event* pevent;
  event_entry* event;
  ssize_t r;
  size_t event_size, q_event_size;
  int count = 0;

  r = read(inotify_fd, buffer, 16384);

  if (r <= 0) {
    std::cerr << "ERROR READING INOTIFY FD" << std::endl;
    exit(-1);
  }
  buffer_i = 0;
  while (buffer_i < r) {
    // Parse events and queue them
    pevent = (struct inotify_event*)&buffer[buffer_i];
    event_size = offsetof(struct inotify_event, name) + pevent->len;
    q_event_size = offsetof(struct event_entry, inot_ev.name) + pevent->len;
    event = (event_entry*)malloc(q_event_size);
    memmove(&(event->inot_ev), pevent, event_size);

    /* Parse Event*/
    /* If the event was associated with a filename, we will store it here */
    std::string cur_event_filename = "";
    std::string cur_event_file_or_dir = "";
    /* This is the watch descriptor the event occurred on */
    int cur_event_wd = event->inot_ev.wd;
    int cur_event_cookie = event->inot_ev.cookie;

    unsigned long flags;

    if (event->inot_ev.len) {
      cur_event_filename = event->inot_ev.name;
    }
    if (event->inot_ev.mask & IN_ISDIR) {
      cur_event_file_or_dir = "Dir";
    } else {
      cur_event_file_or_dir = "File";
    }
    flags = event->inot_ev.mask &
            ~(IN_ALL_EVENTS | IN_UNMOUNT | IN_Q_OVERFLOW | IN_IGNORED);

    /* Perform event dependent handler routines */
    /* The mask is the magic that tells us what file operation occurred */
    switch (event->inot_ev.mask &
            (IN_ALL_EVENTS | IN_UNMOUNT | IN_Q_OVERFLOW | IN_IGNORED)) {
      /* File was accessed */
      int work_queue_status;
      case IN_CREATE: {
        uv_work_t* req = (uv_work_t*)malloc(sizeof(uv_work_t));
        event_args event_args_val;
        event_args_val.local_handler = this;
        event_args_val.inot_event_entry = event;
        req->data = (void*)&event_args_val;  // cur_event_filename.c_str();
        int work_queue_status = uv_queue_work(
            util::uvLoop, req, local_request_handler::handle_event,
            local_request_handler::after_event_handled);
        break;
      }

      /* Some unknown message received */
      default:
        break;
    }
    /* If any flags were set other than IN_ISDIR, report the flags */
    if (flags & (~IN_ISDIR)) {
      flags = event->inot_ev.mask;
      printf("Flags=%lX\n", flags);
    }
    /*____________*/

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
  int wd = inotify_add_watch(inotify_fd, watched_dir.c_str(), IN_ALL_EVENTS);
  if (wd < 0) {
    std::cout << "Error Adding Watch" << std::endl;
    // handle error
    exit(-1);
  }

  uv_poll_t* uv_poll_handler = (uv_poll_t*)malloc(sizeof(uv_poll_t));
  uv_poll_handler->data = this;

  int init_status =
      uv_poll_init(uv_default_loop(), uv_poll_handler,
                   inotify_fd);  // UV_READABLE | UV_WRITABLE | UV_DISCONNECT

  int start_status =
      uv_poll_start(uv_poll_handler,
                    UV_READABLE | UV_WRITABLE | UV_DISCONNECT | UV_PRIORITIZED,
                    inotify_callback);

  uv_run(uv_default_loop(), UV_RUN_DEFAULT);
  return 0;
}