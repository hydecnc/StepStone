#if SYZ_EXECUTOR_NVIDIA
#include <cuda.h>

#define CUDA_API(func, args_with_types, args)      \
	static CUresult syz_##func args_with_types \
	{                                          \
		return func args;                  \
	}
#else

#define CUDA_API(func, args_with_types, args) \
	static void syz_##func(void)          \
	{                                     \
		return;                       \
	}
#endif

#if SYZ_EXECUTOR_NVIDIA
static const char* ptxSource = R"(
.version 6.5
.target sm_30
.address_size 64

.visible .entry vectorAdd(
    .param .u64 param_A,
    .param .u64 param_B,
    .param .u64 param_C,
    .param .u32 param_N
) {
    .reg .u32 t0, t1, t2;
    .reg .u64 ptrA, ptrB, ptrC;

    ld.param.u64 ptrA, [param_A];
    ld.param.u64 ptrB, [param_B];
    ld.param.u64 ptrC, [param_C];
    ld.param.u32 t0, [param_N];

    mov.u32 t1, %tid.x;
    mov.u32 t2, %ntid.x;

    setp.ge.u32 t0, t1, t0;
    @t0 bra EXIT;

    ld.global.f32 t0, [ptrA + t1 * 4];
    ld.global.f32 t1, [ptrB + t1 * 4];
    add.f32 t0, t0, t1;
    st.global.f32 [ptrC + t1 * 4], t0;

EXIT:
    ret;
}
)";
#endif

static void loop();

static void setup_nvidia_driver()
{
#if SYZ_EXECUTOR
	if (!flag_nvidia)
		return;
#endif

#if SYZ_EXECUTOR_NVIDIA
	int ret = cuInit(0);
	if (ret != 0)
		debug("[GPU Fuzzing] ========> Initialize nvidia driver -> %d\n", ret);
#endif
	return;
}

#if SYZ_EXECUTOR || __NR_syz_cuDeviceGet
CUDA_API(cuDeviceGet, (CUdevice * device, int ordinal), (device, ordinal))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuDeviceGetAttribute
CUDA_API(cuDeviceGetAttribute, (int* pi, CUdevice_attribute attrib, int dev), (pi, attrib, dev))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuDeviceGetCount
CUDA_API(cuDeviceGetCount, (int* count), (count))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuDeviceGetDefaultMemPool
CUDA_API(cuDeviceGetDefaultMemPool, (CUmemoryPool * pool_out, int dev), (pool_out, dev))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuDeviceGetExecAffinitySupport
CUDA_API(cuDeviceGetExecAffinitySupport, (int* pi, CUexecAffinityType type, int dev), (pi, type, dev))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuDeviceGetLuid
CUDA_API(cuDeviceGetLuid, (char* luid, unsigned int* deviceNodeMask, int dev), (luid, deviceNodeMask, dev))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuDeviceGetMemPool
CUDA_API(cuDeviceGetMemPool, (CUmemoryPool * pool, int dev), (pool, dev))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuDeviceGetName
CUDA_API(cuDeviceGetName, (char* name, int len, int dev), (name, len, dev))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuDeviceGetNvSciSyncAttributes
CUDA_API(cuDeviceGetNvSciSyncAttributes, (void* nvSciSyncAttrList, int dev, int flags), (nvSciSyncAttrList, dev, flags))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuDeviceGetTexture1DLinearMaxWidth
CUDA_API(cuDeviceGetTexture1DLinearMaxWidth, (size_t * maxWidthInElements, CUarray_format format, unsigned numChannels, int dev), (maxWidthInElements, format, numChannels, dev))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuDeviceGetUuid
CUDA_API(cuDeviceGetUuid, (CUuuid * uuid, int dev), (uuid, dev))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuDeviceGetUuid_v2
CUDA_API(cuDeviceGetUuid_v2, (CUuuid * uuid, int dev), (uuid, dev))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuDeviceSetMemPool
CUDA_API(cuDeviceSetMemPool, (int dev, CUmemoryPool pool), (dev, pool))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuDeviceTotalMem
CUDA_API(cuDeviceTotalMem, (size_t * bytes, int dev), (bytes, dev))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuFlushGPUDirectRDMAWrites
CUDA_API(cuFlushGPUDirectRDMAWrites, (CUflushGPUDirectRDMAWritesTarget target, CUflushGPUDirectRDMAWritesScope scope), (target, scope))
#endif

// Primary context management
#if SYZ_EXECUTOR || __NR_syz_cuDevicePrimaryCtxSetFlags
CUDA_API(cuDevicePrimaryCtxSetFlags, (int dev, unsigned int flag), (dev, flag))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuDevicePrimaryCtxRetain
CUDA_API(cuDevicePrimaryCtxRetain, (CUcontext * pctx, int dev), (pctx, dev))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuDevicePrimaryCtxReset
CUDA_API(cuDevicePrimaryCtxReset, (int dev), (dev))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuDevicePrimaryCtxRelease
CUDA_API(cuDevicePrimaryCtxRelease, (int dev), (dev))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuDevicePrimaryCtxGetState
CUDA_API(cuDevicePrimaryCtxGetState, (int dev, unsigned int* flag, int* active), (dev, flag, active))
#endif

// C Code Definitions for CUDA APIs

// Context Management
#if SYZ_EXECUTOR || __NR_syz_cuCtxCreate
CUDA_API(cuCtxCreate, (CUcontext * pctx, unsigned int flags, int dev), (pctx, flags, dev))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuCtxCreate_v3
CUDA_API(cuCtxCreate_v3, (CUcontext * pctx, CUexecAffinityParam* paramsArray, int numParams, unsigned int flags, int dev), (pctx, paramsArray, numParams, flags, dev))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuCtxDestroy
CUDA_API(cuCtxDestroy, (CUcontext ctx), (ctx))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuCtxGetApiVersion
CUDA_API(cuCtxGetApiVersion, (CUcontext ctx, unsigned int* version), (ctx, version))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuCtxGetCacheConfig
CUDA_API(cuCtxGetCacheConfig, (CUfunc_cache * pconfig), (pconfig))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuCtxGetCurrent
CUDA_API(cuCtxGetCurrent, (CUcontext * pctx), (pctx))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuCtxGetDevice
CUDA_API(cuCtxGetDevice, (CUdevice * device), (device))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuCtxGetExecAffinity
CUDA_API(cuCtxGetExecAffinity, (CUexecAffinityParam * pExecAffinity, CUexecAffinityType type), (pExecAffinity, type))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuCtxGetFlags
CUDA_API(cuCtxGetFlags, (unsigned int* flags), (flags))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuCtxGetId
CUDA_API(cuCtxGetId, (CUcontext ctx, unsigned long long* ctxId), (ctx, ctxId))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuCtxGetLimit
CUDA_API(cuCtxGetLimit, (size_t * pvalue, CUlimit limit), (pvalue, limit))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuCtxGetStreamPriorityRange
CUDA_API(cuCtxGetStreamPriorityRange, (int* leastPriority, int* greatestPriority), (leastPriority, greatestPriority))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuCtxPopCurrent
CUDA_API(cuCtxPopCurrent, (CUcontext * pctx), (pctx))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuCtxPushCurrent
CUDA_API(cuCtxPushCurrent, (CUcontext ctx), (ctx))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuCtxResetPersistingL2Cache
CUDA_API(cuCtxResetPersistingL2Cache, (), ())
#endif

#if SYZ_EXECUTOR || __NR_syz_cuCtxSetCacheConfig
CUDA_API(cuCtxSetCacheConfig, (CUfunc_cache config), (config))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuCtxSetCurrent
CUDA_API(cuCtxSetCurrent, (CUcontext ctx), (ctx))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuCtxSetFlags
CUDA_API(cuCtxSetFlags, (unsigned int flags), (flags))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuCtxSetLimit
CUDA_API(cuCtxSetLimit, (CUlimit limit, size_t value), (limit, value))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuCtxSynchronize
CUDA_API(cuCtxSynchronize, (), ())
#endif
// CUDA_API(cuCtxCreate_v4, (CUcontext * pctx, CUctxCreateParams* ctxCreateParams, unsigned int flags, int dev), (pctx, ctxCreateParams, flags, dev))
// CUDA_API(cuCtxRecordEvent, (CUcontext hCtx, CUevent hEvent), (hCtx, hEvent))
// CUDA_API(cuCtxWaitEvent, (CUcontext hCtx, CUevent hEvent), (hCtx, hEvent))

