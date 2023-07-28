#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec2 inTexCoord;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec3 fragNormal;
layout(location = 2) out vec2 fragTexCoord;

layout(push_constant) uniform pushConstants {
	// Only guaranteed a total of 128 bytes.
	mat4 model; // 64 bytes
} uPushConstants;

const vec3 DIRECTION_TO_LIGHT = normalize(vec3(1.0, -3.0, 1.0));
const float AMBIENT = 0.05;

void main() {
    gl_Position = ubo.proj * ubo.view * uPushConstants.model * vec4(inPosition, 1.0);

    mat3 normalMatrix = transpose(inverse(mat3(uPushConstants.model)));
    vec3 normalWorldSpace = normalize(normalMatrix * inNormal);

    float lightIntensity = AMBIENT + max(dot(normalWorldSpace, DIRECTION_TO_LIGHT), 0);

    fragColor = lightIntensity * inColor;
    fragNormal = inNormal;
    fragTexCoord = inTexCoord;
}