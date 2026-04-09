#include <CL/opencl.h>

#define OPENCL_API(ret, func, args_with_types, args) \
	static ret syz_##func args_with_types        \
	{                                            \
		return func args;                    \
	}

#define EMPTY_OPENCL_API(ret, func, args_with_types, args) \
	static void syz_##func(void)                       \
	{                                                  \
		return;                                    \
	}

static void loop();

#if SYZ_EXECUTOR || __NR_syz_clCreateContext
OPENCL_API(cl_context, clCreateContext, (const cl_context_properties* properties, cl_uint num_devices, const cl_device_id* devices, void(CL_CALLBACK* pfn_notify)(const char* errinfo, const void* private_info, size_t cb, void* user_data), void* user_data, cl_int* errcode_ret), (properties, num_devices, devices, pfn_notify, user_data, errcode_ret))
#endif

#if SYZ_EXECUTOR || __NR_syz_clCreateContextFromType
OPENCL_API(cl_context, clCreateContextFromType, (const cl_context_properties* properties, cl_device_type device_type, void(CL_CALLBACK* pfn_notify)(const char* errinfo, const void* private_info, size_t cb, void* user_data), void* user_data, cl_int* errcode_ret), (properties, device_type, pfn_notify, user_data, errcode_ret))
#endif

#if SYZ_EXECUTOR || __NR_syz_clRetainContext
OPENCL_API(cl_int, clRetainContext, (cl_context context), (context))
#endif

#if SYZ_EXECUTOR || __NR_syz_clReleaseContext
OPENCL_API(cl_int, clReleaseContext, (cl_context context), (context))
#endif

#if SYZ_EXECUTOR || __NR_syz_clGetContextInfo
OPENCL_API(cl_int, clGetContextInfo, (cl_context context, cl_context_info param_name, size_t param_value_size, void* param_value, size_t* param_value_size_ret), (context, param_name, param_value_size, param_value, param_value_size_ret))
#endif

#if SYZ_EXECUTOR_NVIDIA || __NR_syz_clSetContextDestructorCallback
OPENCL_API(cl_int, clSetContextDestructorCallback, (cl_context context, void(CL_CALLBACK* pfn_notify)(cl_context context, void* user_data), void* user_data), (context, pfn_notify, user_data))
#elif SYZ_EXECUTOR
EMPTY_OPENCL_API(cl_int, clSetContextDestructorCallback, (cl_context context, void(CL_CALLBACK* pfn_notify)(cl_context context, void* user_data), void* user_data), (context, pfn_notify, user_data))
#endif

#if SYZ_EXECUTOR || __NR_syz_clCreateCommandQueueWithProperties
OPENCL_API(cl_command_queue, clCreateCommandQueueWithProperties, (cl_context context, cl_device_id device, const cl_queue_properties* properties, cl_int* errcode_ret), (context, device, properties, errcode_ret))
#endif

#if SYZ_EXECUTOR || __NR_syz_clRetainCommandQueue
OPENCL_API(cl_int, clRetainCommandQueue, (cl_command_queue command_queue), (command_queue))
#endif

#if SYZ_EXECUTOR || __NR_syz_clReleaseCommandQueue
OPENCL_API(cl_int, clReleaseCommandQueue, (cl_command_queue command_queue), (command_queue))
#endif

#if SYZ_EXECUTOR || __NR_syz_clGetCommandQueueInfo
OPENCL_API(cl_int, clGetCommandQueueInfo, (cl_command_queue command_queue, cl_command_queue_info param_name, size_t param_value_size, void* param_value, size_t* param_value_size_ret), (command_queue, param_name, param_value_size, param_value, param_value_size_ret))
#endif

#if SYZ_EXECUTOR || __NR_syz_clGetEventProfilingInfo
OPENCL_API(cl_int, clGetEventProfilingInfo, (cl_event event, cl_profiling_info param_name, size_t param_value_size, void* param_value, size_t* param_value_size_ret), (event, param_name, param_value_size, param_value, param_value_size_ret))
#endif

#if SYZ_EXECUTOR || __NR_syz_clFlush
OPENCL_API(cl_int, clFlush, (cl_command_queue command_queue), (command_queue))
#endif

#if SYZ_EXECUTOR || __NR_syz_clFinish
OPENCL_API(cl_int, clFinish, (cl_command_queue command_queue), (command_queue))
#endif

#if SYZ_EXECUTOR || __NR_syz_clEnqueueReadBuffer
OPENCL_API(cl_int, clEnqueueReadBuffer, (cl_command_queue command_queue, cl_mem buffer, cl_bool blocking_read, size_t offset, size_t size, void* ptr, cl_uint num_events_in_wait_list, const cl_event* event_wait_list, cl_event* event), (command_queue, buffer, blocking_read, offset, size, ptr, num_events_in_wait_list, event_wait_list, event))
#endif

