#ifndef PARISH_MATH
#define PARISH_MATH

#include "shared_code.h" 

//=================================
// ROUNDING, TRUNCATING
//=================================
inline int32_t RoundFloatToInt32(float f)
{
	int32_t result = (int32_t)(f + 0.5f);
	return result;
}

inline uint32_t RoundFloatToUInt32(float f)
{
	uint32_t result = (uint32_t)(f + 0.5f);
	return result;
}

inline int32_t TruncateFloatToInt32(float f)
{
	int32_t result = (int32_t)(f);
	return result;
}

inline uint32_t TruncateFloatToUInt32(float f)
{
	uint32_t result = (uint32_t)(f);
	return result;
}

//=================================
// SQRT CODE -
// https://www.codeproject.com/Articles/570700/SquareplusRootplusalgorithmplusforplusC
//=================================

double powerOfTen(int num)
{
   double rst = 1.0;
   if(num >= 0)
   {
       for(int i = 0; i < num ; i++)
	   {
           rst *= 10.0;
       }
   }
   else
   {
       for(int i = 0; i < (0 - num ); i++)
	   {
           rst *= 0.1;
       }
   }
   return rst;
}

double squareRoot(double a)
{
    double z = a; 
    double rst = 0.0;
    int max = 8;
    int i;
    double j = 1.0;
    for(i = max ; i > 0 ; i--)
	{
        // value must be bigger then 0
        if(z - (( 2 * rst ) + ( j * powerOfTen(i)))*( j * powerOfTen(i)) >= 0)
        {
            while( z - (( 2 * rst ) + ( j * powerOfTen(i)))*( j * powerOfTen(i)) >= 0)
            {
                j++;
                if(j >= 10) break;
            }
            j--;
            z -= (( 2 * rst ) + ( j * powerOfTen(i)))*( j * powerOfTen(i));
            rst += j * powerOfTen(i);
            j = 1.0;
        }
 
    }
 
    for(i = 0 ; i >= 0 - max ; i--)
	{
        if(z - (( 2 * rst ) + ( j * powerOfTen(i)))*( j * powerOfTen(i)) >= 0)
        {
            while( z - (( 2 * rst ) + ( j * powerOfTen(i)))*( j * powerOfTen(i)) >= 0)
            {
              j++;
            }
            j--;
            z -= (( 2 * rst ) + ( j * powerOfTen(i)))*( j * powerOfTen(i));
            rst += j * powerOfTen(i);
            j = 1.0;
        }
    }
    return rst;
}

//=================================
// 2D VECTOR - SINGLE PRECISION
//=================================
typedef struct 
{
	float x;
	float y;
} v2f;

inline v2f V2F(float x, float y)
{
	v2f result;
	result.x = x;
	result.y = y;
	return result;
}

inline float Length(v2f a)
{
	return (float)sqrt((a.x * a.x) + (a.y * a.y));
}

inline float Dot(v2f a, v2f b)
{
	return ((a.x * b.x) + (a.y * b.y));
}

inline float Cross(v2f a, v2f b)
{
	return ((a.x * b.y) - (a.y * b.x));
}

inline v2f Min(v2f a, v2f b)
{
	if (Length(a) < Length(b))
	{
		return a;
	}
	return b;
}
	
inline v2f Max(v2f a, v2f b)
{
	if (Length(a) > Length(b))
	{
		return a;
	}
	return b;
}

inline v2f Normalize(v2f a)
{
	v2f result;
	result.x = 0.0f;
	result.y = 0.0f;

	float length = Length(a);
	if (length != 0.0f)
	{
		result.x = (a.x / length);
		result.y = (a.y / length);
	}
	return result;
}

inline v2f Left(v2f a, bool yIsUp) 
{
	v2f result = Normalize(a);
	if (yIsUp)
	{
		return V2F(-result.y, result.x);
	}
	
	return V2F(result.y, -result.x);
}

inline v2f Right(v2f a, bool yIsUp) 
{
	v2f result = Normalize(a);
	if (yIsUp)
	{
		return V2F(result.y, -result.x);
	}
	return V2F(-result.y, result.x);
}

inline v2f AddV(v2f a, v2f b)
{
	v2f result;
	result.x = a.x + b.x;
	result.y = a.y + b.y;
	return result;
}

inline v2f AddF(v2f a, float b)
{
	v2f result;
	result.x = a.x + b;
	result.y = a.y + b;
	return result;
}

inline v2f SubtractV(v2f a, v2f b)
{
	v2f result;
	result.x = a.x - b.x;
	result.y = a.y - b.y;
	return result;
}

inline v2f SubtractF(v2f a, float b)
{
	v2f result;
	result.x = a.x - b;
	result.y = a.y - b;
	return result;
}

inline v2f MultiplyV(v2f a, v2f b)
{
	v2f result;
	result.x = a.x*b.x;
	result.y = a.y*b.y;
	return result;
}

inline v2f MultiplyF(v2f a, float b)
{
	v2f result;
	result.x = a.x*b;
	result.y = a.y*b;
	return result;
}

inline v2f DivideV(v2f a, v2f b)
{
	v2f result;
	result.x = a.x/b.x;
	result.y = a.y/b.y;
	return result;
}

inline v2f DivideF(v2f a, float b)
{
	v2f result;
	result.x = a.x/b;
	result.y = a.y/b;
	return result;
}

inline v2f Negate(v2f a)
{
	v2f result;
	result.x = -a.x;
	result.y = -a.y;
	return result;
}

inline float Distance(v2f a, v2f b)
{
	v2f result = V2F(b.x - a.x, b.y - a.y);
	float length = Length(result);
	if (length < 0.0f)
	{
		length *= -0.1f;
	}
	return length;
}
	

#endif