// Library management
#if SYZ_EXECUTOR || __NR_syz_cuKernelGetAttribute
CUDA_API(cuKernelGetAttribute, (int* pi, CUfunction_attribute attrib, CUkernel kernel, int dev), (pi, attrib, kernel, dev))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuKernelGetFunction
CUDA_API(cuKernelGetFunction, (CUfunction * pFunc, CUkernel kernel), (pFunc, kernel))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuKernelGetName
CUDA_API(cuKernelGetName, (const char** name, CUkernel hfunc), (name, hfunc))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuKernelGetParamInfo
CUDA_API(cuKernelGetParamInfo, (CUkernel kernel, size_t paramIndex, size_t* paramOffset, size_t* paramSize), (kernel, paramIndex, paramOffset, paramSize))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuKernelSetAttribute
CUDA_API(cuKernelSetAttribute, (CUfunction_attribute attrib, int val, CUkernel kernel, int dev), (attrib, val, kernel, dev))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuKernelSetCacheConfig
CUDA_API(cuKernelSetCacheConfig, (CUkernel kernel, CUfunc_cache config, int dev), (kernel, config, dev))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuLibraryEnumerateKernels
CUDA_API(cuLibraryEnumerateKernels, (CUkernel * kernels, unsigned int numKernels, CUlibrary lib), (kernels, numKernels, lib))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuLibraryGetGlobal
CUDA_API(cuLibraryGetGlobal, (CUdeviceptr * dptr, size_t* bytes, CUlibrary library, const char* name), (dptr, bytes, library, name))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuLibraryGetKernel
CUDA_API(cuLibraryGetKernel, (CUkernel * pKernel, CUlibrary library, const char* name), (pKernel, library, name))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuLibraryGetKernelCount
CUDA_API(cuLibraryGetKernelCount, (unsigned int* count, CUlibrary lib), (count, lib))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuLibraryGetManaged
CUDA_API(cuLibraryGetManaged, (CUdeviceptr * dptr, size_t* bytes, CUlibrary library, const char* name), (dptr, bytes, library, name))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuLibraryGetModule
CUDA_API(cuLibraryGetModule, (CUmodule * pMod, CUlibrary library), (pMod, library))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuLibraryGetUnifiedFunction
CUDA_API(cuLibraryGetUnifiedFunction, (void** fptr, CUlibrary library, const char* symbol), (fptr, library, symbol))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuLibraryLoadData
CUDA_API(cuLibraryLoadData, (CUlibrary * library, const void* code, CUjit_option* jitOptions, void** jitOptionsValues, unsigned int numJitOptions, CUlibraryOption* libraryOptions, void** libraryOptionValues, unsigned int numLibraryOptions), (library, code, jitOptions, jitOptionsValues, numJitOptions, libraryOptions, libraryOptionValues, numLibraryOptions))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuLibraryLoadFromFile
CUDA_API(cuLibraryLoadFromFile, (CUlibrary * library, const char* fileName, CUjit_option* jitOptions, void** jitOptionsValues, unsigned int numJitOptions, CUlibraryOption* libraryOptions, void** libraryOptionValues, unsigned int numLibraryOptions), (library, fileName, jitOptions, jitOptionsValues, numJitOptions, libraryOptions, libraryOptionValues, numLibraryOptions))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuLibraryUnload
CUDA_API(cuLibraryUnload, (CUlibrary library), (library))
#endif
// CUDA_API(cuKernelGetLibrary, (CUlibrary * pLib, CUkernel kernel), (pLib, kernel))

// Module Management
#if (SYZ_EXECUTOR || __NR_syz_cuLinkAddData) && SYZ_EXECUTOR_NVIDIA
static CUresult syz_cuLinkAddData(CUlinkState state, CUjitInputType type, const char* name, unsigned int numOptions, CUjit_option* options, void** optionValues)
{
	return cuLinkAddData(state, type, (void*)ptxSource, strlen(ptxSource) + 1, name, numOptions, options, optionValues);
}
#else
CUDA_API(cuLinkAddData, (CUlinkState state, CUjitInputType type, const void* data, size_t size, const char* name, unsigned int numOptions, CUjit_option* options, void** optionValues), (state, type, data, size, name, numOptions, options, optionValues))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuLinkAddFile
CUDA_API(cuLinkAddFile, (CUlinkState state, CUjitInputType type, const char* path, unsigned int numOptions, CUjit_option* options, void** optionValues), (state, type, path, numOptions, options, optionValues))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuLinkComplete
CUDA_API(cuLinkComplete, (CUlinkState state, void** cubinOut, size_t* sizeOut), (state, cubinOut, sizeOut))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuLinkCreate
CUDA_API(cuLinkCreate, (unsigned int numOptions, CUjit_option* options, void** optionValues, CUlinkState* stateOut), (numOptions, options, optionValues, stateOut))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuLinkDestroy
CUDA_API(cuLinkDestroy, (CUlinkState state), (state))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuModuleEnumerateFunctions
CUDA_API(cuModuleEnumerateFunctions, (CUfunction * functions, unsigned int numFunctions, CUmodule mod), (functions, numFunctions, mod))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuModuleGetFunction
CUDA_API(cuModuleGetFunction, (CUfunction * hfunc, CUmodule hmod, const char* name), (hfunc, hmod, name))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuModuleGetFunctionCount
CUDA_API(cuModuleGetFunctionCount, (unsigned int* count, CUmodule mod), (count, mod))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuModuleGetGlobal
CUDA_API(cuModuleGetGlobal, (CUdeviceptr * dptr, size_t* bytes, CUmodule hmod, const char* name), (dptr, bytes, hmod, name))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuModuleGetLoadingMode
CUDA_API(cuModuleGetLoadingMode, (CUmoduleLoadingMode * mode), (mode))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuModuleLoad
CUDA_API(cuModuleLoad, (CUmodule * module, const char* fname), (module, fname))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuModuleLoadData
CUDA_API(cuModuleLoadData, (CUmodule * module, const void* image), (module, image))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuModuleLoadDataEx
CUDA_API(cuModuleLoadDataEx, (CUmodule * module, const void* image, unsigned int numOptions, CUjit_option* options, void** optionValues), (module, image, numOptions, options, optionValues))
#endif

#if (SYZ_EXECUTOR || __NR_syz_cuModuleLoadProg) && SYZ_EXECUTOR_NVIDIA
static CUresult syz_cuModuleLoadProg(CUmodule* cuModule)
{
	return cuModuleLoadData(cuModule, ptxSource);
}
#else
CUDA_API(cuModuleLoadProg, (CUmodule * module), (module))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuModuleLoadFatBinary
CUDA_API(cuModuleLoadFatBinary, (CUmodule * module, const void* fatCubin), (module, fatCubin))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuModuleUnload
CUDA_API(cuModuleUnload, (CUmodule hmod), (hmod))
#endif

