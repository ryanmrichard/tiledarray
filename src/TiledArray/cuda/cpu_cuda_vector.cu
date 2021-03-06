
#include <TiledArray/cuda/cpu_cuda_vector.h>


namespace thrust {
template<>
void resize<double,thrust::device_allocator<double>>(
    thrust::device_vector<double, thrust::device_allocator<double>>& dev_vec,
    size_t size) {
    dev_vec.resize(size);
}
template<>
void resize<float,thrust::device_allocator<float>>(
    thrust::device_vector<float, thrust::device_allocator<float>>& dev_vec,
    size_t size) {
    dev_vec.resize(size);
}
}

namespace TiledArray {
template class cpu_cuda_vector<double>;
template class cpu_cuda_vector<float>;
}

// Thrust included in CUDA 9 seems to generate uninstantiated CUB calls
#if __CUDACC_VER_MAJOR__ == 10
template <typename Real> auto force_missing_copy_instantiations() {
  return thrust::cuda_cub::copy<thrust::cuda_cub::tag, thrust::detail::normal_iterator<thrust::device_ptr<Real const> >, thrust::device_ptr<Real> >;
}
auto force_missing_copy_instantiations_double() {
  return force_missing_copy_instantiations<double>();
}
auto force_missing_copy_instantiations_float() {
  return force_missing_copy_instantiations<float>();
}
auto force_missing_copy_instantiations_unsigned_long() {
  return force_missing_copy_instantiations<unsigned long>();
}

template <typename Input, typename Output>
auto force_missing_copy_n_instantiations() {
  return thrust::cuda_cub::copy_n<thrust::cuda_cub::tag, thrust::pointer<Input, thrust::cuda_cub::tag>, long, thrust::device_ptr<Output> >;
}

auto force_missing_copy_n_instantiations_float_double(){
  return force_missing_copy_n_instantiations<float, double>();
}

auto force_missing_copy_n_instantiations_double_float(){
  return force_missing_copy_n_instantiations<double, float>();
}

auto force_missing_copy_n_instantiations_long_long(){
  return force_missing_copy_n_instantiations<unsigned long, unsigned long>();
}

#elif __CUDACC_VER_MAJOR__ == 9


template <typename Real> auto force_missing_copy_instantiations() {
  return thrust::cuda_cub::copy<thrust::cuda_cub::tag, thrust::detail::normal_iterator<thrust::device_ptr<Real const> >, thrust::device_ptr<Real> >;
}
auto force_missing_copy_instantiations_double() {
  return force_missing_copy_instantiations<double>();
}
auto force_missing_copy_instantiations_float() {
  return force_missing_copy_instantiations<float>();
}
auto force_missing_copy_instantiations_unsigned_long() {
  return force_missing_copy_instantiations<unsigned long>();
}


template <typename Input, typename Output>
auto force_missing_copy_n_instantiations() {
  return thrust::cuda_cub::copy_n<thrust::cuda_cub::tag, Input*, long, thrust::device_ptr<Output> >;
}

auto force_missing_copy_n_instantiations_float_double(){
  return force_missing_copy_n_instantiations<float, double>();
}

auto force_missing_copy_n_instantiations_double_float(){
  return force_missing_copy_n_instantiations<double, float>();
}

auto force_missing_copy_n_instantiations_long_long(){
  return force_missing_copy_n_instantiations<unsigned long, unsigned long>();
}
#endif
