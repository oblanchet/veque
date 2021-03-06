/* 
 * 
 * veque::veque test suite.
 * 
 * Additionally, valgrind claims there is no bad behavior throughout this usage.
 *
 *  Copyright (C) 2019 Drew Dormann
 * 
 */

#include <cstdint> 
#include <string> 
#include <unordered_set> 
#include <string> 
#include <functional> 
#include "catch.hpp"
#include "test_types.h"

TEMPLATE_PRODUCT_TEST_CASE( "veque::veques can be sized and resized", "[veque::veque][template]", (StdVeque, GrumpyVeque, PropogatingGrumpyVeque, AllocCountingVeque), (
        (int,veque::fast_resize_traits), (int,veque::std_vector_traits), (int,veque::no_reserve_traits), (int,front_vector_traits), 
        (std::string,veque::fast_resize_traits), (std::string,veque::std_vector_traits), (std::string,veque::no_reserve_traits), (std::string,front_vector_traits), 
        (double,veque::fast_resize_traits), (double,veque::std_vector_traits), (double,veque::no_reserve_traits), (double,front_vector_traits), 
        (LargeTrivialObject,veque::fast_resize_traits), (LargeTrivialObject,veque::std_vector_traits), (LargeTrivialObject,veque::no_reserve_traits), (LargeTrivialObject,front_vector_traits), 
        (NonTrivialObject,veque::fast_resize_traits), (NonTrivialObject,veque::std_vector_traits), (NonTrivialObject,veque::no_reserve_traits), (NonTrivialObject,front_vector_traits), 
        (ThrowingMoveConstructObject,veque::fast_resize_traits), (ThrowingMoveConstructObject,veque::std_vector_traits), (ThrowingMoveConstructObject,veque::no_reserve_traits), (ThrowingMoveConstructObject,front_vector_traits), 
        (ThrowingMoveAssignObject,veque::fast_resize_traits), (ThrowingMoveAssignObject,veque::std_vector_traits), (ThrowingMoveAssignObject,veque::no_reserve_traits), (ThrowingMoveAssignObject,front_vector_traits), 
        (ThrowingMoveObject,veque::fast_resize_traits), (ThrowingMoveObject,veque::std_vector_traits), (ThrowingMoveObject,veque::no_reserve_traits), (ThrowingMoveObject,front_vector_traits)
        ) )
{
    TestType v( 5 );

    REQUIRE( v.size() == 5 );
    REQUIRE( v.capacity() >= 5 );

    SECTION( "resizing bigger changes size and capacity" )
    {
        v.resize_back( 10 );

        REQUIRE( v.size() == 10 );
        REQUIRE( v.capacity() >= 10 );
        REQUIRE( v.capacity_back() >= 10 );

        v.resize_front( 15 );

        REQUIRE( v.size() == 15 );
        REQUIRE( v.capacity() >= 15 );
        REQUIRE( v.capacity_front() >= 15 );
        
        if constexpr( std::is_same_v< typename TestType::allocator_type, CountingAllocator<typename TestType::value_type> > )
        {
            REQUIRE( v.get_allocator().counter == 15 );
        }
    }
    SECTION( "resizing smaller changes size but not capacity" )
    {
        v.resize( 0 );

        REQUIRE( v.size() == 0 );
        REQUIRE( v.capacity() >= 5 );

        v.resize( 5 );

        SECTION( "We can use the 'swap trick' to reset the capacity" )
        {
            TestType( v ).swap( v );

            REQUIRE( v.capacity() == 5 );
            if constexpr( is_using_counting_allocator<TestType> )
            {
                REQUIRE( v.get_allocator().counter == 5 );
            }
        }
        SECTION( "Or we can use shrink_to_fit()" )
        {
            REQUIRE( v.size() == 5 );

            v.shrink_to_fit();

            CHECK( v.size() == 5 );
            CHECK( v.capacity() == 5 );
            if constexpr( is_using_counting_allocator<TestType> )
            {
                REQUIRE( v.get_allocator().counter == 5 );
            }
        }
    }
    SECTION( "reserving smaller does not change size or capacity" )
    {
        v.reserve( 0 );

        CHECK( v.size() == 5 );
        CHECK( v.capacity() >= 5 );
    }
    SECTION( "clearing" )
    {
        v.clear();

        CHECK( v.size() == 0 );
        CHECK( v.empty() );
    }
    SECTION( "reserve_front" )
    {
        v.reserve_front( 20 );
        CHECK( v.capacity_front() >= 20 );
    }
    SECTION( "reserve_back" )
    {
        v.reserve_back( 20 );
        CHECK( v.capacity_back() >= 20 );
    }
    SECTION( "reserve" )
    {
        v.reserve( 20 );
        CHECK( v.capacity_front() >= 20 );
        CHECK( v.capacity_back() >= 20 );
    }
    SECTION( "reserve less" )
    {
        auto old_capacity_front = v.capacity_front();
        auto old_capacity_back = v.capacity_back();

        v.reserve( 0 );
        
        CHECK( v.capacity_front() == old_capacity_front );
        CHECK( v.capacity_back() == old_capacity_back );
    }
}