#if SYZ_EXECUTOR || __NR_syz_clEnqueueReadBufferRect
OPENCL_API(cl_int, clEnqueueReadBufferRect, (cl_command_queue command_queue, cl_mem buffer, cl_bool blocking_read, const size_t* buffer_origin, const size_t* host_origin, const size_t* region, size_t buffer_row_pitch, size_t buffer_slice_pitch, size_t host_row_pitch, size_t host_slice_pitch, void* ptr, cl_uint num_events_in_wait_list, const cl_event* event_wait_list, cl_event* event), (command_queue, buffer, blocking_read, buffer_origin, host_origin, region, buffer_row_pitch, buffer_slice_pitch, host_row_pitch, host_slice_pitch, ptr, num_events_in_wait_list, event_wait_list, event))
#endif

#if SYZ_EXECUTOR || __NR_syz_clEnqueueWriteBuffer
OPENCL_API(cl_int, clEnqueueWriteBuffer, (cl_command_queue command_queue, cl_mem buffer, cl_bool blocking_write, size_t offset, size_t size, const void* ptr, cl_uint num_events_in_wait_list, const cl_event* event_wait_list, cl_event* event), (command_queue, buffer, blocking_write, offset, size, ptr, num_events_in_wait_list, event_wait_list, event))
#endif

#if SYZ_EXECUTOR || __NR_syz_clEnqueueWriteBufferRect
OPENCL_API(cl_int, clEnqueueWriteBufferRect, (cl_command_queue command_queue, cl_mem buffer, cl_bool blocking_write, const size_t* buffer_origin, const size_t* host_origin, const size_t* region, size_t buffer_row_pitch, size_t buffer_slice_pitch, size_t host_row_pitch, size_t host_slice_pitch, const void* ptr, cl_uint num_events_in_wait_list, const cl_event* event_wait_list, cl_event* event), (command_queue, buffer, blocking_write, buffer_origin, host_origin, region, buffer_row_pitch, buffer_slice_pitch, host_row_pitch, host_slice_pitch, ptr, num_events_in_wait_list, event_wait_list, event))
#endif

#if SYZ_EXECUTOR || __NR_syz_clEnqueueFillBuffer
OPENCL_API(cl_int, clEnqueueFillBuffer, (cl_command_queue command_queue, cl_mem buffer, const void* pattern, size_t pattern_size, size_t offset, size_t size, cl_uint num_events_in_wait_list, const cl_event* event_wait_list, cl_event* event), (command_queue, buffer, pattern, pattern_size, offset, size, num_events_in_wait_list, event_wait_list, event))
#endif

#if SYZ_EXECUTOR || __NR_syz_clEnqueueCopyBuffer
OPENCL_API(cl_int, clEnqueueCopyBuffer, (cl_command_queue command_queue, cl_mem src_buffer, cl_mem dst_buffer, size_t src_offset, size_t dst_offset, size_t size, cl_uint num_events_in_wait_list, const cl_event* event_wait_list, cl_event* event), (command_queue, src_buffer, dst_buffer, src_offset, dst_offset, size, num_events_in_wait_list, event_wait_list, event))
#endif

#if SYZ_EXECUTOR || __NR_syz_clEnqueueCopyBufferRect
OPENCL_API(cl_int, clEnqueueCopyBufferRect, (cl_command_queue command_queue, cl_mem src_buffer, cl_mem dst_buffer, const size_t* src_origin, const size_t* dst_origin, const size_t* region, size_t src_row_pitch, size_t src_slice_pitch, size_t dst_row_pitch, size_t dst_slice_pitch, cl_uint num_events_in_wait_list, const cl_event* event_wait_list, cl_event* event), (command_queue, src_buffer, dst_buffer, src_origin, dst_origin, region, src_row_pitch, src_slice_pitch, dst_row_pitch, dst_slice_pitch, num_events_in_wait_list, event_wait_list, event))
#endif

#if SYZ_EXECUTOR || __NR_syz_clEnqueueReadImage
OPENCL_API(cl_int, clEnqueueReadImage, (cl_command_queue command_queue, cl_mem image, cl_bool blocking_read, const size_t* origin, const size_t* region, size_t row_pitch, size_t slice_pitch, void* ptr, cl_uint num_events_in_wait_list, const cl_event* event_wait_list, cl_event* event), (command_queue, image, blocking_read, origin, region, row_pitch, slice_pitch, ptr, num_events_in_wait_list, event_wait_list, event))
#endif

#if SYZ_EXECUTOR || __NR_syz_clEnqueueWriteImage
OPENCL_API(cl_int, clEnqueueWriteImage, (cl_command_queue command_queue, cl_mem image, cl_bool blocking_write, const size_t* origin, const size_t* region, size_t input_row_pitch, size_t input_slice_pitch, const void* ptr, cl_uint num_events_in_wait_list, const cl_event* event_wait_list, cl_event* event), (command_queue, image, blocking_write, origin, region, input_row_pitch, input_slice_pitch, ptr, num_events_in_wait_list, event_wait_list, event))
#endif

