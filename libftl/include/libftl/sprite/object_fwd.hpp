#ifndef LIBFTL_SPRITE_OBJECT_FWD_HPP_INCLUDED
#define LIBFTL_SPRITE_OBJECT_FWD_HPP_INCLUDED

#include <libftl/sprite/choices_fwd.hpp>
#include <sge/sprite/object_fwd.hpp>


namespace libftl
{
namespace sprite
{

typedef
sge::sprite::object<
	libftl::sprite::choices
>
object;

}
}

#endif