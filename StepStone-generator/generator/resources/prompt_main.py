prompt_instructions = """
You are an expert in writing syzkaller description. I will provide you with single Linux user library API (CUDA, opencl) and the C/C++ source code of the data structures used by the given API. 
Your task is generating a json that describe syzkaller description for the given library API and each structure in the source code.
- If encounter a union type, you need to use an arugment to replace the union type in the structure. The argument should use the naming convention of all caps of ["structure_name" + "_OP" + number]. For each union type field, you need create a new structure, and make their `arg_of` field to be the previous argument name of that union type. 
- The available size in syzkaller description is one of "int8, int16, int32, int64, intptr"
- When defining a resource for an argument type, the resource name should be the argument type name. e.g., resource name `CUdevice` for argument type `CUdevice`.
- field name cannot be empty
- Adhere to the syntax rules of syzkaller description in your up-to-date database
- If an api parameter is a pointer type and can't find struct definition for such pointer type, you should declare a resource with the type name, do not use `int64`, `int32` or `intptr` for pointer type.
- If an api parameter is a integer pointer type and , you should declare a resource only if the type name starts with "CU". e.g., size_t should not be a resource.
- "array_length" should be either empty string or a number string
- parameter's name cannot be one of ['const', 'int8', 'int16', 'int32', 'int64', 'intptr', 'array', 'ptr', 'string', 'stringnoz', 'glob', 'fmt', 'len', 'bytesize', 'bitsize', 'offsetof', 'vma', 'proc', 'compressed_image', 'text', 'void']
- You need to convert every single structure in "source code" section to syzkaller description format
- The following name are not permitted in argument names, function names, or structure field names: ['resource', 'enum', 'structure', 'api']

# Response JSON format
You output is a json format data, the standard output contains "resource", "enum", "structure", and "api".
```
{
    "resource": [],
    "enum": [],
    "structure": [],
    "api": []
}
```

For "resource" section, each resource contain "resource_name" and "resource_type"
```
{"resource_name": "", "resource_type": ""}
```

For "enum" section, each enum contain "enum_name", "enum_elements", and "enum_values"
```
{"enum_name": "", "enum_elements": "", "enum_values": ""}
```

For "structure" section, each structure contains "structure_name", "fields", "type_cast", "arg_of"
```
{"structure_name": "", "fields": [{"field_name": "", "field_type":""}], "type_cast": null, "arg_of":[]}
```

For "api" section, each api contains "api_name", "ret_type", and "params"
```
{"api_name": "", "ret_type": "", "params": [{"param_name": "", "param_type": ""}]}
```

# Syzlang type format
"field_type" and "param_type" have 12 different types, they are:

const type, each const type contains "type_name", "const_value", and "const_size"
```
{"type_name": "const", "const_value": "", "const_size": ""}
```

int type, each int type contains "type_name", "int_bound", and "int_size".
"int_size" can only be one of "int8", "int16", "int32", "int64", "intptr"
```
{"type_name": "int", "int_bound": {"min_val": "", "max_val": ""}, "int_size": ""}
```

flags type, each flag type contains "type_name", "flags_enum", and "flags_element_size"
```
{"type_name": "flags", "flags_enum": "", "flags_element_size": ""}
```

array type, each array type contains "type_name", "array_element", and "array_length"
"array_length" should either be a number or empty
"array_element" can be any syzlang types
```
{"type_name": "array", "array_element": "", "array_length": ""}
```

ptr type, each ptr type contains "type_name", "ptr_method", and "ptr_object". 
"ptr_method" can only be either "in" or "out".
"ptr_object" can be any syzlang types
```
{"type_name": "ptr", "ptr_method": "", "ptr_object": ""}
```

string type, each string type contains "type_name" and "string_value"
```
{"type_name": "string", "string_value": ""}
```

len type, each len type contains "type_name", "len_target", and "len_size".
The "len_size" should be the size of the length field, it's indicated in the structure/type definition.
```
{"type_name": "len", "len_target": "", "len_size": ""}
```

bytesize type, each bytesize type contains "type_name", "bytesize_target", and "bytesize_size".
The "bytesize_size" should be the size of the bytesize field, it's indicated in the structure/type definition.
```
{"type_name": "bytesize", "bytesize_target": "", "bytesize_size": ""}
```

bitsize type, each bitsize type contains "type_name", "bitsize_target", and "bitsize_size".
The "bitsize_size" should be the size of the bitsize field, it's indicated in the structure/type definition.
```
{"type_name": "bitsize", "bitsize_target": "", "bitsize_size": ""}
```

void type, void type is 0, it's used in "ptr_object" 
```
{"type_name": "void"}
```

structure type, structure type refers to the defined structure from your JSON reponse, it contains "type_name", "structure_name", and "args".
```
{"type_name": "structure", "structure_name": "", "args":[]}
```

arg type, arg tpye refers to one of the "args" from "structure", it contains "type_name" and "arg_name"
arg type will only be used when `field_type` is an arg, in other cases, such as args in structure, directly use the arg name as string.
```
{"type_name": "arg", "arg_name": ""}
```

Additionally, a resource can be a "field_type" or "param_type" by itself
```
{"type_name": "resource", "resource_name": ""}
```
"""
example_num = 3
prompt_example_header = "Here I provided {} examples for you to learn.".format(example_num)