#if SYZ_EXECUTOR || __NR_syz_clEnqueueFillImage
OPENCL_API(cl_int, clEnqueueFillImage, (cl_command_queue command_queue, cl_mem image, const void* fill_color, const size_t* origin, const size_t* region, cl_uint num_events_in_wait_list, const cl_event* event_wait_list, cl_event* event), (command_queue, image, fill_color, origin, region, num_events_in_wait_list, event_wait_list, event))
#endif

#if SYZ_EXECUTOR || __NR_syz_clEnqueueCopyImage
OPENCL_API(cl_int, clEnqueueCopyImage, (cl_command_queue command_queue, cl_mem src_image, cl_mem dst_image, const size_t* src_origin, const size_t* dst_origin, const size_t* region, cl_uint num_events_in_wait_list, const cl_event* event_wait_list, cl_event* event), (command_queue, src_image, dst_image, src_origin, dst_origin, region, num_events_in_wait_list, event_wait_list, event))
#endif

#if SYZ_EXECUTOR || __NR_syz_clEnqueueCopyImageToBuffer
OPENCL_API(cl_int, clEnqueueCopyImageToBuffer, (cl_command_queue command_queue, cl_mem src_image, cl_mem dst_buffer, const size_t* src_origin, const size_t* region, size_t dst_offset, cl_uint num_events_in_wait_list, const cl_event* event_wait_list, cl_event* event), (command_queue, src_image, dst_buffer, src_origin, region, dst_offset, num_events_in_wait_list, event_wait_list, event))
#endif

#if SYZ_EXECUTOR || __NR_syz_clEnqueueCopyBufferToImage
OPENCL_API(cl_int, clEnqueueCopyBufferToImage, (cl_command_queue command_queue, cl_mem src_buffer, cl_mem dst_image, size_t src_offset, const size_t* dst_origin, const size_t* region, cl_uint num_events_in_wait_list, const cl_event* event_wait_list, cl_event* event), (command_queue, src_buffer, dst_image, src_offset, dst_origin, region, num_events_in_wait_list, event_wait_list, event))
#endif

#if SYZ_EXECUTOR || __NR_syz_clEnqueueMapBuffer
OPENCL_API(void*, clEnqueueMapBuffer, (cl_command_queue command_queue, cl_mem buffer, cl_bool blocking_map, cl_map_flags map_flags, size_t offset, size_t size, cl_uint num_events_in_wait_list, const cl_event* event_wait_list, cl_event* event, cl_int* errcode_ret), (command_queue, buffer, blocking_map, map_flags, offset, size, num_events_in_wait_list, event_wait_list, event, errcode_ret))
#endif

#if SYZ_EXECUTOR || __NR_syz_clEnqueueMapImage
OPENCL_API(void*, clEnqueueMapImage, (cl_command_queue command_queue, cl_mem image, cl_bool blocking_map, cl_map_flags map_flags, const size_t* origin, const size_t* region, size_t* image_row_pitch, size_t* image_slice_pitch, cl_uint num_events_in_wait_list, const cl_event* event_wait_list, cl_event* event, cl_int* errcode_ret), (command_queue, image, blocking_map, map_flags, origin, region, image_row_pitch, image_slice_pitch, num_events_in_wait_list, event_wait_list, event, errcode_ret))
#endif

#if SYZ_EXECUTOR || __NR_syz_clEnqueueUnmapMemObject
OPENCL_API(cl_int, clEnqueueUnmapMemObject, (cl_command_queue command_queue, cl_mem memobj, void* mapped_ptr, cl_uint num_events_in_wait_list, const cl_event* event_wait_list, cl_event* event), (command_queue, memobj, mapped_ptr, num_events_in_wait_list, event_wait_list, event))
#endif

#if SYZ_EXECUTOR || __NR_syz_clEnqueueMigrateMemObjects
OPENCL_API(cl_int, clEnqueueMigrateMemObjects, (cl_command_queue command_queue, cl_uint num_mem_objects, const cl_mem* mem_objects, cl_mem_migration_flags flags, cl_uint num_events_in_wait_list, const cl_event* event_wait_list, cl_event* event), (command_queue, num_mem_objects, mem_objects, flags, num_events_in_wait_list, event_wait_list, event))
#endif

#if SYZ_EXECUTOR || __NR_syz_clEnqueueNDRangeKernel
OPENCL_API(cl_int, clEnqueueNDRangeKernel, (cl_command_queue command_queue, cl_kernel kernel, cl_uint work_dim, const size_t* global_work_offset, const size_t* global_work_size, const size_t* local_work_size, cl_uint num_events_in_wait_list, const cl_event* event_wait_list, cl_event* event), (command_queue, kernel, work_dim, global_work_offset, global_work_size, local_work_size, num_events_in_wait_list, event_wait_list, event))
#endif

