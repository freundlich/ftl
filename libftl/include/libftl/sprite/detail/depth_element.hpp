#ifndef LIBFTL_SPRITE_DETAIL_DEPTH_ELEMENT_HPP_INCLUDED
#define LIBFTL_SPRITE_DETAIL_DEPTH_ELEMENT_HPP_INCLUDED

#include <libftl/sprite/depth.hpp>
#include <libftl/sprite/depth_role.hpp>
#include <libftl/sprite/detail/depth_element_fwd.hpp>
#include <fcppt/mpl/list/object.hpp>
#include <fcppt/record/element.hpp>


namespace libftl
{
namespace sprite
{
namespace detail
{

struct depth_element
{
	template<
		typename TypeChoices
	>
	struct apply
	{
		using
		type
		=
		fcppt::mpl::list::object<
			fcppt::record::element<
				libftl::sprite::depth_role,
				libftl::sprite::depth
			>
		>;
	};
};

}
}
}

#endif
