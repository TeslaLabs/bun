#include <boost/proto/proto.hpp>
#include <boost/mpl/vector.hpp>
#include <type_traits>
#include <vector>
#include <string>
#include <tuple>
#include <iostream>
// BOOST_PROTO_MAX_LOGICAL_ARITY
using namespace boost;

namespace test {
  struct Person {
    std::string name;
    int age;
    float height;
  };
}

namespace bun {
  namespace _details {

    using TypesUsed = boost::mpl::vector<decltype(test::Person::name), decltype(test::Person::age), decltype(test::Person::height)>;

    template<std::int32_t I>
    struct QueryVariablePlaceholderIndex : std::integral_constant <std::int32_t, I> {
    };

    struct QueryContext;

    struct QueryTerminals : proto::or_<
      proto::terminal<proto::convertible_to<int>>,
      proto::terminal<proto::convertible_to<unsigned int>>,
      proto::terminal<proto::convertible_to<char>>,
      proto::terminal<proto::convertible_to<unsigned char>>,
      proto::terminal<proto::convertible_to<float>>,
      proto::terminal<proto::convertible_to<double>>,
      proto::terminal<proto::convertible_to<bool>>,
      proto::terminal<proto::convertible_to<std::string>>
    > {
    };

    struct QueryEqualTo : proto::or_<
      proto::equal_to<QueryEqualTo, QueryTerminals>
    > {
    };

    struct QueryGrammar : proto::or_<
      QueryTerminals,
      proto::equal_to<QueryGrammar, QueryGrammar>,
      proto::logical_and<QueryGrammar, QueryGrammar>,
      proto::logical_or<QueryGrammar, QueryGrammar>,
      proto::shift_right<QueryGrammar, QueryGrammar>
    > {
    };

    // Query Context
    struct QueryContext : proto::callable_context<QueryContext const> {
      std::vector<std::string> values;

      struct _Internal {
        static std::string const& mapping( const std::uint32_t in_index ) {
          static const std::vector<std::string> ret = { "name", "age", "height" };
          return ret.at( in_index );
        }
      };

      using result_type = std::string const&;
      // Handle the placeholders:
      template<std::int32_t I>
      result_type operator()( proto::tag::terminal, QueryVariablePlaceholderIndex<I> ) const {
        return _Internal::mapping( I );
      }
    };

    // Forward-declare an expression wrapper
    template<typename Expr>
    struct _Query;

    // Define a Query domain. Expression within
    // the Query domain will be wrapped in the
    // Query<> expression wrapper.
    struct QueryDomain : proto::domain<proto::generator<_Query>, QueryGrammar> {
    };

    // Define a Query expression wrapper. It behaves just like
    // the expression it wraps, but with an extra operator() member
    // function that evaluates the expression.
    template<typename Expr>
    struct _Query/* : proto::extends<Expr, _Query<Expr>, QueryDomain>*/ {
      typedef proto::extends<Expr, _Query<Expr>, QueryDomain> base_type;
      //_Query( Expr const &expr = Expr()) : base_type( expr ) {
      //}
      // Use BOOST_PROTO_EXTENDS() instead of proto::extends<> to
      // make this type a Proto expression extension.
      BOOST_PROTO_EXTENDS( Expr, _Query<Expr>, QueryDomain );
      using result_type = QueryContext;
      // Overload operator() to invoke proto::eval() with
      // our calculator_context.
      result_type operator()( std::string const& in_left, std::string const& in_right ) {
        QueryContext ctx;
        ctx.values.push_back( "" );
        ctx.values.push_back( "" );
        return proto::eval( *this, ctx );;
      }
    };
  }

  namespace _details {
    struct _Internal {
      static std::string const& mapping( const std::uint32_t in_index ) {
        static const std::vector<std::string> ret = { "name", "age", "height" };
        return ret.at( in_index );
      }
    };

    struct QueryIntermediateResults {
      std::vector<std::string> values;
      std::vector<std::int32_t> commands;
    };

    struct QueryLiteralTerminals : proto::or_<
      proto::terminal<proto::convertible_to<int>>,
      proto::terminal<proto::convertible_to<unsigned int>>,
      proto::terminal<proto::convertible_to<char>>,
      proto::terminal<proto::convertible_to<unsigned char>>,
      proto::terminal<proto::convertible_to<float>>,
      proto::terminal<proto::convertible_to<double>>,
      proto::terminal<proto::convertible_to<bool>>,
      proto::terminal<proto::convertible_to<std::string>>
    > {
    };

    struct PlaceHoldersTerminals : proto::or_<
      QueryLiteralTerminals,
      proto::terminal<QueryVariablePlaceholderIndex<0>>,
      proto::terminal<QueryVariablePlaceholderIndex<1>>,
      proto::terminal<QueryVariablePlaceholderIndex<2>>
    > {
    };

    struct GreaterSymbols : proto::or_<
      PlaceHoldersTerminals,
      proto::greater<proto::terminal<QueryVariablePlaceholderIndex<0>>, QueryLiteralTerminals>,
      proto::greater_equal<proto::terminal<QueryVariablePlaceholderIndex<0>>, QueryLiteralTerminals>,
      proto::greater<proto::terminal<QueryVariablePlaceholderIndex<1>>, QueryLiteralTerminals>,
      proto::greater_equal<proto::terminal<QueryVariablePlaceholderIndex<1>>, QueryLiteralTerminals>,
      proto::greater<proto::terminal<QueryVariablePlaceholderIndex<2>>, QueryLiteralTerminals>,
      proto::greater_equal<proto::terminal<QueryVariablePlaceholderIndex<2>>, QueryLiteralTerminals>
    > {
    };