#if SYZ_EXECUTOR || __NR_syz_clEnqueueNativeKernel
OPENCL_API(cl_int, clEnqueueNativeKernel, (cl_command_queue command_queue, void(CL_CALLBACK* user_func)(void*), void* args, size_t cb_args, cl_uint num_mem_objects, const cl_mem* mem_list, const void** args_mem_loc, cl_uint num_events_in_wait_list, const cl_event* event_wait_list, cl_event* event), (command_queue, user_func, args, cb_args, num_mem_objects, mem_list, args_mem_loc, num_events_in_wait_list, event_wait_list, event))
#endif

#if SYZ_EXECUTOR || __NR_syz_clEnqueueMarkerWithWaitList
OPENCL_API(cl_int, clEnqueueMarkerWithWaitList, (cl_command_queue command_queue, cl_uint num_events_in_wait_list, const cl_event* event_wait_list, cl_event* event), (command_queue, num_events_in_wait_list, event_wait_list, event))
#endif

#if SYZ_EXECUTOR || __NR_syz_clEnqueueBarrierWithWaitList
OPENCL_API(cl_int, clEnqueueBarrierWithWaitList, (cl_command_queue command_queue, cl_uint num_events_in_wait_list, const cl_event* event_wait_list, cl_event* event), (command_queue, num_events_in_wait_list, event_wait_list, event))
#endif

#if SYZ_EXECUTOR || __NR_syz_clEnqueueSVMFree
OPENCL_API(cl_int, clEnqueueSVMFree, (cl_command_queue command_queue, cl_uint num_svm_pointers, void* svm_pointers[], void(CL_CALLBACK* pfn_free_func)(cl_command_queue queue, cl_uint num_svm_pointers, void* svm_pointers[], void* user_data), void* user_data, cl_uint num_events_in_wait_list, const cl_event* event_wait_list, cl_event* event), (command_queue, num_svm_pointers, svm_pointers, pfn_free_func, user_data, num_events_in_wait_list, event_wait_list, event))
#endif

#if SYZ_EXECUTOR || __NR_syz_clEnqueueSVMMemcpy
OPENCL_API(cl_int, clEnqueueSVMMemcpy, (cl_command_queue command_queue, cl_bool blocking_copy, void* dst_ptr, const void* src_ptr, size_t size, cl_uint num_events_in_wait_list, const cl_event* event_wait_list, cl_event* event), (command_queue, blocking_copy, dst_ptr, src_ptr, size, num_events_in_wait_list, event_wait_list, event))
#endif

#if SYZ_EXECUTOR || __NR_syz_clEnqueueSVMMemFill
OPENCL_API(cl_int, clEnqueueSVMMemFill, (cl_command_queue command_queue, void* svm_ptr, const void* pattern, size_t pattern_size, size_t size, cl_uint num_events_in_wait_list, const cl_event* event_wait_list, cl_event* event), (command_queue, svm_ptr, pattern, pattern_size, size, num_events_in_wait_list, event_wait_list, event))
#endif

#if SYZ_EXECUTOR || __NR_syz_clEnqueueSVMMap
OPENCL_API(cl_int, clEnqueueSVMMap, (cl_command_queue command_queue, cl_bool blocking_map, cl_map_flags flags, void* svm_ptr, size_t size, cl_uint num_events_in_wait_list, const cl_event* event_wait_list, cl_event* event), (command_queue, blocking_map, flags, svm_ptr, size, num_events_in_wait_list, event_wait_list, event))
#endif

#if SYZ_EXECUTOR || __NR_syz_clEnqueueSVMUnmap
OPENCL_API(cl_int, clEnqueueSVMUnmap, (cl_command_queue command_queue, void* svm_ptr, cl_uint num_events_in_wait_list, const cl_event* event_wait_list, cl_event* event), (command_queue, svm_ptr, num_events_in_wait_list, event_wait_list, event))
#endif

#if SYZ_EXECUTOR || __NR_syz_clEnqueueSVMMigrateMem
OPENCL_API(cl_int, clEnqueueSVMMigrateMem, (cl_command_queue command_queue, cl_uint num_svm_pointers, const void** svm_pointers, const size_t* sizes, cl_mem_migration_flags flags, cl_uint num_events_in_wait_list, const cl_event* event_wait_list, cl_event* event), (command_queue, num_svm_pointers, svm_pointers, sizes, flags, num_events_in_wait_list, event_wait_list, event))
#endif

