#pragma once
#ifndef LIB_INTER
#define LIB_INTER

#ifdef __cplusplus
extern "C" {
#endif

/////////////////////////////////////////////////////
//
// ENUMS & STRUCTS
//
/////////////////////////////////////////////////////

typedef struct lib_inter {
	zpl_vec3 start;
	zpl_vec3 target;
	zpl_vec3 error;
	f64 start_time;
	f64 finish_time;
	f32 last_alpha;
	f32 interp_time;
	b32 apply_error;
	f32 error_treshold; // defaults to 0.1f
};

#ifdef __cplusplus
}
#endif

/////////////////////////////////////////////////////
//
// DECL
//
/////////////////////////////////////////////////////

lib_inter*		            lib_inter_create_iterpolator(f32);
void						lib_inter_destroy_interpolator(lib_inter*);

zpl_vec3					lib_inter_interpolate(lib_inter*, zpl_vec3 current);
void					    lib_inter_set_target(lib_inter*, zpl_vec3 current, zpl_vec3 target);
void                        lib_inter_reset(lib_inter*, zpl_vec3 current);

/////////////////////////////////////////////////////
//
// IMPLEMENTATION
//
/////////////////////////////////////////////////////

#ifdef LIB_INTER_IMPLEMENTATION	

lib_inter* lib_inter_create_interpolator(f32 interp_time, b32 apply_error) {
    lib_inter* new_interpolator = (lib_inter*)zpl_malloc(sizeof(lib_inter));
    zpl_zero_item(new_interpolator);
    new_interpolator->interp_time = interp_time;
    new_interpolator->apply_error = apply_error;
    return new_interpolator;
}

void lib_inter_destroy_interpolator(lib_inter* interpolator) {
	if (interpolator) {
		zpl_free(zpl_heap(), interpolator);
		interpolator = NULL;
	}
}

zpl_vec3 lib_inter_interpolate(lib_inter* interp, zpl_vec3 current_val) {
	if (interp->finish_time > 0.0f) {
		f64 current_time = zpl_time_now();
		f32 alpha = zpl_unlerp(current_time, interp->start_time, interp->finish_time);
		alpha = zpl_clamp(alpha, 0.0f, 1.0f);

		f32 current_alpha = alpha - interp->last_alpha;
		interp->last_alpha = alpha;

		zpl_vec3 compensation;
		zpl_vec3_lerp(&compensation, zpl_vec3f_zero(), interp->error, current_alpha);

		if (alpha == 1.0f) {
			interp->finish_time = 0.0f;
		}

		zpl_vec3 compensated;
		zpl_vec3_add(&compensated, current_val, compensation);
		return compensated;
	}

    return current_val;
}

void lib_inter_set_target(lib_inter* interp, zpl_vec3 current_val, zpl_vec3 target_val) {
	interp->start   = current_val;
	interp->target  = target_val;

	zpl_vec3 sub_error;
	zpl_vec3_sub(&sub_error, target_val, current_val);
	interp->error = sub_error;

	if (interp->apply_error) {
		f32 error_mag = zpl_vec3_mag(interp->error);
		if (error_mag > interp->error_treshold) {
			interp->start = target_val;
		}

		// Apply the error over 250ms (i.e. 2/5 per 100ms )
		zpl_vec3_mul(&interp->error, interp->error, zpl_lerp(0.25f, 1.0f, zpl_clamp01(zpl_unlerp(interp->interp_time, 0.1f, 0.4f))));
	}
	
	interp->start_time  = zpl_time_now();
	interp->finish_time = interp->start_time + interp->interp_time;
	interp->last_alpha  = 0.0f;
}

void lib_inter_reset(lib_inter* interp, zpl_vec3 current) {
    interp->start       = current;
    interp->target      = current;
    interp->last_alpha  = 0.0f;
    interp->finish_time = 0.0f;
}

#endif

#endif
