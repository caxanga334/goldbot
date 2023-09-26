#ifndef MATH_VECTORS_H_
#define MATH_VECTORS_H_

// Vector stuff imported from the Source Engine

#define M_PI_F		((float)(M_PI))	// Shouldn't collide with anything.

#ifndef RAD2DEG
#define RAD2DEG( x  )  ( (float)(x) * (float)(180.f / M_PI_F) )
#endif

#ifndef DEG2RAD
#define DEG2RAD( x  )  ( (float)(x) * (float)(M_PI_F / 180.f) )
#endif

// MOVEMENT INFO
enum
{
	PITCH = 0,	// up / down
	YAW,		// left / right
	ROLL		// fall over
};

inline float anglemod(float a)
{
	a = (360.f / 65536) * ((int)(a * (65536.f / 360.0f)) & 65535);
	return a;
}

inline float Approach(float target, float value, float speed)
{
	float delta = target - value;

	if (delta > speed)
		value += speed;
	else if (delta < -speed)
		value -= speed;
	else
		value = target;

	return value;
}

// BUGBUG: Why doesn't this call angle diff?!?!?
inline float ApproachAngle(float target, float value, float speed)
{
	target = anglemod(target);
	value = anglemod(value);

	float delta = target - value;

	// Speed is assumed to be positive
	if (speed < 0)
		speed = -speed;

	if (delta < -180)
		delta += 360;
	else if (delta > 180)
		delta -= 360;

	if (delta > speed)
		value += speed;
	else if (delta < -speed)
		value -= speed;
	else
		value = target;

	return value;
}


// BUGBUG: Why do we need both of these?
inline float AngleDiff(float destAngle, float srcAngle)
{
	float delta;

	delta = fmodf(destAngle - srcAngle, 360.0f);
	if (destAngle > srcAngle)
	{
		if (delta >= 180)
			delta -= 360;
	}
	else
	{
		if (delta <= -180)
			delta += 360;
	}
	return delta;
}

inline float AngleDistance(float next, float cur)
{
	float delta = next - cur;

	if (delta < -180)
		delta += 360;
	else if (delta > 180)
		delta -= 360;

	return delta;
}


inline float AngleNormalize(float angle)
{
	angle = fmodf(angle, 360.0f);
	if (angle > 180)
	{
		angle -= 360;
	}
	if (angle < -180)
	{
		angle += 360;
	}
	return angle;
}

//--------------------------------------------------------------------------------------------------------------
// ensure that 0 <= angle <= 360
inline float AngleNormalizePositive(float angle)
{
	angle = fmodf(angle, 360.0f);

	if (angle < 0.0f)
	{
		angle += 360.0f;
	}

	return angle;
}

//--------------------------------------------------------------------------------------------------------------
inline bool AnglesAreEqual(float a, float b, float tolerance)
{
	return (fabs(AngleDiff(a, b)) < tolerance);
}

inline bool VectorsAreEqual(const Vector* a, const Vector* b, float tolerance = 0.1f)
{
	if (abs(a->x - b->x) < tolerance &&
		abs(a->y - b->y) < tolerance &&
		abs(a->z - b->z) < tolerance)
		return true;

	return false;
}

inline void SinCos(float radians, float* sine, float* cosine)
{
	*sine = sin(radians);
	*cosine = cos(radians);
}

//-----------------------------------------------------------------------------
// Euler QAngle -> Basis Vectors
//-----------------------------------------------------------------------------
inline void AngleVectors(const Vector& angles, Vector* forward)
{
	float sp, sy, cp, cy;

	SinCos(DEG2RAD(angles[YAW]), &sy, &cy);
	SinCos(DEG2RAD(angles[PITCH]), &sp, &cp);

	forward->x = cp * cy;
	forward->y = cp * sy;
	forward->z = -sp;
}

//-----------------------------------------------------------------------------
// Euler Angle -> Basis Vectors.  Each vector is optional
//-----------------------------------------------------------------------------
inline void AngleVectors(const Vector& angles, Vector* forward, Vector* right, Vector* up)
{

	float sr, sp, sy, cr, cp, cy;

	SinCos(DEG2RAD(angles[YAW]), &sy, &cy);
	SinCos(DEG2RAD(angles[PITCH]), &sp, &cp);
	SinCos(DEG2RAD(angles[ROLL]), &sr, &cr);

	if (forward)
	{
		forward->x = cp * cy;
		forward->y = cp * sy;
		forward->z = -sp;
	}

	if (right)
	{
		right->x = (-1 * sr * sp * cy + -1 * cr * -sy);
		right->y = (-1 * sr * sp * sy + -1 * cr * cy);
		right->z = -1 * sr * cp;
	}

	if (up)
	{
		up->x = (cr * sp * cy + -sr * -sy);
		up->y = (cr * sp * sy + -sr * cy);
		up->z = cr * cp;
	}
}

