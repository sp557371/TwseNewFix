#ifdef __hpux
//---------------------------------------------------------------------------
extern "C" void __cxa_finalize(void*);  // libns/cxa_atexit.cpp
//---------------------------------------------------------------------------
#pragma GCC visibility push(hidden)
  void* __dso_handle;
  void __attribute__((destructor)) dll_fini();   // ADD: "libns/cxa_dll_fini.cpp" in your shared lib.
#pragma GCC visibility pop
//---------------------------------------------------------------------------
#endif
