#include "../../../Source/GacUI.h"
#include "../../../Source/PlatformProviders/Hosted/GuiHostedWindowManager.h"

using namespace vl;
using namespace vl::collections;
using namespace vl::stream;
using namespace vl::filesystem;
using namespace vl::presentation;

namespace hosted_window_manager_tests
{
	extern WString GetTestBaselinePath();
	extern WString GetTestOutputPath();
}
using namespace hosted_window_manager_tests;

#define UPDATE_SNAPSHOT

using Window = hosted_window_manager::Window<wchar_t>;

struct WindowManager : hosted_window_manager::WindowManager<wchar_t>
{
	WString						unitTestTitle;
	List<Pair<vint, WString>>	snapshots;

	WindowManager(const wchar_t* _unitTestTitle)
	{
		unitTestTitle = GenerateToStream([&](StreamWriter& writer)
		{
			while (auto c = *_unitTestTitle++)
			{
				writer.WriteChar(c == L' ' ? L'_' : c);
			}
		});
	}

#pragma warning(push)
#pragma warning(disable: 4297)
	~WindowManager()
	{
		auto snapshotPath = FilePath(GetTestBaselinePath()) / (unitTestTitle + L".txt");
#ifdef UPDATE_SNAPSHOT
		FileStream fileStream(snapshotPath.GetFullPath(), FileStream::WriteOnly);
		BomEncoder encoder(BomEncoder::Utf8);
		EncoderStream encoderStream(fileStream, encoder);
		StreamWriter writer(encoderStream);
		for (auto [lines, snapshot] : snapshots)
		{
			writer.WriteLine(snapshot);
		}
		writer.WriteLine(L"END");
#else
		FileStream fileStream(snapshotPath.GetFullPath(), FileStream::ReadOnly);
		BomDecoder decoder;
		DecoderStream decoderStream(fileStream, decoder);
		StreamReader reader(decoderStream);
		for (auto [lines, snapshot] : snapshots)
		{
			WString baseline = GenerateToStream([&](StreamWriter& writer)
			{
				for (vint i = 0; i < lines; i++)
				{
					writer.WriteLine(reader.ReadLine());
				}
			});
			TEST_ASSERT(snapshot == baseline);
			TEST_ASSERT(reader.ReadLine() == L"");
		}
		TEST_ASSERT(reader.ReadLine() == L"END");
#endif
	}
#pragma warning(pop)

	void CheckWindowStatus()
	{
		SortedList<Window*> activeWindows;
		{
			auto current = activeWindow;
			while (current)
			{
				if (current->enabled)
				{
					activeWindows.Add(current);
				}
				current = current->parent;
			}
		}

		for (auto window : registeredWindows.Values())
		{
			TEST_ASSERT(window->active == (window == activeWindow));
			TEST_ASSERT(window->renderedAsActive == activeWindows.Contains(window));
			TEST_ASSERT(!window->active || window->enabled);
			TEST_ASSERT(!window->active || window->renderedAsActive);
			TEST_ASSERT((window->parent != nullptr) == (window != mainWindow));

			bool topMost = window->IsEventuallyTopMost();
			if (window->visible && topMost)
			{
				TEST_ASSERT(topMostedWindowsInOrder.Contains(window));
				auto current = window;
				while (current && !current->visible)
				{
					current = current->parent;
				}
				if (current && current != window && current->IsEventuallyTopMost())
				{
					TEST_ASSERT(topMostedWindowsInOrder.IndexOf(window) < topMostedWindowsInOrder.IndexOf(current));
				}
			}
			else
			{
				TEST_ASSERT(!topMostedWindowsInOrder.Contains(window));
			}

			if (window->visible && !topMost)
			{
				TEST_ASSERT(ordinaryWindowsInOrder.Contains(window));
				auto current = window;
				while (current && !current->visible)
				{
					current = current->parent;
				}
				if (current && current != window)
				{
					TEST_ASSERT(!current->IsEventuallyTopMost());
					TEST_ASSERT(ordinaryWindowsInOrder.IndexOf(window) < ordinaryWindowsInOrder.IndexOf(current));
				}
			}
			else
			{
				TEST_ASSERT(!ordinaryWindowsInOrder.Contains(window));
			}
		}
	}