// Memory management
#if SYZ_EXECUTOR || __NR_syz_cuArray3DCreate
CUDA_API(cuArray3DCreate, (CUarray * pHandle, const CUDA_ARRAY3D_DESCRIPTOR* pAllocateArray), (pHandle, pAllocateArray))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuArray3DGetDescriptor
CUDA_API(cuArray3DGetDescriptor, (CUDA_ARRAY3D_DESCRIPTOR * pArrayDescriptor, CUarray hArray), (pArrayDescriptor, hArray))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuArrayCreate
CUDA_API(cuArrayCreate, (CUarray * pHandle, const CUDA_ARRAY_DESCRIPTOR* pAllocateArray), (pHandle, pAllocateArray))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuArrayDestroy
CUDA_API(cuArrayDestroy, (CUarray hArray), (hArray))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuArrayGetDescriptor
CUDA_API(cuArrayGetDescriptor, (CUDA_ARRAY_DESCRIPTOR * pArrayDescriptor, CUarray hArray), (pArrayDescriptor, hArray))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuArrayGetMemoryRequirements
CUDA_API(cuArrayGetMemoryRequirements, (CUDA_ARRAY_MEMORY_REQUIREMENTS * memoryRequirements, CUarray array, int device), (memoryRequirements, array, device))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuArrayGetPlane
CUDA_API(cuArrayGetPlane, (CUarray * pPlaneArray, CUarray hArray, unsigned int planeIdx), (pPlaneArray, hArray, planeIdx))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuArrayGetSparseProperties
CUDA_API(cuArrayGetSparseProperties, (CUDA_ARRAY_SPARSE_PROPERTIES * sparseProperties, CUarray array), (sparseProperties, array))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuDeviceGetByPCIBusId
CUDA_API(cuDeviceGetByPCIBusId, (CUdevice * dev, const char* pciBusId), (dev, pciBusId))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuDeviceGetPCIBusId
CUDA_API(cuDeviceGetPCIBusId, (char* pciBusId, int len, int dev), (pciBusId, len, dev))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuDeviceRegisterAsyncNotification
CUDA_API(cuDeviceRegisterAsyncNotification, (int device, CUasyncCallback callbackFunc, void* userData, CUasyncCallbackHandle* callback), (device, callbackFunc, userData, callback))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuDeviceUnregisterAsyncNotification
CUDA_API(cuDeviceUnregisterAsyncNotification, (int device, CUasyncCallbackHandle callback), (device, callback))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuIpcCloseMemHandle
CUDA_API(cuIpcCloseMemHandle, (CUdeviceptr dptr), (dptr))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuIpcGetEventHandle
CUDA_API(cuIpcGetEventHandle, (CUipcEventHandle * pHandle, CUevent event), (pHandle, event))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuIpcGetMemHandle
CUDA_API(cuIpcGetMemHandle, (CUipcMemHandle * pHandle, CUdeviceptr dptr), (pHandle, dptr))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuIpcOpenEventHandle
CUDA_API(cuIpcOpenEventHandle, (CUevent * phEvent, CUipcEventHandle handle), (phEvent, handle))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuIpcOpenMemHandle
CUDA_API(cuIpcOpenMemHandle, (CUdeviceptr * pdptr, CUipcMemHandle handle, unsigned int Flags), (pdptr, handle, Flags))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuMemAlloc
CUDA_API(cuMemAlloc, (CUdeviceptr * dptr, size_t bytesize), (dptr, bytesize))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuMemAllocHost
CUDA_API(cuMemAllocHost, (void** pp, size_t bytesize), (pp, bytesize))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuMemAllocManaged
CUDA_API(cuMemAllocManaged, (CUdeviceptr * dptr, size_t bytesize, unsigned int flags), (dptr, bytesize, flags))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuMemAllocPitch
CUDA_API(cuMemAllocPitch, (CUdeviceptr * dptr, size_t* pPitch, size_t WidthInBytes, size_t Height, unsigned int ElementSizeBytes), (dptr, pPitch, WidthInBytes, Height, ElementSizeBytes))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuMemFree
CUDA_API(cuMemFree, (CUdeviceptr dptr), (dptr))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuMemFreeHost
CUDA_API(cuMemFreeHost, (void* p), (p))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuMemGetAddressRange
CUDA_API(cuMemGetAddressRange, (CUdeviceptr * pbase, size_t* psize, CUdeviceptr dptr), (pbase, psize, dptr))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuMemGetHandleForAddressRange
CUDA_API(cuMemGetHandleForAddressRange, (void* handle, CUdeviceptr dptr, size_t size, CUmemRangeHandleType handleType, unsigned long long flags), (handle, dptr, size, handleType, flags))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuMemGetInfo
CUDA_API(cuMemGetInfo, (size_t * free, size_t* total), (free, total))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuMemHostAlloc
CUDA_API(cuMemHostAlloc, (void** pp, size_t bytesize, unsigned int Flags), (pp, bytesize, Flags))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuMemHostGetDevicePointer
CUDA_API(cuMemHostGetDevicePointer, (CUdeviceptr * pdptr, void* p, unsigned int Flags), (pdptr, p, Flags))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuMemHostGetFlags
CUDA_API(cuMemHostGetFlags, (unsigned int* pFlags, void* p), (pFlags, p))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuMemHostRegister
CUDA_API(cuMemHostRegister, (void* p, size_t bytesize, unsigned int Flags), (p, bytesize, Flags))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuMemHostUnregister
CUDA_API(cuMemHostUnregister, (void* p), (p))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuMemcpy
CUDA_API(cuMemcpy, (CUdeviceptr dst, CUdeviceptr src, size_t ByteCount), (dst, src, ByteCount))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuMemcpy2D
CUDA_API(cuMemcpy2D, (const CUDA_MEMCPY2D* pCopy), (pCopy))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuMemcpy2DAsync
CUDA_API(cuMemcpy2DAsync, (const CUDA_MEMCPY2D* pCopy, CUstream hStream), (pCopy, hStream))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuMemcpy2DUnaligned
CUDA_API(cuMemcpy2DUnaligned, (const CUDA_MEMCPY2D* pCopy), (pCopy))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuMemcpy3D
CUDA_API(cuMemcpy3D, (const CUDA_MEMCPY3D* pCopy), (pCopy))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuMemcpy3DAsync
CUDA_API(cuMemcpy3DAsync, (const CUDA_MEMCPY3D* pCopy, CUstream hStream), (pCopy, hStream))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuMemcpy3DPeer
CUDA_API(cuMemcpy3DPeer, (const CUDA_MEMCPY3D_PEER* pCopy), (pCopy))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuMemcpy3DPeerAsync
CUDA_API(cuMemcpy3DPeerAsync, (const CUDA_MEMCPY3D_PEER* pCopy, CUstream hStream), (pCopy, hStream))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuMemcpyAsync
CUDA_API(cuMemcpyAsync, (CUdeviceptr dst, CUdeviceptr src, size_t ByteCount, CUstream hStream), (dst, src, ByteCount, hStream))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuMemcpyAtoA
CUDA_API(cuMemcpyAtoA, (CUarray dstArray, size_t dstOffset, CUarray srcArray, size_t srcOffset, size_t ByteCount), (dstArray, dstOffset, srcArray, srcOffset, ByteCount))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuMemcpyAtoD
CUDA_API(cuMemcpyAtoD, (CUdeviceptr dstDevice, CUarray srcArray, size_t srcOffset, size_t ByteCount), (dstDevice, srcArray, srcOffset, ByteCount))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuMemcpyAtoH
CUDA_API(cuMemcpyAtoH, (void* dstHost, CUarray srcArray, size_t srcOffset, size_t ByteCount), (dstHost, srcArray, srcOffset, ByteCount))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuMemcpyAtoHAsync
CUDA_API(cuMemcpyAtoHAsync, (void* dstHost, CUarray srcArray, size_t srcOffset, size_t ByteCount, CUstream hStream), (dstHost, srcArray, srcOffset, ByteCount, hStream))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuMemcpyDtoA
CUDA_API(cuMemcpyDtoA, (CUarray dstArray, size_t dstOffset, CUdeviceptr srcDevice, size_t ByteCount), (dstArray, dstOffset, srcDevice, ByteCount))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuMemcpyDtoD
CUDA_API(cuMemcpyDtoD, (CUdeviceptr dstDevice, CUdeviceptr srcDevice, size_t ByteCount), (dstDevice, srcDevice, ByteCount))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuMemcpyDtoDAsync
CUDA_API(cuMemcpyDtoDAsync, (CUdeviceptr dstDevice, CUdeviceptr srcDevice, size_t ByteCount, CUstream hStream), (dstDevice, srcDevice, ByteCount, hStream))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuMemcpyDtoH
CUDA_API(cuMemcpyDtoH, (void* dstHost, CUdeviceptr srcDevice, size_t ByteCount), (dstHost, srcDevice, ByteCount))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuMemcpyDtoHAsync
CUDA_API(cuMemcpyDtoHAsync, (void* dstHost, CUdeviceptr srcDevice, size_t ByteCount, CUstream hStream), (dstHost, srcDevice, ByteCount, hStream))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuMemcpyHtoA
CUDA_API(cuMemcpyHtoA, (CUarray dstArray, size_t dstOffset, const void* srcHost, size_t ByteCount), (dstArray, dstOffset, srcHost, ByteCount))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuMemcpyHtoAAsync
CUDA_API(cuMemcpyHtoAAsync, (CUarray dstArray, size_t dstOffset, const void* srcHost, size_t ByteCount, CUstream hStream), (dstArray, dstOffset, srcHost, ByteCount, hStream))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuMemcpyHtoD
CUDA_API(cuMemcpyHtoD, (CUdeviceptr dstDevice, const void* srcHost, size_t ByteCount), (dstDevice, srcHost, ByteCount))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuMemcpyHtoDAsync
CUDA_API(cuMemcpyHtoDAsync, (CUdeviceptr dstDevice, const void* srcHost, size_t ByteCount, CUstream hStream), (dstDevice, srcHost, ByteCount, hStream))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuMemcpyPeer
CUDA_API(cuMemcpyPeer, (CUdeviceptr dstDevice, CUcontext dstContext, CUdeviceptr srcDevice, CUcontext srcContext, size_t ByteCount), (dstDevice, dstContext, srcDevice, srcContext, ByteCount))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuMemcpyPeerAsync
CUDA_API(cuMemcpyPeerAsync, (CUdeviceptr dstDevice, CUcontext dstContext, CUdeviceptr srcDevice, CUcontext srcContext, size_t ByteCount, CUstream hStream), (dstDevice, dstContext, srcDevice, srcContext, ByteCount, hStream))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuMemsetD16
CUDA_API(cuMemsetD16, (CUdeviceptr dstDevice, unsigned short us, size_t N), (dstDevice, us, N))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuMemsetD16Async
CUDA_API(cuMemsetD16Async, (CUdeviceptr dstDevice, unsigned short us, size_t N, CUstream hStream), (dstDevice, us, N, hStream))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuMemsetD2D16
CUDA_API(cuMemsetD2D16, (CUdeviceptr dstDevice, size_t dstPitch, unsigned short us, size_t Width, size_t Height), (dstDevice, dstPitch, us, Width, Height))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuMemsetD2D16Async
CUDA_API(cuMemsetD2D16Async, (CUdeviceptr dstDevice, size_t dstPitch, unsigned short us, size_t Width, size_t Height, CUstream hStream), (dstDevice, dstPitch, us, Width, Height, hStream))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuMemsetD2D32
CUDA_API(cuMemsetD2D32, (CUdeviceptr dstDevice, size_t dstPitch, unsigned int ui, size_t Width, size_t Height), (dstDevice, dstPitch, ui, Width, Height))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuMemsetD2D32Async
CUDA_API(cuMemsetD2D32Async, (CUdeviceptr dstDevice, size_t dstPitch, unsigned int ui, size_t Width, size_t Height, CUstream hStream), (dstDevice, dstPitch, ui, Width, Height, hStream))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuMemsetD2D8
CUDA_API(cuMemsetD2D8, (CUdeviceptr dstDevice, size_t dstPitch, unsigned char uc, size_t Width, size_t Height), (dstDevice, dstPitch, uc, Width, Height))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuMemsetD2D8Async
CUDA_API(cuMemsetD2D8Async, (CUdeviceptr dstDevice, size_t dstPitch, unsigned char uc, size_t Width, size_t Height, CUstream hStream), (dstDevice, dstPitch, uc, Width, Height, hStream))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuMemsetD32
CUDA_API(cuMemsetD32, (CUdeviceptr dstDevice, unsigned int ui, size_t N), (dstDevice, ui, N))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuMemsetD32Async
CUDA_API(cuMemsetD32Async, (CUdeviceptr dstDevice, unsigned int ui, size_t N, CUstream hStream), (dstDevice, ui, N, hStream))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuMemsetD8
CUDA_API(cuMemsetD8, (CUdeviceptr dstDevice, unsigned char uc, size_t N), (dstDevice, uc, N))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuMemsetD8Async
CUDA_API(cuMemsetD8Async, (CUdeviceptr dstDevice, unsigned char uc, size_t N, CUstream hStream), (dstDevice, uc, N, hStream))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuMipmappedArrayCreate
CUDA_API(cuMipmappedArrayCreate, (CUmipmappedArray * pHandle, const CUDA_ARRAY3D_DESCRIPTOR* pMipmappedArrayDesc, unsigned int numMipmapLevels), (pHandle, pMipmappedArrayDesc, numMipmapLevels))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuMipmappedArrayDestroy
CUDA_API(cuMipmappedArrayDestroy, (CUmipmappedArray hMipmappedArray), (hMipmappedArray))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuMipmappedArrayGetLevel
CUDA_API(cuMipmappedArrayGetLevel, (CUarray * pLevelArray, CUmipmappedArray hMipmappedArray, unsigned int level), (pLevelArray, hMipmappedArray, level))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuMipmappedArrayGetMemoryRequirements
CUDA_API(cuMipmappedArrayGetMemoryRequirements, (CUDA_ARRAY_MEMORY_REQUIREMENTS * memoryRequirements, CUmipmappedArray mipmap, int device), (memoryRequirements, mipmap, device))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuMipmappedArrayGetSparseProperties
CUDA_API(cuMipmappedArrayGetSparseProperties, (CUDA_ARRAY_SPARSE_PROPERTIES * sparseProperties, CUmipmappedArray mipmap), (sparseProperties, mipmap))
#endif

