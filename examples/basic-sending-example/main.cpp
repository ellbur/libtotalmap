
// http://stackoverflow.com/questions/27581500/hook-into-linux-key-event-handling/27693340#27693340

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <linux/input.h>
#include <linux/uinput.h>
#include <thread>
#include <chrono>

#define die(n, str, args...) do { \
  perror(str); \
  exit(n); \
} while(0)

void sendTypeCodeValue(int fdo, int type, int code, int value) {
  struct input_event ev2;
  ev2.time.tv_sec = 0;
  ev2.time.tv_usec = 0;
  ev2.type = type;
  ev2.code = code;
  ev2.value = value;
  write(fdo, &ev2, sizeof(struct input_event));
}

void sendFullSet(int fdo, int code, int value) {
  sendTypeCodeValue(fdo, 4, 4, code);
  sendTypeCodeValue(fdo, 1, code, value);
  sendTypeCodeValue(fdo, 0, 0, 0);
}

int main(int argc, char **argv) {
  int fdo = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
  if (fdo < 0) die(3, "error: open");

  if (ioctl(fdo, UI_SET_EVBIT, EV_SYN) < 0) die(5, "error: ioctl");
  if (ioctl(fdo, UI_SET_EVBIT, EV_KEY) < 0) die(6, "error: ioctl");
  if (ioctl(fdo, UI_SET_EVBIT, EV_MSC) < 0) die(7, "error: ioctl");

  for (int i=0; i<KEY_MAX; ++i) {
    if (ioctl(fdo, UI_SET_KEYBIT, i) < 0) die(8, "error: ioctl");
  }
  
  uinput_user_dev uidev;
  memset(&uidev, 0, sizeof(uidev));
  snprintf(uidev.name, UINPUT_MAX_NAME_SIZE, "send_a");
  uidev.id.bustype = BUS_USB;
  uidev.id.vendor  = 0x1;
  uidev.id.product = 0x1;
  uidev.id.version = 1;

  if (write(fdo, &uidev, sizeof(uidev)) < 0) die(9, "error: write");
  if (ioctl(fdo, UI_DEV_CREATE) < 0) die(10, "error: ioctl");
  
  const int pressed = 1;
  const int released = 0;
  const int repeated = 2;
  
  auto press = [&](int code) {
    sendFullSet(fdo, code, pressed);
  };
  
  auto release = [&](int code) {
    sendFullSet(fdo, code, released);
  };
  
  int counter = 0;
  while (true) {
    printf("Sending %d.\n", counter);
    fflush(stdout);
    counter += 1;
    press(30);
    release(30);
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }

  if (ioctl(fdo, UI_DEV_DESTROY) < 0) die(12, "error: ioctl");
  close(fdo);

  return 0;
}