#if SYZ_EXECUTOR || __NR_syz_clCreateUserEvent
OPENCL_API(cl_event, clCreateUserEvent, (cl_context context, cl_int* errcode_ret), (context, errcode_ret))
#endif

#if SYZ_EXECUTOR || __NR_syz_clGetEventInfo
OPENCL_API(cl_int, clGetEventInfo, (cl_event event, cl_event_info param_name, size_t param_value_size, void* param_value, size_t* param_value_size_ret), (event, param_name, param_value_size, param_value, param_value_size_ret))
#endif

#if SYZ_EXECUTOR || __NR_syz_clReleaseEvent
OPENCL_API(cl_int, clReleaseEvent, (cl_event event), (event))
#endif

#if SYZ_EXECUTOR || __NR_syz_clRetainEvent
OPENCL_API(cl_int, clRetainEvent, (cl_event event), (event))
#endif

#if SYZ_EXECUTOR || __NR_syz_clSetEventCallback
OPENCL_API(cl_int, clSetEventCallback, (cl_event event, cl_int command_exec_callback_type, void(CL_CALLBACK* pfn_notify)(cl_event event, cl_int event_command_status, void* user_data), void* user_data), (event, command_exec_callback_type, pfn_notify, user_data))
#endif

#if SYZ_EXECUTOR || __NR_syz_clSetUserEventStatus
OPENCL_API(cl_int, clSetUserEventStatus, (cl_event event, cl_int execution_status), (event, execution_status))
#endif

#if SYZ_EXECUTOR || __NR_syz_clWaitForEvents
OPENCL_API(cl_int, clWaitForEvents, (cl_uint num_events, const cl_event* event_list), (num_events, event_list))
#endif

#if SYZ_EXECUTOR || __NR_syz_clCreateKernel
OPENCL_API(cl_kernel, clCreateKernel, (cl_program program, const char* kernel_name, cl_int* errcode_ret), (program, kernel_name, errcode_ret))
#endif

#if SYZ_EXECUTOR || __NR_syz_clCreateKernelsInProgram
OPENCL_API(cl_int, clCreateKernelsInProgram, (cl_program program, cl_uint num_kernels, cl_kernel* kernels, cl_uint* num_kernels_ret), (program, num_kernels, kernels, num_kernels_ret))
#endif

#if SYZ_EXECUTOR || __NR_syz_clCloneKernel
OPENCL_API(cl_kernel, clCloneKernel, (cl_kernel source_kernel, cl_int* errcode_ret), (source_kernel, errcode_ret))
#endif

#if SYZ_EXECUTOR || __NR_syz_clRetainKernel
OPENCL_API(cl_int, clRetainKernel, (cl_kernel kernel), (kernel))
#endif

#if SYZ_EXECUTOR || __NR_syz_clReleaseKernel
OPENCL_API(cl_int, clReleaseKernel, (cl_kernel kernel), (kernel))
#endif

#if SYZ_EXECUTOR || __NR_syz_clSetKernelArg
OPENCL_API(cl_int, clSetKernelArg, (cl_kernel kernel, cl_uint arg_index, size_t arg_size, const void* arg_value), (kernel, arg_index, arg_size, arg_value))
#endif

#if SYZ_EXECUTOR || __NR_syz_clSetKernelArgSVMPointer
OPENCL_API(cl_int, clSetKernelArgSVMPointer, (cl_kernel kernel, cl_uint arg_index, const void* arg_value), (kernel, arg_index, arg_value))
#endif

#if SYZ_EXECUTOR || __NR_syz_clSetKernelExecInfo
OPENCL_API(cl_int, clSetKernelExecInfo, (cl_kernel kernel, cl_kernel_exec_info param_name, size_t param_value_size, const void* param_value), (kernel, param_name, param_value_size, param_value))
#endif

#if SYZ_EXECUTOR || __NR_syz_clGetKernelInfo
OPENCL_API(cl_int, clGetKernelInfo, (cl_kernel kernel, cl_kernel_info param_name, size_t param_value_size, void* param_value, size_t* param_value_size_ret), (kernel, param_name, param_value_size, param_value, param_value_size_ret))
#endif

#if SYZ_EXECUTOR || __NR_syz_clGetKernelArgInfo
OPENCL_API(cl_int, clGetKernelArgInfo, (cl_kernel kernel, cl_uint arg_indx, cl_kernel_arg_info param_name, size_t param_value_size, void* param_value, size_t* param_value_size_ret), (kernel, arg_indx, param_name, param_value_size, param_value, param_value_size_ret))
#endif

#if SYZ_EXECUTOR || __NR_syz_clGetKernelWorkGroupInfo
OPENCL_API(cl_int, clGetKernelWorkGroupInfo, (cl_kernel kernel, cl_device_id device, cl_kernel_work_group_info param_name, size_t param_value_size, void* param_value, size_t* param_value_size_ret), (kernel, device, param_name, param_value_size, param_value, param_value_size_ret))
#endif

