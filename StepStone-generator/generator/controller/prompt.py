prompt_syzlang_generator_instructions = """
As a Nvidia CUDA APIs expert and a Syzkaller engineer, you are assigned to create comprehensive Syzkaller descriptions for a given CUDA API. Your task is to develop a .txt file with detailed function and type definitions and a .txt.const file containing all necessary constant definitions. Adhere to the syntax rules specified in the syzlang.pdf and base your work primarily on the uploaded CUDA documentation to ensure accuracy in API functionality understanding.

Detailed Steps:
1. Review the CUDA documentation thoroughly to fully understand each API's function and parameters.
2. Modify CUDA API names by prefixing them with "syz_", following the naming conventions outlined in syzlang.pdf.
3. Focus on completing the descriptions for the given CUDA API follow the syntax of syzkaller description that defined in "syzlang.pdf"
4. For APIs where there is uncertainty or concerns about the descriptions, note these by prefixing with "[uncertain]" and provide a brief explanatory comment.
5. Upon finalizing the API descriptions, compile the constant definitions in the .txt.const file, ensuring consistency with the documented APIs. Add "arches = 386, amd64, arm, arm64, mips64le, ppc64le, riscv64, s390x" to the beginning of .txt.const file.

Execution Suggestions
1. Maintain a consistent format for each API description using a template based on the syzlang.pdf to ensure uniformity across the document.
2. Tackle the API list systematically, preserving the organized sequence provided to avoid errors.
3. Regularly cross-reference the CUDA documentation while writing the descriptions to capture all functionalities accurately and to mitigate discrepancies.
4. For every flag enum and structure that was used in the description must also be defined in the same .txt file.

Final Notes
1. Your answer should only be the generated text without any explanation before or after the generated text.
2. No specific prioritization within the API descriptions, workflow adjustments, or additional database consultation details were needed beyond the primary documentation and existing instructions. This clear and structured approach will ensure that the Syzkaller descriptions for the CUDA APIs are accurately compiled, adhering to all specified guidelines and effectively organized for optimal clarity and compliance.
"""
prompt_syzlang_generator_example_introduction = "I will provide {0} examples to help you understand the task."

prompt_syzlang_const_list ="""
From the previous response, list all the enum type:
"""

prompt_syzlang_const_ele ="""
For each enum type, list its elements in CUDA API:
"""

prompt_syzlang_const_def = """
Generate .txt.const file from your previous response 
"""

prompt_api_input_example = """
# CUDA API
{}

# Syzlang States
```
{}
```
"""

prompt_api_input_continue = """
Please revise your generated description and const definition with the v12.6.1 CUDA API library source code. You need to make sure
- The resource type definition is legitimate
- The enum type and structure type are throughly defined without missing any elements
- The API arguments definition is legitimate
- If `const` definitions are correct, that means whether the variable could be value that other than the fixed value according to CUDA documentation.
- Only insert the provdied API, do not insert other API from previous context
- Only replace "[APPEND_NEW_CONTENT_HERE]" with the new syzkaller description, do not delete other contents in `Syzlang States` 


# CUDA API
{}

# Syzlang States
```
{}
```
"""

prompt_syzlang_blank = """
include <cuda.h>

# Resource Definition
[APPEND_NEW_CONTENT_HERE]

# Enum Definition
[APPEND_NEW_CONTENT_HERE]

# API Definition
[APPEND_NEW_CONTENT_HERE]

# Structure Definition
[APPEND_NEW_CONTENT_HERE]
"""