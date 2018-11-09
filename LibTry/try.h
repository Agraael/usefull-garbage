#ifndef LIBTRY_H
#define LIBTRY_H

#include <setjmp.h>
#include <stdlib.h>
#include <memory.h>
#include <signal.h>
#include <unistd.h>

extern jmp_buf * __LIBTRY__raise_point;
extern jmp_buf * __LIBTRY__retry_point;

enum error_code_t {
	MALLOC_EXCEPTION = 84,
	SEGFAULT_EXCEPTION = 42,
	RETRY_EXCEPTION               // raise'd when RETRY not within a CATCH block
};

#define lambda(return_type, function_body) \
({ \
      return_type __fn__ function_body \
          __fn__; \
})

#define handler(exception) lambda(void, (int sig, siginfo_t* info, void* ptr) {raise(exception);})

#define raise(exception) do {\
  if (__LIBTRY__raise_point)\
    longjmp(*__LIBTRY__raise_point, exception);\
  else {\
     write(2, "libtry error raised !\n", 22);\
     abort();\
  }\
} while (0)

#define retry do {\
  if (__LIBTRY__retry_point)\
    longjmp(*__LIBTRY__retry_point, 1);\
  else\
    raise(RETRY_EXCEPTION);\
} while(0)

//  TRY(
//    some code
//  ) CATCH ( exception == 1,
//    // what to do with a 1 exception
//  ) CATCH ( exception == 2,
//    // what to do with a 2 exception
//    // ...
//  ) FINALLY (
//    // some code
//  );

#define try(block) do\
{\
  jmp_buf * const __LIBTRY__old_raise_point = __LIBTRY__raise_point;\
  jmp_buf * const __LIBTRY__old_retry_point = __LIBTRY__retry_point;\
  jmp_buf __LIBTRY__actual_raise_point;\
  int const __LIBTRY__reraise = setjmp(__LIBTRY__actual_raise_point);\
  \
  if (!__LIBTRY__reraise) {\
    jmp_buf __LIBTRY__try_raise_point;\
    int const __LIBTRY__exception = setjmp(__LIBTRY__try_raise_point);\
    jmp_buf __LIBTRY__try_retry_point;\
    int const __LIBTRY__retry  = setjmp(__LIBTRY__try_retry_point);\
    \
    __LIBTRY__raise_point = &__LIBTRY__try_raise_point; \
    __LIBTRY__retry_point = __LIBTRY__old_retry_point; \
    if (!__LIBTRY__exception || __LIBTRY__retry) {\
      block\
    }\
    else {\
      int const exception = __LIBTRY__exception;\
      \
      __LIBTRY__raise_point = &__LIBTRY__actual_raise_point;\
      __LIBTRY__retry_point = &__LIBTRY__try_retry_point;


#define catch(condition, block)\
      if (condition) {\
        block\
      }\
      else


#define finally(block)\
      {\
        raise(exception);\
      }\
    }\
  }\
  __LIBTRY__retry_point = __LIBTRY__old_retry_point;\
  __LIBTRY__raise_point = __LIBTRY__old_raise_point;\
  block\
  if (__LIBTRY__reraise) {\
    raise(__LIBTRY__reraise);\
  }\
} while(0)


#define catch_all(block)\
    }\
  }\
  __LIBTRY__retry_point = __LIBTRY__old_retry_point;\
  __LIBTRY__raise_point = __LIBTRY__old_raise_point;\
  block\
  if (__LIBTRY__reraise) {\
    raise(__LIBTRY__reraise);\
  }\
} while(0)

#define end {\
        raise(exception);\
      }\
    }\
  }\
  __LIBTRY__retry_point = __LIBTRY__old_retry_point;\
  __LIBTRY__raise_point = __LIBTRY__old_raise_point;\
  if (__LIBTRY__reraise) {\
    raise(__LIBTRY__reraise);\
  }\
} while(0)

void * rmalloc(size_t nbytes);

#endif