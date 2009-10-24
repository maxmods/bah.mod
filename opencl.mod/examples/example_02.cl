
//--------------------------------------------------------------
__kernel void add(__global float *a, __global float *b, __global float *result) {
	int gid = get_global_id(0);
	result[gid] = a[gid] + b[gid];
}

//--------------------------------------------------------------
__kernel void subtract(__global float *a, __global float *b, __global float *result) {
	int gid = get_global_id(0);
	result[gid] = a[gid] - b[gid];
}

//--------------------------------------------------------------
__kernel void multiply(__global float *a, __global float *b, __global float *result) {
	int gid = get_global_id(0);
	result[gid] = a[gid] * b[gid];
}

//--------------------------------------------------------------
__kernel void divide(__global float *a, __global float *b, __global float *result) {
	int gid = get_global_id(0);
	result[gid] = a[gid] / b[gid];
}


//--------------------------------------------------------------
__kernel void inverseSquareroot(__global float *a, __global float *result) {
	int gid = get_global_id(0);
	result[gid] = 1.0f/sqrt(a[gid]);
}

//--------------------------------------------------------------
__kernel void inverse(__global float *a, __global float *result) {
	int gid = get_global_id(0);
	result[gid] = 1.0f/a[gid];
}

//--------------------------------------------------------------
__kernel void square(__global float *a, __global float *result) {
	int gid = get_global_id(0);
	result[gid] = a[gid] * a[gid];
}