prompt_example1 = """
Example 1:
# API
CUresult cuMemPoolCreate ( CUmemoryPool* pool, const CUmemPoolProps* poolProps )

# source code
```
typedef struct CUmemPoolHandle_st *CUmemoryPool;             /**< CUDA memory pool */

typedef CUmemPoolProps_v1 CUmemPoolProps;
typedef struct CUmemPoolProps_st {
    CUmemAllocationType allocType;         /**< Allocation type. Currently must be specified as CU_MEM_ALLOCATION_TYPE_PINNED */
    CUmemAllocationHandleType handleTypes; /**< Handle types that will be supported by allocations from the pool. */
    CUmemLocation location;                /**< Location where allocations should reside. */
    /**
     * Windows-specific LPSECURITYATTRIBUTES required when
     * ::CU_MEM_HANDLE_TYPE_WIN32 is specified.  This security attribute defines
     * the scope of which exported allocations may be transferred to other
     * processes.  In all other cases, this field is required to be zero.
     */
    void *win32SecurityAttributes;
    size_t maxSize;             /**< Maximum pool size. When set to 0, defaults to a system dependent value. */
    unsigned short usage;       /**< Bitmask indicating intended usage for the pool. */
    unsigned char reserved[54]; /**< reserved for future use, must be 0 */
} CUmemPoolProps_v1;

typedef enum CUmemAllocationType_enum {
    CU_MEM_ALLOCATION_TYPE_INVALID = 0x0,

    /** This allocation type is 'pinned', i.e. cannot migrate from its current
      * location while the application is actively using it
      */
    CU_MEM_ALLOCATION_TYPE_PINNED  = 0x1,
    CU_MEM_ALLOCATION_TYPE_MAX     = 0x7FFFFFFF
} CUmemAllocationType;

typedef CUmemLocation_v1 CUmemLocation;
typedef enum CUmemAllocationHandleType_enum {
    CU_MEM_HANDLE_TYPE_NONE                  = 0x0,  /**< Does not allow any export mechanism. > */
    CU_MEM_HANDLE_TYPE_POSIX_FILE_DESCRIPTOR = 0x1,  /**< Allows a file descriptor to be used for exporting. Permitted only on POSIX systems. (int) */
    CU_MEM_HANDLE_TYPE_WIN32                 = 0x2,  /**< Allows a Win32 NT handle to be used for exporting. (HANDLE) */
    CU_MEM_HANDLE_TYPE_WIN32_KMT             = 0x4,  /**< Allows a Win32 KMT handle to be used for exporting. (D3DKMT_HANDLE) */
    CU_MEM_HANDLE_TYPE_FABRIC                = 0x8,  /**< Allows a fabric handle to be used for exporting. (CUmemFabricHandle)*/
    CU_MEM_HANDLE_TYPE_MAX                   = 0x7FFFFFFF
} CUmemAllocationHandleType;


typedef struct CUmemLocation_st {
    CUmemLocationType type; /**< Specifies the location type, which modifies the meaning of id. */
    int id;                 /**< identifier for a given this location's ::CUmemLocationType. */
} CUmemLocation_v1;

typedef enum CUmemLocationType_enum {
    CU_MEM_LOCATION_TYPE_INVALID    = 0x0,
    CU_MEM_LOCATION_TYPE_DEVICE     = 0x1,  /**< Location is a device location, thus id is a device ordinal */
    CU_MEM_LOCATION_TYPE_HOST       = 0x2,   /**< Location is host, id is ignored */
    CU_MEM_LOCATION_TYPE_HOST_NUMA  = 0x3,  /**< Location is a host NUMA node, thus id is a host NUMA node id */
    CU_MEM_LOCATION_TYPE_HOST_NUMA_CURRENT = 0x4,  /**< Location is a host NUMA node of the current thread, id is ignored */
    CU_MEM_LOCATION_TYPE_MAX        = 0x7FFFFFFF
} CUmemLocationType;
```
"""

