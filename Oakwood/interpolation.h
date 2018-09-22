#pragma once
constexpr float DELTA_TIME = 0.25f;
class Interpolator
{
public:
	auto init(zplm_vec3 target) {
		target_val = target;
		last_value = target;
	}

	/* feeds the interpolator */
	auto set(zplm_vec3 target) -> void {
		target_val = target;
	}

	/* do the interpolation */
	auto interpolate() -> zplm_vec3 {
		zplm_vec3 dest;
		zplm_vec3_lerp(&dest, last_value, target_val, DELTA_TIME);
		last_value = dest;
		return dest;
	}
private:
	zplm_vec3 last_value;
	zplm_vec3 target_val;
};