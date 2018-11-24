#include <libftl/error.hpp>
#include <libftl/ship/layout/door.hpp>
#include <libftl/ship/layout/ellipse.hpp>
#include <libftl/ship/layout/object.hpp>
#include <libftl/ship/layout/parse.hpp>
#include <libftl/ship/layout/room.hpp>
#include <libftl/ship/layout/room_id.hpp>
#include <libftl/ship/layout/tile_coordinate.hpp>
#include <sge/parse/install_error_handler.hpp>
#include <sge/parse/make_result.hpp>
#include <sge/parse/optional_error_string.hpp>
#include <sge/parse/result.hpp>
#include <sge/parse/result_code.hpp>
#include <fcppt/exception.hpp>
#include <fcppt/output_to_fcppt_string.hpp>
#include <fcppt/make_cref.hpp>
#include <fcppt/make_ref.hpp>
#include <fcppt/noncopyable.hpp>
#include <fcppt/string.hpp>
#include <fcppt/text.hpp>
#include <fcppt/algorithm/map.hpp>
#include <fcppt/cast/to_unsigned.hpp>
#include <fcppt/either/object_impl.hpp>
#include <fcppt/optional/from.hpp>
#include <fcppt/optional/make_if.hpp>
#include <fcppt/optional/map.hpp>
#include <fcppt/optional/object.hpp>
#include <fcppt/optional/value_type.hpp>
#include <fcppt/preprocessor/disable_clang_warning.hpp>
#include <fcppt/preprocessor/pop_warning.hpp>
#include <fcppt/preprocessor/push_warning.hpp>
#include <fcppt/config/external_begin.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/spirit/include/qi_char.hpp>
#include <boost/spirit/include/qi_eol.hpp>
#include <boost/spirit/include/qi_grammar.hpp>
#include <boost/spirit/include/qi_int.hpp>
#include <boost/spirit/include/qi_kleene.hpp>
#include <boost/spirit/include/qi_lit.hpp>
#include <boost/spirit/include/qi_optional.hpp>
#include <boost/spirit/include/qi_parse.hpp>
#include <boost/spirit/include/qi_rule.hpp>
#include <boost/spirit/include/qi_sequence.hpp>
#include <boost/spirit/include/support_istream_iterator.hpp>
#include <ios>
#include <type_traits>
#include <vector>
#include <fcppt/config/external_end.hpp>


namespace
{

typedef
std::make_signed_t<
	libftl::ship::layout::room_id::value_type
>
room_id_signed;

struct room
{
	libftl::ship::layout::room_id::value_type id_;

	libftl::ship::layout::tile_coordinate x_;

	libftl::ship::layout::tile_coordinate y_;

	libftl::ship::layout::tile_coordinate w_;

	libftl::ship::layout::tile_coordinate h_;
};

struct door
{
	libftl::ship::layout::tile_coordinate x_;

	libftl::ship::layout::tile_coordinate y_;

	room_id_signed left_top_;

	room_id_signed bottom_right_;

	int vertical_;
};

struct layout
{
	libftl::ship::layout::object::offset_vector::value_type offset_x_;

	libftl::ship::layout::object::offset_vector::value_type offset_y_;

	int vertical_;

	fcppt::optional::object<int> horizontal_;

	libftl::ship::layout::ellipse::value_type::value_type ellipse_x_;

	libftl::ship::layout::ellipse::value_type::value_type ellipse_y_;

	libftl::ship::layout::ellipse::value_type::value_type ellipse_w_;

	libftl::ship::layout::ellipse::value_type::value_type ellipse_h_;

	std::vector<room> rooms_;

	std::vector<door> doors_;
};

}

FCPPT_PP_PUSH_WARNING
FCPPT_PP_DISABLE_CLANG_WARNING(-Wunused-member-function)
FCPPT_PP_DISABLE_CLANG_WARNING(-Wdisabled-macro-expansion)

BOOST_FUSION_ADAPT_STRUCT(
	room,
	(libftl::ship::layout::room_id::value_type, id_)
	(libftl::ship::layout::tile_coordinate, x_)
	(libftl::ship::layout::tile_coordinate, y_)
	(libftl::ship::layout::tile_coordinate, w_)
	(libftl::ship::layout::tile_coordinate, h_)
)