// Virtual memory management
#if SYZ_EXECUTOR || __NR_syz_cuMemAddressFree
CUDA_API(cuMemAddressFree, (CUdeviceptr ptr, size_t size), (ptr, size))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuMemAddressReserve
CUDA_API(cuMemAddressReserve, (CUdeviceptr * ptr, size_t size, size_t alignment, CUdeviceptr addr, unsigned long long flags), (ptr, size, alignment, addr, flags))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuMemCreate
CUDA_API(cuMemCreate, (CUmemGenericAllocationHandle * handle, size_t size, const CUmemAllocationProp* prop, unsigned long long flags), (handle, size, prop, flags))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuMemExportToShareableHandle
CUDA_API(cuMemExportToShareableHandle, (void* shareableHandle, CUmemGenericAllocationHandle handle, CUmemAllocationHandleType handleType, unsigned long long flags), (shareableHandle, handle, handleType, flags))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuMemGetAccess
CUDA_API(cuMemGetAccess, (unsigned long long* flags, const CUmemLocation* location, CUdeviceptr ptr), (flags, location, ptr))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuMemGetAllocationGranularity
CUDA_API(cuMemGetAllocationGranularity, (size_t * granularity, const CUmemAllocationProp* prop, CUmemAllocationGranularity_flags option), (granularity, prop, option))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuMemGetAllocationPropertiesFromHandle
CUDA_API(cuMemGetAllocationPropertiesFromHandle, (CUmemAllocationProp * prop, CUmemGenericAllocationHandle handle), (prop, handle))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuMemImportFromShareableHandle
CUDA_API(cuMemImportFromShareableHandle, (CUmemGenericAllocationHandle * handle, void* osHandle, CUmemAllocationHandleType shHandleType), (handle, osHandle, shHandleType))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuMemMap
CUDA_API(cuMemMap, (CUdeviceptr ptr, size_t size, size_t offset, CUmemGenericAllocationHandle handle, unsigned long long flags), (ptr, size, offset, handle, flags))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuMemMapArrayAsync
CUDA_API(cuMemMapArrayAsync, (CUarrayMapInfo * mapInfoList, unsigned int count, CUstream hStream), (mapInfoList, count, hStream))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuMemRelease
CUDA_API(cuMemRelease, (CUmemGenericAllocationHandle handle), (handle))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuMemRetainAllocationHandle
CUDA_API(cuMemRetainAllocationHandle, (CUmemGenericAllocationHandle * handle, void* addr), (handle, addr))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuMemSetAccess
CUDA_API(cuMemSetAccess, (CUdeviceptr ptr, size_t size, const CUmemAccessDesc* desc, size_t count), (ptr, size, desc, count))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuMemUnmap
CUDA_API(cuMemUnmap, (CUdeviceptr ptr, size_t size), (ptr, size))
#endif

// Stream ordered memory management
#if SYZ_EXECUTOR || __NR_syz_cuMemAllocAsync
CUDA_API(cuMemAllocAsync, (CUdeviceptr * dptr, size_t bytesize, CUstream hStream), (dptr, bytesize, hStream))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuMemAllocFromPoolAsync
CUDA_API(cuMemAllocFromPoolAsync, (CUdeviceptr * dptr, size_t bytesize, CUmemoryPool pool, CUstream hStream), (dptr, bytesize, pool, hStream))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuMemFreeAsync
CUDA_API(cuMemFreeAsync, (CUdeviceptr dptr, CUstream hStream), (dptr, hStream))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuMemPoolCreate
CUDA_API(cuMemPoolCreate, (CUmemoryPool * pool, const CUmemPoolProps* poolProps), (pool, poolProps))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuMemPoolDestroy
CUDA_API(cuMemPoolDestroy, (CUmemoryPool pool), (pool))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuMemPoolExportPointer
CUDA_API(cuMemPoolExportPointer, (CUmemPoolPtrExportData * shareData_out, CUdeviceptr ptr), (shareData_out, ptr))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuMemPoolExportToShareableHandle
CUDA_API(cuMemPoolExportToShareableHandle, (void* handle_out, CUmemoryPool pool, CUmemAllocationHandleType handleType, unsigned long long flags), (handle_out, pool, handleType, flags))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuMemPoolGetAccess
CUDA_API(cuMemPoolGetAccess, (CUmemAccess_flags * flags, CUmemoryPool memPool, CUmemLocation* location), (flags, memPool, location))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuMemPoolGetAttribute
CUDA_API(cuMemPoolGetAttribute, (CUmemoryPool pool, CUmemPool_attribute attr, void* value), (pool, attr, value))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuMemPoolImportFromShareableHandle
CUDA_API(cuMemPoolImportFromShareableHandle, (CUmemoryPool * pool_out, void* handle, CUmemAllocationHandleType handleType, unsigned long long flags), (pool_out, handle, handleType, flags))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuMemPoolImportPointer
CUDA_API(cuMemPoolImportPointer, (CUdeviceptr * ptr_out, CUmemoryPool pool, CUmemPoolPtrExportData* shareData), (ptr_out, pool, shareData))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuMemPoolSetAccess
CUDA_API(cuMemPoolSetAccess, (CUmemoryPool pool, const CUmemAccessDesc* map, size_t count), (pool, map, count))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuMemPoolSetAttribute
CUDA_API(cuMemPoolSetAttribute, (CUmemoryPool pool, CUmemPool_attribute attr, void* value), (pool, attr, value))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuMemPoolTrimTo
CUDA_API(cuMemPoolTrimTo, (CUmemoryPool pool, size_t minBytesToKeep), (pool, minBytesToKeep))
#endif

// Multicast object management
#if SYZ_EXECUTOR || __NR_syz_cuMulticastAddDevice
CUDA_API(cuMulticastAddDevice, (CUmemGenericAllocationHandle mcHandle, int dev), (mcHandle, dev))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuMulticastBindAddr
CUDA_API(cuMulticastBindAddr, (CUmemGenericAllocationHandle mcHandle, size_t mcOffset, CUdeviceptr memptr, size_t size, unsigned long long flags), (mcHandle, mcOffset, memptr, size, flags))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuMulticastBindMem
CUDA_API(cuMulticastBindMem, (CUmemGenericAllocationHandle mcHandle, size_t mcOffset, CUmemGenericAllocationHandle memHandle, size_t memOffset, size_t size, unsigned long long flags), (mcHandle, mcOffset, memHandle, memOffset, size, flags))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuMulticastCreate
CUDA_API(cuMulticastCreate, (CUmemGenericAllocationHandle * mcHandle, const CUmulticastObjectProp* prop), (mcHandle, prop))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuMulticastGetGranularity
CUDA_API(cuMulticastGetGranularity, (size_t * granularity, const CUmulticastObjectProp* prop, CUmulticastGranularity_flags option), (granularity, prop, option))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuMulticastUnbind
CUDA_API(cuMulticastUnbind, (CUmemGenericAllocationHandle mcHandle, int dev, size_t mcOffset, size_t size), (mcHandle, dev, mcOffset, size))
#endif

// Unified addressing management
#if SYZ_EXECUTOR || __NR_syz_cuMemAdvise
CUDA_API(cuMemAdvise, (CUdeviceptr devPtr, size_t count, CUmem_advise advice, int device), (devPtr, count, advice, device))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuMemAdvise_v2
CUDA_API(cuMemAdvise_v2, (CUdeviceptr devPtr, size_t count, CUmem_advise advice, CUmemLocation location), (devPtr, count, advice, location))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuMemPrefetchAsync
CUDA_API(cuMemPrefetchAsync, (CUdeviceptr devPtr, size_t count, int dstDevice, CUstream hStream), (devPtr, count, dstDevice, hStream))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuMemPrefetchAsync_v2
CUDA_API(cuMemPrefetchAsync_v2, (CUdeviceptr devPtr, size_t count, CUmemLocation location, unsigned int flags, CUstream hStream), (devPtr, count, location, flags, hStream))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuMemRangeGetAttribute
CUDA_API(cuMemRangeGetAttribute, (void* data, size_t dataSize, CUmem_range_attribute attribute, CUdeviceptr devPtr, size_t count), (data, dataSize, attribute, devPtr, count))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuMemRangeGetAttributes
CUDA_API(cuMemRangeGetAttributes, (void** data, size_t* dataSizes, CUmem_range_attribute* attributes, size_t numAttributes, CUdeviceptr devPtr, size_t count), (data, dataSizes, attributes, numAttributes, devPtr, count))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuPointerGetAttribute
CUDA_API(cuPointerGetAttribute, (void* data, CUpointer_attribute attribute, CUdeviceptr ptr), (data, attribute, ptr))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuPointerGetAttributes
CUDA_API(cuPointerGetAttributes, (unsigned int numAttributes, CUpointer_attribute* attributes, void** data, CUdeviceptr ptr), (numAttributes, attributes, data, ptr))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuPointerSetAttribute
CUDA_API(cuPointerSetAttribute, (const void* value, CUpointer_attribute attribute, CUdeviceptr ptr), (value, attribute, ptr))
#endif

