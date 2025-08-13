#version 330 core
out vec4 FragColor; // 输出的最终颜色

in vec3 Normal;     // 从顶点着色器传来的法向量（已变换到世界空间）
in vec3 FragPos;    // 当前片元的位置（世界空间）

// 光照和视角相关的 uniform 变量
uniform vec3 lightPos;     // 光源位置
uniform vec3 viewPos;      // 摄像机（观察者）位置
uniform vec3 lightColor;   // 光源颜色
uniform vec3 objectColor;  // 物体本身的颜色

void main()
{
    // Ambient 光照：模拟环境光，给物体一个基本亮度
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    // Diffuse 光照：模拟光线从某个方向照射到物体表面
    vec3 norm = normalize(Normal);                  // 单位化法向量
    vec3 lightDir = normalize(lightPos - FragPos);  // 从片元指向光源的方向
    float diff = max(dot(norm, lightDir), 0.0);     // 计算光照强度（夹角越小越亮）
    vec3 diffuse = diff * lightColor;               // 乘以光源颜色得到漫反射颜色

    // Specular 光照：模拟高光反射（镜面反射）
    float specularStrength = 0.5;                   // 高光强度系数
    vec3 viewDir = normalize(viewPos - FragPos);    // 从片元指向摄像机的方向
    vec3 reflectDir = reflect(-lightDir, norm);     // 计算反射方向
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32); // 高光分量（32 是 shininess）
    vec3 specular = specularStrength * spec * lightColor;     // 高光颜色

    // 最终颜色：将三种光照分量加起来并乘以物体颜色
    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0); // 输出最终颜色，alpha 为 1.0（不透明）
}
