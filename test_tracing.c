#include "steves_tools.h"
#include "minitrace.h"

int main(int argc, const char *argv[]) {
  int i;
  mtr_init("frack.json");

  MTR_META_PROCESS_NAME("minitrace_test");
  MTR_META_THREAD_NAME("main thread");

  int long_running_thing_1;
  int long_running_thing_2;

  MTR_START("background", "long_running", &long_running_thing_1);
  MTR_START("background", "long_running", &long_running_thing_2);

  MTR_BEGIN("main", "outer");
  sleep(8);
  for (i = 0; i < 10; i++) {
    MTR_BEGIN("main", "inner");
    usleep(4000);
    MTR_END("main", "inner");
    usleep(1000);
  }
  MTR_STEP("background", "long_running", &long_running_thing_1, "middle step");
  sleep(8);
  MTR_END("main", "outer");

  sleep(5);
  MTR_INSTANT("main", "the end");
  sleep(10);
  MTR_FINISH("background", "long_running", &long_running_thing_1);
  MTR_FINISH("background", "long_running", &long_running_thing_2);

  mtr_flush();
  mtr_shutdown();
  return 0;
}