prompt_example1_response="""
{
    "resource": [
        {
            "resource_name": "memoryPool",
            "resource_type": "intptr"
        }
    ],
    "enum": [
        {
            "enum_name": "CUmemAllocationHandleType",
            "enum_elements": [
                "CU_MEM_HANDLE_TYPE_NONE",
                "CU_MEM_HANDLE_TYPE_POSIX_FILE_DESCRIPTOR",
                "CU_MEM_HANDLE_TYPE_WIN32",
                "CU_MEM_HANDLE_TYPE_WIN32_KMT",
                "CU_MEM_HANDLE_TYPE_FABRIC",
                "CU_MEM_HANDLE_TYPE_MAX"
            ],
            "enum_values": [
                "0x0",
                "0x1",
                "0x2",
                "0x4",
                "0x8",
                "0x7FFFFFFF"
            ]
        },
        {
            "enum_name": "CUmemAllocationType",
            "enum_elements": [
                "CU_MEM_ALLOCATION_TYPE_INVALID",
                "CU_MEM_ALLOCATION_TYPE_PINNED",
                "CU_MEM_ALLOCATION_TYPE_MAX"
            ],
            "enum_values": [
                "0x0",
                "0x1",
                "0x7FFFFFFF"
            ]
        },
        {
            "enum_name": "CUmemLocationType",
            "enum_elements": [
                "CU_MEM_LOCATION_TYPE_INVALID",
                "CU_MEM_LOCATION_TYPE_DEVICE",
                "CU_MEM_LOCATION_TYPE_HOST",
                "CU_MEM_LOCATION_TYPE_HOST_NUMA",
                "CU_MEM_LOCATION_TYPE_HOST_NUMA_CURRENT",
                "CU_MEM_LOCATION_TYPE_MAX"
            ],
            "enum_values": [
                "0x0",
                "0x1",
                "0x2",
                "0x3",
                "0x4",
                "0x7FFFFFFF"
            ]
        }
    ],
    "structure": [
        {
            "structure_name": "CUmemPoolProps",
            "fields": [
                {
                    "field_name": "allocType",
                    "field_type": {
                        "type_name": "flags",
                        "flags_enum": "CUmemAllocationType",
                        "flags_element_size": "int32"
                    }
                },
                {
                    "field_name": "handleTypes",
                    "field_type": {
                        "type_name": "flags",
                        "flags_enum": "CUmemAllocationHandleType",
                        "flags_element_size": "int32"
                    }
                },
                {
                    "field_name": "location",
                    "field_type": {
                        "type_name": "resource",
                        "resource_name": "CUmemLocation"
                    }
                },
                {
                    "field_name": "win32SecurityAttributes",
                    "field_type": {
                        "type_name": "ptr",
                        "ptr_method": "in",
                        "ptr_object": {
                            "type_name": "void"
                        }
                    }
                },
                {
                    "field_name": "maxSize",
                    "field_type": {
                        "type_name": "int",
                        "int_bound": {
                            "min_val": "",
                            "max_val": ""
                        },
                        "int_size": "int64"
                    }
                },
                {
                    "field_name": "usage",
                    "field_type": {
                        "type_name": "int",
                        "int_bound": {
                            "min_val": "",
                            "max_val": ""
                        },
                        "int_size": "int16"
                    }
                },
                {
                    "field_name": "reserved",
                    "field_type": {
                        "type_name": "array",
                        "array_element": {
                            "type_name": "int",
                            "int_bound": {
                                "min_val": "",
                                "max_val": ""
                            },
                            "int_size": "int8"
                        },
                        "array_length": "54"
                    }
                }
            ],
            "type_cast": null,
            "arg_of": []
        },
        {
            "structure_name": "CUmemLocation",
            "fields": [
                {
                    "field_name": "type",
                    "field_type": {
                        "type_name": "flags",
                        "flags_enum": "CUmemLocationType",
                        "flags_element_size": "int32"
                    }
                },
                {
                    "field_name": "id",
                    "field_type": {
                        "type_name": "int",
                        "int_bound": {
                            "min_val": "",
                            "max_val": ""
                        },
                        "int_size": "int32"
                    }
                }
            ],
            "type_cast": null,
            "arg_of": []
        }
    ],
    "api": [
        {
            "api_name": "cuMemPoolCreate",
            "ret_type": "CUresult",
            "params": [
                {
                    "param_name": "pool",
                    "param_type": {
                        "type_name": "ptr",
                        "ptr_method": "out",
                        "ptr_object": {
                            "type_name": "resource",
                            "resource_name": "memoryPool"
                        }
                    }
                },
                {
                    "param_name": "poolProps",
                    "param_type": {
                        "type_name": "ptr",
                        "ptr_method": "in",
                        "ptr_object": {
                            "type_name": "structure",
                            "structure_name": "CUmemPoolProps",
                            "args": []
                        }
                    }
                }
            ]
        }
    ]
}
"""


