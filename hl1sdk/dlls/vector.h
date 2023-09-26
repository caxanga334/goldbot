/***
*
*       Copyright (c) 1999, Valve LLC. All rights reserved.
*       
*       This product contains software technology licensed from Id 
*       Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*       All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/
#ifndef VECTOR_H
#define VECTOR_H

class Vector;
class Vector2D;

float Vector2DNormalize(Vector2D& v);

//=========================================================
// 2DVector - used for many pathfinding and many other 
// operations that are treated as planar rather than 3d.
//=========================================================
class Vector2D
{
public:
        inline Vector2D(void): x(0.0), y(0.0)                                                   { }
        inline Vector2D(float X, float Y): x(0.0), y(0.0)                               { x = X; y = Y; }
        inline Vector2D operator+(const Vector2D& v)    const   { return Vector2D(x+v.x, y+v.y);        }
        inline Vector2D operator-(const Vector2D& v)    const   { return Vector2D(x-v.x, y-v.y);        }
        inline Vector2D operator*(float fl)                             const   { return Vector2D(x*fl, y*fl);  }
        inline Vector2D operator/(float fl)                             const   { fl=1/fl; return Vector2D(x*fl, y*fl);  }
        Vector2D& operator/=(float s);
        
        inline float Length(void)                                               const   { return (float)sqrt(x*x + y*y );              }

        inline Vector2D Normalize ( void ) const
        {
                // Vector2D vec2;

                float flLen = Length();
                if ( flLen == 0 )
                {
                        return Vector2D( 0, 0 );
                }
                else
                {
                        flLen = 1 / flLen;
                        return Vector2D( x * flLen, y * flLen );
                }
        }

        inline float Dot(const Vector2D& other) { return this->x * other.x + this->y * other.y; }
        inline float NormalizeInPlace() { return Vector2DNormalize(*this); }

        vec_t   x, y;
};

inline float DotProduct(const Vector2D& a, const Vector2D& b) { return( a.x*b.x + a.y*b.y ); }
inline Vector2D operator*(float fl, const Vector2D& v)  { return v * fl; }


inline Vector2D& Vector2D::operator/=(float fl)
{
    float oofl = 1.0f / fl;
    x *= oofl;
    y *= oofl;
    return *this;
}

inline float Vector2DNormalize(Vector2D& v)
{
    float l = v.Length();
    if (l != 0.0f)
    {
        v /= l;
    }
    else
    {
        v.x = v.y = 0.0f;
    }
    return l;
}

//=========================================================
// 3D Vector
//=========================================================
class Vector                                            // same data-layout as engine's vec3_t,
{                                                               //              which is a vec_t[3]
public:
        // Construction/destruction
        inline Vector(void): x(0.0), y(0.0), z(0.0)                                     { }
        inline Vector(float X, float Y, float Z): x(0.0), y(0.0), z(0.0)        { x = X; y = Y; z = Z;                          }
        //inline Vector(double X, double Y, double Z)                   { x = (float)X; y = (float)Y; z = (float)Z;     }
        //inline Vector(int X, int Y, int Z)                            { x = (float)X; y = (float)Y; z = (float)Z;     }
        inline Vector(const Vector& v): x(0.0), y(0.0), z(0.0)  { x = v.x; y = v.y; z = v.z;                            } 
        inline Vector(float rgfl[3]): x(0.0), y(0.0), z(0.0)    { x = rgfl[0]; y = rgfl[1]; z = rgfl[2];        }
        inline Vector(const Vector2D& v): x(0.0), y(0.0), z(0.0){ x = v.x; y = v.y; }

        // Operators
        inline Vector operator-(void) const                     { return Vector(-x,-y,-z);                              }
        inline int operator==(const Vector& v) const            { return fabsf(x - v.x) < 0.0001f && fabsf(y - v.y) < 0.0001f && fabsf(z - v.z) < 0.0001f;    }
        inline int operator!=(const Vector& v) const            { return !(*this==v);                                   }
        inline Vector operator+(const Vector& v) const          { return Vector(x+v.x, y+v.y, z+v.z);   }
        inline Vector operator-(const Vector& v) const          { return Vector(x-v.x, y-v.y, z-v.z);   }
        inline Vector operator*(float fl) const                 { return Vector(x*fl, y*fl, z*fl);              }
        inline Vector operator/(float fl) const                 { fl=1/fl; return Vector(x*fl, y*fl, z*fl);              }
        inline int operator!() const                            { return fabsf(x) < 0.0001f && fabsf(y) < 0.0001f && fabsf(z) < 0.0001f; }
        inline float operator[](int i) const                    { return ((float*)this)[i]; }
        inline float& operator[](int i)                         { return ((float*)this)[i]; }
        
        // Methods
        inline void CopyToArray(float* rgfl) const              { rgfl[0] = x, rgfl[1] = y, rgfl[2] = z; }
        inline float Length(void) const                         { return (float)sqrt(x*x + y*y + z*z); }
        inline operator float *()                               { return &x; } // Vectors will now automatically convert to float * when needed
        inline operator Vector2D () const                       { return (*this).Make2D();      }

#if !defined(__GNUC__) || (__GNUC__ >= 3)
        inline operator const float *() const                   { return &x; } // Vectors will now automatically convert to float * when needed
#endif

        inline float NormalizeInPlace()
        {
            float flLen = Length();
            if (flLen == 0) return 0.0f;
            flLen = 1 / flLen;
            this->x = this->x * flLen;
            this->y = this->y * flLen;
            this->z = this->z * flLen;
            return flLen;
        }

        inline Vector Normalize(void) const
        {
                float flLen = Length();
                if (flLen == 0) return Vector(0,0,1); // ????
                flLen = 1 / flLen;
                return Vector(x * flLen, y * flLen, z * flLen);
        }

        inline Vector2D Make2D ( void ) const
        {
                Vector2D        Vec2;

                Vec2.x = x;
                Vec2.y = y;

                return Vec2;
        }
        inline float Length2D(void) const                       { return (float)sqrt(x*x + y*y); }

        // Get the vector's magnitude squared.
        inline float LengthSqr(void) const
        {
            return (x * x + y * y + z * z);
        }

        inline bool IsLengthLessThan(const float dist) const
        {
            return this->LengthSqr() < (dist * dist);
        }

        inline bool IsLengthGreaterThan(const float dist) const
        {
            return this->LengthSqr() > (dist * dist);
        }

        inline float DistToSqr(const Vector& vOther) const
        {
            Vector delta;

            delta.x = x - vOther.x;
            delta.y = y - vOther.y;
            delta.z = z - vOther.z;

            return delta.LengthSqr();
        }

        inline float DistTo(const Vector& vOther) const
        {
            Vector delta;

            delta.x = x - vOther.x;
            delta.y = y - vOther.y;
            delta.z = z - vOther.z;

            return delta.Length();
        }

        inline float Dot(const Vector& other) const { return (this->x * other.x + this->y * other.y + this->z * other.z); }
        /**
         * @brief Calculates the cross product between vectors
         * @param b Other vector
         * @param result Vector to store the cross product result
        */
        inline void Cross(const Vector& b, Vector& result) const
        {
            result.x = this->y * b.z - this->z * b.y;
            result.y = this->z * b.x - this->x * b.z;
            result.z = this->x * b.y - this->y * b.x;
        }

        // return true if this vector is (0,0,0) within tolerance
        inline bool IsZero(float tolerance = 0.01f) const
        {
            return (x > -tolerance && x < tolerance &&
                y > -tolerance && y < tolerance &&
                z > -tolerance && z < tolerance);
        }

        inline bool WithinAABox(const Vector& boxmin, const Vector& boxmax) const
        {
            return (
                (x >= boxmin.x) && (x <= boxmax.x) &&
                (y >= boxmin.y) && (y <= boxmax.y) &&
                (z >= boxmin.z) && (z <= boxmax.z)
                );
        }

        // Members
        vec_t x, y, z;
};
inline Vector operator*(float fl, const Vector& v)      { return v * fl; }
inline float DotProduct(const Vector& a, const Vector& b) { return(a.x*b.x+a.y*b.y+a.z*b.z); }
inline Vector CrossProduct(const Vector& a, const Vector& b) { return Vector( a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x ); }



#endif
