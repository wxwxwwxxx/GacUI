#include "TestCompositions.h"
using namespace composition_bounds_tests;

TEST_FILE
{
	TEST_CATEGORY(L"Test <SideAligned> with all directions")
	{
		auto bounds = new GuiBoundsComposition;
		bounds->SetPreferredMinSize(Size(100, 50));

		auto sideAligned = new GuiSideAlignedComposition;
		bounds->AddChild(sideAligned);

		TEST_CASE(L"Default properties")
		{
			TEST_ASSERT(sideAligned->GetDirection() == GuiSideAlignedComposition::Top);
			TEST_ASSERT(sideAligned->GetMaxLength() == 10);
			TEST_ASSERT(sideAligned->GetMaxRatio() == 1.0);
			sideAligned->SetMaxLength(25);
			TEST_ASSERT(sideAligned->GetMaxLength() == 25);
		});

		TEST_CASE(L"Left")
		{
			sideAligned->SetDirection(GuiSideAlignedComposition::Left);
			TEST_ASSERT(sideAligned->GetDirection() == GuiSideAlignedComposition::Left);
			sideAligned->SetMaxRatio(0.6);
			TEST_ASSERT(sideAligned->GetMaxRatio() == 0.6);
			TEST_ASSERT(sideAligned->GetBounds() == Rect({ 0,0 }, { 25,50 }));
			sideAligned->SetMaxRatio(0.1);
			TEST_ASSERT(sideAligned->GetMaxRatio() == 0.1);
			TEST_ASSERT(sideAligned->GetBounds() == Rect({ 0,0 }, { 10,50 }));
		});

		TEST_CASE(L"Top")
		{
			sideAligned->SetDirection(GuiSideAlignedComposition::Top);
			TEST_ASSERT(sideAligned->GetDirection() == GuiSideAlignedComposition::Top);
			sideAligned->SetMaxRatio(0.6);
			TEST_ASSERT(sideAligned->GetMaxRatio() == 0.6);
			TEST_ASSERT(sideAligned->GetBounds() == Rect({ 0,0 }, { 100,25 }));
			sideAligned->SetMaxRatio(0.1);
			TEST_ASSERT(sideAligned->GetMaxRatio() == 0.1);
			TEST_ASSERT(sideAligned->GetBounds() == Rect({ 0,0 }, { 100,5 }));
		});

		TEST_CASE(L"Right")
		{
			sideAligned->SetDirection(GuiSideAlignedComposition::Right);
			TEST_ASSERT(sideAligned->GetDirection() == GuiSideAlignedComposition::Right);
			sideAligned->SetMaxRatio(0.6);
			TEST_ASSERT(sideAligned->GetMaxRatio() == 0.6);
			TEST_ASSERT(sideAligned->GetBounds() == Rect({ 75,0 }, { 25,50 }));
			sideAligned->SetMaxRatio(0.1);
			TEST_ASSERT(sideAligned->GetMaxRatio() == 0.1);
			TEST_ASSERT(sideAligned->GetBounds() == Rect({ 90,0 }, { 10,50 }));
		});

		TEST_CASE(L"Bottom")
		{
			sideAligned->SetDirection(GuiSideAlignedComposition::Bottom);
			TEST_ASSERT(sideAligned->GetDirection() == GuiSideAlignedComposition::Bottom);
			sideAligned->SetMaxRatio(0.6);
			TEST_ASSERT(sideAligned->GetMaxRatio() == 0.6);
			TEST_ASSERT(sideAligned->GetBounds() == Rect({ 0,25 }, { 100,25 }));
			sideAligned->SetMaxRatio(0.1);
			TEST_ASSERT(sideAligned->GetMaxRatio() == 0.1);
			TEST_ASSERT(sideAligned->GetBounds() == Rect({ 0,45 }, { 100,5 }));
		});

		SafeDeleteComposition(bounds);
	});

	TEST_CASE(L"Test <PartialView>")
	{
	});
}