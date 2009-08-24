#include "variable_list.h"
#include "permutation.h"
#include <boost/test/unit_test.hpp>
#include <boost/test/output_test_stream.hpp>

//using namespace TiledArray;
using namespace TiledArray::math;
using TiledArray::Permutation;

struct VariableListFixture {
  VariableListFixture() : v("a,b,c,d") { }

  VariableList v;
};

BOOST_FIXTURE_TEST_SUITE( variable_list_suite , VariableListFixture )

BOOST_AUTO_TEST_CASE( accessors )
{
  BOOST_CHECK_EQUAL(v.dim(), 4); // check for variable count
  BOOST_CHECK_EQUAL(v.at(0), "a"); // check 1st variable access
  BOOST_CHECK_EQUAL(v.at(3), "d"); // check last variable access
  BOOST_CHECK_EQUAL(v[0], "a"); // check 1st variable access
  BOOST_CHECK_EQUAL(v[3], "d"); // check last variable access
  BOOST_CHECK_THROW(v.at(4), std::out_of_range); // check for out of range throw.
}

BOOST_AUTO_TEST_CASE( constructor )
{
  BOOST_REQUIRE_NO_THROW(VariableList v0); // Check default constructor
  VariableList v0;
  BOOST_CHECK_EQUAL(v0.dim(), 0);

  BOOST_REQUIRE_NO_THROW(VariableList v1("a,b,c,d")); // check string constructor
  VariableList v1("a,b,c,d");
  BOOST_CHECK_EQUAL(v1.dim(), 4);
  BOOST_CHECK_EQUAL(v1.at(0), "a");  // check for corret data
  BOOST_CHECK_EQUAL(v1.at(1), "b");
  BOOST_CHECK_EQUAL(v1.at(2), "c");
  BOOST_CHECK_EQUAL(v1.at(3), "d");

  BOOST_REQUIRE_NO_THROW(VariableList v2(v)); // check string constructor
  VariableList v2(v);
  BOOST_CHECK_EQUAL(v2.dim(), 4);
  BOOST_CHECK_EQUAL(v2.at(0), "a");  // check for corret data
  BOOST_CHECK_EQUAL(v2.at(1), "b");
  BOOST_CHECK_EQUAL(v2.at(2), "c");
  BOOST_CHECK_EQUAL(v2.at(3), "d");

  boost::array<std::string, 4> a10 = {{"a", "b", "c", "d"}};
  BOOST_REQUIRE_NO_THROW(VariableList v10(a10.begin(), a10.end())); // check string constructor
  VariableList v10(a10.begin(), a10.end());
  BOOST_CHECK_EQUAL(v10.dim(), 4);
  BOOST_CHECK_EQUAL(v10.at(0), "a");  // check for corret data
  BOOST_CHECK_EQUAL(v10.at(1), "b");
  BOOST_CHECK_EQUAL(v10.at(2), "c");
  BOOST_CHECK_EQUAL(v10.at(3), "d");

  BOOST_CHECK_THROW(VariableList v3(",a,b,c"), std::runtime_error); // check invalid input
  BOOST_CHECK_THROW(VariableList v4("a,,b,c"), std::runtime_error);
  BOOST_CHECK_THROW(VariableList v5(" ,a,b"), std::runtime_error);
  BOOST_CHECK_THROW(VariableList v6("a,  b,   , c"), std::runtime_error);
  BOOST_CHECK_THROW(VariableList v8("a,b,a,c"), std::runtime_error);
  BOOST_CHECK_THROW(VariableList v9("a,a,b,c"), std::runtime_error);

  VariableList v7(" a , b, c, d , e e ,f f, g10,h, i "); // check input with various spacings.
  BOOST_CHECK_EQUAL(v7.at(0), "a");
  BOOST_CHECK_EQUAL(v7.at(1), "b");
  BOOST_CHECK_EQUAL(v7.at(2), "c");
  BOOST_CHECK_EQUAL(v7.at(3), "d");
  BOOST_CHECK_EQUAL(v7.at(4), "ee");
  BOOST_CHECK_EQUAL(v7.at(5), "ff");
  BOOST_CHECK_EQUAL(v7.at(6), "g10");
  BOOST_CHECK_EQUAL(v7.at(7), "h");
  BOOST_CHECK_EQUAL(v7.at(8), "i");

  BOOST_REQUIRE_NO_THROW(VariableList v11("")); // Check default constructor
  VariableList v11("");
  BOOST_CHECK_EQUAL(v11.dim(), 0);
}

BOOST_AUTO_TEST_CASE( iterator )
{
  boost::array<std::string, 4> a1 = {{"a", "b", "c", "d"}};

  BOOST_CHECK_EQUAL_COLLECTIONS(v.begin(), v.end(), a1.begin(), a1.end()); // check that all values are equal
  boost::array<std::string, 4>::const_iterator it_a = a1.begin();
  for(VariableList::const_iterator it = v.begin(); it != v.end(); ++it, ++it_a) // check basic iterator functionality.
    BOOST_CHECK_EQUAL(*it, *it_a);

}

BOOST_AUTO_TEST_CASE( comparison )
{
  VariableList v1("a,b,c,d");
  VariableList v2("d,b,c,a");

  BOOST_CHECK(v1 == v);    // check variable list comparison operators
  BOOST_CHECK(!(v2 == v));
  BOOST_CHECK(v2 != v);
  BOOST_CHECK(!(v1 != v));
}