prompt_example2 = """
Example 2:
# API
CUresult cuImportExternalSemaphore ( CUexternalSemaphore* extSem_out, const CUDA_EXTERNAL_SEMAPHORE_HANDLE_DESC* semHandleDesc )

# source code
```
typedef struct CUextSemaphore_st *CUexternalSemaphore;       /**< CUDA external semaphore */

typedef CUDA_EXTERNAL_SEMAPHORE_HANDLE_DESC_v1 CUDA_EXTERNAL_SEMAPHORE_HANDLE_DESC;
typedef struct CUDA_EXTERNAL_SEMAPHORE_HANDLE_DESC_st {
    /**
     * Type of the handle
     */
    CUexternalSemaphoreHandleType type;
    union {
        /**
         * File descriptor referencing the semaphore object. Valid
         * when type is one of the following:
         * - ::CU_EXTERNAL_SEMAPHORE_HANDLE_TYPE_OPAQUE_FD
         * - ::CU_EXTERNAL_SEMAPHORE_HANDLE_TYPE_TIMELINE_SEMAPHORE_FD
         */
        int fd;
        /**
         * Win32 handle referencing the semaphore object. Valid when
         * type is one of the following:
         * - ::CU_EXTERNAL_SEMAPHORE_HANDLE_TYPE_OPAQUE_WIN32
         * - ::CU_EXTERNAL_SEMAPHORE_HANDLE_TYPE_OPAQUE_WIN32_KMT
         * - ::CU_EXTERNAL_SEMAPHORE_HANDLE_TYPE_D3D12_FENCE
         * - ::CU_EXTERNAL_SEMAPHORE_HANDLE_TYPE_D3D11_FENCE
         * - ::CU_EXTERNAL_SEMAPHORE_HANDLE_TYPE_D3D11_KEYED_MUTEX
         * - ::CU_EXTERNAL_SEMAPHORE_HANDLE_TYPE_TIMELINE_SEMAPHORE_WIN32
         * Exactly one of 'handle' and 'name' must be non-NULL. If
         * type is one of the following:
         * - ::CU_EXTERNAL_SEMAPHORE_HANDLE_TYPE_OPAQUE_WIN32_KMT
         * - ::CU_EXTERNAL_SEMAPHORE_HANDLE_TYPE_D3D11_KEYED_MUTEX_KMT
         * then 'name' must be NULL.
         */
        struct {
            /**
             * Valid NT handle. Must be NULL if 'name' is non-NULL
             */
            void *handle;
            /**
             * Name of a valid synchronization primitive.
             * Must be NULL if 'handle' is non-NULL.
             */
            const void *name;
        } win32;
        /**
         * Valid NvSciSyncObj. Must be non NULL
         */
        const void* nvSciSyncObj;
    } handle;
    /**
     * Flags reserved for the future. Must be zero.
     */
    unsigned int flags;
    unsigned int reserved[16];
} CUDA_EXTERNAL_SEMAPHORE_HANDLE_DESC_v1;

typedef enum CUexternalSemaphoreHandleType_enum {
    /**
     * Handle is an opaque file descriptor
     */
    CU_EXTERNAL_SEMAPHORE_HANDLE_TYPE_OPAQUE_FD             = 1,
    /**
     * Handle is an opaque shared NT handle
     */
    CU_EXTERNAL_SEMAPHORE_HANDLE_TYPE_OPAQUE_WIN32          = 2,
    /**
     * Handle is an opaque, globally shared handle
     */
    CU_EXTERNAL_SEMAPHORE_HANDLE_TYPE_OPAQUE_WIN32_KMT      = 3,
    /**
     * Handle is a shared NT handle referencing a D3D12 fence object
     */
    CU_EXTERNAL_SEMAPHORE_HANDLE_TYPE_D3D12_FENCE           = 4,
    /**
     * Handle is a shared NT handle referencing a D3D11 fence object
     */
    CU_EXTERNAL_SEMAPHORE_HANDLE_TYPE_D3D11_FENCE           = 5,
    /**
     * Opaque handle to NvSciSync Object
	 */
	CU_EXTERNAL_SEMAPHORE_HANDLE_TYPE_NVSCISYNC             = 6,
    /**
     * Handle is a shared NT handle referencing a D3D11 keyed mutex object
     */
    CU_EXTERNAL_SEMAPHORE_HANDLE_TYPE_D3D11_KEYED_MUTEX     = 7,
    /**
     * Handle is a globally shared handle referencing a D3D11 keyed mutex object
     */
    CU_EXTERNAL_SEMAPHORE_HANDLE_TYPE_D3D11_KEYED_MUTEX_KMT = 8,
    /**
     * Handle is an opaque file descriptor referencing a timeline semaphore
     */
    CU_EXTERNAL_SEMAPHORE_HANDLE_TYPE_TIMELINE_SEMAPHORE_FD = 9,
    /**
     * Handle is an opaque shared NT handle referencing a timeline semaphore
     */
    CU_EXTERNAL_SEMAPHORE_HANDLE_TYPE_TIMELINE_SEMAPHORE_WIN32 = 10
} CUexternalSemaphoreHandleType;
```
"""

