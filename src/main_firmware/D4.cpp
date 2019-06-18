#include "D4.h"

void D4::transform( float32_t* a, const int n )
{
  if (n >= 4) {
     int i, j;
     const int half = n >> 1;
     
     float32_t* tmp = new float32_t[n];

     for (i = 0, j = 0; j < n-3; j += 2, i++) {
        tmp[i]      = a[j]*h0 + a[j+1]*h1 + a[j+2]*h2 + a[j+3]*h3;
        tmp[i+half] = a[j]*g0 + a[j+1]*g1 + a[j+2]*g2 + a[j+3]*g3;
     }

     tmp[i]      = a[n-2]*h0 + a[n-1]*h1 + a[0]*h2 + a[1]*h3;
     tmp[i+half] = a[n-2]*g0 + a[n-1]*g1 + a[0]*g2 + a[1]*g3;

     for (i = 0; i < n; i++) {
        a[i] = tmp[i];
     }
     delete [] tmp;
  }
}

/**
 Inverse Daubechies D4 transform
*/
void D4::invTransform( float32_t* a, const int n )
{
 if (n >= 4) {
   int i, j;
   const int half = n >> 1;
   const int halfPls1 = half + 1;

   float32_t* tmp = new float32_t[n];

   //      last smooth val  last coef.  first smooth  first coef
   tmp[0] = a[half-1]*Ih0 + a[n-1]*Ih1 + a[0]*Ih2 + a[half]*Ih3;
   tmp[1] = a[half-1]*Ig0 + a[n-1]*Ig1 + a[0]*Ig2 + a[half]*Ig3;
   for (i = 0, j = 2; i < half-1; i++) {
     //     smooth val     coef. val       smooth val    coef. val
     tmp[j++] = a[i]*Ih0 + a[i+half]*Ih1 + a[i+1]*Ih2 + a[i+halfPls1]*Ih3;
     tmp[j++] = a[i]*Ig0 + a[i+half]*Ig1 + a[i+1]*Ig2 + a[i+halfPls1]*Ig3;
   }
   for (i = 0; i < n; i++) {
     a[i] = tmp[i];
   }
   delete [] tmp;
 }
}

D4::D4() 
{
  const float32_t sqrt_3 = 1.73205080757;
  const float32_t denom = 4 * 1.41421356237;

  //
  // forward transform scaling (smoothing) coefficients
  //
  h0 = (1 + sqrt_3)/denom;
  h1 = (3 + sqrt_3)/denom;
  h2 = (3 - sqrt_3)/denom;
  h3 = (1 - sqrt_3)/denom;
  //
  // forward transform wavelet coefficients
  //
  g0 =  h3;
  g1 = -h2;
  g2 =  h1;
  g3 = -h0;

  Ih0 = h2;
  Ih1 = g2;  // h1
  Ih2 = h0;
  Ih3 = g0;  // h3

  Ig0 = h3;
  Ig1 = g3;  // -h0
  Ig2 = h1;
  Ig3 = g1;  // -h2
}

void D4::daubTrans( float32_t* ts, int N )
{
  int n;
  for (n = N; n >= 4; n >>= 1) {
     transform( ts, n );
  }
}


void D4::invDaubTrans( float32_t* coef, int N )
{
  int n;
  for (n = 4; n <= N; n <<= 1) {
     invTransform( coef, n );
  }
}