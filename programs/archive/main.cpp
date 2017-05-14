#include <libftl/archive/entry_output.hpp>
#include <libftl/archive/extract.hpp>
#include <libftl/archive/index.hpp>
#include <libftl/archive/read_index.hpp>
#include <fcppt/args_char.hpp>
#include <fcppt/args_from_second.hpp>
#include <fcppt/exception.hpp>
#include <fcppt/from_std_string.hpp>
#include <fcppt/insert_to_fcppt_string.hpp>
#include <fcppt/loop.hpp>
#include <fcppt/main.hpp>
#include <fcppt/string.hpp>
#include <fcppt/strong_typedef_output.hpp>
#include <fcppt/text.hpp>
#include <fcppt/unit.hpp>
#include <fcppt/algorithm/fold_break.hpp>
#include <fcppt/cast/to_signed.hpp>
#include <fcppt/container/output.hpp>
#include <fcppt/either/match.hpp>
#include <fcppt/either/object.hpp>
#include <fcppt/filesystem/create_directories_recursive.hpp>
#include <fcppt/filesystem/path_to_string.hpp>
#include <fcppt/io/buffer.hpp>
#include <fcppt/io/cerr.hpp>
#include <fcppt/io/cout.hpp>
#include <fcppt/io/ostream.hpp>
#include <fcppt/optional/maybe.hpp>
#include <fcppt/options/apply.hpp>
#include <fcppt/options/argument.hpp>
#include <fcppt/options/error.hpp>
#include <fcppt/options/help_text.hpp>
#include <fcppt/options/long_name.hpp>
#include <fcppt/options/make_optional.hpp>
#include <fcppt/options/no_default_value.hpp>
#include <fcppt/options/option.hpp>
#include <fcppt/options/optional_help_text.hpp>
#include <fcppt/options/optional_short_name.hpp>
#include <fcppt/options/parse.hpp>
#include <fcppt/options/short_name.hpp>
#include <fcppt/options/result_of.hpp>
#include <fcppt/preprocessor/disable_gcc_warning.hpp>
#include <fcppt/preprocessor/pop_warning.hpp>
#include <fcppt/preprocessor/push_warning.hpp>
#include <fcppt/record/get.hpp>
#include <fcppt/record/make_label.hpp>
#include <fcppt/system/error_code_to_string.hpp>
#include <fcppt/variant/output.hpp>
#include <fcppt/config/external_begin.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/system/error_code.hpp>
#include <cstdlib>
#include <exception>
#include <ios>
#include <iostream>
#include <istream>
#include <ostream>
#include <utility>
#include <fcppt/config/external_end.hpp>


FCPPT_PP_PUSH_WARNING
FCPPT_PP_DISABLE_GCC_WARNING(-Wmissing-declarations)

// FIXME
namespace std
{

fcppt::io::ostream &
operator<<(
	fcppt::io::ostream &_stream,
	libftl::archive::index::value_type const &_pair
)
{
	return
		_stream
		<<
		FCPPT_TEXT('(')
		<<
		fcppt::from_std_string(
			_pair.first
		)
		<<
		FCPPT_TEXT(',')
		<<
		_pair.second
		<<
		FCPPT_TEXT(')');
}

}

namespace
{

typedef
fcppt::either::object<
	fcppt::string,
	fcppt::unit
>
either_error;

either_error
write_output(
	std::istream &_stream,
	boost::filesystem::path const &_output_path,
	libftl::archive::index::value_type const &_entry
)
{
	boost::filesystem::path const output_directory{
		(
			_output_path
			/
			_entry.first
		).remove_filename()
	};

	return
		fcppt::optional::maybe(
			fcppt::filesystem::create_directories_recursive(
				output_directory
			),
			[
				&_stream,
				&_output_path,
				&_entry
			]
			{
				return
					fcppt::optional::maybe(
						libftl::archive::extract(
							_stream,
							_entry.second
						),
						[
							&_entry
						]()
						{
							return
								either_error{
									FCPPT_TEXT("Failed to extract ")
									+
									fcppt::insert_to_fcppt_string(
										_entry
									)
								};
						},
						[
							&_stream,
							&_output_path,
							&_entry
						](
							fcppt::io::buffer &&_buffer
						)
						{
							boost::filesystem::path const path{
								_output_path
								/
								_entry.first
							};

							boost::filesystem::ofstream output_stream{
								path,
								std::ios_base::binary
							};

							fcppt::io::cout()
								<<
								FCPPT_TEXT("Extracting ")
								<<
								fcppt::filesystem::path_to_string(
									path
								)
								<<
								FCPPT_TEXT('\n');

							if(
								!output_stream.is_open()
							)
								return
									either_error{
										FCPPT_TEXT("Failed to open ")
										+
										fcppt::filesystem::path_to_string(
											path
										)
									};

							// TODO: Make a function for this?
							output_stream.write(
								_buffer.data(),
								fcppt::cast::to_signed(
									_buffer.size()
								)
							);

							return
								output_stream
								?
									either_error{
										fcppt::unit{}
									}
								:
									either_error{
										FCPPT_TEXT("Failed to write ")
										+
										fcppt::filesystem::path_to_string(
											path
										)
									}
								;
						}
					);
			},
			[
				&output_directory
			](
				boost::system::error_code const _error
			)
			{
				return
					either_error{
						FCPPT_TEXT("Failed to create directory ")
						+
						fcppt::filesystem::path_to_string(
							output_directory
						)
						+
						FCPPT_TEXT(" - ")
						+
						fcppt::system::error_code_to_string(
							_error
						)
					};
			}
		);
}

either_error
create_outputs(
	std::istream &_stream,
	boost::filesystem::path const &_output_path,
	libftl::archive::index const &_index
)
{
	// TODO: Create some algorithm like foldM_ for this!
	return
		fcppt::algorithm::fold_break(
			_index,
			either_error{
				fcppt::unit{}
			},
			[
				&_stream,
				&_output_path
			](
				libftl::archive::index::value_type const &_entry,
				either_error
			)
			{
				return
					fcppt::either::match(
						write_output(
							_stream,
							_output_path,
							_entry
						),
						[](
							fcppt::string &&_error
						)
						{
							return
								std::make_pair(
									fcppt::loop::break_,
									either_error{
										std::move(
											_error
										)
									}
								);
						},
						[
							&_stream,
							&_entry
						](
							fcppt::unit
						){
							return
								std::make_pair(
									fcppt::loop::continue_,
									either_error{
										fcppt::unit{}
									}
								);
						}
					);

			}
		);
}

}