// Stream management
#if SYZ_EXECUTOR || __NR_syz_cuStreamAddCallback
CUDA_API(cuStreamAddCallback, (CUstream hStream, CUstreamCallback callback, void* userData, unsigned int flags), (hStream, callback, userData, flags))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuStreamAttachMemAsync
CUDA_API(cuStreamAttachMemAsync, (CUstream hStream, CUdeviceptr dptr, size_t length, unsigned int flags), (hStream, dptr, length, flags))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuStreamBeginCapture
CUDA_API(cuStreamBeginCapture, (CUstream hStream, CUstreamCaptureMode mode), (hStream, mode))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuStreamBeginCaptureToGraph
CUDA_API(cuStreamBeginCaptureToGraph, (CUstream hStream, CUgraph hGraph, const CUgraphNode* dependencies, const CUgraphEdgeData* dependencyData, size_t numDependencies, CUstreamCaptureMode mode), (hStream, hGraph, dependencies, dependencyData, numDependencies, mode))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuStreamCopyAttributes
CUDA_API(cuStreamCopyAttributes, (CUstream dst, CUstream src), (dst, src))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuStreamCreate
CUDA_API(cuStreamCreate, (CUstream * phStream, unsigned int Flags), (phStream, Flags))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuStreamCreateWithPriority
CUDA_API(cuStreamCreateWithPriority, (CUstream * phStream, unsigned int flags, int priority), (phStream, flags, priority))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuStreamDestroy
CUDA_API(cuStreamDestroy, (CUstream hStream), (hStream))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuStreamEndCapture
CUDA_API(cuStreamEndCapture, (CUstream hStream, CUgraph* phGraph), (hStream, phGraph))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuStreamGetAttribute
CUDA_API(cuStreamGetAttribute, (CUstream hStream, CUstreamAttrID attr, CUstreamAttrValue* value_out), (hStream, attr, value_out))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuStreamGetCaptureInfo
CUDA_API(cuStreamGetCaptureInfo, (CUstream hStream, CUstreamCaptureStatus* captureStatus_out, cuuint64_t* id_out, CUgraph* graph_out, const CUgraphNode** dependencies_out, size_t* numDependencies_out), (hStream, captureStatus_out, id_out, graph_out, dependencies_out, numDependencies_out))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuStreamGetCaptureInfo_v3
CUDA_API(cuStreamGetCaptureInfo_v3, (CUstream hStream, CUstreamCaptureStatus* captureStatus_out, cuuint64_t* id_out, CUgraph* graph_out, const CUgraphNode** dependencies_out, const CUgraphEdgeData** edgeData_out, size_t* numDependencies_out), (hStream, captureStatus_out, id_out, graph_out, dependencies_out, edgeData_out, numDependencies_out))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuStreamGetCtx
CUDA_API(cuStreamGetCtx, (CUstream hStream, CUcontext* pctx), (hStream, pctx))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuStreamGetFlags
CUDA_API(cuStreamGetFlags, (CUstream hStream, unsigned int* flags), (hStream, flags))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuStreamGetId
CUDA_API(cuStreamGetId, (CUstream hStream, unsigned long long* streamId), (hStream, streamId))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuStreamGetPriority
CUDA_API(cuStreamGetPriority, (CUstream hStream, int* priority), (hStream, priority))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuStreamIsCapturing
CUDA_API(cuStreamIsCapturing, (CUstream hStream, CUstreamCaptureStatus* captureStatus), (hStream, captureStatus))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuStreamQuery
CUDA_API(cuStreamQuery, (CUstream hStream), (hStream))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuStreamSetAttribute
CUDA_API(cuStreamSetAttribute, (CUstream hStream, CUstreamAttrID attr, const CUstreamAttrValue* value), (hStream, attr, value))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuStreamSynchronize
CUDA_API(cuStreamSynchronize, (CUstream hStream), (hStream))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuStreamUpdateCaptureDependencies
CUDA_API(cuStreamUpdateCaptureDependencies, (CUstream hStream, CUgraphNode* dependencies, size_t numDependencies, unsigned int flags), (hStream, dependencies, numDependencies, flags))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuStreamUpdateCaptureDependencies_v2
CUDA_API(cuStreamUpdateCaptureDependencies_v2, (CUstream hStream, CUgraphNode* dependencies, const CUgraphEdgeData* dependencyData, size_t numDependencies, unsigned int flags), (hStream, dependencies, dependencyData, numDependencies, flags))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuStreamWaitEvent
CUDA_API(cuStreamWaitEvent, (CUstream hStream, CUevent hEvent, unsigned int Flags), (hStream, hEvent, Flags))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuThreadExchangeStreamCaptureMode
CUDA_API(cuThreadExchangeStreamCaptureMode, (CUstreamCaptureMode * mode), (mode))
#endif
// CUDA_API(cuStreamGetCtx_v2, (CUstream hStream, CUcontext* pCtx, CUgreenCtx* pGreenCtx), (hStream, pCtx, pGreenCtx))

// Event management
#if SYZ_EXECUTOR || __NR_syz_cuEventCreate
CUDA_API(cuEventCreate, (CUevent * phEvent, unsigned int Flags), (phEvent, Flags))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuEventDestroy
CUDA_API(cuEventDestroy, (CUevent hEvent), (hEvent))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuEventElapsedTime
CUDA_API(cuEventElapsedTime, (float* pMilliseconds, CUevent hStart, CUevent hEnd), (pMilliseconds, hStart, hEnd))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuEventQuery
CUDA_API(cuEventQuery, (CUevent hEvent), (hEvent))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuEventRecord
CUDA_API(cuEventRecord, (CUevent hEvent, CUstream hStream), (hEvent, hStream))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuEventRecordWithFlags
CUDA_API(cuEventRecordWithFlags, (CUevent hEvent, CUstream hStream, unsigned int flags), (hEvent, hStream, flags))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuEventSynchronize
CUDA_API(cuEventSynchronize, (CUevent hEvent), (hEvent))
#endif

// External resource
#if SYZ_EXECUTOR || __NR_syz_cuDestroyExternalMemory
CUDA_API(cuDestroyExternalMemory, (CUexternalMemory extMem), (extMem))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuDestroyExternalSemaphore
CUDA_API(cuDestroyExternalSemaphore, (CUexternalSemaphore extSem), (extSem))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuExternalMemoryGetMappedBuffer
CUDA_API(cuExternalMemoryGetMappedBuffer, (CUdeviceptr * devPtr, CUexternalMemory extMem, const CUDA_EXTERNAL_MEMORY_BUFFER_DESC* bufferDesc), (devPtr, extMem, bufferDesc))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuExternalMemoryGetMappedMipmappedArray
CUDA_API(cuExternalMemoryGetMappedMipmappedArray, (CUmipmappedArray * mipmap, CUexternalMemory extMem, const CUDA_EXTERNAL_MEMORY_MIPMAPPED_ARRAY_DESC* mipmapDesc), (mipmap, extMem, mipmapDesc))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuImportExternalMemory
CUDA_API(cuImportExternalMemory, (CUexternalMemory * extMem_out, const CUDA_EXTERNAL_MEMORY_HANDLE_DESC* memHandleDesc), (extMem_out, memHandleDesc))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuImportExternalSemaphore
CUDA_API(cuImportExternalSemaphore, (CUexternalSemaphore * extSem_out, const CUDA_EXTERNAL_SEMAPHORE_HANDLE_DESC* semHandleDesc), (extSem_out, semHandleDesc))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuSignalExternalSemaphoresAsync
CUDA_API(cuSignalExternalSemaphoresAsync, (const CUexternalSemaphore* extSemArray, const CUDA_EXTERNAL_SEMAPHORE_SIGNAL_PARAMS* paramsArray, unsigned int numExtSems, CUstream stream), (extSemArray, paramsArray, numExtSems, stream))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuWaitExternalSemaphoresAsync
CUDA_API(cuWaitExternalSemaphoresAsync, (const CUexternalSemaphore* extSemArray, const CUDA_EXTERNAL_SEMAPHORE_WAIT_PARAMS* paramsArray, unsigned int numExtSems, CUstream stream), (extSemArray, paramsArray, numExtSems, stream))
#endif

// Stream memory management
#if SYZ_EXECUTOR || __NR_syz_cuStreamBatchMemOp
CUDA_API(cuStreamBatchMemOp, (CUstream stream, unsigned int count, CUstreamBatchMemOpParams* paramArray, unsigned int flags), (stream, count, paramArray, flags))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuStreamWaitValue32
CUDA_API(cuStreamWaitValue32, (CUstream stream, CUdeviceptr addr, cuuint32_t value, unsigned int flags), (stream, addr, value, flags))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuStreamWaitValue64
CUDA_API(cuStreamWaitValue64, (CUstream stream, CUdeviceptr addr, cuuint64_t value, unsigned int flags), (stream, addr, value, flags))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuStreamWriteValue32
CUDA_API(cuStreamWriteValue32, (CUstream stream, CUdeviceptr addr, cuuint32_t value, unsigned int flags), (stream, addr, value, flags))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuStreamWriteValue64
CUDA_API(cuStreamWriteValue64, (CUstream stream, CUdeviceptr addr, cuuint64_t value, unsigned int flags), (stream, addr, value, flags))
#endif

// Execution control
#if SYZ_EXECUTOR || __NR_syz_cuFuncGetAttribute
CUDA_API(cuFuncGetAttribute, (int* pi, CUfunction_attribute attrib, CUfunction hfunc), (pi, attrib, hfunc))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuFuncGetModule
CUDA_API(cuFuncGetModule, (CUmodule * hmod, CUfunction hfunc), (hmod, hfunc))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuFuncGetName
CUDA_API(cuFuncGetName, (const char** name, CUfunction hfunc), (name, hfunc))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuFuncGetParamInfo
CUDA_API(cuFuncGetParamInfo, (CUfunction func, size_t paramIndex, size_t* paramOffset, size_t* paramSize), (func, paramIndex, paramOffset, paramSize))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuFuncIsLoaded
CUDA_API(cuFuncIsLoaded, (CUfunctionLoadingState * state, CUfunction function), (state, function))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuFuncLoad
CUDA_API(cuFuncLoad, (CUfunction function), (function))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuFuncSetAttribute
CUDA_API(cuFuncSetAttribute, (CUfunction hfunc, CUfunction_attribute attrib, int value), (hfunc, attrib, value))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuFuncSetCacheConfig
CUDA_API(cuFuncSetCacheConfig, (CUfunction hfunc, CUfunc_cache config), (hfunc, config))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuLaunchKernelEx
CUDA_API(cuLaunchKernelEx, (const CUlaunchConfig* config, CUfunction f, void** kernelParams, void** extra), (config, f, kernelParams, extra))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuLaunchKernel
CUDA_API(cuLaunchKernel, (CUfunction f, unsigned int gridDimX, unsigned int gridDimY, unsigned int gridDimZ, unsigned int blockDimX, unsigned int blockDimY, unsigned int blockDimZ, unsigned int sharedMemBytes, CUstream hStream, void** kernelParams, void** extra), (f, gridDimX, gridDimY, gridDimZ, blockDimX, blockDimY, blockDimZ, sharedMemBytes, hStream, kernelParams, extra))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuLaunchCooperativeKernel
CUDA_API(cuLaunchCooperativeKernel, (CUfunction f, unsigned int gridDimX, unsigned int gridDimY, unsigned int gridDimZ, unsigned int blockDimX, unsigned int blockDimY, unsigned int blockDimZ, unsigned int sharedMemBytes, CUstream hStream, void** kernelParams), (f, gridDimX, gridDimY, gridDimZ, blockDimX, blockDimY, blockDimZ, sharedMemBytes, hStream, kernelParams))
#endif

