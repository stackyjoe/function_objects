#ifndef JL_FUNCTION_OBJECT
#define JL_FUNCTION_OBJECT

#include "concepts.hpp"
#include "copyable.hpp"
#include "move_only.hpp"
#include "copy_only.hpp"

namespace jl {
	template<class Signature, concepts::copyable Impl>
	copyable::type_erased_callable<Signature> make_function_object(Impl const& impl) {
		return copyable::type_erased_callable<Signature>(impl);
	}

	template<class Signature, concepts::copyable Impl>
	copyable::type_erased_callable<Signature> make_function_object(Impl&& impl) {
		return copyable::type_erased_callable<Signature>(static_cast<std::remove_reference_t<Impl>&&>(impl));
	}

	template<class Signature, concepts::move_only Impl>
	move_only::type_erased_callable<Signature> make_function_object(Impl&& impl) {
		return move_only::type_erased_callable<Signature>(static_cast<std::remove_reference_t<Impl>&&>(impl));
	}

	template<class Signature, concepts::copy_only Impl>
	copy_only::type_erased_callable<Signature> make_function_object(Impl const & impl) {
		return copy_only::type_erased_callable<Signature>(impl);
	}

}
#endif