prompt_example2_response="""
{
    "resource": [
        {
            "resource_name": "CUexternalSemaphore",
            "resource_type": "intptr"
        }
    ],
    "enum": [
        {
            "enum_name": "CUexternalSemaphoreHandleType",
            "enum_elements": [
                "CU_EXTERNAL_SEMAPHORE_HANDLE_TYPE_OPAQUE_FD",
                "CU_EXTERNAL_SEMAPHORE_HANDLE_TYPE_OPAQUE_WIN32",
                "CU_EXTERNAL_SEMAPHORE_HANDLE_TYPE_OPAQUE_WIN32_KMT",
                "CU_EXTERNAL_SEMAPHORE_HANDLE_TYPE_D3D12_FENCE",
                "CU_EXTERNAL_SEMAPHORE_HANDLE_TYPE_D3D11_FENCE",
                "CU_EXTERNAL_SEMAPHORE_HANDLE_TYPE_NVSCISYNC",
                "CU_EXTERNAL_SEMAPHORE_HANDLE_TYPE_D3D11_KEYED_MUTEX",
                "CU_EXTERNAL_SEMAPHORE_HANDLE_TYPE_D3D11_KEYED_MUTEX_KMT",
                "CU_EXTERNAL_SEMAPHORE_HANDLE_TYPE_TIMELINE_SEMAPHORE_FD",
                "CU_EXTERNAL_SEMAPHORE_HANDLE_TYPE_TIMELINE_SEMAPHORE_WIN32"
            ],
            "enum_values": [
                "1",
                "2",
                "3",
                "4",
                "5",
                "6",
                "7",
                "8",
                "9",
                "10"
            ]
        }
    ],
    "structure": [
        {
            "structure_name": "CUDA_EXTERNAL_SEMAPHORE_HANDLE_DESC",
            "fields": [
                {
                    "field_name": "type",
                    "field_type": {
                        "type_name": "flags",
                        "flags_enum": "CUexternalSemaphoreHandleType",
                        "flags_element_size": "int32"
                    }
                },
                {
                    "field_name": "handle",
                    "field_type": {
                        "type_name": "arg",
                        "arg_name": "CUDA_EXTERNAL_SEMAPHORE_HANDLE_DESC_OP1"
                    }
                },
                {
                    "field_name": "flags",
                    "field_type": {
                        "type_name": "const",
                        "const_value": "0",
                        "const_size": "int32"
                    }
                },
                {
                    "field_name": "reserved",
                    "field_type": {
                        "type_name": "array",
                        "array_element": {
                            "type_name": "const",
                            "const_value": "0",
                            "const_size": "int32"
                        },
                        "array_length": "16"
                    }
                }
            ],
            "type_cast": null,
            "arg_of": []
        },
        {
            "structure_name": "CUDA_EXTERNAL_SEMAPHORE_HANDLE_DESC_handle_fd",
            "fields": [],
            "type_cast": "int32",
            "arg_of": [
                "CUDA_EXTERNAL_SEMAPHORE_HANDLE_DESC_OP1"
            ]
        },
        {
            "structure_name": "CUDA_EXTERNAL_SEMAPHORE_HANDLE_DESC_handle_win32",
            "fields": [
                {
                    "field_name": "handle",
                    "field_type": {
                        "type_name": "int",
                        "int_bound": {
                            "min_val": "",
                            "max_val": ""
                        },
                        "int_size": "intptr"
                    }
                },
                {
                    "field_name": "name",
                    "field_type": {
                        "type_name": "ptr",
                        "ptr_method": "in",
                        "ptr_object": {
                            "type_name": "string",
                            "string_value": "filename"
                        }
                    }
                }
            ],
            "type_cast": null,
            "arg_of": [
                "CUDA_EXTERNAL_SEMAPHORE_HANDLE_DESC_OP1"
            ]
        },
        {
            "structure_name": "CUDA_EXTERNAL_SEMAPHORE_HANDLE_DESC_handle_nvSciSyncObj",
            "fields": [],
            "type_cast": "intptr",
            "arg_of": [
                "CUDA_EXTERNAL_SEMAPHORE_HANDLE_DESC_OP1"
            ]
        }
    ],
    "api": [
        {
            "api_name": "cuImportExternalSemaphore",
            "ret_type": "CUresult",
            "params": [
                {
                    "param_name": "extSem_out",
                    "param_type": {
                        "type_name": "ptr",
                        "ptr_method": "out",
                        "ptr_object": {
                            "type_name": "resource",
                            "resource_name": "CUexternalSemaphore"
                        }
                    }
                },
                {
                    "param_name": "semHandleDesc",
                    "param_type": {
                        "type_name": "ptr",
                        "ptr_method": "in",
                        "ptr_object": {
                            "type_name": "structure",
                            "structure_name": "CUDA_EXTERNAL_SEMAPHORE_HANDLE_DESC",
                            "args": ["CUDA_EXTERNAL_SEMAPHORE_HANDLE_DESC_OP1"]
                        }
                    }
                }
            ]
        }
    ]
}
"""

