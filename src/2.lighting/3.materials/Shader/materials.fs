#version 330 core
out vec4 FragColor; // 输出的颜色值

// 定义材质结构体，包含环境光、漫反射、镜面反射颜色和高光系数
struct Material {
    vec3 ambient;   // 材质的环境光颜色
    vec3 diffuse;   // 材质的漫反射颜色
    vec3 specular;  // 材质的镜面反射颜色
    float shininess; // 材质的高光系数（影响高光的锐利程度）
}; 

// 定义光源结构体，包含位置和光的三种分量
struct Light {
    vec3 position;  // 光源的位置

    vec3 ambient;   // 光源的环境光分量
    vec3 diffuse;   // 光源的漫反射分量
    vec3 specular;  // 光源的镜面反射分量
};

// 从顶点着色器传入的变量
in vec3 FragPos;  // 当前片元的位置（世界空间）
in vec3 Normal;   // 当前片元的法向量（世界空间）

// uniform 变量：观察者位置、材质属性、光源属性
uniform vec3 viewPos;     // 摄像机（观察者）的位置
uniform Material material; // 材质信息
uniform Light light;       // 光源信息

void main()
{
    // 计算环境光分量：光源的环境光 × 材质的环境光
    vec3 ambient = light.ambient * material.ambient;
    
    // 计算漫反射分量
    vec3 norm = normalize(Normal); // 单位化法向量
    vec3 lightDir = normalize(light.position - FragPos); // 从片元指向光源的方向
    float diff = max(dot(norm, lightDir), 0.0); // 计算光照角度，确保不为负
    vec3 diffuse = light.diffuse * (diff * material.diffuse); // 漫反射颜色

    // 计算镜面反射分量
    vec3 viewDir = normalize(viewPos - FragPos); // 从片元指向观察者的方向
    vec3 reflectDir = reflect(-lightDir, norm);  // 计算反射方向
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess); // 镜面反射强度
    vec3 specular = light.specular * (spec * material.specular); // 镜面反射颜色
        
    // 合并三种光照分量
    vec3 result = ambient + diffuse + specular;

    // 输出最终颜色，alpha 设置为 1.0（不透明）
    FragColor = vec4(result, 1.0);
}
