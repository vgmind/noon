#ifndef NOON_H_
#define NOON_H_
// A minimal test framework.
// Created by Paul Reeves 2021 (November)
// https://github.com/vgmind/noon
//
// A single header file so add this define before ONE include. 
// #define NOON_IMPLEMENTATION
// #include "noon.h"
//
// After that include noon freely.
//
// Release 1.0 (November 2021)
//   - Initial release
//
// Noon was slight inspired but 1am a lisp testing frame
// https://github.com/lmj/1am and is aimed at small projects. If you expect to
// have a hundred thousand tests this minimal tests if not for you. Although if
// you are just getting going and want a no hassle single header test runner
// that this might just be for you. Noon is very minimal.
//
// At the moment this is very much a personal project and while I consider the
// project fairly minimal in terms of code we do:
//
//  - Use exceptions
//  - Rely on Clib for output
//
// I understand and appreciate this is not to everyone taste. As you
// are not going to ship with your tests you only need to use these features
// your test builds.
//
// I haven't been able to figure out a nice alternative to exceptions, Result
// like objects just didn't feel right in C++ and with special syntax they can
// be a burden when returning from nested functions.
//
// Take a look at example.cpp for a couple of simple examples.
//
// Basic idea is create a test like this:
// test({
//        check(true);
// })
//
// At the start of main.cpp do:
// Test::run() // or Test::run(true) if you want to exit on failure
//
// You can turn off lowercase macros and only have the uppercase ones like so:
// #define TEST_NO_LOWERCASE
//
// As you have to do this before every include. It is probably easier to just
// delete them from this file.
//
//
// Details:
//
// There really is not much to say here. As you construct each test the
// constructor for the struct adds itself to a linked list of tests.
//
// The root of this is a static variable inside the Test struct.
//
// The runner just walks the list calling the function pointers.
//
#include <cstdio>
#include <cstdlib>

using Test_fn = void (*)();

struct Test {
  static Test *start; // Our global

  static void run(const bool abort_if_any_failures = false) {

    puts("Testing");
    int num_run{0};
    int num_failed{0};
    for (auto t = start; t; t = t->next) {
      ++num_run;
      printf("(%i) %s", num_run, t->name ? t->name : "Anonymous Test");
      try {
        t->fn();
        puts("👍");
      } catch (char const *msg) {
        puts("");
        fflush(stdout);
        fprintf(stderr, "❌ Test: %s failed, msg: %s❌\n", t->name, msg);
        fprintf(stderr, "❌ File: %s Line: %i ❌\n", t->file_name,
                t->line_number);
        fflush(stderr);
        ++num_failed;
      }
    }

    float pct_failed = 100.0f * num_failed / num_run;
    printf("Summary: %i test run with %d (%.2f%%) fails\n\n", num_run,
           num_failed, pct_failed);

    if (num_failed) {
      puts("❌❌❌  TEST FAIL(S) ❌❌❌");
      puts("Tip: use \"./test_exe 1> /dev/null\" to hide successful tests");
    } else {
      puts("🎉🎉🎉 YAY All GOOD 🎉🎉🎉");
    }

    if (abort_if_any_failures) {
      puts("Exiting");
      exit(EXIT_FAILURE);
    }
  }

  Test(char const *name, Test_fn fn, char const *file_name,
       const int line_number)
      : name(name), file_name(file_name), line_number(line_number), fn(fn) {
    if (Test::start) {
      auto t = Test::start;
      for (; t->next; t = t->next) {
      }
      t->next = this;
    } else {
      Test::start = this;
    }
  }

  char const *name{nullptr};
  char const *file_name{nullptr};
  int line_number{0};
  Test_fn fn;
  Test *next{nullptr};
};

#if defined NOON_IMPLEMENTATION
Test *Test::start; // allocate space for static
#endif
#define TEST_NAMED(name, body)                                                 \
  void name();                                                                 \
  static Test test_##name(#name, &name, __FILE__, __LINE__);                   \
  void name() body

#define NOON_STRINGIFY(x) #x
#define NOON_TOSTRING(x) NOON_STRINGIFY(x)
#define NOON_AT __FILE__ ":" NOON_TOSTRING(__LINE__)
#define NOON_CAT_I(a, b) a##b
#define NOON_CAT(a, b) NOON_CAT_I(a, b)

#define TEST(body)                                                             \
  static Test NOON_CAT(test_, __LINE__)(nullptr, [] body, __FILE__, __LINE__);

#define CHECK(expr)                                                            \
  do {                                                                         \
    if (!(expr))                                                               \
      throw NOON_AT;                                                           \
  } while (false)

#define CHECK_MESSAGE(msg, expr)                                               \
  do {                                                                         \
    if (!(expr))                                                               \
      throw msg " " NOON_AT;                                                   \
  } while (false)

//
// On by default, mostly because I use them...
#if !defined(TEST_NO_LOWERCASE)
#define test TEST
#define test_named TEST_NAMED
#define check CHECK
#define check_message CHECK_MESSAGE
#endif

#endif // NOON_H_
