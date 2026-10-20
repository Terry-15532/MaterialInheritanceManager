# Material Inheritance Manager / 材质继承管理器

Unreal Engine 编辑器插件，用于查看和批量管理材质、材质实例的继承关系。

在编辑器主菜单的 **管线工具 → 材质工具 → 材质继承管理器** 中打开。详细操作说明见 [Docs/Help.html](Docs/Help.html)。

本仓库跟踪插件源码、配置和文档；`Binaries`、`Intermediate` 等生成文件保留在本机，由 `.gitignore` 排除。

- `MaterialInheritanceManager.uplugin`：插件描述。
- `Source/MaterialInheritanceManager`：编辑器模块源码和回归测试。
- `Config`：插件打包配置。
- `Docs`：使用说明。

将此目录放入 UE 项目的 `Plugins/MaterialInheritanceManager`，使用项目对应的 Unreal Engine 和 C++ 工具链编译。