	void TakeSnapshot(const wchar_t* title)
	{
		vint w = 0;
		vint h = 0;
		for (auto window : registeredWindows.Values())
		{
			if (w < window->bounds.Width().value) w = window->bounds.Width().value;
			if (h < window->bounds.Height().value) h = window->bounds.Height().value;
		}

		WString snapshot = GenerateToStream([=](StreamWriter& writer)
		{
			writer.WriteLine(title);
			for (vint y = 0; y < h; y++)
			{
				writer.WriteChar(L'[');
				for (vint x = 0; x < w; x++)
				{
					auto window = HitTest({ {x},{y} });
					if (!window)
					{
						writer.WriteChar(L' ');
					}
					else if (!window->enabled)
					{
						writer.WriteChar((x + y) % 2 == 0 ? '~' : window->id + (L'a' - L'A'));
					}
					else if (!window->renderedAsActive)
					{
						writer.WriteChar(window->id + (L'a' - L'A'));
					}
					else
					{
						writer.WriteChar(window->id);
					}
				}
				writer.WriteLine(WString::Unmanaged(L"]"));
			}
		});
		snapshots.Add({ h + 1,snapshot });
	}

	void DontTakeSnapshot(const wchar_t* title)
	{
		snapshots.Add({ 1,WString::Unmanaged(title) + WString::Unmanaged(L"\r\n") });
	}
};

