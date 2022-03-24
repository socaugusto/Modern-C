#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Structure Initialization */
typedef struct v2 {
  float x, y;
} v2;

#define some_constant \
  (v2) { 3.0f, 4.0f }

/* This macro in conjunction with static assert allows to
 * check if the variable has a type */
#define IS_FLOAT(N) _Generic((N), float : 1, default : 0)

/* Overloading functions */
#define cbrt(X) \
  _Generic((X), long double : cbrtl, default : cbrt, float : cbrtf)(X)

/* Overloading functions complex example */
#define typename(x, y)                       \
  _Generic((x), char                         \
           : _Generic((y), int               \
                      : "char, int", default \
                      : "ups"),              \
                                             \
             default                         \
           : "default",                      \
                                             \
             int                             \
           : _Generic((y), default           \
                      : "int, ignored", v2   \
                      : "int, v2"))

/* Awesome macros: defer */
#define concat(a, b) a##b
#define macro_var(name) concat(name, __LINE__)
#define defer(start, end)                                    \
  for (int macro_var(_idx_) = (start, 0); !macro_var(_idx_); \
       (macro_var(_idx_) += 1), end)

void begin(void) { printf("Awesome macro begin\n"); }
void end(void) { printf("Awesome macro end\n"); }

/* Error Handling Example */
typedef struct file_contents_t {
  char* data;
  size_t size;
  bool valid;
} file_contents_t;

file_contents_t read_file_contents(const char* data) {
  file_contents_t result = {0};

  result.data = (char*)data;
  result.size = strlen(data);
  result.valid = false;

  printf("%s - size = %d, is %svalid\n", result.data, result.size,
         result.valid ? "" : "not ");

  return result;
}

/* Dynamic arrays */
#define dynarray(T) T*

typedef struct dynarray_info {
  size_t size;
  size_t capacity;
  size_t element_size;
} dynarray_info;

#define dynarray_init(T, size) (T*)dynarray_initf(sizeof(T), size)
#define dynarray_add(arr, ...)   \
  dynarray_ensure_capacity(arr); \
  (*arr)[dynarray_size(*arr)] = __VA_ARGS__
#define dynarray_get(arr, idx) (*arr)[dynarray_elem(*arr, idx)]

void* dynarray_initf(size_t element_size, size_t size) {
  dynarray_info* d = malloc(element_size * size + sizeof(dynarray_info));
  d->capacity = size;
  d->size = size;
  d->element_size = element_size;
  return d;
}

void dynarray_ensure_capacity(void* data) {
  dynarray_info* d = (*(dynarray_info**)data);
  if (d->capacity == 0) {
    d->capacity = d->size;
    d->size += d->size;
    data = realloc(data, d->size);
  }
}

size_t dynarray_size(void* data) {
  dynarray_info* d = (dynarray_info*)data;
  size_t result =
      ((sizeof(dynarray_info) / d->element_size) + (d->size - d->capacity));
  d->capacity--;
  return result;
}

size_t dynarray_elem(void* data, size_t idx) {
  dynarray_info* d = (dynarray_info*)data;
  size_t result = sizeof(dynarray_info) / d->element_size;
  if (idx < d->size) {
    result += idx;
  }
  return result;
}

int main(int argc, char** argv) {
  static_assert(IS_FLOAT(some_constant.x), "Not a float, fix it");

  v2 v = {.x = 1.0f};  // or  v = (v2){.y = 2.0f};
  printf("Structure Initialization: v = {%f, %f}\n", v.x, v.y);

  printf("/-----------------------------------------------------------/\n");
  {
    printf("Testing macro structures initialized: some_constant(%f, %f)\n",
           some_constant.x, some_constant.y);
  }

  printf("/-----------------------------------------------------------/\n");
  {
    double x = 8.0;
    const float y = 3.375;
    printf("Overloading - Function that operates in doubles = %f\n", cbrt(x));
    printf("Overloading - Function that operates in floats = %f\n", cbrt(y));
    printf("Overloading - typename(%s)\n", typename((char)'a', 2));
    printf("Overloading - typename(%s)\n", typename(55, (v2){.y = 2.0f}));
  }

  printf("/-----------------------------------------------------------/\n");
  {
    defer(begin(), end()) { printf("Awesome macro body\n"); }
    /* This is what the macro expands to
    for (int _i_71 = (begin(), 0); !_i_71; (_i_71 += 1), end())
    {
      printf("Awesome macro body\n");
    } */
  }

  printf("/-----------------------------------------------------------/\n");
  {
    const char* data = "Error Handling Example";
    file_contents_t fc = read_file_contents(data);
    if (!fc.valid) {
      printf("Yupp confirmed, data is invalid\n");
    }
  }

  printf("/-----------------------------------------------------------/\n");
  dynarray(int) array = dynarray_init(int, 2);
  dynarray_add(&array, 99);

  printf("Added value to dynamic array %d", dynarray_get(&array,0));
}