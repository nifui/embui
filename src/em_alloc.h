// A simple define directive probably isn't enough. Might need to abstract into a function call or
// something akin.

#ifdef __linux__
#define TEST
#endif

#ifdef _WIN32
#endif

#ifdef _WIN64
#endif

#ifdef __APPLE__
#endif

#ifdef __ANDROID__
#endif

#ifndef TEST
#error Unsupported operating system.
#endif

// Defines a custom pool allocator for use with the tree.
