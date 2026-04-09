prompt_instructions = """
Task: Your objective is to identify arguments or return values in an API that should be considered “resources” based on their use as inputs or outputs in other APIs. You should only refer to the provided API documentation for this task.

Instructions:
1. Check Pointer Types:
    - Look at pointer types in both arguments and return values of the given API.
    - Determine if any other APIs in the documentation use these as input values or produce them as output values.
2. Identify Resources for Cross-Type Pairing:
    - Focus only on arguments or return values that differ in type from the corresponding arguments or return values in the current API.
	- Exclude any obvious resources of the same type across APIs.
3. Output Format:
    - For each resource found, output a line in natural language in English, such as:
        "<producer argument>/<producer return value> should be marked as a resource "<producer API name>_<producer argument>/<producer return value>", because it is used/produced by <API name>."
    - If there are no dependencies (i.e., no arguments or return values are used by other APIs), respond with:
        "NO DEPENDENCY"
    - When respond with "NO DEPENDENCY", your response should only contain the "NO DEPENDENCY" string, no explanation is needed.
"""

example_num = 2
prompt_example_header = "Here I provided {} examples for you to learn.".format(example_num)

prompt_example1 = """
CUresult cuLinkComplete ( CUlinkState state, void** cubinOut, size_t* sizeOut )
"""

prompt_example1_response = """
Argument `void** cubinOut` from `cuLinkComplete` should be a resource, because it will be used by `cuModuleLoadData`'s second argument `const void* image`, which consumes the pointer to `cubinOut`. The resource name should be `cuLinkComplete_cubinOut`
Argument `void** cubinOut` from `cuLinkComplete` should be a resource, because it will be used by `cuModuleLoadDataEx`'s second argument `const void* image`, which consumes the pointer to `cubinOut`. The resource name should be `cuLinkComplete_cubinOut`
"""
prompt_example2 = """
cl_int clEnqueueSVMMemFill(cl_command_queue command_queue, void* svm_ptr, const void* pattern, size_t pattern_size, size_t size, cl_uint num_events_in_wait_list, const cl_event* event_wait_list, cl_event* event);
"""

prompt_example2_response = """
Argument `void* svm_ptr` from `clEnqueueSVMMemFill` should be a resource because it comes from the return value of `clSVMAlloc`. `svm_ptr` is a pointer to a memory region that is allocated by `clSVMAlloc`. The resource name should be `clSVMAlloc_ret`
"""

prompt_input_prompt = """
{}
"""

prompt_input_revise = """
Revise your response based on the following statement (Your new response should only contain the revised json, no explanation is needed):
- {} If there are multiple resources for one argument, add new api entry for each resource, append `$1`, `$2` and etc to the api name for differentiation."
"""