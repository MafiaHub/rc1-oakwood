#pragma once
// t is a value that goes from 0 to 1 to interpolate in a C1 continuous way across uniformly sampled data points.
// when t is 0, this will return B.  When t is 1, this will return C.
inline f32 cubic_hermite_f32(float A, float B, float C, float D, float t)
{
    float a = -A / 2.0f + (3.0f*B) / 2.0f - (3.0f*C) / 2.0f + D / 2.0f;
    float b = A - (5.0f*B) / 2.0f + 2.0f*C - D / 2.0f;
    float c = -A / 2.0f + C / 2.0f;
    float d = B;

    return a*t*t*t + b*t*t + c*t + d;
}

struct interpolate3_hermite_t {
	zpl_vec3 A;
	zpl_vec3 B;
    zpl_vec3 C;
    zpl_vec3 D;
};

inline void cubic_hermite_v3_value(interpolate3_hermite_t *src, zpl_vec3 value) {
    src->A = src->B;
    src->B = src->C;
    src->C = src->D;
    src->D = value;

    if (src->C.x == 0.0f  && 
		src->C.y == 0.0f  && 
		src->C.z == 0.0f) {
        src->A = src->D;
        src->B = src->D;
        src->C = src->D;
    }
}

inline zpl_vec3 cubic_hermite_v3(zpl_vec3 A, zpl_vec3 B, zpl_vec3 C, zpl_vec3 D, f32 t) {
	return {
		cubic_hermite_f32(A.x, B.x, C.x, D.x, t),
		cubic_hermite_f32(A.y, B.y, C.y, D.y, t),
		cubic_hermite_f32(A.z, B.z, C.z, D.z, t)
	};
}

inline zpl_vec3 cubic_hermite_v3_interpolate(interpolate3_hermite_t *inter, f32 t) {
    return cubic_hermite_v3(inter->A, inter->B, inter->C, inter->D, t);
}
