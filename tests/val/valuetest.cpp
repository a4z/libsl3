
#include <a4testing.hpp>

#include <sl3/value.hpp>
#include <sl3/error.hpp>

#include <functional>
#include <algorithm>


namespace sl3
{
  namespace
  {


    using VauleRelation = std::function<bool(const Value&, const Value&)>;

    VauleRelation strong_eq = std::equal_to<Value> ;
    VauleRelation strong_lt = std::less<Value> ;

    VauleRelation weak_eq = sl3::weak_eq;
    VauleRelation weak_lt = sl3::weak_lt ;



    bool
    eq_reflexive (const Value& a, VauleRelation equal)
    {
      return equal (a, a);
    }

    bool
    weak_reflexive (const Value& a, const Value& b, VauleRelation weakeq)
    {
      if (a == b)
        return weakeq (a, b);

      BOOST_ERROR(__FUNCTION__ + " wrong input, expected: a == b");
      return false;
    }

    bool
    eq_symmetric (const Value& a, const Value& b, VauleRelation equal)
    {
      if (equal (a, b))
        return equal (b, a);

      BOOST_ERROR(__FUNCTION__ + " wrong input, expected: a == b");
      return false;
    }

    // transitive: a == b && b == c implies a == c
    bool
    eq_transitive (const Value& a, const Value& b, const Value& c,
                   VauleRelation equal)
    {
      if (!equal (a, b))
        {
          BOOST_ERROR(__FUNCTION__ + " wrong input, expected: equal (a, b)");
        }
      if (!equal (b, c))
        {
          BOOST_ERROR(__FUNCTION__ + " wrong input, expected: equal (b, c)");
        }

      return equal (a, c);
    }

    //Transitive law: r(a, b) && r(b, c) implies r(a, c)
    // From that we can easily show that !(r(a, b) && r(b, a))
    bool
    lt_transitive (const Value& a, const Value& b, const Value& c,
                   VauleRelation r)
    {
      if (!r (a, b))
        {
          BOOST_ERROR(__FUNCTION__ + " wrong input, expected: r (a, b)");
        }
      if (!r (b, c))
        {
          BOOST_ERROR(__FUNCTION__ + " wrong input, expected: r (b, c)");
        }

      return r (a, c) && !(r (a, b) && r (b, a));
    }


    //symmetry of eq, eq(a, b) implies eq(b, a) and that implies !r(b, a).
    bool
    irreflexive  (const Value& a,
                  const Value& b,
                  VauleRelation eq,
                  VauleRelation r)
    {
      if (eq_symmetric (a, b, eq))
        {
          BOOST_ERROR(__FUNCTION__ + " , expected eq_symmetric (a,b)");
          return false;
        }

      return !r(a,b) && !r(b,a) ;
    }




    //Trichotomy law: !eq(a, b) implies r(a, b) || r(b, a)

    bool
    trichotomy  (const Value& a,
                  const Value& b,
                  VauleRelation equal,
                  VauleRelation less)
    {
      if (!equal(a, b))
        {
          return less (a,b) || less (b, a) ;
        }

      BOOST_ERROR(__FUNCTION__ + " wrong input, expected: a != b" );
      return false ;
    }



// TODO define a weak order eq und < for Value that ignores the type but
// the weak order eq funciton must be true if == (strict total ordered)


