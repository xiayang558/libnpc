// Return header file directory path (can be modified based on actual install location)
const char* get_include(void) {
    // Default returns the include path under the current source directory (or install path)
    // Users can modify this to an actual path as needed, or define via compilation macro
#ifdef ARRAY_INCLUDE_PATH
    return ARRAY_INCLUDE_PATH;
#else
    return "/Users/xiayang/work/numpy";   // Example path
#endif
}