prompt_example3 = """
Example 3:
# API
CUresult cuGraphAddExternalSemaphoresSignalNode ( CUgraphNode* phGraphNode, CUgraph hGraph, const CUgraphNode* dependencies, size_t numDependencies, const CUDA_EXT_SEM_SIGNAL_NODE_PARAMS* nodeParams )

# source code
```
typedef struct CUgraphNode_st *CUgraphNode;                  /**< CUDA graph node */
typedef struct CUgraph_st *CUgraph;                          /**< CUDA graph */

typedef CUDA_EXT_SEM_SIGNAL_NODE_PARAMS_v1 CUDA_EXT_SEM_SIGNAL_NODE_PARAMS;
typedef struct CUDA_EXT_SEM_SIGNAL_NODE_PARAMS_st {
    CUexternalSemaphore* extSemArray;                         /**< Array of external semaphore handles. */
    const CUDA_EXTERNAL_SEMAPHORE_SIGNAL_PARAMS* paramsArray; /**< Array of external semaphore signal parameters. */
    unsigned int numExtSems;                                  /**< Number of handles and parameters supplied in extSemArray and paramsArray. */
} CUDA_EXT_SEM_SIGNAL_NODE_PARAMS_v1;

typedef struct CUextSemaphore_st *CUexternalSemaphore;       /**< CUDA external semaphore */
typedef CUDA_EXTERNAL_SEMAPHORE_SIGNAL_PARAMS_v1 CUDA_EXTERNAL_SEMAPHORE_SIGNAL_PARAMS;
typedef struct CUDA_EXTERNAL_SEMAPHORE_SIGNAL_PARAMS_st {
    struct {
        /**
         * Parameters for fence objects
         */
        struct {
            /**
             * Value of fence to be signaled
             */
            unsigned long long value;
        } fence;
        union {
            /**
             * Pointer to NvSciSyncFence. Valid if ::CUexternalSemaphoreHandleType
             * is of type ::CU_EXTERNAL_SEMAPHORE_HANDLE_TYPE_NVSCISYNC.
             */
            void *fence;
            unsigned long long reserved;
        } nvSciSync;
        /**
         * Parameters for keyed mutex objects
         */
        struct {
            /**
             * Value of key to release the mutex with
             */
            unsigned long long key;
        } keyedMutex;
        unsigned int reserved[12];
    } params;
    /**
     * Only when ::CUDA_EXTERNAL_SEMAPHORE_SIGNAL_PARAMS is used to
     * signal a ::CUexternalSemaphore of type
     * ::CU_EXTERNAL_SEMAPHORE_HANDLE_TYPE_NVSCISYNC, the valid flag is
     * ::CUDA_EXTERNAL_SEMAPHORE_SIGNAL_SKIP_NVSCIBUF_MEMSYNC which indicates
     * that while signaling the ::CUexternalSemaphore, no memory synchronization
     * operations should be performed for any external memory object imported
     * as ::CU_EXTERNAL_MEMORY_HANDLE_TYPE_NVSCIBUF.
     * For all other types of ::CUexternalSemaphore, flags must be zero.
     */
    unsigned int flags;
    unsigned int reserved[16];
} CUDA_EXTERNAL_SEMAPHORE_SIGNAL_PARAMS_v1;
```
"""

