#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 FragPos;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    // inverse(model)	计算模型矩阵的逆，用于正确变换法向量
    // transpose(...)	转置逆矩阵，得到所谓的法向量变换矩阵
    // mat3(...)	去掉平移部分，只保留旋转和缩放（因为法向量不受平移影响）
    // * aNormal	应用变换，将法向量从模型空间转换到世界空间
    Normal = mat3(transpose(inverse(model))) * aNormal;    
    
    gl_Position = projection * view * vec4(FragPos, 1.0);
}