int
FCPPT_MAIN(
	int argc,
	fcppt::args_char **argv
)
try
{
	FCPPT_RECORD_MAKE_LABEL(
		path_label
	);

	FCPPT_RECORD_MAKE_LABEL(
		output_path_label
	);

	auto const parser{
		fcppt::options::apply(
			fcppt::options::argument<
				path_label,
				fcppt::string
			>{
				fcppt::options::long_name{
					FCPPT_TEXT("Path")
				},
				fcppt::options::optional_help_text{
					fcppt::options::help_text{
						FCPPT_TEXT("Path to the .dat file")
					}
				}
			},
			fcppt::options::make_optional(
				fcppt::options::option<
					output_path_label,
					fcppt::string
				>{
					fcppt::options::optional_short_name{
						fcppt::options::short_name{
							FCPPT_TEXT("o")
						}
					},
					fcppt::options::long_name{
						FCPPT_TEXT("output-path")
					},
					fcppt::options::no_default_value<
						fcppt::string
					>(),
					fcppt::options::optional_help_text{
						fcppt::options::help_text{
							FCPPT_TEXT("Path to output the files to")
						}
					}
				}
			)
		)
	};

	typedef
	decltype(
		parser
	)
	parser_type;

	return
		fcppt::either::match(
			fcppt::options::parse(
				parser,
				fcppt::args_from_second(
					argc,
					argv
				)
			),
			[
				&parser
			](
				fcppt::options::error const &_error
			)
			{
				fcppt::io::cerr()
					<<
					_error
					<<
					FCPPT_TEXT("\nUsage: ")
					<<
					parser.usage()
					<<
					FCPPT_TEXT('\n');

				return
					EXIT_FAILURE;
			},
			[](
				fcppt::options::result_of<
					parser_type
				> const &_arguments
			)
			{
				boost::filesystem::path const path{
					fcppt::record::get<
						path_label
					>(
						_arguments
					)
				};

				boost::filesystem::ifstream stream{
					path,
					std::ios_base::binary
				};

				if(
					!stream.is_open()
				)
				{
					fcppt::io::cerr()
						<<
						FCPPT_TEXT("Cannot open ")
						<<
						fcppt::filesystem::path_to_string(
							path
						)
						<<
						FCPPT_TEXT('\n');

					return
						EXIT_FAILURE;
				}

				return
					fcppt::either::match(
						libftl::archive::read_index(
							stream
						),
						[](
							fcppt::string const &_error
						)
						{
							fcppt::io::cerr()
								<<
								_error
								<<
								FCPPT_TEXT('\n');

							return
								EXIT_FAILURE;
						},
						[
							&stream,
							&_arguments
						](
							libftl::archive::index const &_index
						)
						{
							return
								fcppt::optional::maybe(
									fcppt::record::get<
										output_path_label
									>(
										_arguments
									),
									[
										&_index
									]{
										fcppt::io::cout()
											<<
											fcppt::container::output(
												_index
											)
											<<
											FCPPT_TEXT('\n');

										return
											EXIT_SUCCESS;
									},
									[
										&stream,
										&_index
									](
										fcppt::string const &_output_path
									)
									{
										return
											fcppt::either::match(
												create_outputs(
													stream,
													boost::filesystem::path{
														_output_path
													},
													_index
												),
												[](
													fcppt::string const &_error
												)
												{
													fcppt::io::cerr()
														<<
														_error
														<<
														FCPPT_TEXT('\n');

													return
														EXIT_FAILURE;
												},
												[](
													fcppt::unit
												)
												{
													return
														EXIT_SUCCESS;
												}
											);
									}
								);
						}
					);
			}
		);
}
catch(
	fcppt::exception const &_error
)
{
	fcppt::io::cerr()
		<<
		_error.string()
		<<
		FCPPT_TEXT('\n');

	return
		EXIT_FAILURE;
}
catch(
	std::exception const &_error
)
{
	std::cerr
		<<
		_error.what()
		<<
		'\n';

	return
		EXIT_FAILURE;
}

FCPPT_PP_POP_WARNING
