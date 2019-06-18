//Adapted from Ian Kaplan's code at http://bearcave.com/misl/misl_tech/wavelets/daubechies/index.html


#include "nrf.h"

#include "math.h"
#include "arm_math.h"

class D4 {
   private:
   /** forward transform scaling coefficients */
   float32_t h0, h1, h2, h3;
   /** forward transform wave coefficients */
   float32_t g0, g1, g2, g3;

   float32_t Ih0, Ih1, Ih2, Ih3;
   float32_t Ig0, Ig1, Ig2, Ig3;

   /**
     Forward Daubechies D4 transform
    */
   void transform( float32_t* a, const int n );

   /**
     Inverse Daubechies D4 transform
    */
   void invTransform( float32_t* a, const int n );

public:

   D4();

   void daubTrans( float32_t* ts, int N );

   void invDaubTrans( float32_t* coef, int N );

};