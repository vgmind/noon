
#define NOON_IMPLEMENTATION
#include "noon.h"

test({
  const auto expr = true;

  // Basic check, There is an Uppercase macro as well.
  // Yep you can turn of the lowercase version is you don't like it.
  check(expr);
})

//
test({
  // Yep you can have a message when your check fails.
  // To keep things simple they have to be known at compile time.
  // So no dynamic allocated things like strings.
  // Just keep things really simple.
  check_message("hey", true);
})

test_named(check_sun_shines,{
  // The above tests don't have names, but you can have
  // them using test_named macro.
  CHECK_MESSAGE("Yes it most certainly does", true);
})

//
int main() {
  // Default is not to exit if a test fails. You might want to.
  Test::run(true); // Exit program if we get a test failing.

  return 0;
}