#if SYZ_EXECUTOR || __NR_syz_clGetKernelSubGroupInfo
OPENCL_API(cl_int, clGetKernelSubGroupInfo, (cl_kernel kernel, cl_device_id device, cl_kernel_sub_group_info param_name, size_t input_value_size, const void* input_value, size_t param_value_size, void* param_value, size_t* param_value_size_ret), (kernel, device, param_name, input_value_size, input_value, param_value_size, param_value, param_value_size_ret))
#endif

#if SYZ_EXECUTOR || __NR_syz_clCreateBuffer
OPENCL_API(cl_mem, clCreateBuffer, (cl_context context, cl_mem_flags flags, size_t size, void* host_ptr, cl_int* errcode_ret), (context, flags, size, host_ptr, errcode_ret))
#endif

#if SYZ_EXECUTOR || __NR_syz_clCreateSubBuffer
OPENCL_API(cl_mem, clCreateSubBuffer, (cl_mem buffer, cl_mem_flags flags, cl_buffer_create_type buffer_create_type, const void* buffer_create_info, cl_int* errcode_ret), (buffer, flags, buffer_create_type, buffer_create_info, errcode_ret))
#endif

#if SYZ_EXECUTOR || __NR_syz_clCreateImage
OPENCL_API(cl_mem, clCreateImage, (cl_context context, cl_mem_flags flags, const cl_image_format* image_format, const cl_image_desc* image_desc, void* host_ptr, cl_int* errcode_ret), (context, flags, image_format, image_desc, host_ptr, errcode_ret))
#endif

#if SYZ_EXECUTOR || __NR_syz_clCreatePipe
OPENCL_API(cl_mem, clCreatePipe, (cl_context context, cl_mem_flags flags, cl_uint pipe_packet_size, cl_uint pipe_max_packets, const cl_pipe_properties* properties, cl_int* errcode_ret), (context, flags, pipe_packet_size, pipe_max_packets, properties, errcode_ret))
#endif

#if SYZ_EXECUTOR_NVIDIA || __NR_syz_clCreateBufferWithProperties
OPENCL_API(cl_mem, clCreateBufferWithProperties, (cl_context context, const cl_mem_properties* properties, cl_mem_flags flags, size_t size, void* host_ptr, cl_int* errcode_ret), (context, properties, flags, size, host_ptr, errcode_ret))
#elif SYZ_EXECUTOR
EMPTY_OPENCL_API(cl_mem, clCreateBufferWithProperties, (cl_context context, const cl_mem_properties* properties, cl_mem_flags flags, size_t size, void* host_ptr, cl_int* errcode_ret), (context, properties, flags, size, host_ptr, errcode_ret))
#endif

#if SYZ_EXECUTOR_NVIDIA || __NR_syz_clCreateImageWithProperties
OPENCL_API(cl_mem, clCreateImageWithProperties, (cl_context context, const cl_mem_properties* properties, cl_mem_flags flags, const cl_image_format* image_format, const cl_image_desc* image_desc, void* host_ptr, cl_int* errcode_ret), (context, properties, flags, image_format, image_desc, host_ptr, errcode_ret))
#elif SYZ_EXECUTOR
EMPTY_OPENCL_API(cl_mem, clCreateImageWithProperties, (cl_context context, const cl_mem_properties* properties, cl_mem_flags flags, const cl_image_format* image_format, const cl_image_desc* image_desc, void* host_ptr, cl_int* errcode_ret), (context, properties, flags, image_format, image_desc, host_ptr, errcode_ret))
#endif

#if SYZ_EXECUTOR || __NR_syz_clRetainMemObject
OPENCL_API(cl_int, clRetainMemObject, (cl_mem memobj), (memobj))
#endif

#if SYZ_EXECUTOR || __NR_syz_clReleaseMemObject
OPENCL_API(cl_int, clReleaseMemObject, (cl_mem memobj), (memobj))
#endif

#if SYZ_EXECUTOR || __NR_syz_clGetSupportedImageFormats
OPENCL_API(cl_int, clGetSupportedImageFormats, (cl_context context, cl_mem_flags flags, cl_mem_object_type image_type, cl_uint num_entries, cl_image_format* image_formats, cl_uint* num_image_formats), (context, flags, image_type, num_entries, image_formats, num_image_formats))
#endif

#if SYZ_EXECUTOR || __NR_syz_clGetMemObjectInfo
OPENCL_API(cl_int, clGetMemObjectInfo, (cl_mem memobj, cl_mem_info param_name, size_t param_value_size, void* param_value, size_t* param_value_size_ret), (memobj, param_name, param_value_size, param_value, param_value_size_ret))
#endif

