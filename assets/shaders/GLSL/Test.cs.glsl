#version 450

// Define the input and output buffer bindings
layout(set = 0, binding = 0) buffer InputData {
    vec4 data[];
} inputData;

layout(set = 0, binding = 1) buffer OutputData {
    vec4 data[];
} outputData;

void main() {
    // Calculate the global invocation index
    uint idx = gl_GlobalInvocationID.x;

    // Perform the computation (add two vectors)
    outputData.data[idx] = inputData.data[idx] + inputData.data[idx + 1];
}