#define WM_TEST_CASE(NAME)							\
	TEST_CASE(NAME)									\
	{												\
		WindowManager wm(NAME);						\

#define TAKE_SNAPSHOT_INTERNAL(COMMAND, TITLE)		\
	do{												\
		COMMAND;									\
		TEST_ASSERT(wm.needRefresh);				\
		wm.needRefresh = false;						\
		wm.CheckWindowStatus();						\
		wm.TakeSnapshot(TITLE);						\
	}while(false)									\

#define TAKE_SNAPSHOT(COMMAND) TAKE_SNAPSHOT_INTERNAL(COMMAND, L ## #COMMAND)
#define TAKE_SNAPSHOT_INITIAL() TAKE_SNAPSHOT_INTERNAL((void)nullptr, L"<Initial>")

#define DONT_TAKE_SNAPSHOT(...)						\
	do {											\
		__VA_ARGS__;								\
		TEST_ASSERT(!wm.needRefresh);				\
		wm.CheckWindowStatus();						\
		wm.DontTakeSnapshot(L ## #__VA_ARGS__);		\
	}while(false)									\

NativeRect Bounds(vint x, vint y, vint w, vint h)
{
	return { { {x},{y} },{ {w},{h} } };
}

TEST_FILE
{
	WM_TEST_CASE(L"Start and stop")
	{
		Window mainWindow(L'X', true);
		wm.RegisterWindow(&mainWindow);

		mainWindow.SetBounds(Bounds(0, 0, 6, 4));

		wm.Start(&mainWindow);
		mainWindow.Show();
		TAKE_SNAPSHOT_INITIAL();

		wm.Stop();
		wm.UnregisterWindow(&mainWindow);
		wm.EnsureCleanedUp();
	}});

	WM_TEST_CASE(L"Activing windows")
	{
		Window mainWindow(L'X', true);
		wm.RegisterWindow(&mainWindow);

		Window windowA(L'A', true);
		wm.RegisterWindow(&windowA);

		Window windowB(L'B', true);
		wm.RegisterWindow(&windowB);

		mainWindow.SetBounds(Bounds(0, 0, 7, 6));
		windowA.SetBounds(Bounds(1, 1, 4, 3));
		windowB.SetBounds(Bounds(2, 2, 4, 3));

		wm.Start(&mainWindow);
		TEST_ASSERT(windowA.parent == &mainWindow);
		TEST_ASSERT(windowB.parent == &mainWindow);
		mainWindow.Show();
		windowA.Show();
		windowB.Show();
		TAKE_SNAPSHOT_INITIAL();

		TAKE_SNAPSHOT(windowA.Activate());
		TAKE_SNAPSHOT(mainWindow.Activate());
		TAKE_SNAPSHOT(windowB.Activate());
		TAKE_SNAPSHOT(mainWindow.Activate());

		wm.Stop();
		wm.UnregisterWindow(&mainWindow);
		wm.UnregisterWindow(&windowA);
		wm.UnregisterWindow(&windowB);
		wm.EnsureCleanedUp();
	}});

	WM_TEST_CASE(L"Deactivating windows")
	{
		Window mainWindow(L'X', true);
		wm.RegisterWindow(&mainWindow);

		Window windowA(L'A', true);
		wm.RegisterWindow(&windowA);

		Window windowB(L'B', true);
		wm.RegisterWindow(&windowB);

		mainWindow.SetBounds(Bounds(0, 0, 7, 6));
		windowA.SetBounds(Bounds(1, 1, 4, 3));
		windowB.SetBounds(Bounds(2, 2, 4, 3));

		wm.Start(&mainWindow);
		TEST_ASSERT(windowA.parent == &mainWindow);
		TEST_ASSERT(windowB.parent == &mainWindow);
		mainWindow.Show();
		windowA.Show();
		windowB.Show();
		TAKE_SNAPSHOT_INITIAL();

		DONT_TAKE_SNAPSHOT(windowA.Inactivate());
		DONT_TAKE_SNAPSHOT(mainWindow.Inactivate());
		TAKE_SNAPSHOT(windowB.Inactivate());
		TAKE_SNAPSHOT(mainWindow.Inactivate());

		wm.Stop();
		wm.UnregisterWindow(&mainWindow);
		wm.UnregisterWindow(&windowA);
		wm.UnregisterWindow(&windowB);
		wm.EnsureCleanedUp();
	}});

	WM_TEST_CASE(L"Activing many windows")
	{
		Window mainWindow(L'X', true);
		wm.RegisterWindow(&mainWindow);

		Window windowA(L'A', true);
		wm.RegisterWindow(&windowA);

		Window windowB(L'B', true);
		wm.RegisterWindow(&windowB);

		Window windowC(L'C', true);
		wm.RegisterWindow(&windowC);

		Window windowI(L'I', true);
		wm.RegisterWindow(&windowI);

		Window windowJ(L'J', true);
		wm.RegisterWindow(&windowJ);

		Window windowK(L'K', true);
		wm.RegisterWindow(&windowK);

		mainWindow.SetBounds(Bounds(0, 0, 12, 10));
		windowA.SetBounds(Bounds(1, 3, 6, 4));
		windowB.SetBounds(Bounds(2, 4, 6, 4));
		windowC.SetBounds(Bounds(3, 5, 6, 4));
		windowI.SetBounds(Bounds(3, 1, 6, 4));
		windowJ.SetBounds(Bounds(4, 2, 6, 4));
		windowK.SetBounds(Bounds(5, 3, 6, 4));

		windowB.SetParent(&windowA);
		windowC.SetParent(&windowB);
		windowI.SetParent(&mainWindow);
		windowJ.SetParent(&windowI);
		windowK.SetParent(&windowI);

		wm.Start(&mainWindow);
		TEST_ASSERT(windowA.parent == &mainWindow);
		TEST_ASSERT(windowB.parent == &windowA);
		TEST_ASSERT(windowC.parent == &windowB);
		TEST_ASSERT(windowI.parent == &mainWindow);
		TEST_ASSERT(windowJ.parent == &windowI);
		TEST_ASSERT(windowK.parent == &windowI);
		mainWindow.Show();
		windowA.Show();
		windowB.Show();
		windowC.Show();
		windowI.Show();
		windowJ.Show();
		windowK.Show();
		TAKE_SNAPSHOT_INITIAL();

		TAKE_SNAPSHOT(windowA.Activate());
		TAKE_SNAPSHOT(windowB.Activate());
		TAKE_SNAPSHOT(windowC.Activate());
		TAKE_SNAPSHOT(mainWindow.Activate());
		TAKE_SNAPSHOT(windowI.Activate());
		TAKE_SNAPSHOT(windowJ.Activate());
		TAKE_SNAPSHOT(windowK.Activate());
		TAKE_SNAPSHOT(mainWindow.Activate());

		wm.Stop();
		wm.UnregisterWindow(&mainWindow);
		wm.UnregisterWindow(&windowA);
		wm.UnregisterWindow(&windowB);
		wm.UnregisterWindow(&windowC);
		wm.UnregisterWindow(&windowI);
		wm.UnregisterWindow(&windowJ);
		wm.UnregisterWindow(&windowK);
		wm.EnsureCleanedUp();
	}});

	WM_TEST_CASE(L"Deactivating many windows")
	{
		Window mainWindow(L'X', true);
		wm.RegisterWindow(&mainWindow);

		Window windowA(L'A', true);
		wm.RegisterWindow(&windowA);

		Window windowB(L'B', true);
		wm.RegisterWindow(&windowB);

		Window windowC(L'C', true);
		wm.RegisterWindow(&windowC);

		Window windowI(L'I', true);
		wm.RegisterWindow(&windowI);

		Window windowJ(L'J', true);
		wm.RegisterWindow(&windowJ);

		Window windowK(L'K', true);
		wm.RegisterWindow(&windowK);

		mainWindow.SetBounds(Bounds(0, 0, 12, 10));
		windowA.SetBounds(Bounds(1, 3, 6, 4));
		windowB.SetBounds(Bounds(2, 4, 6, 4));
		windowC.SetBounds(Bounds(3, 5, 6, 4));
		windowI.SetBounds(Bounds(3, 1, 6, 4));
		windowJ.SetBounds(Bounds(4, 2, 6, 4));
		windowK.SetBounds(Bounds(5, 3, 6, 4));

		windowB.SetParent(&windowA);
		windowC.SetParent(&windowB);
		windowI.SetParent(&mainWindow);
		windowJ.SetParent(&windowI);
		windowK.SetParent(&windowI);

		wm.Start(&mainWindow);
		TEST_ASSERT(windowA.parent == &mainWindow);
		TEST_ASSERT(windowB.parent == &windowA);
		TEST_ASSERT(windowC.parent == &windowB);
		TEST_ASSERT(windowI.parent == &mainWindow);
		TEST_ASSERT(windowJ.parent == &windowI);
		TEST_ASSERT(windowK.parent == &windowI);
		mainWindow.Show();
		windowA.Show();
		windowB.Show();
		windowC.Show();
		windowI.Show();
		windowJ.Show();
		windowK.Show();
		TAKE_SNAPSHOT_INITIAL();

		TAKE_SNAPSHOT(windowK.Inactivate());
		TAKE_SNAPSHOT(windowI.Inactivate());
		TAKE_SNAPSHOT(windowJ.Activate());
		TAKE_SNAPSHOT(windowJ.Inactivate());
		TAKE_SNAPSHOT(windowI.Inactivate());
		TAKE_SNAPSHOT(windowC.Activate());
		TAKE_SNAPSHOT(windowC.Inactivate());
		TAKE_SNAPSHOT(windowB.Inactivate());
		TAKE_SNAPSHOT(windowA.Inactivate());
		TAKE_SNAPSHOT(mainWindow.Inactivate());

		wm.Stop();
		wm.UnregisterWindow(&mainWindow);
		wm.UnregisterWindow(&windowA);
		wm.UnregisterWindow(&windowB);
		wm.UnregisterWindow(&windowC);
		wm.UnregisterWindow(&windowI);
		wm.UnregisterWindow(&windowJ);
		wm.UnregisterWindow(&windowK);
		wm.EnsureCleanedUp();
	}});

	WM_TEST_CASE(L"Disabling windows")
	{
	}});

	WM_TEST_CASE(L"Hiding windows")
	{
	}});

	WM_TEST_CASE(L"Closing windows")
	{
		Window mainWindow(L'X', true);
		wm.RegisterWindow(&mainWindow);

		Window windowA(L'A', true);
		wm.RegisterWindow(&windowA);

		Window windowB(L'B', true);
		wm.RegisterWindow(&windowB);

		Window windowC(L'C', true);
		wm.RegisterWindow(&windowC);

		mainWindow.SetBounds(Bounds(0, 0, 8, 7));
		windowA.SetBounds(Bounds(1, 1, 4, 3));
		windowB.SetBounds(Bounds(2, 2, 4, 3));
		windowC.SetBounds(Bounds(3, 3, 4, 3));

		windowC.SetParent(&windowB);

		wm.Start(&mainWindow);
		TEST_ASSERT(windowA.parent == &mainWindow);
		TEST_ASSERT(windowB.parent == &mainWindow);
		TEST_ASSERT(windowC.parent == &windowB);
		mainWindow.Show();
		windowA.Show();
		windowB.Show();
		windowC.Show();
		TAKE_SNAPSHOT_INITIAL();

		TAKE_SNAPSHOT(windowC.SetVisible(false));
		TAKE_SNAPSHOT(windowB.SetVisible(false));
		TAKE_SNAPSHOT(windowA.SetVisible(false));
		TAKE_SNAPSHOT(windowC.SetVisible(true));
		TAKE_SNAPSHOT(windowB.SetVisible(true));
		TAKE_SNAPSHOT(windowA.SetVisible(true));

		TAKE_SNAPSHOT(windowA.SetVisible(false));
		TAKE_SNAPSHOT(windowA.SetVisible(true));
		TAKE_SNAPSHOT(windowC.SetVisible(false));
		TAKE_SNAPSHOT(windowC.SetVisible(true));
		TAKE_SNAPSHOT(windowB.SetVisible(false));
		TAKE_SNAPSHOT(windowB.SetVisible(true));

		wm.Stop();
		wm.UnregisterWindow(&mainWindow);
		wm.UnregisterWindow(&windowA);
		wm.UnregisterWindow(&windowB);
		wm.UnregisterWindow(&windowC);
		wm.EnsureCleanedUp();
	}});

	WM_TEST_CASE(L"Changing topmost of windows")
	{
		Window mainWindow(L'X', true);
		wm.RegisterWindow(&mainWindow);

		Window windowA(L'A', true);
		wm.RegisterWindow(&windowA);

		Window windowB(L'B', true);
		wm.RegisterWindow(&windowB);

		Window windowC(L'C', true);
		wm.RegisterWindow(&windowC);

		mainWindow.SetBounds(Bounds(0, 0, 8, 7));
		windowA.SetBounds(Bounds(1, 1, 4, 3));
		windowB.SetBounds(Bounds(2, 2, 4, 3));
		windowC.SetBounds(Bounds(3, 3, 4, 3));

		windowC.SetParent(&windowB);

		wm.Start(&mainWindow);
		TEST_ASSERT(windowA.parent == &mainWindow);
		TEST_ASSERT(windowB.parent == &mainWindow);
		TEST_ASSERT(windowC.parent == &windowB);
		mainWindow.Show();
		windowA.Show();
		windowB.Show();
		windowC.Show();
		TAKE_SNAPSHOT_INITIAL();

		TAKE_SNAPSHOT(windowA.SetTopMost(true));
		TAKE_SNAPSHOT(windowA.Activate());
		TAKE_SNAPSHOT(windowB.Activate());
		TAKE_SNAPSHOT(windowC.Activate());

		TAKE_SNAPSHOT(windowB.SetTopMost(true));
		TAKE_SNAPSHOT(windowA.Activate());
		TAKE_SNAPSHOT(windowB.Activate());
		TAKE_SNAPSHOT(windowC.Activate());

		DONT_TAKE_SNAPSHOT(windowC.SetTopMost(true));
		TAKE_SNAPSHOT(windowA.Activate());
		TAKE_SNAPSHOT(windowB.Activate());
		TAKE_SNAPSHOT(windowC.Activate());

		TAKE_SNAPSHOT(windowA.SetTopMost(false));
		TAKE_SNAPSHOT(windowA.Activate());
		TAKE_SNAPSHOT(windowB.Activate());
		TAKE_SNAPSHOT(windowC.Activate());

		TAKE_SNAPSHOT(windowB.SetTopMost(false));
		TAKE_SNAPSHOT(windowA.Activate());
		TAKE_SNAPSHOT(windowB.Activate());
		TAKE_SNAPSHOT(windowC.Activate());

		TAKE_SNAPSHOT(windowC.SetTopMost(false));
		TAKE_SNAPSHOT(windowA.Activate());
		TAKE_SNAPSHOT(windowB.Activate());
		TAKE_SNAPSHOT(windowC.Activate());

		wm.Stop();
		wm.UnregisterWindow(&mainWindow);
		wm.UnregisterWindow(&windowA);
		wm.UnregisterWindow(&windowB);
		wm.UnregisterWindow(&windowC);
		wm.EnsureCleanedUp();
	}});

	WM_TEST_CASE(L"Activing windows with topmost")
	{
		Window mainWindow(L'X', true);
		wm.RegisterWindow(&mainWindow);

		Window windowA(L'A', true);
		wm.RegisterWindow(&windowA);

		Window windowB(L'B', true);
		wm.RegisterWindow(&windowB);

		mainWindow.SetBounds(Bounds(0, 0, 7, 6));
		windowA.SetBounds(Bounds(1, 1, 4, 3));
		windowB.SetBounds(Bounds(2, 2, 4, 3));

		wm.Start(&mainWindow);
		TEST_ASSERT(windowA.parent == &mainWindow);
		TEST_ASSERT(windowB.parent == &mainWindow);
		mainWindow.Show();
		windowA.Show();
		windowB.Show();
		TAKE_SNAPSHOT_INITIAL();

		TAKE_SNAPSHOT(windowA.SetTopMost(true));
		TAKE_SNAPSHOT(windowA.Activate());
		TAKE_SNAPSHOT(mainWindow.Activate());
		TAKE_SNAPSHOT(windowB.Activate());
		TAKE_SNAPSHOT(mainWindow.Activate());
		TAKE_SNAPSHOT(windowA.SetTopMost(false));
		TAKE_SNAPSHOT(windowA.Activate());
		TAKE_SNAPSHOT(windowB.SetTopMost(true));
		TAKE_SNAPSHOT(windowB.SetTopMost(false));
		TAKE_SNAPSHOT(mainWindow.Activate());
		TAKE_SNAPSHOT(windowA.Activate());
		TAKE_SNAPSHOT(mainWindow.Activate());

		wm.Stop();
		wm.UnregisterWindow(&mainWindow);
		wm.UnregisterWindow(&windowA);
		wm.UnregisterWindow(&windowB);
		wm.EnsureCleanedUp();
	}});

	WM_TEST_CASE(L"Deactivating windows with topmost")
	{
		Window mainWindow(L'X', true);
		wm.RegisterWindow(&mainWindow);

		Window windowA(L'A', true);
		wm.RegisterWindow(&windowA);

		Window windowB(L'B', true);
		wm.RegisterWindow(&windowB);

		mainWindow.SetBounds(Bounds(0, 0, 7, 6));
		windowA.SetBounds(Bounds(1, 1, 4, 3));
		windowB.SetBounds(Bounds(2, 2, 4, 3));

		wm.Start(&mainWindow);
		TEST_ASSERT(windowA.parent == &mainWindow);
		TEST_ASSERT(windowB.parent == &mainWindow);
		mainWindow.Show();
		windowA.Show();
		windowB.Show();
		TAKE_SNAPSHOT_INITIAL();

		TAKE_SNAPSHOT(windowA.SetTopMost(true));
		DONT_TAKE_SNAPSHOT(windowA.Inactivate());
		DONT_TAKE_SNAPSHOT(mainWindow.Inactivate());
		TAKE_SNAPSHOT(windowB.Inactivate());
		TAKE_SNAPSHOT(mainWindow.Inactivate());

		wm.Stop();
		wm.UnregisterWindow(&mainWindow);
		wm.UnregisterWindow(&windowA);
		wm.UnregisterWindow(&windowB);
		wm.EnsureCleanedUp();
	}});

	WM_TEST_CASE(L"Activing many windows with topmost")
	{
		Window mainWindow(L'X', true);
		wm.RegisterWindow(&mainWindow);

		Window windowA(L'A', true);
		wm.RegisterWindow(&windowA);

		Window windowB(L'B', true);
		wm.RegisterWindow(&windowB);

		Window windowC(L'C', true);
		wm.RegisterWindow(&windowC);

		Window windowI(L'I', true);
		wm.RegisterWindow(&windowI);

		Window windowJ(L'J', true);
		wm.RegisterWindow(&windowJ);

		Window windowK(L'K', true);
		wm.RegisterWindow(&windowK);

		mainWindow.SetBounds(Bounds(0, 0, 12, 10));
		windowA.SetBounds(Bounds(1, 3, 6, 4));
		windowB.SetBounds(Bounds(2, 4, 6, 4));
		windowC.SetBounds(Bounds(3, 5, 6, 4));
		windowI.SetBounds(Bounds(3, 1, 6, 4));
		windowJ.SetBounds(Bounds(4, 2, 6, 4));
		windowK.SetBounds(Bounds(5, 3, 6, 4));

		windowB.SetParent(&windowA);
		windowC.SetParent(&windowB);
		windowI.SetParent(&mainWindow);
		windowJ.SetParent(&windowI);
		windowK.SetParent(&windowI);

		wm.Start(&mainWindow);
		TEST_ASSERT(windowA.parent == &mainWindow);
		TEST_ASSERT(windowB.parent == &windowA);
		TEST_ASSERT(windowC.parent == &windowB);
		TEST_ASSERT(windowI.parent == &mainWindow);
		TEST_ASSERT(windowJ.parent == &windowI);
		TEST_ASSERT(windowK.parent == &windowI);
		mainWindow.Show();
		windowA.Show();
		windowB.Show();
		windowC.Show();
		windowI.Show();
		windowJ.Show();
		windowK.Show();
		TAKE_SNAPSHOT_INITIAL();

		TAKE_SNAPSHOT(windowI.SetTopMost(true));
		TAKE_SNAPSHOT(windowA.Activate());
		TAKE_SNAPSHOT(windowB.Activate());
		TAKE_SNAPSHOT(windowC.Activate());
		TAKE_SNAPSHOT(mainWindow.Activate());
		TAKE_SNAPSHOT(windowI.Activate());
		TAKE_SNAPSHOT(windowJ.Activate());
		TAKE_SNAPSHOT(windowK.Activate());
		TAKE_SNAPSHOT(mainWindow.Activate());

		TAKE_SNAPSHOT(windowA.SetTopMost(true));
		TAKE_SNAPSHOT(windowA.Activate());
		TAKE_SNAPSHOT(windowB.Activate());
		TAKE_SNAPSHOT(windowC.Activate());
		TAKE_SNAPSHOT(mainWindow.Activate());
		TAKE_SNAPSHOT(windowI.Activate());
		TAKE_SNAPSHOT(windowJ.Activate());
		TAKE_SNAPSHOT(windowK.Activate());
		TAKE_SNAPSHOT(mainWindow.Activate());

		TAKE_SNAPSHOT(windowI.SetTopMost(false));
		TAKE_SNAPSHOT(windowA.Activate());
		TAKE_SNAPSHOT(windowB.Activate());
		TAKE_SNAPSHOT(windowC.Activate());
		TAKE_SNAPSHOT(mainWindow.Activate());
		TAKE_SNAPSHOT(windowI.Activate());
		TAKE_SNAPSHOT(windowJ.Activate());
		TAKE_SNAPSHOT(windowK.Activate());
		TAKE_SNAPSHOT(mainWindow.Activate());

		DONT_TAKE_SNAPSHOT(windowC.SetTopMost(true));
		TAKE_SNAPSHOT(windowA.Activate());
		TAKE_SNAPSHOT(windowB.Activate());
		TAKE_SNAPSHOT(windowC.Activate());
		TAKE_SNAPSHOT(mainWindow.Activate());
		TAKE_SNAPSHOT(windowI.Activate());
		TAKE_SNAPSHOT(windowJ.Activate());
		TAKE_SNAPSHOT(windowK.Activate());
		TAKE_SNAPSHOT(mainWindow.Activate());

		TAKE_SNAPSHOT(windowA.SetTopMost(false));
		TAKE_SNAPSHOT(windowA.Activate());
		TAKE_SNAPSHOT(windowB.Activate());
		TAKE_SNAPSHOT(windowC.Activate());
		TAKE_SNAPSHOT(mainWindow.Activate());
		TAKE_SNAPSHOT(windowI.Activate());
		TAKE_SNAPSHOT(windowJ.Activate());
		TAKE_SNAPSHOT(windowK.Activate());
		TAKE_SNAPSHOT(mainWindow.Activate());

		wm.Stop();
		wm.UnregisterWindow(&mainWindow);
		wm.UnregisterWindow(&windowA);
		wm.UnregisterWindow(&windowB);
		wm.UnregisterWindow(&windowC);
		wm.UnregisterWindow(&windowI);
		wm.UnregisterWindow(&windowJ);
		wm.UnregisterWindow(&windowK);
		wm.EnsureCleanedUp();
	}});

	WM_TEST_CASE(L"Deactivating many windows with topmost")
	{
		Window mainWindow(L'X', true);
		wm.RegisterWindow(&mainWindow);

		Window windowA(L'A', true);
		wm.RegisterWindow(&windowA);

		Window windowB(L'B', true);
		wm.RegisterWindow(&windowB);

		Window windowC(L'C', true);
		wm.RegisterWindow(&windowC);

		Window windowI(L'I', true);
		wm.RegisterWindow(&windowI);

		Window windowJ(L'J', true);
		wm.RegisterWindow(&windowJ);

		Window windowK(L'K', true);
		wm.RegisterWindow(&windowK);

		mainWindow.SetBounds(Bounds(0, 0, 12, 10));
		windowA.SetBounds(Bounds(1, 3, 6, 4));
		windowB.SetBounds(Bounds(2, 4, 6, 4));
		windowC.SetBounds(Bounds(3, 5, 6, 4));
		windowI.SetBounds(Bounds(3, 1, 6, 4));
		windowJ.SetBounds(Bounds(4, 2, 6, 4));
		windowK.SetBounds(Bounds(5, 3, 6, 4));

		windowB.SetParent(&windowA);
		windowC.SetParent(&windowB);
		windowI.SetParent(&mainWindow);
		windowJ.SetParent(&windowI);
		windowK.SetParent(&windowI);

		wm.Start(&mainWindow);
		TEST_ASSERT(windowA.parent == &mainWindow);
		TEST_ASSERT(windowB.parent == &windowA);
		TEST_ASSERT(windowC.parent == &windowB);
		TEST_ASSERT(windowI.parent == &mainWindow);
		TEST_ASSERT(windowJ.parent == &windowI);
		TEST_ASSERT(windowK.parent == &windowI);
		mainWindow.Show();
		windowA.Show();
		windowB.Show();
		windowC.Show();
		windowI.Show();
		windowJ.Show();
		windowK.Show();
		TAKE_SNAPSHOT_INITIAL();

		TAKE_SNAPSHOT(windowA.SetTopMost(true));
		TAKE_SNAPSHOT(windowK.Inactivate());
		TAKE_SNAPSHOT(windowI.Inactivate());
		TAKE_SNAPSHOT(windowJ.Activate());
		TAKE_SNAPSHOT(windowJ.Inactivate());
		TAKE_SNAPSHOT(windowI.Inactivate());
		TAKE_SNAPSHOT(windowC.Activate());
		TAKE_SNAPSHOT(windowC.Inactivate());
		TAKE_SNAPSHOT(windowB.Inactivate());
		TAKE_SNAPSHOT(windowA.Inactivate());
		TAKE_SNAPSHOT(mainWindow.Inactivate());
		TAKE_SNAPSHOT(windowK.Activate());

		TAKE_SNAPSHOT(windowI.SetTopMost(true));
		TAKE_SNAPSHOT(windowK.Inactivate());
		TAKE_SNAPSHOT(windowI.Inactivate());
		TAKE_SNAPSHOT(windowJ.Activate());
		TAKE_SNAPSHOT(windowJ.Inactivate());
		TAKE_SNAPSHOT(windowI.Inactivate());
		TAKE_SNAPSHOT(windowC.Activate());
		TAKE_SNAPSHOT(windowC.Inactivate());
		TAKE_SNAPSHOT(windowB.Inactivate());
		TAKE_SNAPSHOT(windowA.Inactivate());
		TAKE_SNAPSHOT(mainWindow.Inactivate());
		TAKE_SNAPSHOT(windowK.Activate());

		DONT_TAKE_SNAPSHOT(windowC.SetTopMost(true));
		DONT_TAKE_SNAPSHOT(windowJ.SetTopMost(true));
		TAKE_SNAPSHOT(windowA.SetTopMost(false));
		TAKE_SNAPSHOT(windowI.SetTopMost(false));
		TAKE_SNAPSHOT(windowK.Inactivate());
		TAKE_SNAPSHOT(windowI.Inactivate());
		TAKE_SNAPSHOT(windowJ.Activate());
		TAKE_SNAPSHOT(windowJ.Inactivate());
		TAKE_SNAPSHOT(windowI.Inactivate());
		TAKE_SNAPSHOT(windowC.Activate());
		TAKE_SNAPSHOT(windowC.Inactivate());
		TAKE_SNAPSHOT(windowB.Inactivate());
		TAKE_SNAPSHOT(windowA.Inactivate());
		TAKE_SNAPSHOT(mainWindow.Inactivate());
		TAKE_SNAPSHOT(windowK.Activate());

		wm.Stop();
		wm.UnregisterWindow(&mainWindow);
		wm.UnregisterWindow(&windowA);
		wm.UnregisterWindow(&windowB);
		wm.UnregisterWindow(&windowC);
		wm.UnregisterWindow(&windowI);
		wm.UnregisterWindow(&windowJ);
		wm.UnregisterWindow(&windowK);
		wm.EnsureCleanedUp();
	}});

	WM_TEST_CASE(L"Disabling windows with topmost")
	{
	}});

	WM_TEST_CASE(L"Hiding windows with topmost")
	{
	}});

	WM_TEST_CASE(L"Closing windows with topmost")
	{
	}});

	// TODO: Assert events sent from window manager
	// TODO: non-normal windows
}