    struct LessSymbols : proto::or_<
      GreaterSymbols,
      proto::less<proto::terminal<QueryVariablePlaceholderIndex<0>>, QueryLiteralTerminals>,
      proto::less_equal<proto::terminal<QueryVariablePlaceholderIndex<0>>, QueryLiteralTerminals>,
      proto::less<proto::terminal<QueryVariablePlaceholderIndex<1>>, QueryLiteralTerminals>,
      proto::less_equal<proto::terminal<QueryVariablePlaceholderIndex<1>>, QueryLiteralTerminals>,
      proto::less<proto::terminal<QueryVariablePlaceholderIndex<2>>, QueryLiteralTerminals>,
      proto::less_equal<proto::terminal<QueryVariablePlaceholderIndex<2>>, QueryLiteralTerminals>
    > {
    };

    struct EqualSymbols : proto::or_<
      LessSymbols,
      proto::equal_to<proto::terminal<QueryVariablePlaceholderIndex<0>>, QueryLiteralTerminals>,
      proto::equal_to<proto::terminal<QueryVariablePlaceholderIndex<1>>, QueryLiteralTerminals>,
      proto::equal_to<proto::terminal<QueryVariablePlaceholderIndex<2>>, QueryLiteralTerminals>
    > {
    };

    struct BunQueryGrammar : proto::or_<
      EqualSymbols,
      proto::logical_and<BunQueryGrammar, BunQueryGrammar>,
      proto::logical_or<BunQueryGrammar, BunQueryGrammar>
    > {
    };
  }

  namespace {
    struct ObjectFields {
      static proto::terminal<_details::QueryVariablePlaceholderIndex<0> >::type const name;
      //proto::terminal<_details::QueryVariablePlaceholderIndex<1> >::type const age;
      //proto::terminal<_details::QueryVariablePlaceholderIndex<2> >::type const height;
    };
    proto::terminal<_details::QueryVariablePlaceholderIndex<0> >::type const ObjectFields::name = { {} };
  }
}


template<typename Expr>
void matches( Expr const & in_expr ) {
  proto::display_expr( in_expr );
  if (proto::matches<Expr, bun::_details::BunQueryGrammar>::value) {
    std::cout << "Matches" << std::endl;
  }
  else {
    std::cout << "No Matches" << std::endl;
  }
}

std::string const& mapping( const std::uint32_t in_index ) {
  static const std::vector<std::string> ret = { "name", "age", "height" };
  return ret.at( in_index );
}

template<int I>
struct placeholder {
};
proto::terminal<placeholder<0> >::type const name = { {} };

// The calculator_context from the "Hello Calculator" section,
// implemented from scratch.
struct calculator_context {
  // The values with which we'll replace the placeholders
  std::vector<std::string> args;

  template<
    typename Expr
    // defaulted template parameters, so we can
    // specialize on the expressions that need
    // special handling.
    , typename Tag = typename proto::tag_of<Expr>::type
    , typename Arg0 = typename proto::child_c<Expr, 0>::type
  >
    struct eval;

  // Handle placeholder terminals here...
  template<typename Expr, int I>
  struct eval<Expr, proto::tag::terminal, placeholder<I> > {
    typedef std::string result_type;

    result_type operator()( Expr &, calculator_context &ctx ) {
      const std::string ret = std::to_string( I );
      args.push_back( ret );
      return ret;
    }
  };

  // Handle other terminals here...
  //template<typename Expr, typename Arg0>
  //struct eval<Expr, proto::tag::terminal, Arg0> {
  //  typedef std::string result_type;

  //  result_type operator()( Expr &expr, MyContext & ) const {
  //    const std::string ret = std::to_string( proto::child( expr ) );
  //    args.push_back( ret );
  //    return ret;
  //  }
  //};

  // Handle addition here...
  //template<typename Expr, typename Arg0>
  //struct eval<Expr, proto::tag::plus, Arg0> {
  //  typedef std::string result_type;

  //  result_type operator()( Expr &expr, MyContext &ctx ) const {
  //    return proto::eval( proto::left( expr ), ctx )
  //      + proto::eval( proto::right( expr ), ctx );
  //  }
  //};

  // ... other eval<> specializations for other node types ...
};


struct TArrayPrintCtx
  : proto::callable_context< TArrayPrintCtx const > {
  typedef std::ostream &result_type;

  TArrayPrintCtx() {}

  template<int I>
  std::ostream &operator ()( proto::tag::terminal, int I ) const {
    return std::cout << i;
  }

  template<typename L, typename R>
  std::ostream &operator ()( proto::tag::plus, L const &l, R const &r ) const {
    //typedef typename proto::tag_of<Expr>::type tag;
    return std::cout << '(' << typeid(proto::value(l)).name() << " + " << proto::value( r ) << ')' << std::endl;
  }

  template<typename L, typename R>
  std::ostream &operator ()( proto::tag::minus, L const &l, R const &r ) const {
    return std::cout << '(' << l << " - " << r << ')';
  }

  template<typename L, typename R>
  std::ostream &operator ()( proto::tag::multiplies, L const &l, R const &r ) const {
    return std::cout << l << " * " << r;
  }

  template<typename L, typename R>
  std::ostream &operator ()( proto::tag::divides, L const &l, R const &r ) const {
    return std::cout << l << " / " << r;
  }
};


int main() {
  auto p = proto::as_expr( std::string( "10" ) );
  TArrayPrintCtx ctx;
  proto::eval( name + 1, ctx );
  matches( name + 1 );
  return 0;
}
