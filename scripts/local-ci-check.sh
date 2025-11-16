#!/bin/bash
# Local CI Check Script
# Run this before committing to catch issues early

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"

cd "$PROJECT_ROOT"

echo "======================================"
echo "🔍 Running Local CI Checks"
echo "======================================"
echo ""

# Color codes
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Check if gradlew exists
if [ ! -f "./gradlew" ]; then
    echo -e "${RED}❌ gradlew not found${NC}"
    exit 1
fi

chmod +x ./gradlew

echo "======================================"
echo "📦 Step 1: Clean Build"
echo "======================================"
./gradlew clean || { echo -e "${RED}❌ Clean failed${NC}"; exit 1; }
echo -e "${GREEN}✓ Clean successful${NC}"
echo ""

echo "======================================"
echo "🔨 Step 2: Build Debug APK"
echo "======================================"
./gradlew assembleDebug --stacktrace || { echo -e "${RED}❌ Build failed${NC}"; exit 1; }
echo -e "${GREEN}✓ Build successful${NC}"
echo ""

echo "======================================"
echo "🧹 Step 3: Run Lint Checks"
echo "======================================"
./gradlew lint --stacktrace || { echo -e "${YELLOW}⚠️  Lint found issues${NC}"; }
if [ -f "app/build/reports/lint-results-debug.html" ]; then
    echo "📄 Lint report: app/build/reports/lint-results-debug.html"
fi
echo ""

echo "======================================"
echo "🔍 Step 4: C++ Static Analysis"
echo "======================================"
if command -v cppcheck &> /dev/null; then
    echo "Running cppcheck..."
    cppcheck --enable=warning,style,performance,portability \
        --suppress=missingIncludeSystem \
        --std=c++11 \
        app/src/main/cpp/ 2>&1 | tee cppcheck-output.txt || true
    echo -e "${GREEN}✓ C++ analysis complete${NC}"
else
    echo -e "${YELLOW}⚠️  cppcheck not installed, skipping C++ analysis${NC}"
    echo "Install with: sudo apt-get install cppcheck"
fi
echo ""

echo "======================================"
echo "🔐 Step 5: Security Checks"
echo "======================================"

# Check for System.out.println
echo "Checking for System.out usage..."
if grep -r "System.out.println\|System.err.println" app/src/main/java --include="*.java" > /dev/null 2>&1; then
    echo -e "${YELLOW}⚠️  Found System.out/err usage (should use Log instead)${NC}"
    grep -rn "System.out.println\|System.err.println" app/src/main/java --include="*.java" | head -5
else
    echo -e "${GREEN}✓ No System.out usage${NC}"
fi

# Check for unsafe C++ functions
echo "Checking for unsafe C++ functions..."
if grep -r "strcpy\|strcat\|sprintf" app/src/main/cpp/ --include="*.cpp" --include="*.h" > /dev/null 2>&1; then
    echo -e "${YELLOW}⚠️  Found potentially unsafe string functions${NC}"
    grep -rn "strcpy\|strcat\|sprintf" app/src/main/cpp/ --include="*.cpp" --include="*.h"
else
    echo -e "${GREEN}✓ No unsafe string functions${NC}"
fi

# Check for malloc/free pairs
echo "Checking for memory management..."
MALLOC_COUNT=$(grep -r "malloc\|calloc" app/src/main/cpp/ --include="*.cpp" | wc -l)
FREE_COUNT=$(grep -r "free(" app/src/main/cpp/ --include="*.cpp" | wc -l)
echo "  malloc/calloc calls: $MALLOC_COUNT"
echo "  free calls: $FREE_COUNT"
if [ "$MALLOC_COUNT" -ne "$FREE_COUNT" ]; then
    echo -e "${YELLOW}⚠️  Potential memory leak (malloc/free count mismatch)${NC}"
fi

echo ""

echo "======================================"
echo "📝 Step 6: Code Quality Checks"
echo "======================================"

# Check for TODO/FIXME
echo "Checking for TODO/FIXME comments..."
TODO_COUNT=$(grep -r "TODO\|FIXME\|XXX" app/src/main --include="*.java" --include="*.cpp" --include="*.h" 2>/dev/null | wc -l || echo "0")
if [ "$TODO_COUNT" -gt 0 ]; then
    echo -e "${YELLOW}⚠️  Found $TODO_COUNT TODO/FIXME comments${NC}"
    grep -rn "TODO\|FIXME\|XXX" app/src/main --include="*.java" --include="*.cpp" --include="*.h" 2>/dev/null | head -5 || true
else
    echo -e "${GREEN}✓ No TODO comments${NC}"
fi

echo ""

echo "======================================"
echo "📊 Build Summary"
echo "======================================"
if [ -f "app/build/outputs/apk/debug/app-debug.apk" ]; then
    APK_SIZE=$(du -h app/build/outputs/apk/debug/app-debug.apk | cut -f1)
    echo "✓ Debug APK built successfully"
    echo "  Size: $APK_SIZE"
    echo "  Location: app/build/outputs/apk/debug/app-debug.apk"
fi

if [ -f "app/build/intermediates/cmake/debug/obj/arm64-v8a/virtualMouse" ]; then
    echo "✓ Native binary built"
    ls -lh app/build/intermediates/cmake/debug/obj/*/virtualMouse 2>/dev/null || true
fi

echo ""
echo "======================================"
echo -e "${GREEN}✅ All checks completed!${NC}"
echo "======================================"
echo ""
echo "Next steps:"
echo "  1. Review any warnings above"
echo "  2. Check lint report: app/build/reports/lint-results-debug.html"
echo "  3. Run: git add . && git commit -m 'your message'"
echo ""