#if SYZ_EXECUTOR || __NR_syz_clGetImageInfo
OPENCL_API(cl_int, clGetImageInfo, (cl_mem image, cl_image_info param_name, size_t param_value_size, void* param_value, size_t* param_value_size_ret), (image, param_name, param_value_size, param_value, param_value_size_ret))
#endif

#if SYZ_EXECUTOR || __NR_syz_clGetPipeInfo
OPENCL_API(cl_int, clGetPipeInfo, (cl_mem pipe, cl_pipe_info param_name, size_t param_value_size, void* param_value, size_t* param_value_size_ret), (pipe, param_name, param_value_size, param_value, param_value_size_ret))
#endif

#if SYZ_EXECUTOR || __NR_syz_clSetMemObjectDestructorCallback
OPENCL_API(cl_int, clSetMemObjectDestructorCallback, (cl_mem memobj, void(CL_CALLBACK* pfn_notify)(cl_mem memobj, void* user_data), void* user_data), (memobj, pfn_notify, user_data))
#endif

#if SYZ_EXECUTOR || __NR_syz_clGetPlatformIDs
OPENCL_API(cl_int, clGetPlatformIDs, (cl_uint num_entries, cl_platform_id* platforms, cl_uint* num_platforms), (num_entries, platforms, num_platforms))
#endif

#if SYZ_EXECUTOR || __NR_syz_clGetPlatformInfo
OPENCL_API(cl_int, clGetPlatformInfo, (cl_platform_id platform, cl_platform_info param_name, size_t param_value_size, void* param_value, size_t* param_value_size_ret), (platform, param_name, param_value_size, param_value, param_value_size_ret))
#endif

#if SYZ_EXECUTOR || __NR_syz_clGetDeviceIDs
OPENCL_API(cl_int, clGetDeviceIDs, (cl_platform_id platform, cl_device_type device_type, cl_uint num_entries, cl_device_id* devices, cl_uint* num_devices), (platform, device_type, num_entries, devices, num_devices))
#endif

#if SYZ_EXECUTOR || __NR_syz_clGetDeviceInfo
OPENCL_API(cl_int, clGetDeviceInfo, (cl_device_id device, cl_device_info param_name, size_t param_value_size, void* param_value, size_t* param_value_size_ret), (device, param_name, param_value_size, param_value, param_value_size_ret))
#endif

#if SYZ_EXECUTOR || __NR_syz_clCreateSubDevices
OPENCL_API(cl_int, clCreateSubDevices, (cl_device_id in_device, const cl_device_partition_property* properties, cl_uint num_devices, cl_device_id* out_devices, cl_uint* num_devices_ret), (in_device, properties, num_devices, out_devices, num_devices_ret))
#endif

#if SYZ_EXECUTOR || __NR_syz_clRetainDevice
OPENCL_API(cl_int, clRetainDevice, (cl_device_id device), (device))
#endif

#if SYZ_EXECUTOR || __NR_syz_clReleaseDevice
OPENCL_API(cl_int, clReleaseDevice, (cl_device_id device), (device))
#endif

#if SYZ_EXECUTOR || __NR_syz_clSetDefaultDeviceCommandQueue
OPENCL_API(cl_int, clSetDefaultDeviceCommandQueue, (cl_context context, cl_device_id device, cl_command_queue command_queue), (context, device, command_queue))
#endif

#if SYZ_EXECUTOR || __NR_syz_clGetDeviceAndHostTimer
OPENCL_API(cl_int, clGetDeviceAndHostTimer, (cl_device_id device, cl_ulong* device_timestamp, cl_ulong* host_timestamp), (device, device_timestamp, host_timestamp))
#endif

#if SYZ_EXECUTOR || __NR_syz_clGetHostTimer
OPENCL_API(cl_int, clGetHostTimer, (cl_device_id device, cl_ulong* host_timestamp), (device, host_timestamp))
#endif

#if SYZ_EXECUTOR || __NR_syz_clCreateProgramWithSource
OPENCL_API(cl_program, clCreateProgramWithSource, (cl_context context, cl_uint count, const char** strings, const size_t* lengths, cl_int* errcode_ret), (context, count, strings, lengths, errcode_ret))
#endif

#if SYZ_EXECUTOR || __NR_syz_clCreateProgramWithBinary
OPENCL_API(cl_program, clCreateProgramWithBinary, (cl_context context, cl_uint num_devices, const cl_device_id* device_list, const size_t* lengths, const unsigned char** binaries, cl_int* binary_status, cl_int* errcode_ret), (context, num_devices, device_list, lengths, binaries, binary_status, errcode_ret))
#endif

#if SYZ_EXECUTOR || __NR_syz_clCreateProgramWithBuiltInKernels
OPENCL_API(cl_program, clCreateProgramWithBuiltInKernels, (cl_context context, cl_uint num_devices, const cl_device_id* device_list, const char* kernel_names, cl_int* errcode_ret), (context, num_devices, device_list, kernel_names, errcode_ret))
#endif

