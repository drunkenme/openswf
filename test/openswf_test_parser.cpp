#include "openswf_test.hpp"

using namespace openswf;
using namespace openswf::record;

TEST_CASE("PARSE_TAG_HEADER", "[OPENSWF]")
{
    auto stream = create_from_file("../test/resources/openswf_test_parser.swf");
    REQUIRE( stream.get_position() == 0 );
    REQUIRE( stream.get_bit_position() == 0 );
    REQUIRE( stream.get_size() == 1340 );

    auto header = Header::read(stream);

    REQUIRE( header.frame_count == 1 );
    REQUIRE( header.frame_rate == Approx(24) );
    REQUIRE( header.frame_size.get_width() == Approx(320) );
    REQUIRE( header.frame_size.get_width() == Approx(320) );

    {
        auto tag = TagHeader::read(stream);
        stream.set_position(tag.end_pos);
        REQUIRE( tag.code == TagCode::FILE_ATTRIBUTES );
    }

    {
        auto tag = TagHeader::read(stream);
        stream.set_position(tag.end_pos);
        REQUIRE( tag.code == TagCode::METADATA );
    }
    
    {
        auto tag = TagHeader::read(stream);
        stream.set_position(tag.end_pos);
        REQUIRE( tag.code == TagCode::SET_BACKGROUND_COLOR );
    }

    {
        auto tag = TagHeader::read(stream);
        stream.set_position(tag.end_pos);
        REQUIRE( tag.code == TagCode::DEFINE_SCENE_AND_FRAME_LABEL_DATA );
    }

    {
        auto tag = TagHeader::read(stream);
        stream.set_position(tag.end_pos);
        REQUIRE( tag.code == TagCode::SHOW_FRAME );
    }

    {
        auto tag = TagHeader::read(stream);
        stream.set_position(tag.end_pos);
        REQUIRE( tag.code == TagCode::END );
    }
    REQUIRE( stream.is_finished() );
}

TEST_CASE("DEFINE-SHAPE-PARSE", "[OPENSWF]")
{
    auto stream = create_from_file("../test/resources/simple-shape-1.swf");
    auto header = get_tag_at(stream, 5);

    REQUIRE( header.code == TagCode::DEFINE_SHAPE );
    auto def = DefineShape::read(stream);

    std::vector<std::vector<Point>> paths;
    std::vector<int> fill_0_array, fill_1_array, line_array;
    {
        Point path[] = { 
            Point(4660, 2580), 
            Point(7629, 2580), 
            Point(7629, 6089),
            Point(4120, 6089),
            Point(4120, 3280)
        };
        paths.push_back(std::vector<Point>(path, path+5));
        fill_0_array.push_back(0);
        fill_1_array.push_back(1);
        line_array.push_back(0);
    }
    {
        Point path[] = { 
            Point(4120, 3280),
            Point(440, 3280),
            Point(440, 1220),
            Point(4660, 1220),
            Point(4660, 2580)
        };
        paths.push_back(std::vector<Point>(path, path+5));
        fill_0_array.push_back(0);
        fill_1_array.push_back(2);
        line_array.push_back(0);
    }
    {
        Point path[] = { 
            Point(4660, 2580),
            Point(4120, 2580),
            Point(4120, 3280)
        };
        paths.push_back(std::vector<Point>(path, path+3));
        fill_0_array.push_back(1);
        fill_1_array.push_back(0);
        line_array.push_back(0);
    }

    REQUIRE( def.paths.size() == paths.size() );
    for( int i=0; i<paths.size(); i++ )
    {
        auto& path = def.paths[i];
        auto& compare = paths[i];

        REQUIRE( path.fill_0 == fill_0_array[i] );
        REQUIRE( path.fill_1 == fill_1_array[i] );
        REQUIRE( path.line == line_array[i] );

        REQUIRE( path.x == compare[0].x );
        REQUIRE( path.y == compare[0].y );
        REQUIRE( path.edges.size() == compare.size() - 1 );
        for( int i=0; i<path.edges.size(); i++ )
        {
            REQUIRE( path.edges[i].anchor_x == compare[i+1].x );
            REQUIRE( path.edges[i].anchor_y == compare[i+1].y );
        }
    }
}
