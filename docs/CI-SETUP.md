# CI/CD 设置文档

## 概述

本项目配置了完整的 CI/CD 流程，用于自动化构建、测试和代码质量检查。

## GitHub Actions Workflows

### 1. Android CI (`.github/workflows/android-ci.yml`)

**触发条件**:
- Push 到 `master`, `main`, `claude/**` 分支
- Pull Request 到 `master`, `main` 分支

**执行任务**:
- ✅ **Build and Test**: 构建 Debug/Release APK
- ✅ **Run Lint**: Android 代码检查
- ✅ **C++ Analysis**: C++ 静态代码分析 (cppcheck)
- ✅ **Native Build**: 验证原生库编译
- ✅ **Dependency Check**: 依赖安全检查

**产出物**:
- Debug APK
- Release APK
- Lint 报告
- cppcheck 报告
- 依赖树

---

### 2. Pull Request Checks (`.github/workflows/pr-check.yml`)

**触发条件**:
- 创建或更新 Pull Request

**执行任务**:
- ✅ **Quick Build**: 快速构建检查（15分钟超时）
- ✅ **Code Review**: 自动代码审查
  - 检查硬编码凭据
  - 检查 System.out 使用
  - 检查 TODO 注释
  - 检查空指针处理
- ✅ **Gradle Wrapper Validation**: 验证 Gradle 包装器完整性

**特性**:
- 自动在 PR 中添加评论反馈构建结果

---

### 3. Code Quality (`.github/workflows/code-quality.yml`)

**触发条件**:
- Push 到主分支
- Pull Request

**执行任务**:

#### Android Lint Analysis
- 详细的 Android 代码质量检查
- 生成 HTML 和 XML 格式报告

#### Security Vulnerability Scan
- 使用 Trivy 扫描安全漏洞
- 上传结果到 GitHub Security

#### C++ Security Analysis
- cppcheck 安全检查
- 检查不安全的字符串函数 (`strcpy`, `strcat`)
- 检查内存泄漏风险
- 检查 NULL 指针解引用

#### Java Security Check
- SQL 注入风险检查
- 硬编码密钥检查
- 不安全的随机数生成
- 命令注入风险
- 路径遍历风险

---

## 本地 CI 检查

### 使用本地检查脚本

在提交代码前，运行本地检查脚本：

```bash
./scripts/local-ci-check.sh
```

**脚本功能**:
1. ✅ Clean 构建
2. ✅ 构建 Debug APK
3. ✅ 运行 Lint 检查
4. ✅ C++ 静态分析（需要安装 cppcheck）
5. ✅ 安全检查
6. ✅ 代码质量检查

**安装依赖** (可选):
```bash
# Ubuntu/Debian
sudo apt-get install cppcheck

# macOS
brew install cppcheck
```

---

## CI 状态徽章

在 README 中添加以下徽章来显示 CI 状态：

```markdown
![Android CI](https://github.com/your-username/Android-Virtual-Mouse/workflows/Android%20CI/badge.svg)
![Code Quality](https://github.com/your-username/Android-Virtual-Mouse/workflows/Code%20Quality/badge.svg)
```

---

## 常见问题排查

### 1. 构建失败

**检查**:
- 查看 GitHub Actions 日志
- 运行本地检查脚本: `./scripts/local-ci-check.sh`
- 确保 Java 11 已安装

### 2. Lint 警告

**查看报告**:
```bash
./gradlew lint
open app/build/reports/lint-results-debug.html
```

### 3. C++ 编译错误

**本地测试**:
```bash
./gradlew externalNativeBuild --stacktrace
```

### 4. 权限错误

**修复 gradlew 权限**:
```bash
chmod +x gradlew
```

---

## 配置要求

### GitHub Secrets

当前配置不需要额外的 secrets。如果未来需要签名发布版本，添加：

```
KEYSTORE_FILE      # Base64 编码的 keystore 文件
KEYSTORE_PASSWORD  # Keystore 密码
KEY_ALIAS          # 密钥别名
KEY_PASSWORD       # 密钥密码
```

### 环境变量

CI 环境自动配置以下变量：
- `ANDROID_HOME`: Android SDK 路径
- `JAVA_HOME`: Java JDK 路径

---

## CI/CD 最佳实践

### 提交前检查清单

- [ ] 运行本地 CI 检查脚本
- [ ] 修复所有 Lint 错误
- [ ] 修复所有编译警告
- [ ] 检查代码中没有 TODO/FIXME
- [ ] 确保没有调试代码（System.out.println）
- [ ] 验证 APK 可以成功构建

### Pull Request 流程

1. 创建功能分支: `git checkout -b feature/your-feature`
2. 提交更改并运行本地检查
3. Push 到 GitHub
4. 创建 Pull Request
5. 等待 CI 检查通过
6. 根据反馈修复问题
7. Merge 到主分支

### 持续改进

- 定期更新依赖版本
- 关注 Security 标签页的漏洞警告
- Review Lint 报告并逐步修复
- 增加代码覆盖率

---

## 故障排除

### Gradle 缓存问题

```bash
./gradlew clean
rm -rf ~/.gradle/caches
./gradlew build --no-daemon
```

### NDK 版本问题

检查 `local.properties`:
```properties
ndk.dir=/path/to/ndk
sdk.dir=/path/to/sdk
```

### CI 超时

- 检查是否有死循环
- 确保测试不会阻塞
- 考虑拆分大型测试

---

## 监控和报告

### 查看 CI 结果

1. 访问 GitHub Actions 页面
2. 点击具体的 workflow run
3. 查看每个 job 的日志
4. 下载 artifacts（APK、报告等）

### 报告位置

- **Lint 报告**: `app/build/reports/lint-results-debug.html`
- **C++ 分析**: GitHub Actions artifacts
- **安全扫描**: GitHub Security 标签页

---

## 参考资源

- [GitHub Actions 文档](https://docs.github.com/en/actions)
- [Android Lint 参考](https://developer.android.com/studio/write/lint)
- [cppcheck 手册](http://cppcheck.sourceforge.net/manual.pdf)
- [Gradle 构建指南](https://docs.gradle.org/current/userguide/userguide.html)

---

## 维护者

如需帮助或发现 CI 问题，请创建 Issue 并添加 `ci` 标签。
