#ifndef LIBFTL_OPTIONS_UNPACKED_RESOURCE_RECORD_FWD_HPP_INCLUDED
#define LIBFTL_OPTIONS_UNPACKED_RESOURCE_RECORD_FWD_HPP_INCLUDED

#include <libftl/options/resource_path_label.hpp>
#include <fcppt/record/element_fwd.hpp>
#include <fcppt/record/variadic_fwd.hpp>
#include <fcppt/config/external_begin.hpp>
#include <boost/filesystem/path.hpp>
#include <fcppt/config/external_end.hpp>


namespace libftl
{
namespace options
{

typedef
fcppt::record::variadic<
	fcppt::record::element<
		libftl::options::resource_path_label,
		boost::filesystem::path
	>
>
unpacked_resource_record;

}
}

#endif