prompt_example3_response="""
{
    "resource": [
        {
            "resource_name": "CUexternalSemaphore",
            "resource_type": "intptr"
        },
        {
            "resource_name": "CUgraphNode",
            "resource_type": "intptr"
        },
        {
            "resource_name": "CUgraph",
            "resource_type": "intptr"
        }
    ],
    "enum": [
        {
            "enum_name": "CUDA_EXTERNAL_SEMAPHORE_SIGNAL_FLAGS",
            "enum_elements": [
                "CUDA_EXTERNAL_SEMAPHORE_SIGNAL_SKIP_NVSCIBUF_MEMSYNC",
                "CUDA_EXTERNAL_SEMAPHORE_WAIT_SKIP_NVSCIBUF_MEMSYNC"
            ],
            "enum_values": [
                "1",
                "2"
            ]
        }
    ],
    "structure": [
        {
            "structure_name": "CUDA_EXT_SEM_SIGNAL_NODE_PARAMS",
            "fields": [
                {
                    "field_name": "extSemArray",
                    "field_type": {
                        "type_name": "ptr",
                        "ptr_method": "in",
                        "ptr_object": {
                            "type_name": "array",
                            "array_element": {
                                "type_name": "resource",
                                "resource_name": "CUexternalSemaphore"
                            },
                            "array_length": ""
                        }
                    }
                },
                {
                    "field_name": "paramsArray",
                    "field_type": {
                        "type_name": "ptr",
                        "ptr_method": "in",
                        "ptr_object": {
                            "type_name": "array",
                            "array_element": {
                                "type_name": "structure",
                                "structure_name": "CUDA_EXTERNAL_SEMAPHORE_SIGNAL_PARAMS",
                                "args": [
                                    "CUDA_EXT_SEM_SIGNAL_NODE_PARAMS_OP1"
                                ]
                            },
                            "array_length": ""
                        }
                    }
                },
                {
                    "field_name": "numExtSems",
                    "field_type": {
                        "type_name": "int",
                        "int_bound": {
                            "min_val": "",
                            "max_val": ""
                        },
                        "int_size": "int32"
                    }
                }
            ],
            "type_cast": null,
            "arg_of": []
        },
        {
            "structure_name": "CUDA_EXTERNAL_SEMAPHORE_SIGNAL_PARAMS",
            "fields": [
                {
                    "field_name": "params",
                    "field_type": {
                        "type_name": "structure",
                        "structure_name": "CUDA_EXTERNAL_SEMAPHORE_SIGNAL_PARAMS_params",
                        "args": [
                            "CUDA_EXTERNAL_SEMAPHORE_SIGNAL_PARAMS_OP1"
                        ]
                    }
                },
                {
                    "field_name": "flags",
                    "field_type": {
                        "type_name": "flags",
                        "flags_enum": "CUDA_EXTERNAL_SEMAPHORE_SIGNAL_FLAGS",
                        "flags_element_size": "int32"
                    }
                },
                {
                    "field_name": "reserved",
                    "field_type": {
                        "type_name": "array",
                        "array_element": {
                            "type_name": "const",
                            "const_value": "0",
                            "const_size": "int32"
                        },
                        "array_length": "16"
                    }
                }
            ],
            "type_cast": null,
            "arg_of": []
        },
        {
            "structure_name": "CUDA_EXTERNAL_SEMAPHORE_SIGNAL_PARAMS_params",
            "fields": [
                {
                    "field_name": "fence",
                    "field_type": {
                        "type_name": "structure",
                        "structure_name": "CUDA_EXTERNAL_SEMAPHORE_SIGNAL_PARAMS_params_fence",
                        "args": []
                    }
                },
                {
                    "field_name": "nvSciSync",
                    "field_type": {
                        "type_name": "arg",
                        "arg_name": "CUDA_EXTERNAL_SEMAPHORE_SIGNAL_PARAMS_PARAMS_OP1"
                    }
                },
                {
                    "field_name": "keyedMutex",
                    "field_type": {
                        "type_name": "structure",
                        "structure_name": "CUDA_EXTERNAL_SEMAPHORE_SIGNAL_PARAMS_params_keyedMutex",
                        "args": []
                    }
                },
                {
                    "field_name": "reserved",
                    "field_type": {
                        "type_name": "array",
                        "array_element": {
                            "type_name": "const",
                            "const_value": "0",
                            "const_size": "int32"
                        },
                        "array_length": "12"
                    }
                }
            ],
            "type_cast": null,
            "arg_of": []
        },
        {
            "structure_name": "CUDA_EXTERNAL_SEMAPHORE_SIGNAL_PARAMS_params_fence",
            "fields": [
                {
                    "field_name": "value",
                    "field_type": {
                        "type_name": "int",
                        "int_bound": {
                            "min_val": "",
                            "max_val": ""
                        },
                        "int_size": "int64"
                    }
                }
            ],
            "type_cast": null,
            "arg_of": []
        },
        {
            "structure_name": "CUDA_EXTERNAL_SEMAPHORE_SIGNAL_PARAMS_params_nvSciSync_fence",
            "fields": [
                {
                    "field_name": "fence",
                    "field_type": {
                        "type_name": "int",
                        "int_bound": {
                            "min_val": "",
                            "max_val": ""
                        },
                        "int_size": "intptr"
                    }
                }
            ],
            "type_cast": null,
            "arg_of": [
                "CUDA_EXTERNAL_SEMAPHORE_SIGNAL_PARAMS_PARAMS_OP1"
            ]
        },
        {
            "structure_name": "CUDA_EXTERNAL_SEMAPHORE_SIGNAL_PARAMS_params_nvSciSync_reserved",
            "fields": [
                {
                    "field_name": "reserved",
                    "field_type": {
                        "type_name": "int",
                        "int_bound": {
                            "min_val": "",
                            "max_val": ""
                        },
                        "int_size": "int64"
                    }
                }
            ],
            "type_cast": null,
            "arg_of": [
                "CUDA_EXTERNAL_SEMAPHORE_SIGNAL_PARAMS_PARAMS_OP1"
            ]
        },
        {
            "structure_name": "CUDA_EXTERNAL_SEMAPHORE_SIGNAL_PARAMS_params_keyedMutex",
            "fields": [
                {
                    "field_name": "key",
                    "field_type": {
                        "type_name": "int",
                        "int_bound": {
                            "min_val": "",
                            "max_val": ""
                        },
                        "int_size": "int64"
                    }
                }
            ],
            "type_cast": null,
            "arg_of": []
        }
    ],
    "api": [
        {
            "api_name": "cuGraphAddExternalSemaphoresSignalNode",
            "ret_type": "CUresult",
            "params": [
                {
                    "param_name": "phGraphNode",
                    "param_type": {
                        "type_name": "ptr",
                        "ptr_method": "out",
                        "ptr_object": {
                            "type_name": "resource",
                            "resource_name": "CUgraphNode"
                        }
                    }
                },
                {
                    "param_name": "hGraph",
                    "param_type": {
                        "type_name": "resource",
                        "resource_name": "CUgraph"
                    }
                },
                {
                    "param_name": "dependencies",
                    "param_type": {
                        "type_name": "ptr",
                        "ptr_method": "in",
                        "ptr_object": {
                            "type_name": "array",
                            "array_element": {
                                "type_name": "resource",
                                "resource_name": "CUgraphNode"
                            },
                            "array_length": ""
                        }
                    }
                },
                {
                    "param_name": "numDependencies",
                    "param_type": {
                        "type_name": "len",
                        "len_target": "dependencies",
                        "len_size": "int32"
                    }
                },
                {
                    "param_name": "nodeParams",
                    "param_type": {
                        "type_name": "ptr",
                        "ptr_method": "in",
                        "ptr_object": {
                            "type_name": "structure",
                            "structure_name": "CUDA_EXT_SEM_SIGNAL_NODE_PARAMS",
                            "args": []
                        }
                    }
                }
            ]
        }
    ]
}
"""

prompt_input_prompt = """
# API
{}

# source code
```
{}
```
"""

prompt_fix_json_error = """
You response cannot be parse into json, please revise your answer.
You new response should only contain the revise the answer that can be parsed in JSON format (do not include ```json``` in your response).
Error: {}"""

prompt_fix_type_error = """
Your response did not follow the type definition, please revise your answer (Your new response should only contain the revised json, no explanation is needed).
Error: {}"""

prompt_fix_missing_struct = """
Your response is missing several structures:
```
{}
```

please revise your answer (Your new response should only contain the revised json, no explanation is needed)."""