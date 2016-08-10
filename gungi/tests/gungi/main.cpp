#include <gungi/gungi.hpp>

#include <CppUTest/CommandLineTestRunner.h>
#include <CppUTest/MemoryLeakWarningPlugin.h>

int main(int argc, char *argv[]) {
  MemoryLeakWarningPlugin::turnOnNewDeleteOverloads();

  return CommandLineTestRunner::RunAllTests(argc, argv);
}