// #if SYZ_EXECUTOR || __NR_syz_cuLaunchCooperativeKernelMultiDevice
// CUDA_API(cuLaunchCooperativeKernelMultiDevice, (CUDA_LAUNCH_PARAMS * launchParamsList, unsigned int numDevices, unsigned int flags), (launchParamsList, numDevices, flags))
// #endif

// CUDA_API(cuLaunchHostFunc, (CUstream hStream, CUhostFn fn, void* userData), (hStream, fn, userData))

// Graphic management
#if SYZ_EXECUTOR || __NR_syz_cuDeviceGetGraphMemAttribute
CUDA_API(cuDeviceGetGraphMemAttribute, (int device, CUgraphMem_attribute attr, void* value), (device, attr, value))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuDeviceGraphMemTrim
CUDA_API(cuDeviceGraphMemTrim, (int device), (device))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuDeviceSetGraphMemAttribute
CUDA_API(cuDeviceSetGraphMemAttribute, (int device, CUgraphMem_attribute attr, void* value), (device, attr, value))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuGraphAddBatchMemOpNode
CUDA_API(cuGraphAddBatchMemOpNode, (CUgraphNode * phGraphNode, CUgraph hGraph, const CUgraphNode* dependencies, size_t numDependencies, const CUDA_BATCH_MEM_OP_NODE_PARAMS* nodeParams), (phGraphNode, hGraph, dependencies, numDependencies, nodeParams))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuGraphAddChildGraphNode
CUDA_API(cuGraphAddChildGraphNode, (CUgraphNode * phGraphNode, CUgraph hGraph, const CUgraphNode* dependencies, size_t numDependencies, CUgraph childGraph), (phGraphNode, hGraph, dependencies, numDependencies, childGraph))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuGraphAddDependencies
CUDA_API(cuGraphAddDependencies, (CUgraph hGraph, const CUgraphNode* from, const CUgraphNode* to, size_t numDependencies), (hGraph, from, to, numDependencies))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuGraphAddDependencies_v2
CUDA_API(cuGraphAddDependencies_v2, (CUgraph hGraph, const CUgraphNode* from, const CUgraphNode* to, const CUgraphEdgeData* edgeData, size_t numDependencies), (hGraph, from, to, edgeData, numDependencies))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuGraphAddEmptyNode
CUDA_API(cuGraphAddEmptyNode, (CUgraphNode * phGraphNode, CUgraph hGraph, const CUgraphNode* dependencies, size_t numDependencies), (phGraphNode, hGraph, dependencies, numDependencies))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuGraphAddEventRecordNode
CUDA_API(cuGraphAddEventRecordNode, (CUgraphNode * phGraphNode, CUgraph hGraph, const CUgraphNode* dependencies, size_t numDependencies, CUevent event), (phGraphNode, hGraph, dependencies, numDependencies, event))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuGraphAddEventWaitNode
CUDA_API(cuGraphAddEventWaitNode, (CUgraphNode * phGraphNode, CUgraph hGraph, const CUgraphNode* dependencies, size_t numDependencies, CUevent event), (phGraphNode, hGraph, dependencies, numDependencies, event))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuGraphAddExternalSemaphoresSignalNode
CUDA_API(cuGraphAddExternalSemaphoresSignalNode, (CUgraphNode * phGraphNode, CUgraph hGraph, const CUgraphNode* dependencies, size_t numDependencies, const CUDA_EXT_SEM_SIGNAL_NODE_PARAMS* nodeParams), (phGraphNode, hGraph, dependencies, numDependencies, nodeParams))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuGraphAddExternalSemaphoresWaitNode
CUDA_API(cuGraphAddExternalSemaphoresWaitNode, (CUgraphNode * phGraphNode, CUgraph hGraph, const CUgraphNode* dependencies, size_t numDependencies, const CUDA_EXT_SEM_WAIT_NODE_PARAMS* nodeParams), (phGraphNode, hGraph, dependencies, numDependencies, nodeParams))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuGraphAddHostNode
CUDA_API(cuGraphAddHostNode, (CUgraphNode * phGraphNode, CUgraph hGraph, const CUgraphNode* dependencies, size_t numDependencies, const CUDA_HOST_NODE_PARAMS* nodeParams), (phGraphNode, hGraph, dependencies, numDependencies, nodeParams))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuGraphAddKernelNode
CUDA_API(cuGraphAddKernelNode, (CUgraphNode * phGraphNode, CUgraph hGraph, const CUgraphNode* dependencies, size_t numDependencies, const CUDA_KERNEL_NODE_PARAMS* nodeParams), (phGraphNode, hGraph, dependencies, numDependencies, nodeParams))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuGraphAddMemAllocNode
CUDA_API(cuGraphAddMemAllocNode, (CUgraphNode * phGraphNode, CUgraph hGraph, const CUgraphNode* dependencies, size_t numDependencies, CUDA_MEM_ALLOC_NODE_PARAMS* nodeParams), (phGraphNode, hGraph, dependencies, numDependencies, nodeParams))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuGraphAddMemFreeNode
CUDA_API(cuGraphAddMemFreeNode, (CUgraphNode * phGraphNode, CUgraph hGraph, const CUgraphNode* dependencies, size_t numDependencies, CUdeviceptr dptr), (phGraphNode, hGraph, dependencies, numDependencies, dptr))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuGraphAddMemcpyNode
CUDA_API(cuGraphAddMemcpyNode, (CUgraphNode * phGraphNode, CUgraph hGraph, const CUgraphNode* dependencies, size_t numDependencies, const CUDA_MEMCPY3D* copyParams, CUcontext ctx), (phGraphNode, hGraph, dependencies, numDependencies, copyParams, ctx))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuGraphAddMemsetNode
CUDA_API(cuGraphAddMemsetNode, (CUgraphNode * phGraphNode, CUgraph hGraph, const CUgraphNode* dependencies, size_t numDependencies, const CUDA_MEMSET_NODE_PARAMS* memsetParams, CUcontext ctx), (phGraphNode, hGraph, dependencies, numDependencies, memsetParams, ctx))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuGraphAddNode
CUDA_API(cuGraphAddNode, (CUgraphNode * phGraphNode, CUgraph hGraph, const CUgraphNode* dependencies, size_t numDependencies, CUgraphNodeParams* nodeParams), (phGraphNode, hGraph, dependencies, numDependencies, nodeParams))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuGraphAddNode_v2
CUDA_API(cuGraphAddNode_v2, (CUgraphNode * phGraphNode, CUgraph hGraph, const CUgraphNode* dependencies, const CUgraphEdgeData* dependencyData, size_t numDependencies, CUgraphNodeParams* nodeParams), (phGraphNode, hGraph, dependencies, dependencyData, numDependencies, nodeParams))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuGraphBatchMemOpNodeGetParams
CUDA_API(cuGraphBatchMemOpNodeGetParams, (CUgraphNode hNode, CUDA_BATCH_MEM_OP_NODE_PARAMS* nodeParams_out), (hNode, nodeParams_out))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuGraphBatchMemOpNodeSetParams
CUDA_API(cuGraphBatchMemOpNodeSetParams, (CUgraphNode hNode, const CUDA_BATCH_MEM_OP_NODE_PARAMS* nodeParams), (hNode, nodeParams))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuGraphChildGraphNodeGetGraph
CUDA_API(cuGraphChildGraphNodeGetGraph, (CUgraphNode hNode, CUgraph* phGraph), (hNode, phGraph))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuGraphClone
CUDA_API(cuGraphClone, (CUgraph * phGraphClone, CUgraph originalGraph), (phGraphClone, originalGraph))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuGraphConditionalHandleCreate
CUDA_API(cuGraphConditionalHandleCreate, (CUgraphConditionalHandle * pHandle_out, CUgraph hGraph, CUcontext ctx, unsigned int defaultLaunchValue, unsigned int flags), (pHandle_out, hGraph, ctx, defaultLaunchValue, flags))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuGraphCreate
CUDA_API(cuGraphCreate, (CUgraph * phGraph, unsigned int flags), (phGraph, flags))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuGraphDebugDotPrint
CUDA_API(cuGraphDebugDotPrint, (CUgraph hGraph, const char* path, unsigned int flags), (hGraph, path, flags))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuGraphDestroy
CUDA_API(cuGraphDestroy, (CUgraph hGraph), (hGraph))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuGraphDestroyNode
CUDA_API(cuGraphDestroyNode, (CUgraphNode hNode), (hNode))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuGraphEventRecordNodeGetEvent
CUDA_API(cuGraphEventRecordNodeGetEvent, (CUgraphNode hNode, CUevent* event_out), (hNode, event_out))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuGraphEventRecordNodeSetEvent
CUDA_API(cuGraphEventRecordNodeSetEvent, (CUgraphNode hNode, CUevent event), (hNode, event))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuGraphEventWaitNodeGetEvent
CUDA_API(cuGraphEventWaitNodeGetEvent, (CUgraphNode hNode, CUevent* event_out), (hNode, event_out))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuGraphEventWaitNodeSetEvent
CUDA_API(cuGraphEventWaitNodeSetEvent, (CUgraphNode hNode, CUevent event), (hNode, event))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuGraphExecBatchMemOpNodeSetParams
CUDA_API(cuGraphExecBatchMemOpNodeSetParams, (CUgraphExec hGraphExec, CUgraphNode hNode, const CUDA_BATCH_MEM_OP_NODE_PARAMS* nodeParams), (hGraphExec, hNode, nodeParams))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuGraphExecChildGraphNodeSetParams
CUDA_API(cuGraphExecChildGraphNodeSetParams, (CUgraphExec hGraphExec, CUgraphNode hNode, CUgraph childGraph), (hGraphExec, hNode, childGraph))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuGraphExecDestroy
CUDA_API(cuGraphExecDestroy, (CUgraphExec hGraphExec), (hGraphExec))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuGraphExecEventRecordNodeSetEvent
CUDA_API(cuGraphExecEventRecordNodeSetEvent, (CUgraphExec hGraphExec, CUgraphNode hNode, CUevent event), (hGraphExec, hNode, event))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuGraphExecEventWaitNodeSetEvent
CUDA_API(cuGraphExecEventWaitNodeSetEvent, (CUgraphExec hGraphExec, CUgraphNode hNode, CUevent event), (hGraphExec, hNode, event))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuGraphExecExternalSemaphoresSignalNodeSetParams
CUDA_API(cuGraphExecExternalSemaphoresSignalNodeSetParams, (CUgraphExec hGraphExec, CUgraphNode hNode, const CUDA_EXT_SEM_SIGNAL_NODE_PARAMS* nodeParams), (hGraphExec, hNode, nodeParams))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuGraphExecExternalSemaphoresWaitNodeSetParams
CUDA_API(cuGraphExecExternalSemaphoresWaitNodeSetParams, (CUgraphExec hGraphExec, CUgraphNode hNode, const CUDA_EXT_SEM_WAIT_NODE_PARAMS* nodeParams), (hGraphExec, hNode, nodeParams))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuGraphExecGetFlags
CUDA_API(cuGraphExecGetFlags, (CUgraphExec hGraphExec, cuuint64_t* flags), (hGraphExec, flags))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuGraphExecHostNodeSetParams
CUDA_API(cuGraphExecHostNodeSetParams, (CUgraphExec hGraphExec, CUgraphNode hNode, const CUDA_HOST_NODE_PARAMS* nodeParams), (hGraphExec, hNode, nodeParams))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuGraphExecKernelNodeSetParams
CUDA_API(cuGraphExecKernelNodeSetParams, (CUgraphExec hGraphExec, CUgraphNode hNode, const CUDA_KERNEL_NODE_PARAMS* nodeParams), (hGraphExec, hNode, nodeParams))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuGraphExecMemcpyNodeSetParams
CUDA_API(cuGraphExecMemcpyNodeSetParams, (CUgraphExec hGraphExec, CUgraphNode hNode, const CUDA_MEMCPY3D* copyParams, CUcontext ctx), (hGraphExec, hNode, copyParams, ctx))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuGraphExecMemsetNodeSetParams
CUDA_API(cuGraphExecMemsetNodeSetParams, (CUgraphExec hGraphExec, CUgraphNode hNode, const CUDA_MEMSET_NODE_PARAMS* memsetParams, CUcontext ctx), (hGraphExec, hNode, memsetParams, ctx))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuGraphExecNodeSetParams
CUDA_API(cuGraphExecNodeSetParams, (CUgraphExec hGraphExec, CUgraphNode hNode, CUgraphNodeParams* nodeParams), (hGraphExec, hNode, nodeParams))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuGraphExecUpdate
CUDA_API(cuGraphExecUpdate, (CUgraphExec hGraphExec, CUgraph hGraph, CUgraphExecUpdateResultInfo* resultInfo), (hGraphExec, hGraph, resultInfo))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuGraphExternalSemaphoresSignalNodeGetParams
CUDA_API(cuGraphExternalSemaphoresSignalNodeGetParams, (CUgraphNode hNode, CUDA_EXT_SEM_SIGNAL_NODE_PARAMS* params_out), (hNode, params_out))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuGraphExternalSemaphoresSignalNodeSetParams
CUDA_API(cuGraphExternalSemaphoresSignalNodeSetParams, (CUgraphNode hNode, const CUDA_EXT_SEM_SIGNAL_NODE_PARAMS* nodeParams), (hNode, nodeParams))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuGraphExternalSemaphoresWaitNodeGetParams
CUDA_API(cuGraphExternalSemaphoresWaitNodeGetParams, (CUgraphNode hNode, CUDA_EXT_SEM_WAIT_NODE_PARAMS* params_out), (hNode, params_out))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuGraphExternalSemaphoresWaitNodeSetParams
CUDA_API(cuGraphExternalSemaphoresWaitNodeSetParams, (CUgraphNode hNode, const CUDA_EXT_SEM_WAIT_NODE_PARAMS* nodeParams), (hNode, nodeParams))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuGraphGetEdges
CUDA_API(cuGraphGetEdges, (CUgraph hGraph, CUgraphNode* from, CUgraphNode* to, size_t* numEdges), (hGraph, from, to, numEdges))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuGraphGetEdges_v2
CUDA_API(cuGraphGetEdges_v2, (CUgraph hGraph, CUgraphNode* from, CUgraphNode* to, CUgraphEdgeData* edgeData, size_t* numEdges), (hGraph, from, to, edgeData, numEdges))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuGraphGetNodes
CUDA_API(cuGraphGetNodes, (CUgraph hGraph, CUgraphNode* nodes, size_t* numNodes), (hGraph, nodes, numNodes))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuGraphGetRootNodes
CUDA_API(cuGraphGetRootNodes, (CUgraph hGraph, CUgraphNode* rootNodes, size_t* numRootNodes), (hGraph, rootNodes, numRootNodes))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuGraphHostNodeGetParams
CUDA_API(cuGraphHostNodeGetParams, (CUgraphNode hNode, CUDA_HOST_NODE_PARAMS* nodeParams), (hNode, nodeParams))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuGraphHostNodeSetParams
CUDA_API(cuGraphHostNodeSetParams, (CUgraphNode hNode, const CUDA_HOST_NODE_PARAMS* nodeParams), (hNode, nodeParams))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuGraphInstantiate
CUDA_API(cuGraphInstantiate, (CUgraphExec * phGraphExec, CUgraph hGraph, unsigned long long flags), (phGraphExec, hGraph, flags))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuGraphInstantiateWithParams
CUDA_API(cuGraphInstantiateWithParams, (CUgraphExec * phGraphExec, CUgraph hGraph, CUDA_GRAPH_INSTANTIATE_PARAMS* instantiateParams), (phGraphExec, hGraph, instantiateParams))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuGraphKernelNodeCopyAttributes
CUDA_API(cuGraphKernelNodeCopyAttributes, (CUgraphNode dst, CUgraphNode src), (dst, src))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuGraphKernelNodeGetAttribute
CUDA_API(cuGraphKernelNodeGetAttribute, (CUgraphNode hNode, CUkernelNodeAttrID attr, CUkernelNodeAttrValue* value_out), (hNode, attr, value_out))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuGraphKernelNodeGetParams
CUDA_API(cuGraphKernelNodeGetParams, (CUgraphNode hNode, CUDA_KERNEL_NODE_PARAMS* nodeParams), (hNode, nodeParams))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuGraphKernelNodeSetAttribute
CUDA_API(cuGraphKernelNodeSetAttribute, (CUgraphNode hNode, CUkernelNodeAttrID attr, const CUkernelNodeAttrValue* value), (hNode, attr, value))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuGraphKernelNodeSetParams
CUDA_API(cuGraphKernelNodeSetParams, (CUgraphNode hNode, const CUDA_KERNEL_NODE_PARAMS* nodeParams), (hNode, nodeParams))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuGraphLaunch
CUDA_API(cuGraphLaunch, (CUgraphExec hGraphExec, CUstream hStream), (hGraphExec, hStream))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuGraphMemAllocNodeGetParams
CUDA_API(cuGraphMemAllocNodeGetParams, (CUgraphNode hNode, CUDA_MEM_ALLOC_NODE_PARAMS* params_out), (hNode, params_out))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuGraphMemFreeNodeGetParams
CUDA_API(cuGraphMemFreeNodeGetParams, (CUgraphNode hNode, CUdeviceptr* dptr_out), (hNode, dptr_out))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuGraphMemcpyNodeGetParams
CUDA_API(cuGraphMemcpyNodeGetParams, (CUgraphNode hNode, CUDA_MEMCPY3D* nodeParams), (hNode, nodeParams))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuGraphMemcpyNodeSetParams
CUDA_API(cuGraphMemcpyNodeSetParams, (CUgraphNode hNode, const CUDA_MEMCPY3D* nodeParams), (hNode, nodeParams))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuGraphMemsetNodeGetParams
CUDA_API(cuGraphMemsetNodeGetParams, (CUgraphNode hNode, CUDA_MEMSET_NODE_PARAMS* nodeParams), (hNode, nodeParams))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuGraphMemsetNodeSetParams
CUDA_API(cuGraphMemsetNodeSetParams, (CUgraphNode hNode, const CUDA_MEMSET_NODE_PARAMS* nodeParams), (hNode, nodeParams))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuGraphNodeFindInClone
CUDA_API(cuGraphNodeFindInClone, (CUgraphNode * phNode, CUgraphNode hOriginalNode, CUgraph hClonedGraph), (phNode, hOriginalNode, hClonedGraph))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuGraphNodeGetDependencies
CUDA_API(cuGraphNodeGetDependencies, (CUgraphNode hNode, CUgraphNode* dependencies, size_t* numDependencies), (hNode, dependencies, numDependencies))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuGraphNodeGetDependencies_v2
CUDA_API(cuGraphNodeGetDependencies_v2, (CUgraphNode hNode, CUgraphNode* dependencies, CUgraphEdgeData* edgeData, size_t* numDependencies), (hNode, dependencies, edgeData, numDependencies))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuGraphNodeGetDependentNodes
CUDA_API(cuGraphNodeGetDependentNodes, (CUgraphNode hNode, CUgraphNode* dependentNodes, size_t* numDependentNodes), (hNode, dependentNodes, numDependentNodes))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuGraphNodeGetDependentNodes_v2
CUDA_API(cuGraphNodeGetDependentNodes_v2, (CUgraphNode hNode, CUgraphNode* dependentNodes, CUgraphEdgeData* edgeData, size_t* numDependentNodes), (hNode, dependentNodes, edgeData, numDependentNodes))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuGraphNodeGetEnabled
CUDA_API(cuGraphNodeGetEnabled, (CUgraphExec hGraphExec, CUgraphNode hNode, unsigned int* isEnabled), (hGraphExec, hNode, isEnabled))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuGraphNodeGetType
CUDA_API(cuGraphNodeGetType, (CUgraphNode hNode, CUgraphNodeType* type), (hNode, type))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuGraphNodeSetEnabled
CUDA_API(cuGraphNodeSetEnabled, (CUgraphExec hGraphExec, CUgraphNode hNode, unsigned int isEnabled), (hGraphExec, hNode, isEnabled))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuGraphNodeSetParams
CUDA_API(cuGraphNodeSetParams, (CUgraphNode hNode, CUgraphNodeParams* nodeParams), (hNode, nodeParams))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuGraphReleaseUserObject
CUDA_API(cuGraphReleaseUserObject, (CUgraph graph, CUuserObject object, unsigned int count), (graph, object, count))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuGraphRemoveDependencies
CUDA_API(cuGraphRemoveDependencies, (CUgraph hGraph, const CUgraphNode* from, const CUgraphNode* to, size_t numDependencies), (hGraph, from, to, numDependencies))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuGraphRemoveDependencies_v2
CUDA_API(cuGraphRemoveDependencies_v2, (CUgraph hGraph, const CUgraphNode* from, const CUgraphNode* to, const CUgraphEdgeData* edgeData, size_t numDependencies), (hGraph, from, to, edgeData, numDependencies))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuGraphRetainUserObject
CUDA_API(cuGraphRetainUserObject, (CUgraph graph, CUuserObject object, unsigned int count, unsigned int flags), (graph, object, count, flags))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuGraphUpload
CUDA_API(cuGraphUpload, (CUgraphExec hGraphExec, CUstream hStream), (hGraphExec, hStream))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuUserObjectCreate
CUDA_API(cuUserObjectCreate, (CUuserObject * object_out, void* ptr, CUhostFn destroy, unsigned int initialRefcount, unsigned int flags), (object_out, ptr, destroy, initialRefcount, flags))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuUserObjectRelease
CUDA_API(cuUserObjectRelease, (CUuserObject object, unsigned int count), (object, count))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuUserObjectRetain
CUDA_API(cuUserObjectRetain, (CUuserObject object, unsigned int count), (object, count))
#endif

