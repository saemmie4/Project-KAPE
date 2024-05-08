#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "geometry.hpp"

TEST_CASE("Testing the Vector2d class"){
    kape::Vector2d v1{1., 5.};
    kape::Vector2d v2{0., -5.};
    SUBCASE("testing operator +"){
        CHECK((v1+v2).x == doctest::Approx(1.));
        CHECK((v1+v2).y == doctest::Approx(0.));
        CHECK((v1+v1+v2+v1).x == doctest::Approx(3.));
        CHECK((v1+v1+v2+v1).y == doctest::Approx(10.));
    }
    SUBCASE("testing operator v-v"){
        CHECK((v1-v2).x == doctest::Approx(1.));
        CHECK((v1-v2).y == doctest::Approx(10.));
        CHECK((v1-v1-v2-v1).x == doctest::Approx(-1.));
        CHECK((v1-v1-v2-v1).y == doctest::Approx(0.));
    }
    SUBCASE("testing operator -v"){
        CHECK((-v1).x == doctest::Approx(-1.));
        CHECK((-v1).y == doctest::Approx(0.));
        CHECK((-v2).x == doctest::Approx(0.));
        CHECK((-v2).y == doctest::Approx(5.));
    }
    SUBCASE("testing operator v*v"){
        
        CHECK((v1*v1).x == doctest::Approx(-1.));
        CHECK((-v1).y == doctest::Approx(0.));
        CHECK((-v2).x == doctest::Approx(0.));
        CHECK((-v2).y == doctest::Approx(5.));
    }
}