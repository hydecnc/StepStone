prompt_instructions = """
Your task is extracting the enum type or macro definition of integer flag arguments/fields in a given API or data structure.
An integer flag argument/field is a variable that does not clearly indicate its potential values or data type (e.g., int flags or long flags).
You should always refer to the uploaded API documentation in your file search to infer the data type of the flag argument.

I will ask you to count how many integer flag arguments/fields are in the given API or data structure.
- Your response should be a JSON object with the following format: {"integer_flag_name": {"type": ["flag_type"], "explain": "explanation"}}
- Your explanation should written in natural language, describing which integer flag should be what types.
- If there are no flag arguments, return an empty dict as {}
"""

example_num = 6
prompt_example_header = "Here I provided {} examples for you to learn.".format(example_num)

prompt_example1 = """
CUresult cuCtxCreate ( CUcontext* pctx, unsigned int  flags, CUdevice dev )
"""

prompt_example1_response = """
{
    "flags": [
        {"type": ["CUctx_flags"], "explain": "argument `unsigned int flags` in API `cuCtxCreate` should be `CUctx_flags` type according to the documentation. You should use `CUctx_flags` to replace `unsigned int`." }
    ]
}
"""
prompt_example2 = """
typedef struct {
    unsigned int Width;
    unsigned int Height;
    unsigned int Depth;
    CUarray_format Format;
    unsigned int NumChannels;
    unsigned int Flags;
} CUDA_ARRAY3D_DESCRIPTOR;
"""

prompt_example2_response = """
{
    "Flags": [
        {"type": ["CUDA_ARRAY3D_LAYERED", "CUDA_ARRAY3D_SURFACE_LDST", "CUDA_ARRAY3D_CUBEMAP", "CUDA_ARRAY3D_TEXTURE_GATHER"], "explain": "field `unsigned int Flags` in structure `CUDA_ARRAY3D_DESCRIPTOR` could be [CUDA_ARRAY3D_LAYERED, CUDA_ARRAY3D_SURFACE_LDST, CUDA_ARRAY3D_CUBEMAP, CUDA_ARRAY3D_TEXTURE_GATHER] types according to the documentation. You should replace `unsigned int` with the new values." }
    ]
}
"""

prompt_example3 = """
typedef struct VkPhysicalDeviceExternalBufferInfo {
    VkStructureType                       sType;
    const void*                           pNext;
    VkBufferCreateFlags                   flags;
    VkBufferUsageFlags                    usage;
    VkExternalMemoryHandleTypeFlagBits    handleType;
} VkPhysicalDeviceExternalBufferInfo;
"""

prompt_example3_response = """
{
    "VkBufferCreateFlags": {"type": ["VkBufferCreateFlagBits"], "explain": "field `VkBufferCreateFlags flags` in structure `VkPhysicalDeviceExternalBufferInfo` should be `VkBufferCreateFlagBits` type according to the documentation, because because `VkBufferCreateFlags` is casted from `VkFlags`, and `VkFlags` is casted from `uint32_t`." },
    "VkBufferUsageFlags":{"type": ["VkBufferUsageFlagBits"], "explain": "field `VkBufferUsageFlags usage` in structure `VkPhysicalDeviceExternalBufferInfo` should be `VkBufferUsageFlagBits` type according to the documentation, because because `VkBufferUsageFlags` is casted from `VkFlags`, and `VkFlags` is casted from `uint32_t`." }
}
"""

prompt_example4 = """
CUresult cuMemAddressFree ( CUdeviceptr ptr, size_t size )
"""

prompt_example4_response = """
{}
"""

prompt_example5 = """
void glTexParameteri(GLenum target, GLenum pname, GLint param);
"""

prompt_example5_response = """
{
    "target": {"type": ["GL_TEXTURE_2D", "GL_TEXTURE_3D", "GL_TEXTURE_2D_ARRAY", "GL_TEXTURE_CUBE_MAP"], "explain": "According to the documentation, argument `GLenum target` in API `glTexParameterf` should be one of ["GL_TEXTURE_2D", "GL_TEXTURE_3D", "GL_TEXTURE_2D_ARRAY", "GL_TEXTURE_CUBE_MAP"] types." },
    "pname": {"type": ["GL_TEXTURE_BASE_LEVEL", "GL_TEXTURE_COMPARE_FUNC", "GL_TEXTURE_COMPARE_MODE", "GL_TEXTURE_MIN_FILTER", "GL_TEXTURE_MAG_FILTER", "GL_TEXTURE_MIN_LOD", "GL_TEXTURE_MAX_LOD", "GL_TEXTURE_MAX_LEVEL", "GL_TEXTURE_SWIZZLE_R", "GL_TEXTURE_SWIZZLE_G", "GL_TEXTURE_SWIZZLE_B", "GL_TEXTURE_SWIZZLE_A", "GL_TEXTURE_WRAP_S", "GL_TEXTURE_WRAP_T", "GL_TEXTURE_WRAP_R"], "explain": "According to the documentation, argument `GLenum pname` in API `glTexParameterf` should be one of ["GL_TEXTURE_BASE_LEVEL", "GL_TEXTURE_COMPARE_FUNC", "GL_TEXTURE_COMPARE_MODE", "GL_TEXTURE_MIN_FILTER", "GL_TEXTURE_MAG_FILTER", "GL_TEXTURE_MIN_LOD", "GL_TEXTURE_MAX_LOD", "GL_TEXTURE_MAX_LEVEL", "GL_TEXTURE_SWIZZLE_R", "GL_TEXTURE_SWIZZLE_G", "GL_TEXTURE_SWIZZLE_B", "GL_TEXTURE_SWIZZLE_A", "GL_TEXTURE_WRAP_S", "GL_TEXTURE_WRAP_T", "GL_TEXTURE_WRAP_R"] types." },
}
"""

prompt_example6 = """
void glTexStorage3DMultisample (GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLboolean fixedsamplelocations);
"""

prompt_example6_response = """
{
    "target": {"type": ["GL_TEXTURE_2D_MULTISAMPLE_ARRAY"], "explain": "According to the documentation, argument `GLenum target` in API `glTexStorage3DMultisample` must be `GL_TEXTURE_2D_MULTISAMPLE_ARRAY`." },
}
"""

prompt_input_prompt = """
{}
"""