BOOST_FUSION_ADAPT_STRUCT(
	door,
	(libftl::ship::layout::tile_coordinate, x_)
	(libftl::ship::layout::tile_coordinate, y_)
	(room_id_signed, left_top_)
	(room_id_signed, bottom_right_)
	(int, vertical_)
)

BOOST_FUSION_ADAPT_STRUCT(
	layout,
	(libftl::ship::layout::object::offset_vector::value_type, offset_x_)
	(libftl::ship::layout::object::offset_vector::value_type, offset_y_)
	(int, vertical_)
	(fcppt::optional::object<int>, horizontal_)
	(libftl::ship::layout::ellipse::value_type::value_type, ellipse_x_)
	(libftl::ship::layout::ellipse::value_type::value_type, ellipse_y_)
	(libftl::ship::layout::ellipse::value_type::value_type, ellipse_w_)
	(libftl::ship::layout::ellipse::value_type::value_type, ellipse_h_)
	(std::vector<room>, rooms_)
	(std::vector<door>, doors_)
)

FCPPT_PP_POP_WARNING

namespace
{

template<
	typename In
>
class grammar final
:
	public
	boost::spirit::qi::grammar<
		In,
		::layout()
	>
{
	FCPPT_NONCOPYABLE(
		grammar
	);
public:
	grammar()
	:
		grammar::base_type(
			layout_
		),
		offset_int_{},
		ellipse_int_{},
		room_id_int_{},
		room_id_signed_int_{},
		tile_coordinate_int_{},
		room_{},
		door_{},
		layout_{},
		error_string_{}
	{
		using
		boost::spirit::eol;

		namespace
		qi
		=
		boost::spirit::qi;

		room_ %=
			qi::lit("ROOM") > eol
			>
			room_id_int_ > eol
			>
			tile_coordinate_int_ > eol
			>
			tile_coordinate_int_ > eol
			>
			tile_coordinate_int_ > eol
			>
			tile_coordinate_int_ > eol;

		door_ %=
			qi::lit("DOOR") > eol
			>
			tile_coordinate_int_ > eol
			>
			tile_coordinate_int_ > eol
			>
			room_id_signed_int_ > eol
			>
			room_id_signed_int_ > eol
			>
			qi::int_ > eol;

		layout_ %=
			-(
				qi::lit("X_OFFSET") > eol
				>
				offset_int_ > eol
			)
			>
			-(
				qi::lit("Y_OFFSET") > eol
				>
				offset_int_ > eol
			)
			>
			qi::lit("VERTICAL") > eol
			>
			qi::int_ > eol
			>
			-(
				qi::lit("HORIZONTAL") > eol
				>
				qi::int_ > eol
			)
			>
			qi::lit("ELLIPSE") > eol
			>
			ellipse_int_ > eol
			>
			ellipse_int_ > eol
			>
			ellipse_int_ > eol
			>
			ellipse_int_ > eol
			>
			*room_
			>
			*door_;

		sge::parse::install_error_handler(
			fcppt::make_ref(
				layout_
			),
			fcppt::make_ref(
				error_string_
			)
		);
	}

	~grammar()
	{
	}

	sge::parse::optional_error_string const &
	error_string() const
	{
		return
			error_string_;
	}
public:
	boost::spirit::qi::int_parser<
		fcppt::optional::value_type<
			libftl::ship::layout::object::offset_vector::value_type
		>
	>
	offset_int_;

	boost::spirit::qi::int_parser<
		libftl::ship::layout::ellipse::value_type::value_type
	>
	ellipse_int_;

	boost::spirit::qi::int_parser<
		libftl::ship::layout::room_id::value_type
	>
	room_id_int_;

	boost::spirit::qi::int_parser<
		room_id_signed
	>
	room_id_signed_int_;

	boost::spirit::qi::int_parser<
		libftl::ship::layout::tile_coordinate
	>
	tile_coordinate_int_;

	boost::spirit::qi::rule<
		In,
		::room()
	>
	room_;

	boost::spirit::qi::rule<
		In,
		::door()
	>
	door_;

	boost::spirit::qi::rule<
		In,
		::layout()
	>
	layout_;

	sge::parse::optional_error_string error_string_;
};

bool
translate_bool(
	int const _value
)
{
	switch(
		_value
	)
	{
	case 0:
		return
			false;
	case 1:
		return
			true;
	}

	throw
		fcppt::exception{
			FCPPT_TEXT("Invalid boolean value ")
			+
			fcppt::output_to_fcppt_string(
				_value
			)
		};
}

libftl::ship::layout::door::optional_room_id
translate_optional_door_room(
	room_id_signed const _id
)
{
	if(
		_id
		<
		-1
	)
		throw
			fcppt::exception{
				FCPPT_TEXT("Invalid door room ")
				+
				fcppt::output_to_fcppt_string(
					_id
				)
			};

	return
		fcppt::optional::make_if(
			_id
			>=
			0,
			[
				_id
			]{
				return
					libftl::ship::layout::room_id{
						fcppt::cast::to_unsigned(
							_id
						)
					};
			}
		);
}

libftl::ship::layout::object
translate_result(
	layout const &_layout
)
{
	return
		libftl::ship::layout::object{
			libftl::ship::layout::object::offset_vector{
				_layout.offset_x_,
				_layout.offset_y_
			},
			libftl::ship::layout::object::vertical{
				_layout.vertical_
			},
			libftl::ship::layout::object::horizontal{
				_layout.horizontal_
			},
			libftl::ship::layout::ellipse{
				libftl::ship::layout::ellipse::value_type{
					libftl::ship::layout::ellipse::value_type::vector{
						_layout.ellipse_x_,
						_layout.ellipse_y_
					},
					libftl::ship::layout::ellipse::value_type::dim{
						_layout.ellipse_w_,
						_layout.ellipse_h_
					}
				}
			},
			fcppt::algorithm::map<
				libftl::ship::layout::object::room_list
			>(
				_layout.rooms_,
				[](
					room const &_room
				)
				{
					return
						libftl::ship::layout::room{
							libftl::ship::layout::room_id{
								_room.id_
							},
							libftl::ship::layout::tile_rect{
								libftl::ship::layout::tile_rect::vector{
									libftl::ship::layout::tile_coordinate{
										_room.x_
									},
									libftl::ship::layout::tile_coordinate{
										_room.y_
									}
								},
								libftl::ship::layout::tile_rect::dim{
									libftl::ship::layout::tile_coordinate{
										_room.w_
									},
									libftl::ship::layout::tile_coordinate{
										_room.h_
									}
								}
							}
						};
				}
			),
			fcppt::algorithm::map<
				libftl::ship::layout::object::door_list
			>(
				_layout.doors_,
				[](
					door const &_door
				)
				{
					return
						libftl::ship::layout::door{
							libftl::ship::layout::tile_pos{
								libftl::ship::layout::tile_coordinate{
									_door.x_
								},
								libftl::ship::layout::tile_coordinate{
									_door.y_
								}
							},
							libftl::ship::layout::door::left_top_room{
								translate_optional_door_room(
									_door.left_top_
								)
							},
							libftl::ship::layout::door::bottom_right_room{
								translate_optional_door_room(
									_door.bottom_right_
								)
							},
							libftl::ship::layout::door::vertical(
								translate_bool(
									_door.vertical_
								)
							)
						};
				}
			)
		};
}

typedef
fcppt::either::object<
	libftl::error,
	libftl::ship::layout::object
>
result_type;

}

