#ifndef LIBFTL_IMPL_XML_SPIRIT_VARIANT_IMPL_HPP_INCLUDED
#define LIBFTL_IMPL_XML_SPIRIT_VARIANT_IMPL_HPP_INCLUDED

#include <libftl/impl/xml/spirit/variant_decl.hpp>


template<
	typename Variant
>
libftl::impl::xml::spirit::variant<
	Variant
>::variant()
:
	impl_{}
{
}

template<
	typename Variant
>
template<
	typename Type
>
libftl::impl::xml::spirit::variant<
	Variant
>::variant(
	Type const &_other
)
:
	impl_{
		Variant{
			_other
		}
	}
{
}

template<
	typename Variant
>
template<
	typename Type
>
libftl::impl::xml::spirit::variant<
 	Variant
> &
libftl::impl::xml::spirit::variant<
	Variant
>::operator=(
	Type const &_other
)
{
	impl_ =
		optional{
			Variant{
				_other
			}
		};

	return
		*this;
}

template<
	typename Variant
>
Variant const &
libftl::impl::xml::spirit::variant<
	Variant
>::get() const
{
	// TODO: Put an exception here
	return
		impl_.get_unsafe();
}

#endif