 /*
 regular type


1. Equality: in order to use find and other related algorithms STL requires
operator== to be defined and assumes certain properties of it.
2. Total ordering: sorting and binary searching that allow us to find equal elements
fast and on which sorted associative containers are based require operator<
and assume that it possesses certain properties. Total ordering must be consistent
with equality.
3. Copying and assignment: in order to put things into containers and to move
them with the help of different mutating algorithms STL assumes the existence of
copy constructors, assignment and related operations. They must be consistent
with equality.
  */



/*
strong order

reflexive: a == a
symmetric: a == b implies b == a
transitive: a == b && b == c implies a == c


Irreflexive law: a == b implies !(a < b)
From this law we can easily derive that a == b implies !(b < a).
Indeed, by symmetry of equality a == b implies b == a and that implies that !(b < a).

Transitive law: a < b && b < c implies a < c
From this we can easily derive that !(a < b && b < a).
Indeed, if a < b && b < a then by transitivity a < a and that contradicts the irreflexive law.
In other words, irreflexivity and transitivity imply anti-symmetry.

And, finally Trichotomy law: a != b implies a < b || b < a


 */



/*

for the weak_eq and weak_lt function

(note, add a check for if Type::Real and NaN or Inf, sane rules are paused

Reflexive: a == b implies eq(a, b)
Symmetric: eq(a, b) implies eq(b, a)
Transitive: eq(a, b) && eq(b, c) implies eq(a, c)

Then a relation r(a, b) is a strict weak ordering if it obeys the following laws:
Irreflexive law: eq(a, b) implies !r(a, b)
From that we can easily show that eq(a, b) implies !r(b, a).
Indeed, by symmetry of eq, eq(a, b) implies eq(b, a) and that implies !r(b, a).

Transitive law: r(a, b) && r(b, c) implies r(a, c)
From that we can easily show that !(r(a, b) && r(b, a)).
Indeed, if r(a, b) && r(b, a) then by transitivity r(a, a) and that contradicts the irreflexive law.
As with strict total ordering, irreflexivity and transitivity imply anti-symmetry.

And, finally, Trichotomy law: !eq(a, b) implies r(a, b) || r(b, a)

 */


/*

for total order, std::equal_to and std::less

 */


    // reflexive: a == a
    // symmetric: a == b implies b == a
    // transitive: a == b && b == c implies a == c

//    bool eq_reflexive (const Value& a)
//    {
//      return a == a;
//    }
//
//    bool eq_symmetric (const Value& a, const Value& b)
//    {
//      if( a == b )
//        return b == a ;
//
//
//      BOOST_ERROR(__FUNCTION__ + " wrong input, expected: a == b" );
//      return false ;
//    }
//
//
//    bool eq_transitive (const Value& a, const Value& b, const Value& c)
//    {
//      if( a == b && b == c)
//        return a == c ;
//
//      BOOST_ERROR(__FUNCTION__ + " wrong input, expected: a == b == c" );
//      return false ;
//    }
//
//    bool copyable(const Value& a)
//    {
//      try
//      {
//          Value b{a} ;
//          BOOST_REQUIRE (eq_symmetric (a,b));
//          Value c = a ;
//          BOOST_REQUIRE (eq_symmetric (a,c));
//          BOOST_REQUIRE (eq_symmetric (b,c));
//          BOOST_REQUIRE (eq_transitive (a, b, c)) ;
//      }
//      catch (...)
//      {
//          return false ;
//      }
//
//      return true ;
//    }




    void createValue ()
    {
   
// regular a == b, b != a 

      {
       Value a ;
       Value b (a);
       Value c = b ;

       BOOST_CHECK_EQUAL (a.getType (), Type::Null);
       BOOST_CHECK_EQUAL (b.getType (), Type::Null);
       BOOST_CHECK_EQUAL (c.getType (), Type::Null);
       BOOST_CHECK (c.isNull ()) ; 
      }
      
       {
       Value a{1} ;
       Value b (a);
       Value c = b ;
       Value d = 1 ;
       BOOST_CHECK_EQUAL (a.getType (), Type::Int);

       BOOST_CHECK_EQUAL (b.getType (), Type::Int);
       BOOST_CHECK_EQUAL (c.getType (), Type::Int);
       BOOST_CHECK_EQUAL (d.getType (), Type::Int);
       BOOST_CHECK (!d.isNull ()) ;        
      }
     
      {
       Value a{1.1} ;
       Value b (a);
       Value c = b ;
       Value d = 1.1 ;
       BOOST_CHECK_EQUAL (a.getType (), Type::Real);
       BOOST_CHECK_EQUAL (b.getType (), Type::Real);
       BOOST_CHECK_EQUAL (c.getType (), Type::Real);
       BOOST_CHECK_EQUAL (d.getType (), Type::Real);
       BOOST_CHECK (!d.isNull ()) ;        
      }
  
      {
       Value a{std::string{}} ;
       Value b (a);
       Value c = b ;
       Value d = std::string{} ;
       BOOST_CHECK_EQUAL (a.getType (), Type::Text);
       BOOST_CHECK_EQUAL (b.getType (), Type::Text);
       BOOST_CHECK_EQUAL (c.getType (), Type::Text);
       BOOST_CHECK_EQUAL (d.getType (), Type::Text);
       BOOST_CHECK (!d.isNull ()) ;        
      }
  

       {
       Value a{Blob{}} ;
       Value b (a);
       Value c = b ;
       Value d = Blob{} ;
       BOOST_CHECK_EQUAL (a.getType (), Type::Blob);

       BOOST_CHECK_EQUAL (b.getType (), Type::Blob);
       BOOST_CHECK_EQUAL (c.getType (), Type::Blob);
       BOOST_CHECK_EQUAL (d.getType (), Type::Blob);
       BOOST_CHECK (!d.isNull ()) ;        
      }
    }


