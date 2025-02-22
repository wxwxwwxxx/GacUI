#include "../../../Source/GacUI.h"
#if defined VCZH_MSVC
#include <Windows.h>
#endif

using namespace vl;

namespace vl
{
	namespace presentation
	{
		void GuiInitializeUtilities() {}
		void GuiFinalizeUtilities() {}
	}
}

#if defined VCZH_MSVC
using namespace vl::filesystem;
#endif

#if defined VCZH_MSVC
WString GetExePath()
{
	wchar_t buffer[65536];
	GetModuleFileName(NULL, buffer, sizeof(buffer) / sizeof(*buffer));
	vint pos = -1;
	vint index = 0;
	while (buffer[index])
	{
		if (buffer[index] == L'\\')
		{
			pos = index;
		}
		index++;
	}
	return WString::CopyFrom(buffer, pos + 1);
}
#endif

namespace compiler_error_tests
{
	WString GetTestResourcePath()
	{
	#if defined VCZH_MSVC
	#ifdef _WIN64
		return GetExePath() + L"..\\..\\..\\Resources\\CompilerErrorTests";
	#else
		return GetExePath() + L"..\\..\\Resources\\CompilerErrorTests";
	#endif
	#elif defined VCZH_GCC
		return L"../../Resources/CompilerErrorTests";
	#endif
	}
	
	WString GetTestBaselinePath()
	{
	#if defined VCZH_MSVC
	#ifdef _WIN64
		return GetExePath() + L"..\\..\\..\\Resources\\CompilerErrorTests\\Baseline_x64";
	#else
		return GetExePath() + L"..\\..\\Resources\\CompilerErrorTests\\Baseline_x86";
	#endif
	#elif defined VCZH_GCC
		return L"../../Resources/CompilerErrorTests/Baseline_x64";
	#endif
	}
	
	WString GetTestOutputPath()
	{
	#if defined VCZH_MSVC
	#ifdef _WIN64
		return GetExePath() + L"..\\..\\..\\Output\\CompilerErrorTests\\x64";
	#else
		return GetExePath() + L"..\\..\\Output\\CompilerErrorTests\\x86";
	#endif
	#elif defined VCZH_GCC
		return L"../../Output/CompilerErrorTests";
	#endif
	}
}

namespace hosted_window_manager_tests
{
	WString GetTestBaselinePath()
	{
	#if defined VCZH_MSVC
	#ifdef _WIN64
		return GetExePath() + L"..\\..\\..\\Resources\\HostedWindowManagerTests";
	#else
		return GetExePath() + L"..\\..\\Resources\\HostedWindowManagerTests";
	#endif
	#elif defined VCZH_GCC
		return L"../../Resources/HostedWindowManagerTests";
	#endif
	}
	
	WString GetTestOutputPath()
	{
	#if defined VCZH_MSVC
	#ifdef _WIN64
		return GetExePath() + L"..\\..\\..\\Output\\HostedWindowManagerTests";
	#else
		return GetExePath() + L"..\\..\\Output\\HostedWindowManagerTests";
	#endif
	#elif defined VCZH_GCC
		return L"../../Output/HostedWindowManagerTests";
	#endif
	}
}

#if defined VCZH_MSVC
TEST_FILE
{
	{
		Folder folder(compiler_error_tests::GetTestOutputPath());
		if (!folder.Exists())
		{
			TEST_CASE_ASSERT(folder.Create(true) == true);
		}
	}
	{
		Folder folder(hosted_window_manager_tests::GetTestOutputPath());
		if (!folder.Exists())
		{
			TEST_CASE_ASSERT(folder.Create(true) == true);
		}
	}
}
#endif

void (*GuiMainProxy)();

void SetGuiMainProxy(void(*proxy)())
{
	GuiMainProxy = proxy;
}

void GuiMain()
{
	GuiMainProxy();
}

namespace vl::presentation::controls
{
	extern bool GACUI_UNITTEST_ONLY_SKIP_THREAD_LOCAL_STORAGE_DISPOSE_STORAGES;
}
using namespace vl::presentation::controls;

#if defined VCZH_MSVC
int wmain(int argc, wchar_t* argv[])
{
	GACUI_UNITTEST_ONLY_SKIP_THREAD_LOCAL_STORAGE_DISPOSE_STORAGES = true;
	int result = unittest::UnitTest::RunAndDisposeTests(argc, argv);
	ThreadLocalStorage::DisposeStorages();
#if defined VCZH_CHECK_MEMORY_LEAKS
	_CrtDumpMemoryLeaks();
#endif
	return result;
}
#elif defined VCZH_GCC
int main(int argc, char* argv[])
{
	GACUI_UNITTEST_ONLY_SKIP_THREAD_LOCAL_STORAGE_DISPOSE_STORAGES = true;
	int result = unittest::UnitTest::RunAndDisposeTests(argc, argv);
	ThreadLocalStorage::DisposeStorages();
	return result;
}
#endif