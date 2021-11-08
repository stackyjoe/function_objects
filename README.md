# function_objects
Yet another type erased function objects library

Uses C++20. Based on the LLVM implementation of std::function, specifically the `__value_func` class. It provides three type erasure classes `$X::type_erased_callable` where `$X` varies over three different namespaces `jl::copyable`, `jl::copy_only` and `jl::move_only`. These classes use different abstract base classes to implement type erasure. Further, it provides constrained template functions `jl::make_function_object()` which provide a unified interface for creating them, sort of like in the factory pattern.