    void 
    compareEqual ()
    {

       {
       Value a{1} ;
       Value b (a);
       Value c = 1;

       BOOST_CHECK_EQUAL (a, b) ;
       BOOST_CHECK_EQUAL (a, c) ;

       BOOST_CHECK (a == Value(1.0));
       // this should be the same as SELECT 1 == 1.00000; // and this is true 

       BOOST_CHECK (a != Value(2));
       BOOST_CHECK (a != Value("a"));
       BOOST_CHECK (a != Value(Blob{1,2,3}));

      }

    }

    void 
    compareLess ()
    {

       {
       Value a{1} ;
       Value b (2);
       Value c = 3;


       BOOST_CHECK (a < b);
       BOOST_CHECK (b < c);
       BOOST_CHECK (a < c);

       BOOST_CHECK ( a < Value{1.0});
      
       }

// TODO this is great, put it on a better place

       auto trichotomous = [](const Value& a, const Value& b) -> bool {

         using namespace std;

         auto once = { a<b, a==b, b<a };

         if (cout(begin(once), end(once), true) != 1)
           return false ;

         // Irreflexive
         once = {a==b, a!=b} ;
         if (cout(begin(once), end(once), true) != 1)
           return false ;

         if (a==b)
           {
             // symmetric
             once = {a==b, b==a} ;
             if (cout(begin(once), end(once), true) != 1)
               return false ;

             if (a < b || b < a)
               return false;
             else
               return true;
           }
         else
           {
             // Trichotomy
             once = { a<b, b<a };
             if (cout(begin(once), end(once), true) != 1)
               return false ;
           }

         return true ;
       };



       BOOST_CHECK (trichotomous (Value(1), Value (2))) ;
       BOOST_CHECK (trichotomous (Value(2), Value (2))) ;
       BOOST_CHECK (trichotomous (Value(3), Value (2))) ;
       BOOST_CHECK (trichotomous (Value(1.0), Value (2))) ;
       BOOST_CHECK (trichotomous (Value(2.0), Value (2))) ;
       BOOST_CHECK (trichotomous (Value(2.0), Value (2))) ;
       BOOST_CHECK (trichotomous (Value(2.0), Value (2.0))) ;



//Reflexive: a == a
//Symmetric: eq(a, b) implies eq(b, a)
//Transitive: eq(a, b) && eq(b, c) implies eq(a, c)


//(strict) weak order 	ordered partitions: a<b implies a<c or c<b
//(strict) total order 	trichotomous: exactly one of a<b, a=b, b<a
    }




// Irreflexive law: a == b implies !(a < b)
// Transitive law: a < b && b < c implies a < c
// Trichotomy law: a != b implies a < b || b < a 

    void 
    strictTotalOrdered ()
    {
      
      
    }



    a4TestAdd (a4test::suite ("value")
          .addTest ("create", createValue)
          .addTest ("equal", compareEqual)
          .addTest ("less", compareLess)
          .addTest ("totalOrder", strictTotalOrdered)

                   );
  }
}