// Occupancy
#if SYZ_EXECUTOR || __NR_syz_cuOccupancyAvailableDynamicSMemPerBlock
CUDA_API(cuOccupancyAvailableDynamicSMemPerBlock, (size_t * dynamicSmemSize, CUfunction func, int numBlocks, int blockSize), (dynamicSmemSize, func, numBlocks, blockSize))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuOccupancyMaxActiveBlocksPerMultiprocessor
CUDA_API(cuOccupancyMaxActiveBlocksPerMultiprocessor, (int* numBlocks, CUfunction func, int blockSize, size_t dynamicSMemSize), (numBlocks, func, blockSize, dynamicSMemSize))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuOccupancyMaxActiveBlocksPerMultiprocessorWithFlags
CUDA_API(cuOccupancyMaxActiveBlocksPerMultiprocessorWithFlags, (int* numBlocks, CUfunction func, int blockSize, size_t dynamicSMemSize, unsigned int flags), (numBlocks, func, blockSize, dynamicSMemSize, flags))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuOccupancyMaxActiveClusters
CUDA_API(cuOccupancyMaxActiveClusters, (int* numClusters, CUfunction func, const CUlaunchConfig* config), (numClusters, func, config))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuOccupancyMaxPotentialBlockSize
CUDA_API(cuOccupancyMaxPotentialBlockSize, (int* minGridSize, int* blockSize, CUfunction func, CUoccupancyB2DSize blockSizeToDynamicSMemSize, size_t dynamicSMemSize, int blockSizeLimit), (minGridSize, blockSize, func, blockSizeToDynamicSMemSize, dynamicSMemSize, blockSizeLimit))
#endif
// CUDA_API(cuOccupancyMaxPotentialBlockSizeWithFlags, (int* minGridSize, int* blockSize, CUfunction func, CUoccupancyB2DSize blockSizeToDynamicSMemSize, size_t dynamicSMemSize, int blockSizeLimit, unsigned int flags), (minGridSize, blockSize, func, blockSizeToDynamicSMemSize, dynamicSMemSize, blockSizeLimit, flags))

