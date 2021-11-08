#ifndef JL_FUNCTION_COPYABLE
#define JL_FUNCTION_COPYABLE
#include <array>
#include <cstddef>
#include <memory>
#include <stdexcept>

#include "concepts.hpp"

namespace jl::copyable {

    constexpr size_t buffer_size = 3 * sizeof(void*);

    template<class T>
    consteval bool will_be_stored_in_buffer() noexcept {
        return sizeof(T) <= buffer_size;
    }

    template<class Signature>
    struct abstract_callable;

    template<class R, class ... Args>
    struct abstract_callable<R(Args...)> {
    public:
        abstract_callable() {}
        virtual abstract_callable * clone() const = 0;
        virtual abstract_callable * clone(abstract_callable*) const = 0;
        virtual abstract_callable* move_to(abstract_callable*) = 0;
        virtual void destroy() = 0;
        virtual R operator()(Args &&... /*args*/) = 0;
    };

    template<class Impl, class Signature>
    struct callable;

    template<class Impl, class R, class ... Args>
    struct callable<Impl, R(Args...)> : public abstract_callable<R(Args...)> {
        using abstract_t = abstract_callable<R(Args...)>;
    private:
        Impl impl;

    public:
        callable() { }
        ~callable() = default;

        explicit callable(Impl&& impl) : impl(static_cast<Impl&&>(impl)) { }
        explicit callable(Impl const& impl) : impl(impl) { }

        abstract_t* clone() const override {
            return new callable(*this);
        }

        abstract_t * clone(abstract_t* that) const override {
            return std::construct_at<callable>(reinterpret_cast<callable*>(that), *this);
        }

        abstract_t* move_to(abstract_t* that) override {
            return std::construct_at<callable>(reinterpret_cast<callable*>(that), static_cast<callable&&>(*this));
        }

        void destroy() override {
            if constexpr (will_be_stored_in_buffer<callable>()) {
                impl.~Impl();
            }
            else {
                delete this;
            }
        }

        R operator()(Args &&... args) override {
            return impl(static_cast<std::remove_reference_t<Args>&&>(args)...);
        }
    };

    template<class Signature>
    struct type_erased_callable;

    template<class R, class ...Args>
    struct type_erased_callable<R(Args...)> {
    private:
        // Member using for brevity
        using abstract_base_t = abstract_callable<R(Args...)>;
        using buffer_t = std::array<std::byte, buffer_size>;
        using this_t = type_erased_callable;

        // Actual member objects
        alignas(void*) buffer_t buffer;
        abstract_base_t* stored_function;

        // Convenience functions
        abstract_base_t* buffer_as_abstract_callable() noexcept {
            return reinterpret_cast<abstract_base_t*>(&buffer);
        }

        constexpr bool function_is_in_buffer() const noexcept {
            return static_cast<void const*>(stored_function) == static_cast<void const*>(&buffer);
        }

        void clear() noexcept {
            if (stored_function != nullptr) {
                stored_function->destroy();
            }

            stored_function = nullptr;
            for (auto& b : buffer) {
                b = std::byte(0);
            }
        }

    public:
        type_erased_callable()
            : stored_function(nullptr) {
            for (auto& b : buffer) {
                b = std::byte(0);
            }
        }

        R operator()(Args &&... args) {
            if (stored_function == nullptr) {
                throw std::runtime_error("Bad function call in jl::copyable::type_erased_callable");
            }

            return (*stored_function)(static_cast<Args&&>(args)...);
        }

        template<concepts::copyable_callable<R, Args...> Impl>
        explicit type_erased_callable(Impl const& actual_callable)
            : stored_function(nullptr) {
            using actual_t = callable<Impl, R(Args...)>;

            if constexpr (will_be_stored_in_buffer<actual_t>()) {
                stored_function = std::construct_at<actual_t>(reinterpret_cast<actual_t*>(&buffer), actual_callable);
            }
            else {
                stored_function = new actual_t(actual_callable);
            }
        }

        template<concepts::copyable_callable<R, Args...> Impl>
        explicit type_erased_callable(Impl&& actual_callable)
            : stored_function(nullptr) {
            for (auto& b : buffer) {
                b = std::byte(0);
            }

            using actual_t = callable<Impl, R(Args...)>;

            if constexpr (will_be_stored_in_buffer<actual_t>()) {
                stored_function = std::construct_at<actual_t>(reinterpret_cast<actual_t*>(&buffer), static_cast<Impl&&>(actual_callable));
            }
            else {
                stored_function = new actual_t(static_cast<Impl&&>(actual_callable));
            }
        }

        ~type_erased_callable() noexcept { clear(); }

        this_t& operator=(std::nullptr_t) noexcept {
            clear();

            return *this;
        }

        this_t& operator=(this_t&& other) noexcept {
            clear();

            if (other.function_is_in_buffer()) {
                other.stored_function = other.stored_function->move_to(buffer_as_abstract_callable());
            }

            std::swap(stored_function, other.stored_function);

            return *this;
        }

        type_erased_callable(this_t&& other) : stored_function(nullptr) {
            *this = static_cast<this_t&&>(other);
        }

        this_t& operator=(this_t const& other) noexcept {
            clear();
            if (other.stored_function != nullptr) {
                if (other.function_is_in_buffer()) {
                    stored_function = other.stored_function->clone(buffer_as_abstract_callable());
                }
                else {
                    stored_function = other.stored_function->clone();
                }
            }

            return *this;
        }

        type_erased_callable(this_t const& other) : stored_function(nullptr) {
            *this = other;
        }
    };
}
#endif // JL_FUNCTION_COPYABLE