#if SYZ_EXECUTOR || __NR_syz_clCreateProgramWithIL
OPENCL_API(cl_program, clCreateProgramWithIL, (cl_context context, const void* il, size_t length, cl_int* errcode_ret), (context, il, length, errcode_ret))
#endif

#if SYZ_EXECUTOR || __NR_syz_clRetainProgram
OPENCL_API(cl_int, clRetainProgram, (cl_program program), (program))
#endif

#if SYZ_EXECUTOR || __NR_syz_clReleaseProgram
OPENCL_API(cl_int, clReleaseProgram, (cl_program program), (program))
#endif

#if SYZ_EXECUTOR || __NR_syz_clBuildProgram
OPENCL_API(cl_int, clBuildProgram, (cl_program program, cl_uint num_devices, const cl_device_id* device_list, const char* options, void(CL_CALLBACK* pfn_notify)(cl_program program, void* user_data), void* user_data), (program, num_devices, device_list, options, pfn_notify, user_data))
#endif

#if SYZ_EXECUTOR || __NR_syz_clCompileProgram
OPENCL_API(cl_int, clCompileProgram, (cl_program program, cl_uint num_devices, const cl_device_id* device_list, const char* options, cl_uint num_input_headers, const cl_program* input_headers, const char** header_include_names, void(CL_CALLBACK* pfn_notify)(cl_program program, void* user_data), void* user_data), (program, num_devices, device_list, options, num_input_headers, input_headers, header_include_names, pfn_notify, user_data))
#endif

#if SYZ_EXECUTOR || __NR_syz_clLinkProgram
OPENCL_API(cl_program, clLinkProgram, (cl_context context, cl_uint num_devices, const cl_device_id* device_list, const char* options, cl_uint num_input_programs, const cl_program* input_programs, void(CL_CALLBACK* pfn_notify)(cl_program program, void* user_data), void* user_data, cl_int* errcode_ret), (context, num_devices, device_list, options, num_input_programs, input_programs, pfn_notify, user_data, errcode_ret))
#endif

#if SYZ_EXECUTOR || __NR_syz_clSetProgramSpecializationConstant
OPENCL_API(cl_int, clSetProgramSpecializationConstant, (cl_program program, cl_uint spec_id, size_t spec_size, const void* spec_value), (program, spec_id, spec_size, spec_value))
#endif

#if SYZ_EXECUTOR || __NR_syz_clUnloadPlatformCompiler
OPENCL_API(cl_int, clUnloadPlatformCompiler, (cl_platform_id platform), (platform))
#endif

#if SYZ_EXECUTOR || __NR_syz_clGetProgramInfo
OPENCL_API(cl_int, clGetProgramInfo, (cl_program program, cl_program_info param_name, size_t param_value_size, void* param_value, size_t* param_value_size_ret), (program, param_name, param_value_size, param_value, param_value_size_ret))
#endif

#if SYZ_EXECUTOR || __NR_syz_clGetProgramBuildInfo
OPENCL_API(cl_int, clGetProgramBuildInfo, (cl_program program, cl_device_id device, cl_program_build_info param_name, size_t param_value_size, void* param_value, size_t* param_value_size_ret), (program, device, param_name, param_value_size, param_value, param_value_size_ret))
#endif

#if SYZ_EXECUTOR || __NR_syz_clSVMAlloc
OPENCL_API(void*, clSVMAlloc, (cl_context context, cl_svm_mem_flags flags, size_t size, cl_uint alignment), (context, flags, size, alignment))
#endif

#if SYZ_EXECUTOR || __NR_syz_clSVMFree
OPENCL_API(void, clSVMFree, (cl_context context, void* svm_pointer), (context, svm_pointer))
#endif

#if SYZ_EXECUTOR || __NR_syz_clCreateSamplerWithProperties
OPENCL_API(cl_sampler, clCreateSamplerWithProperties, (cl_context context, const cl_sampler_properties* sampler_properties, cl_int* errcode_ret), (context, sampler_properties, errcode_ret))
#endif

#if SYZ_EXECUTOR || __NR_syz_clRetainSampler
OPENCL_API(cl_int, clRetainSampler, (cl_sampler sampler), (sampler))
#endif

#if SYZ_EXECUTOR || __NR_syz_clReleaseSampler
OPENCL_API(cl_int, clReleaseSampler, (cl_sampler sampler), (sampler))
#endif

#if SYZ_EXECUTOR || __NR_syz_clGetSamplerInfo
OPENCL_API(cl_int, clGetSamplerInfo, (cl_sampler sampler, cl_sampler_info param_name, size_t param_value_size, void* param_value, size_t* param_value_size_ret), (sampler, param_name, param_value_size, param_value, param_value_size_ret))
#endif