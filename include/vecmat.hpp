#pragma once

#include "vecmat.h"

static inline vms_vector operator+(const vms_vector& a, const vms_vector& b)
{
	vms_vector r;
	vm_vec_add(&r, &a, &b);
	return r;
}

static inline vms_vector operator-(const vms_vector& a, const vms_vector& b)
{
	vms_vector r;
	vm_vec_sub(&r, &a, &b);
	return r;
}

static inline vms_vector operator*(const vms_vector& a, const fix& b)
{
	vms_vector r;
	vm_vec_copy_scale(&r, &a, b);
	return r;
}
