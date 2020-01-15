#ifdef DPCPP_INTERFACES
  #include <CL/sycl.hpp>
#endif

struct CpuDevice {
#ifdef DPCPP_INTERFACES
  static auto get_device() {
    static auto selector = cl::sycl::cpu_selector();
    return selector;
  }
#endif
};

struct GpuDevice {
#ifdef DPCPP_INTERFACES
  static auto get_device() {
    static auto selector = cl::sycl::gpu_selector();
    return selector;
  }
#endif
};