//-----------------------------------------------------------------------------
// Forward direction vector -> Euler angles
//-----------------------------------------------------------------------------
inline void VectorAngles(const Vector& forward, Vector& angles)
{
	float	tmp, yaw, pitch;

	if (forward[1] == 0 && forward[0] == 0)
	{
		yaw = 0;
		if (forward[2] > 0)
			pitch = 270;
		else
			pitch = 90;
	}
	else
	{
		yaw = (atan2(forward[1], forward[0]) * 180 / M_PI);
		if (yaw < 0)
			yaw += 360;

		tmp = sqrt(forward[0] * forward[0] + forward[1] * forward[1]);
		pitch = (atan2(-forward[2], tmp) * 180 / M_PI);
		if (pitch < 0)
			pitch += 360;
	}

	angles.x = pitch;
	angles.y = yaw;
	angles.z = 0.0f;
}

// check if a point is in the field of a view of an object.
inline bool PointWithinViewAngle(Vector const& vecSrcPosition, Vector const& vecTargetPosition, Vector const& vecLookDirection, float flCosHalfFOV)
{
	Vector vecDelta = vecTargetPosition - vecSrcPosition;
	float cosDiff = vecLookDirection.Dot(vecDelta);

	if (flCosHalfFOV <= 0) // >180
	{
		// signs are different, answer is implicit
		if (cosDiff > 0)
			return true;

		// a/sqrt(b) > c  == a^2 < b * c ^2
		// IFF left and right sides are <= 0
		float flLen2 = vecDelta.LengthSqr();
		return (cosDiff * cosDiff <= flLen2 * flCosHalfFOV * flCosHalfFOV);
	}
	else // flCosHalfFOV > 0
	{
		// signs are different, answer is implicit
		if (cosDiff < 0)
			return false;

		// a/sqrt(b) > c  == a^2 > b * c ^2
		// IFF left and right sides are >= 0
		float flLen2 = vecDelta.LengthSqr();
		return (cosDiff * cosDiff >= flLen2 * flCosHalfFOV * flCosHalfFOV);
	}
}

//=========================================================
// FInViewCone - returns true is the passed ent is in
// the caller's forward view cone. The dot product is performed
// in 2d, making the view cone infinitely tall.
//=========================================================
inline bool FInViewCone(edict_t* pEntity, edict_t* pTarget, const float fov)
{
	Vector2D vec2LOS;
	float flDot;

	Vector forward;
	AngleVectors(pEntity->v.v_angle, &forward, nullptr, nullptr);

	vec2LOS = (pTarget->v.origin - pEntity->v.origin).Make2D();
	vec2LOS = vec2LOS.Normalize();

	flDot = DotProduct(vec2LOS, forward.Make2D());

	if (flDot > fov)
	{
		return true;
	}
	else
	{
		return false;
	}
}

//=========================================================
// FInViewCone - returns true is the passed vector is in
// the caller's forward view cone. The dot product is performed
// in 2d, making the view cone infinitely tall.
//=========================================================
inline bool FInViewCone(edict_t* pEntity, const Vector& target, const float fov)
{
	Vector2D vec2LOS;
	float flDot;

	Vector forward;
	AngleVectors(pEntity->v.v_angle, &forward, nullptr, nullptr);

	vec2LOS = (target - pEntity->v.origin).Make2D();
	vec2LOS = vec2LOS.Normalize();

	flDot = DotProduct(vec2LOS, forward.Make2D());

	if (flDot > fov)
	{
		return true;
	}
	else
	{
		return false;
	}
}

//--------------------------------------------------------------------------------------------------------------
/**
 * Given two line segments: startA to endA, and startB to endB, return true if they intesect
 * and put the intersection point in "result".
 * Note that this computes the intersection of the 2D (x,y) projection of the line segments.
 */
inline bool IsIntersecting2D(const Vector& startA, const Vector& endA,
	const Vector& startB, const Vector& endB,
	Vector* result = nullptr)
{
	float denom = (endA.x - startA.x) * (endB.y - startB.y) - (endA.y - startA.y) * (endB.x - startB.x);
	if (denom == 0.0f)
	{
		// parallel
		return false;
	}

	float numS = (startA.y - startB.y) * (endB.x - startB.x) - (startA.x - startB.x) * (endB.y - startB.y);
	if (numS == 0.0f)
	{
		// coincident
		return true;
	}

	float numT = (startA.y - startB.y) * (endA.x - startA.x) - (startA.x - startB.x) * (endA.y - startA.y);

	float s = numS / denom;
	if (s < 0.0f || s > 1.0f)
	{
		// intersection is not within line segment of startA to endA
		return false;
	}

	float t = numT / denom;
	if (t < 0.0f || t > 1.0f)
	{
		// intersection is not within line segment of startB to endB
		return false;
	}

	// compute intesection point
	if (result)
		*result = startA + s * (endA - startA);

	return true;
}

extern Vector vec3_origin;

#endif // !MATH_VECTORS_H_