// Texture object management
#if SYZ_EXECUTOR || __NR_syz_cuTexObjectCreate
CUDA_API(cuTexObjectCreate, (CUtexObject * pTexObject, const CUDA_RESOURCE_DESC* pResDesc, const CUDA_TEXTURE_DESC* pTexDesc, const CUDA_RESOURCE_VIEW_DESC* pResViewDesc), (pTexObject, pResDesc, pTexDesc, pResViewDesc))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuTexObjectDestroy
CUDA_API(cuTexObjectDestroy, (CUtexObject texObject), (texObject))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuTexObjectGetResourceDesc
CUDA_API(cuTexObjectGetResourceDesc, (CUDA_RESOURCE_DESC * pResDesc, CUtexObject texObject), (pResDesc, texObject))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuTexObjectGetResourceViewDesc
CUDA_API(cuTexObjectGetResourceViewDesc, (CUDA_RESOURCE_VIEW_DESC * pResViewDesc, CUtexObject texObject), (pResViewDesc, texObject))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuTexObjectGetTextureDesc
CUDA_API(cuTexObjectGetTextureDesc, (CUDA_TEXTURE_DESC * pTexDesc, CUtexObject texObject), (pTexDesc, texObject))
#endif

// Surface object management
#if SYZ_EXECUTOR || __NR_syz_cuSurfObjectCreate
CUDA_API(cuSurfObjectCreate, (CUsurfObject * pSurfObject, const CUDA_RESOURCE_DESC* pResDesc), (pSurfObject, pResDesc))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuSurfObjectDestroy
CUDA_API(cuSurfObjectDestroy, (CUsurfObject surfObject), (surfObject))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuSurfObjectGetResourceDesc
CUDA_API(cuSurfObjectGetResourceDesc, (CUDA_RESOURCE_DESC * pResDesc, CUsurfObject surfObject), (pResDesc, surfObject))
#endif

// Tensor map object management

// Peer
#if SYZ_EXECUTOR || __NR_syz_cuCtxDisablePeerAccess
CUDA_API(cuCtxDisablePeerAccess, (CUcontext peerContext), (peerContext))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuCtxEnablePeerAccess
CUDA_API(cuCtxEnablePeerAccess, (CUcontext peerContext, unsigned int Flags), (peerContext, Flags))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuDeviceCanAccessPeer
CUDA_API(cuDeviceCanAccessPeer, (int* canAccessPeer, int dev, int peerDev), (canAccessPeer, dev, peerDev))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuDeviceGetP2PAttribute
CUDA_API(cuDeviceGetP2PAttribute, (int* value, CUdevice_P2PAttribute attrib, int srcDevice, int dstDevice), (value, attrib, srcDevice, dstDevice))
#endif

// Interoperatiability

// Green context

#if SYZ_EXECUTOR || __NR_syz_cuCtxFromGreenCtx
CUDA_API(cuCtxFromGreenCtx, (CUcontext * pContext, CUgreenCtx hCtx), (pContext, hCtx))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuCtxGetDevResource
CUDA_API(cuCtxGetDevResource, (CUcontext hCtx, CUdevResource* resource, CUdevResourceType type), (hCtx, resource, type))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuDevResourceGenerateDesc
CUDA_API(cuDevResourceGenerateDesc, (CUdevResourceDesc * phDesc, CUdevResource* resources, unsigned int nbResources), (phDesc, resources, nbResources))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuDevSmResourceSplitByCount
CUDA_API(cuDevSmResourceSplitByCount, (CUdevResource * result, unsigned int* nbGroups, const CUdevResource* input, CUdevResource* remaining, unsigned int useFlags, unsigned int minCount), (result, nbGroups, input, remaining, useFlags, minCount))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuDeviceGetDevResource
CUDA_API(cuDeviceGetDevResource, (CUdevice device, CUdevResource* resource, CUdevResourceType type), (device, resource, type))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuGreenCtxCreate
CUDA_API(cuGreenCtxCreate, (CUgreenCtx * phCtx, CUdevResourceDesc desc, CUdevice dev, unsigned int flags), (phCtx, desc, dev, flags))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuGreenCtxDestroy
CUDA_API(cuGreenCtxDestroy, (CUgreenCtx hCtx), (hCtx))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuGreenCtxGetDevResource
CUDA_API(cuGreenCtxGetDevResource, (CUgreenCtx hCtx, CUdevResource* resource, CUdevResourceType type), (hCtx, resource, type))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuGreenCtxRecordEvent
CUDA_API(cuGreenCtxRecordEvent, (CUgreenCtx hCtx, CUevent hEvent), (hCtx, hEvent))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuGreenCtxWaitEvent
CUDA_API(cuGreenCtxWaitEvent, (CUgreenCtx hCtx, CUevent hEvent), (hCtx, hEvent))
#endif

#if SYZ_EXECUTOR || __NR_syz_cuStreamGetGreenCtx
CUDA_API(cuStreamGetGreenCtx, (CUstream hStream, CUgreenCtx* phCtx), (hStream, phCtx))
#endif
// CUDA_API(cuGreenCtxStreamCreate, (CUstream * phStream, CUgreenCtx greenCtx, unsigned int flags, int priority), (phStream, greenCtx, flags, priority))