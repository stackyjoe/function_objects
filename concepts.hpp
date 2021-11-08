#ifndef JL_FUNCTION_CONCEPTS
#define JL_FUNCTION_CONCEPTS

#include <concepts>

namespace jl::concepts {
    template<class T>
    concept copyable = std::copy_constructible<T> && std::move_constructible<T>;

    template<class T>
    concept move_only = std::move_constructible<T> && (!std::copy_constructible<T>);

    template<class T>
    concept copy_only = std::copy_constructible<T> && (!std::move_constructible<T>);

    template<class T>
    concept pinned = (!std::move_constructible<T>) && (!std::copy_constructible<T>);

    template<class T, class R, class ... Args>
    concept copyable_callable = requires(T t) {
        requires copyable<T>;
        {t(std::declval<Args>()...)} -> std::same_as<R>;
    };

    template<class T, class R, class ... Args>
    concept move_only_callable = requires(T t) {
        requires move_only<T>;
        {t(std::declval<Args>()...)} -> std::same_as<R>;
    };

    template<class T, class R, class ... Args>
    concept copy_only_callable = requires(T t) {
        requires copy_only<T>;
        {t(std::declval<Args>()...)} -> std::same_as<R>;
    };

    template<class T, class R, class ... Args>
    concept pinned_callable = requires(T t) {
        requires pinned<T>;
        {t(std::declval<Args>()...)} -> std::same_as<R>;
    };
}
#endif // JL_FUNCTION_CONCEPTS