fcppt::either::object<
	libftl::error,
	libftl::ship::layout::object
>
libftl::ship::layout::parse(
	std::istream &_stream
)
try
{
	_stream.unsetf(
		std::ios_base::skipws
	);

	boost::spirit::istream_iterator begin(
		_stream
	);

	boost::spirit::istream_iterator end{};

	grammar<
		boost::spirit::istream_iterator
	>
	parser{};

	::layout result_layout{};

	sge::parse::result const result{
		sge::parse::make_result(
			boost::spirit::qi::parse(
				begin,
				end,
				parser,
				result_layout
			),
			fcppt::make_cref(
				begin
			),
			end,
			parser
		)
	};

	return
		result.result_code()
		==
		sge::parse::result_code::ok
		?
			result_type{
				translate_result(
					result_layout
				)
			}
		:
			result_type{
				fcppt::optional::from(
					fcppt::optional::map(
						result.error_string(),
						[](
							sge::parse::error_string const &_error
						)
						{
							return
								libftl::error{
									_error.get()
								};
						}
					),
					[]{
						return
							libftl::error{
								fcppt::string{
									FCPPT_TEXT("Unknown error")
								}
							};
					}
				)
			}
		;
}
catch(
	fcppt::exception const &_error
)
{
	return
		result_type{
			libftl::error{
				_error.string()
			}
		};
}
