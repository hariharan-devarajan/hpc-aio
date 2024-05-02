#include <errno.h>
#include <poll.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/inotify.h>
#include <sys/signalfd.h>
#include <unistd.h>
#include <uv.h>

#include <cassert>
#include <cstdlib>
uv_loop_t *loop;

/* Structure to keep track of monitored directories */
typedef struct {
  /* Path of the directory */
  char *path;
  /* inotify watch descriptor */
  int wd;
} monitored_t;
/* Size of buffer to use when reading inotify events */
#define INOTIFY_BUFFER_SIZE 8192

/* FANotify-like helpers to iterate events */
#define IN_EVENT_DATA_LEN (sizeof(struct inotify_event))
#define IN_EVENT_NEXT(event, length) \
  ((length) -= (event)->len,         \
   (struct inotify_event *)(((char *)(event)) + (event)->len))
#define IN_EVENT_OK(event, length)                  \
  ((long)(length) >= (long)IN_EVENT_DATA_LEN &&     \
   (long)(event)->len >= (long)IN_EVENT_DATA_LEN && \
   (long)(event)->len <= (long)(length))

/* Setup inotify notifications (IN) mask. All these defined in inotify.h. */
static int event_mask =
    (IN_ACCESS |        /* File accessed */
     IN_ATTRIB |        /* File attributes changed */
     IN_OPEN |          /* File was opened */
     IN_CLOSE_WRITE |   /* Writtable File closed */
     IN_CLOSE_NOWRITE | /* Unwrittable File closed */
     IN_CREATE |        /* File created in directory */
     IN_DELETE |        /* File deleted in directory */
     IN_DELETE_SELF |   /* Directory deleted */
     IN_MODIFY |        /* File modified */
     IN_MOVE_SELF |     /* Directory moved */
     IN_MOVED_FROM |    /* File moved away from the directory */
     IN_MOVED_TO);      /* File moved into the directory */

/* Array of directories being monitored */
static monitored_t *monitors;
static int n_monitors;

static int initialize_inotify(int argc, const char **argv) {
  int i;
  int inotify_fd;

  /* Create new inotify device */
  if ((inotify_fd = inotify_init()) < 0) {
    fprintf(stderr, "Couldn't setup new inotify device: '%s'\n",
            strerror(errno));
    return -1;
  }

  /* Allocate array of monitor setups */
  n_monitors = argc - 1;
  monitors = (monitored_t *)malloc(n_monitors * sizeof(monitored_t));

  /* Loop all input directories, setting up watches */
  for (i = 0; i < n_monitors; ++i) {
    monitors[i].path = strdup(argv[i + 1]);
    if ((monitors[i].wd =
             inotify_add_watch(inotify_fd, monitors[i].path, event_mask)) < 0) {
      fprintf(stderr, "Couldn't add monitor in directory '%s': '%s'\n",
              monitors[i].path, strerror(errno));
      exit(EXIT_FAILURE);
    }
    printf("Started monitoring directory '%s'...\n", monitors[i].path);
  }

  return inotify_fd;
}

static void shutdown_inotify(int inotify_fd) {
  int i;

  for (i = 0; i < n_monitors; ++i) {
    free(monitors[i].path);
    inotify_rm_watch(inotify_fd, monitors[i].wd);
  }
  free(monitors);
  close(inotify_fd);
}
static void event_process(struct inotify_event *event) {
  int i;

  /* Need to loop all registered monitors to find the one corresponding to the
   * watch descriptor in the event. A hash table here would be quite a better
   * approach. */
  for (i = 0; i < n_monitors; ++i) {
    /* If watch descriptors match, we found our directory */
    if (monitors[i].wd == event->wd) {
      if (event->len > 0)
        printf("Received event in '%s/%s': ", monitors[i].path, event->name);
      else
        printf("Received event in '%s': ", monitors[i].path);

      if (event->mask & IN_ACCESS) printf("\tIN_ACCESS\n");
      if (event->mask & IN_ATTRIB) printf("\tIN_ATTRIB\n");
      if (event->mask & IN_OPEN) printf("\tIN_OPEN\n");
      if (event->mask & IN_CLOSE_WRITE) printf("\tIN_CLOSE_WRITE\n");
      if (event->mask & IN_CLOSE_NOWRITE) printf("\tIN_CLOSE_NOWRITE\n");
      if (event->mask & IN_CREATE) printf("\tIN_CREATE\n");
      if (event->mask & IN_DELETE) printf("\tIN_DELETE\n");
      if (event->mask & IN_DELETE_SELF) printf("\tIN_DELETE_SELF\n");
      if (event->mask & IN_MODIFY) printf("\tIN_MODIFY\n");
      if (event->mask & IN_MOVE_SELF) printf("\tIN_MOVE_SELF\n");
      if (event->mask & IN_MOVED_FROM)
        printf("\tIN_MOVED_FROM (cookie: %d)\n", event->cookie);
      if (event->mask & IN_MOVED_TO)
        printf("\tIN_MOVED_TO (cookie: %d)\n", event->cookie);
      fflush(stdout);
      return;
    }
  }
}
void read_callback(uv_fs_t *req) {
  int result = req->result;
  if (result < 0) {
    fprintf(stderr, "Error at reading file %s.\n", uv_err_name(req->result));
    return;
  }
  struct inotify_event *event;

  event = (struct inotify_event *)req->data;
  while (IN_EVENT_OK(event, result)) {
    event_process(event);
    event = IN_EVENT_NEXT(event, result);
  }
  free(req->data);
  free(req);
}

void handle_inotify_events(uv_poll_t *handle, int status, int events) {
  uv_fs_t *read_req = new uv_fs_t();
  char *buffer = (char *)calloc(INOTIFY_BUFFER_SIZE, sizeof(char));
  uv_buf_t uvBuf = uv_buf_init(buffer, INOTIFY_BUFFER_SIZE);
  read_req->data = buffer;
  uv_fs_read(loop, read_req, handle->io_watcher.fd, &uvBuf, 1, 0,
             read_callback);
}

int main(int argc, const char *argv[]) {
  loop = uv_default_loop();
  /* Input arguments... */
  if (argc < 2) {
    fprintf(stderr, "Usage: %s directory1 [directory2 ...]\n", argv[0]);
    exit(EXIT_FAILURE);
  }
  int inotify_fd;
  /* Initialize inotify FD and the watch descriptors */
  if ((inotify_fd = initialize_inotify(argc, argv)) < 0) {
    fprintf(stderr, "Couldn't initialize inotify\n");
    exit(EXIT_FAILURE);
  }
  uv_poll_t poll_handle;
  int status = uv_poll_init(loop, &poll_handle, inotify_fd);
  assert(status == 0);
  status = uv_poll_start(&poll_handle, UV_READABLE, handle_inotify_events);
  assert(status == 0);
  uv_run(loop, UV_RUN_DEFAULT);
  shutdown_inotify(inotify_fd);
  return 0;
}