BOOST_AUTO_TEST_CASE( assignment )
{
  VariableList v1;
  v1 = v;
  BOOST_CHECK_EQUAL_COLLECTIONS(v1.begin(), v1.end(), v.begin(), v.end());

  VariableList v2;
  v2 = "a,b,c,d";
  BOOST_CHECK_EQUAL_COLLECTIONS(v2.begin(), v2.end(), v.begin(), v.end());
}

BOOST_AUTO_TEST_CASE( permutation )
{
  Permutation<4> p(1,2,3,0);
  VariableList v1(v);
  VariableList v2 = (p ^ v1);
  BOOST_CHECK_EQUAL(v2[0], "d");
  BOOST_CHECK_EQUAL(v2[1], "a");
  BOOST_CHECK_EQUAL(v2[2], "b");
  BOOST_CHECK_EQUAL(v2[3], "c");

  VariableList v3(v);
  v3 ^= p;
  BOOST_CHECK_EQUAL_COLLECTIONS(v3.begin(), v3.end(), v2.begin(), v2.end());
}

BOOST_AUTO_TEST_CASE( ostream )
{
  boost::test_tools::output_test_stream output;
  output << v;
  BOOST_CHECK( !output.is_empty( false ) ); // check for correct output.
  BOOST_CHECK( output.check_length( 12, false ) );
  BOOST_CHECK( output.is_equal("(a, b, c, d)") );
}

BOOST_AUTO_TEST_CASE( common )
{
  const VariableList v_aib("a,i,b");
  const VariableList v_xiy("x,i,y");
  const VariableList v_ai("a,i");
  const VariableList v_xi("x,i");
  const VariableList v_ia("i,a");
  const VariableList v_ix("i,x");
  const VariableList v_i("i");
  const VariableList v_0("");

  std::pair<VariableList::const_iterator, VariableList::const_iterator> p1;
  std::pair<VariableList::const_iterator, VariableList::const_iterator> p2;

//  find_common(v_aib.begin(), v_aib.end(), v_xiy.begin(), v_xiy.end(), p1, p2);
//  find_common(v_aib.begin(), v_aib.end(), v_xi.begin(), v_xi.end(), p1, p2);
//  find_common(v_aib.begin(), v_aib.end(), v_ix.begin(), v_ix.end(), p1, p2);
//  find_common(v_aib.begin(), v_aib.end(), v_i.begin(), v_i.end(), p1, p2);
//  find_common(v_ai.begin(), v_ai.end(), v_xiy.begin(), v_xiy.end(), p1, p2);
//  find_common(v_ai.begin(), v_ai.end(), v_xi.begin(), v_xi.end(), p1, p2);
}

BOOST_AUTO_TEST_CASE( math_functors ) {
  VariableList v1("a,b,c,d");
  std::plus<VariableList> plus_op;
  std::minus<VariableList> minus_op;
  std::multiplies<VariableList> multiplies_op;
  VariableList vr1 = plus_op(v, v1);  // check std::plus functor
  BOOST_CHECK(vr1 == v1);
  BOOST_CHECK(vr1 == v);

  VariableList vr2 = minus_op(v, v1); // check std::minus functor
  BOOST_CHECK(vr2 == v1);
  BOOST_CHECK(vr2 == v);


  VariableList vr3;
  vr3 = multiplies_op(VariableList("a,i,b"), VariableList("c,i,d")); // check std::multiplies functor
  BOOST_CHECK(vr3 == VariableList("a,c,b,d"));
  vr3 = multiplies_op(VariableList("a,i,b"), VariableList("c,i"));
  BOOST_CHECK(vr3 == VariableList("a,c,b"));
  vr3 = multiplies_op(VariableList("a,i,b"), VariableList("i,c"));
  BOOST_CHECK(vr3 == VariableList("a,b,c"));
  vr3 = multiplies_op(VariableList("a,i,b"), VariableList("i"));
  BOOST_CHECK(vr3 == VariableList("a,b"));
  vr3 = multiplies_op(VariableList("a,i"), VariableList("b,i,c"));
  BOOST_CHECK(vr3 == VariableList("b,a,c"));
  vr3 = multiplies_op(VariableList("a,i"), VariableList("b,i"));
  BOOST_CHECK(vr3 == VariableList("a,b"));
  vr3 = multiplies_op(VariableList("a,i"), VariableList("i,b"));
  BOOST_CHECK(vr3 == VariableList("a,b"));
  vr3 = multiplies_op(VariableList("a,i"), VariableList("i"));
  BOOST_CHECK(vr3 == VariableList("a"));
  vr3 = multiplies_op(VariableList("i,a"), VariableList("b,i,c"));
  BOOST_CHECK(vr3 == VariableList("b,a,c"));
  vr3 = multiplies_op(VariableList("i,a"), VariableList("b,i"));
  BOOST_CHECK(vr3 == VariableList("b,a"));
  vr3 = multiplies_op(VariableList("i,a"), VariableList("i,b"));
  BOOST_CHECK(vr3 == VariableList("a,b"));
  vr3 = multiplies_op(VariableList("i,a"), VariableList("i"));
  BOOST_CHECK(vr3 == VariableList("a"));
  vr3 = multiplies_op(VariableList("i"), VariableList("a,i,b"));
  BOOST_CHECK(vr3 == VariableList("a,b"));
  vr3 = multiplies_op(VariableList("i"), VariableList("a,i"));
  BOOST_CHECK(vr3 == VariableList("a"));
  vr3 = multiplies_op(VariableList("i"), VariableList("i,a"));
  BOOST_CHECK(vr3 == VariableList("a"));
  vr3 = multiplies_op(VariableList("i"), VariableList("i"));
  BOOST_CHECK(vr3 == VariableList(""));
}

BOOST_AUTO_TEST_SUITE_END()
