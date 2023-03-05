RWStructuredBuffer<float4> inputData : register(u0);
RWStructuredBuffer<float4> outputData : register(u1);

[numthreads(64, 1, 1)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID) {
    // Calculate the global invocation index
    uint idx = dispatchThreadId.x;

    // Perform the computation (add two vectors)
    outputData[idx] = inputData[idx] + inputData[idx + 1];
}