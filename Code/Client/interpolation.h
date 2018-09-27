#pragma once
constexpr float DELTA_TIME = 0.25f;
class Interpolator
{
public:
	auto init(zpl_vec3 target) {
		target_val = target;
		last_value = target;
	}

	/* feeds the interpolator */
	auto set(zpl_vec3 target) -> void {
		target_val = target;
	}

	/* do the interpolation */
	auto interpolate() -> zpl_vec3 {
		zpl_vec3 dest;
		zpl_vec3_lerp(&dest, last_value, target_val, DELTA_TIME);
		last_value = dest;
		return dest;
	}
private:
	zpl_vec3 last_value;
	zpl_vec3 target_val;
};