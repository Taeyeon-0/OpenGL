## 项目目标

这是一个使用 C++、CMake 和 Conan 构建，并由 Codex 或其他 AI 工具协作开发的项目。

项目规则的目标是让 AI 能够：

- 正确理解项目结构和模块职责。
- 使用统一命令构建、测试和检查代码。
- 在明确范围内修改文件。
- 通过可重复的验证判断任务是否完成。

长期规则写入本文件；一次性需求写入 `docs/tasks/`，不要把具体任务堆积到 `AGENTS.md`。

## 规则优先级

1. 用户当前请求。
2. 本文件中的项目规则。
3. `docs/` 下的需求、架构和任务文档。
4. AI 的默认行为。

如果规则冲突，遵循优先级更高的规则，并在最终报告中说明。

## 目录职责

```text
AGENTS.md              # AI 和开发者的长期协作规则
README.md              # 项目介绍和快速开始
CMakeLists.txt         # CMake 构建定义
CMakePresets.json      # 构建预设
conanfile.py           # Conan 依赖
.codex/                # 可选的 Codex 项目配置和自定义 agent
docs/                  # 需求、架构、构建、测试和任务文档
include/               # 对外头文件
src/                   # 应用实现
tests/                 # 单元测试、集成测试和测试数据
assets/                # Shader、纹理、模型等资源
scripts/               # 构建、测试、格式化和检查脚本
cmake/                 # 自定义 CMake 模块
deploy/                # 打包和部署文件
build/                 # 构建产物，禁止提交
```

源码建议按职责组织：

```text
src/app/               # 应用启动和依赖组装
src/core/              # 通用基础设施
src/domain/            # 核心业务模型和逻辑
src/services/          # 应用服务
src/infrastructure/    # 文件、网络、数据库等外部实现
src/ui/                # 界面代码（如项目需要）
src/render/            # OpenGL 或其他渲染代码
```

依赖方向保持为：

```text
UI → Services → Domain → Infrastructure
```

避免循环依赖。UI 不应直接访问数据库、网络或底层文件系统。

## AI 任务流程

开始修改前：

1. 检查 Git 工作区状态。
2. 阅读与任务相关的源码、配置和文档。
3. 明确目标、修改范围、排除项、验收标准和验证命令。
4. 先制定简短计划，再执行修改。

任务完成后：

- 只保留与任务相关的改动。
- 检查 diff，确认没有意外修改。
- 运行适用的构建、测试和静态检查。
- 在最终报告中说明结果和未验证内容。

复杂任务应创建 `docs/tasks/T-<编号>-<名称>.md`，至少包含：

```text
背景
目标
范围
不包含的内容
设计方案
涉及文件
验收标准
测试方案
```

## 构建和依赖

项目使用：

- CMake 管理构建。
- Conan 2 管理第三方依赖。
- CMake Presets 统一 Debug、Release 和平台配置。
- Ninja 作为默认构建后端，除非项目配置另有规定。

优先使用项目已有的 `CMakePresets.json`、Conan profile 和脚本。不要绕过项目配置自行创建另一套构建方式。

通用 Debug 流程：

```bash
python3 scripts/setup_conan.py --build-type Debug

cmake -S . -B build/Debug \
    -G Ninja \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_TOOLCHAIN_FILE=build/Debug/generators/conan_toolchain.cmake

cmake --build build/Debug
```

如果项目已有 Preset，优先执行：

```bash
cmake --preset <preset>
cmake --build --preset <preset>
```

不要直接修改 Conan 生成的工具链文件。新增或升级依赖时说明用途、版本和平台影响，并通过 `scripts/setup_conan.py` 重新安装依赖。

`scripts/setup_conan.py` 是项目统一的 Conan 入口。它只使用 Python 标准库，会自动选择平台 profile，并将生成文件放入 `build/<BuildType>/`。Linux 下默认允许 Conan 通过 `sudo` 调用系统包管理器安装缺少的开发依赖：

```bash
python3 scripts/setup_conan.py --build-type Release
python3 scripts/setup_conan.py --build-type RelWithDebInfo
```

如果不希望脚本修改系统依赖，使用：

```bash
python3 scripts/setup_conan.py --build-type Release --no-system-deps
```

如果自动 profile 识别不适用，可以显式传入：

```bash
python3 scripts/setup_conan.py \
    --build-type Release \
    --profile conanprofiles/linux-gcc
```

## 测试和验证

测试按职责放在：

```text
tests/unit/             # 单元测试
tests/integration/      # 集成测试
tests/fixtures/         # 测试数据
```

通用测试命令：

```bash
ctest --test-dir build/Debug --output-on-failure
```

规则：

- 新功能应尽量添加单元测试或集成测试。
- 纯业务逻辑应与 UI、文件系统和网络解耦，便于测试。
- 窗口或图形测试在无显示环境时使用项目规定的无头运行方式。
- 修改 C++ 后，如工具可用，运行 clang-format。
- 必要时运行 clang-tidy、cppcheck 或项目已有的静态检查脚本。
- 不要声称存在覆盖率，除非实际生成并检查了覆盖率数据。
- 测试失败时必须报告真实失败原因，不要隐藏失败。


## AI 多智能体

可选的自定义 agent 放在：

```text
.codex/agents/
├── explorer.toml
├── reviewer.toml
└── tester.toml
```

推荐职责：

- `explorer`：只读分析目录、调用关系和现有实现。
- `reviewer`：检查安全、质量、逻辑和回归风险。
- `tester`：运行测试并检查测试覆盖情况。

适合并行的任务：

- 代码结构分析。
- 安全审查。
- 测试审查。
- 文档检查。
- 互不依赖的模块实现。

并行修改代码时必须使用互不重叠的文件范围，避免多个 agent 同时修改同一文件。不确定时先只读分析。

## 安全和文件规则

- 修改前检查并保留用户已有的未提交修改。
- 只修改完成任务所需的文件。
- 不要使用 `git reset --hard` 或 `git checkout --` 覆盖用户修改。
- 不要删除整个构建目录来掩盖问题。
- 不要提交 `build/`、编译缓存、IDE 临时文件、本地路径或生成文件。
- 不要在源码、配置、日志或测试数据中保存密钥、令牌和私有地址。
- 配置使用 `.example` 模板，真实配置放在 Git 忽略文件中。
- 不要引入未经审查的远程脚本、命令执行、删除文件操作或自动下载行为。
- 不要未经请求重命名项目、重构目录、替换构建系统或升级核心依赖。

## 最终报告

最终响应必须包含：

1. 修改了哪些文件以及原因。
2. 执行过哪些构建、测试和静态检查命令。
3. 每条验证命令的结果。
4. 尚未验证的内容和环境限制。
